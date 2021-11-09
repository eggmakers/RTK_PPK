#include "stm32f4xx_hal.h"
#include "canard_stm32.h"
#include "canard.h"
#include "time.h"
#include "uavcan_autobaud.h"

/**
 * @note 有效的字节率.
 */
static const uint32_t valid_baudrates[] =
{
    1000000,
    500000,
    250000,
    125000,
};

/** Number of bitrate list. */
#define BAUDRATE_LIST_NUM (sizeof(valid_baudrates) / sizeof(valid_baudrates[0]))

/**
 * @brief  确认波特率是否可用.
 * @param  baud: The bitrate need to check.
 * @retval None
 */
static bool canbus_baudrate_valid(uint32_t baud)
{
    for (uint8_t i = 0; i < BAUDRATE_LIST_NUM; i++)
    {
        if (valid_baudrates[i] == baud)
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief  初始化CAN总线.
 * @param  target_bitrate: The bitrate to be set. The bitrate must be in "valid_baudrates".
 * @param  mode: The can mode to be set.
 *     @arg CanardSTM32IfaceModeNormal: Used to the typical data transmission.
 *     @arg CanardSTM32IfaceModeSilent: Used to auto-baudrate detection.
 * @retval False if the baudrate invalid.
 */
bool canbus_init(const uint32_t target_bitrate, CanardSTM32IfaceMode mode)
{
    /** 检查波特率是否可用. */
    if (!canbus_baudrate_valid(target_bitrate))
    {
        return false;
    }

    /** 初始化默认UAVCAN传输速率. */
    CanardSTM32CANTimings timings;
    if (canardSTM32ComputeCANTimings(HAL_RCC_GetPCLK1Freq(), target_bitrate, &timings))
    {
        /** bus error. */
    }

    /** 初始化 UAVCAN 底层驱动. */
    if (canardSTM32Init(&timings, mode))
    {
        /** bus error. */
    }

    return true;
}

/**
 * @brief  Start automatic bitrate detection.
 * @param  state: The instance of auto-baud detection status. @see canbus_autobaud_state_t
 * @param  baud: The bitrate to be set.
 * @retval None
 */
void canbus_autobaud_start(canbus_autobaud_state_t* state, uint32_t baud)
{
    /** 记录开始时间. */
    state->start_ms = millis();

    /** 遍历波特率, 按照数组从上往下进行选择波特率. */
    for (uint8_t i = 0; i < BAUDRATE_LIST_NUM; i++)
    {
        state->crnt_baud_idx = i;

        if (valid_baudrates[i] == baud)
        {
            break;
        }
    }

    /** 状态未成功. */
    state->success = false;

    /** 使用监听模式监听总线内容. */
    canbus_init(valid_baudrates[state->crnt_baud_idx], CanardSTM32IfaceModeSilent);
}

/**
 * @brief  更新波特率, 检查是否正确.
 * @param  state: The instance of auto-baud detection status. @see canbus_autobaud_state_t
 * @retval The detected can bus bit rate.
 */
int32_t canbus_autobaud_update(canbus_autobaud_state_t* state)
{
    /** 成功接收到帧, 返回对应的波特率. */
    CanardCANFrame rx_frame;

    if (canardSTM32Receive(&rx_frame) > 0)
    {
        state->success = true;

        return valid_baudrates[state->crnt_baud_idx];
    }

#define AUTOBAUD_SWITCH_INTERVAL_MS (500)

    /** 未成功, 按照500ms切换一次数组的波特率. */
    if (state->start_ms + AUTOBAUD_SWITCH_INTERVAL_MS <= millis())
    {
        /** 更新时间. */
        state->start_ms = millis();

        /** 切换波特率. */
        if (state->crnt_baud_idx == BAUDRATE_LIST_NUM - 1)
        {
            state->crnt_baud_idx = 0;
        }
        else
        {
            state->crnt_baud_idx++;
        }

        /** 重新初始化总线. */
        canbus_init(valid_baudrates[state->crnt_baud_idx], CanardSTM32IfaceModeSilent);
    }

    return -1;
}
