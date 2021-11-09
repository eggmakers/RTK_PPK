#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_it.h"
#include "info.h"
#include "can_bus.h"

/** 默认波特率. */
#define DEFAULT_CAN_BR    (1000000)

/** 默认节点模式. */
#define DEFAULT_CAN_MODE  CAN_MODE_NORMAL

static CAN_HandleTypeDef hcan1;
static CAN_HandleTypeDef hcan2;

/**
 * These parameters define the timings of the CAN controller.
 * Please refer to the documentation of the bxCAN macrocell for explanation.
 * These values can be computed by the developed beforehand if ROM size is of a concern,
 * or they can be computed at run time using the function defined below.
 */
typedef struct
{
    uint16_t psc;
    uint32_t bs1;
    uint32_t bs2;
    uint32_t sjw;
} can_timings;

/**
 * @brief  Compute the CAN timings.
 * @param  can_clk: can peripheral clock rate
 *                  CAN is uasually mounted on APB1 bus,
 *                  so it's uasually PCLK1.
 * @param  br: target bit rate.
 * @param  out_timings: output timings.
 * @retval -1, Parameters invalid.
 *         0,  Computed complete.
 */
static int8_t compute_can_timings(const uint32_t can_clk, const uint32_t br, can_timings* const out_timings)
{
    if (br < 1000)
    {
        return -1;
    }

    if (out_timings == NULL)
    {
        return -1;
    }

    memset(out_timings, 0, sizeof(*out_timings));

    /*
     * Hardware configuration
     */
    static const uint8_t max_bs1 = 16;
    static const uint8_t max_bs2 = 8;

    /*
     * Ref. "Automatic Baudrate Detection in CANopen Networks", U. Koppe, MicroControl GmbH & Co. KG
     *      CAN in Automation, 2003
     *
     * According to the source, optimal quanta per bit are:
     *   Bitrate        Optimal Maximum
     *   1000 kbps      8       10
     *   500  kbps      16      17
     *   250  kbps      16      17
     *   125  kbps      16      17
     */
    const uint8_t max_quanta_per_bit = (uint8_t)((br >= 1000000) ? 10 : 17);

    /*
     * Ref. "Automatic Baudrate Detection in CANopen Networks", U. Koppe, MicroControl GmbH & Co. KG
     *      CAN in Automation, 2003
     *
     *   Bit-rate   Sample point location      Range
     *   1000 kbps             87.5%       75.0% - 90.0%
     *   500  kbps             87.5%       75.0% - 90.0%
     *   250  kbps             87.5%       75.0% - 90.0%
     *   125  kbps             87.5%       75.0% - 90.0%
     */
    static const uint16_t max_sample_point_location_permill = 900;

    /*
     * Computing (prescaler * BS):
     *         See the Reference Manual
     *   +---------------------------------+
     *   |    BITRATE = 1 / T(one_bit)     |
     *   +---------------------------------+
     *   |T(one_bit) = tq * (1 + BS1 + BS2)|
     *   -----------------------------------
     *   |    tq = PRESCALER * Tpclk       |
     *   -----------------------------------
     *   |      Tpclk = (1 / PCLK)         |
     *   +---------------------------------+
     *
     *   BITRATE = 1 / (PRESCALER * (1 / PCLK) * (1 + BS1 + BS2))       -- Summarize
     *   BITRATE = PCLK / (PRESCALER * (1 + BS1 + BS2))                 -- Simplified
     * let:
     *   BS = 1 + BS1 + BS2                                             -- Number of time quanta per bit
     *   PRESCALER_BS = PRESCALER * BS
     * ==>
     *   PRESCALER_BS = PCLK / BITRATE
     */
    const uint32_t prescaler_bs = can_clk / br;

    /*
     * Searching for such prescaler value so that the number of quanta per bit is highest.
     */
    uint8_t bs1_bs2_sum = (uint8_t)(max_quanta_per_bit - 1); /** one bit = sync(1tq) + bs1(ntq) + bs2(mtq) */

    while ((prescaler_bs % (1U + bs1_bs2_sum)) != 0)
    {
        if (bs1_bs2_sum <= 2)
        {
            return -1;
        }
        bs1_bs2_sum--;
    }

    const uint32_t prescaler = prescaler_bs / (1U + bs1_bs2_sum);
    if ((prescaler < 1U) || (prescaler > 1024U))
    {
        return -1;
    }

    /*
     * Now we have a constraint: (BS1 + BS2) == bs1_bs2_sum.
     * We need to find such values so that the sample point is as close as possible to the optimal value,
     * which is 87.5%, which is 7/8.
     *
     *   Solve[(1 + bs1)/(1 + bs1 + bs2) == 7/8, bs2]  (* Where 7/8 is 0.875, the recommended sample point location *)
     *   {{bs2 -> (1 + bs1)/7}}
     *
     * Hence:
     *   bs2 = (1 + bs1) / 7
     *   bs1 = (7 * bs1_bs2_sum - 1) / 8
     *
     * Sample point location can be computed as follows:
     *   Sample point location = (1 + bs1) / (1 + bs1 + bs2)
     *
     * Since the optimal solution is so close to the maximum, we prepare two solutions, and then pick the best one:
     *   - With rounding to nearest
     *   - With rounding to zero
     */
    uint8_t bs1 = (uint8_t)(((7 * bs1_bs2_sum - 1) + 4) / 8); /** Trying rounding to nearest first +0.5 */
    uint8_t bs2 = (uint8_t)(bs1_bs2_sum - bs1);

    if (bs1_bs2_sum <= bs1)
    {
        return -1;
    }

    {
        const uint16_t sample_point_permill = (uint16_t)(1000U * (1U + bs1) / (1U + bs1 + bs2));

        if (sample_point_permill > max_sample_point_location_permill) /** Strictly more! */
        {
            bs1 = (uint8_t)((7 * bs1_bs2_sum - 1) / 8); /** Nope, too far; now rounding to zero */
            bs2 = (uint8_t)(bs1_bs2_sum - bs1);
        }
    }

    const bool valid = (bs1 >= 1) && (bs1 <= max_bs1) && (bs2 >= 1) && (bs2 <= max_bs2);

    /*
     * Final validation
     * Helpful Python:
     * def sample_point_from_btr(x):
     *     assert 0b0011110010000000111111000000000 & x == 0
     *     ts2,ts1,brp = (x>>20)&7, (x>>16)&15, x&511
     *     return (1+ts1+1)/(1+ts1+1+ts2+1)
     */
    if ((br != (can_clk / (prescaler * (1U + bs1 + bs2)))) || !valid)
    {
        return -1;
    }

    out_timings->psc = (uint16_t) prescaler;                  /** BRP[9:0] */
    out_timings->sjw = (uint32_t)((1 - 1U) & 1023U);          /** SJW[1:0] One is recommended by UAVCAN, CANOpen, and DeviceNet */
    out_timings->bs1 = (uint32_t)(((bs1 - 1U) & 15U) << 16U); /** TS1[3:0] */
    out_timings->bs2 = (uint32_t)(((bs2 - 1U) & 7U) << 20U);  /** TS1[2:0] */

    return 0;
}

/**
 * @brief Initialize can peripheral.
 * @arg   None
 * @ret   None
 */
void can_gpio_init(void)
{
    /* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_CAN1_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;

    GPIO_InitStruct.Pin = CAN1_RX_PIN;
    HAL_GPIO_Init(CAN1_RX_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CAN1_TX_PIN;
    HAL_GPIO_Init(CAN1_TX_GPIO, &GPIO_InitStruct);

    /** NVIC configuration for CAN1 Reception complete interrupt */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

#if USE_CAN2

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_CAN2_CLK_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;

    GPIO_InitStruct.Pin = CAN2_RX_PIN;
    HAL_GPIO_Init(CAN2_RX_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CAN2_TX_PIN;
    HAL_GPIO_Init(CAN2_TX_GPIO, &GPIO_InitStruct);

    /** NVIC configuration for CAN1 Reception complete interrupt */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);

#endif
}

/**
 * @brief 配置 CAN 外设.
 * @arg   None
 * @ret   < 0, 出错
 */
int8_t can_config(void)
{
    can_timings timings;

    /** 获取波特率对应的 Prescaler，BS1，BS2，SJW */
    if (compute_can_timings(HAL_RCC_GetPCLK1Freq(), DEFAULT_CAN_BR, &timings))
    {
        /** Unsupported bit rate or supplied clock was incorrect. */
        return -1;
    }

    /**
     * @note 配置 CAN1.
     */
    hcan1.Instance = CAN1;
    hcan1.Init.Mode          = DEFAULT_CAN_MODE; /** 默认正常模式. */
    hcan1.Init.Prescaler     = timings.psc;      /** 通过设定的波特率计算出 TQ 长度及一个位（BS1 + BS2）占多少 TQ. */
    hcan1.Init.TimeSeg1      = timings.bs1;      /** 通过设定的波特率计算出 TQ 长度及一个位（BS1 + BS2）占多少 TQ. */
    hcan1.Init.TimeSeg2      = timings.bs2;      /** 通过设定的波特率计算出 TQ 长度及一个位（BS1 + BS2）占多少 TQ. */
    hcan1.Init.SyncJumpWidth = timings.sjw;      /** 自动同步偏移值，UAVCAN 和 CANOpen 为 1TQ. */

    hcan1.Init.TimeTriggeredMode    = DISABLE;   /** 不使用时间戳. */
    hcan1.Init.AutoBusOff           = ENABLE;    /** 自动离线. */
    hcan1.Init.AutoWakeUp           = ENABLE;    /** 自动唤醒. */
    hcan1.Init.AutoRetransmission   = ENABLE;    /** 开启自动重传. */
    hcan1.Init.ReceiveFifoLocked    = DISABLE;   /** FIFO 接收满时不锁定. */
    hcan1.Init.TransmitFifoPriority = DISABLE;   /** 按消息 ID 优先级传输. */

    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }

    /**
     * @note 配置 CAN2.
     */
    hcan2.Instance = CAN2;
    hcan2.Init.Mode          = DEFAULT_CAN_MODE; /** 默认正常模式. */
    hcan2.Init.Prescaler     = timings.psc;      /** 通过设定的波特率计算出 TQ 长度及一个位（BS1 + BS2）占多少 TQ. */
    hcan2.Init.TimeSeg1      = timings.bs1;      /** 通过设定的波特率计算出 TQ 长度及一个位（BS1 + BS2）占多少 TQ. */
    hcan2.Init.TimeSeg2      = timings.bs2;      /** 通过设定的波特率计算出 TQ 长度及一个位（BS1 + BS2）占多少 TQ. */
    hcan2.Init.SyncJumpWidth = timings.sjw;      /** 自动同步偏移值，UAVCAN 和 CANOpen 为 1TQ. */

    hcan2.Init.TimeTriggeredMode    = DISABLE;   /** 不使用时间戳. */
    hcan2.Init.AutoBusOff           = ENABLE;    /** 自动离线. */
    hcan2.Init.AutoWakeUp           = ENABLE;    /** 自动唤醒. */
    hcan2.Init.AutoRetransmission   = ENABLE;    /** 开启自动重传. */
    hcan2.Init.ReceiveFifoLocked    = DISABLE;   /** FIFO 接收满时不锁定. */
    hcan2.Init.TransmitFifoPriority = DISABLE;   /** 按消息 ID 优先级传输. */

    if (HAL_CAN_Init(&hcan2) != HAL_OK)
    {
        Error_Handler();
    }

    /**
     * @note 配置过滤器（只有 CAN1 可以配置.）
     */
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank           = 0;                     /** 配置第 0 组过滤器 for CAN1. */
    sFilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK; /** 采用掩码的方式过滤 ID. */
    sFilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT; /** UAVCAN 使用的是 29 位扩展 ID，所以采用 32 位 ID 过滤. */
    sFilterConfig.FilterIdHigh         = 0x0000;                /** 采用掩码的方式过滤，所以 ID 可以随便填. */
    sFilterConfig.FilterIdLow          = 0x0000;                /** 采用掩码的方式过滤，所以 ID 可以随便填. */
    sFilterConfig.FilterMaskIdHigh     = 0x0000;                /** 掩码高位，全零代表所有的 ID 都不过滤. */
    sFilterConfig.FilterMaskIdLow      = 0x0000;                /** 掩码低位，全零代表所有的 ID 都不过滤. */
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;          /** 接收到的数据放到 FIFI0. */
    sFilterConfig.FilterActivation     = ENABLE;                /** 激活过滤器. */
    sFilterConfig.SlaveStartFilterBank = 14;                    /** CAN2 属于 SLAVE，其过滤器从 14 开始. */

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }

    sFilterConfig.FilterBank = 14; /** 配置第 14 组过滤器 for CAN2. */

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }

    /**
     * @note Start the CAN peripheral
     */
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }

    if (HAL_CAN_Start(&hcan2) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }

    /**
     * @note 配置接收中断.
     */

    /**
     * @note 开启接收完成中断（FIFO0）.
     */
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        /* Notification Error */
        Error_Handler();
    }
    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        /* Notification Error */
        Error_Handler();
    }
    /**
     * @note 中断服务函数在“stm32f4xx_it.h”中定义.
     *       回调函数：HAL_CAN_RxFifo0MsgPendingCallback
     */

    return 0;
}

/**
 * @brief  获取 CAN1 句柄指针.
 * @param  None.
 * @retval CAN1 句柄地址.
 */
CAN_HandleTypeDef* get_can1_handle(void)
{
    return &hcan1;
}

/**
 * @brief  获取 CAN2 句柄指针.
 * @param  None.
 * @retval CAN2 句柄地址.
 */
CAN_HandleTypeDef* get_can2_handle(void)
{
    return &hcan2;
}
