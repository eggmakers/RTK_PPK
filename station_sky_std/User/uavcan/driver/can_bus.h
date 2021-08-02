#ifndef __CAN_BUS_H
#define __CAN_BUS_H
#include <stdint.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"

#ifndef USE_CAN2
#define USE_CAN2    0
#endif

#ifndef USE_CAN_POLLING
#define USE_CAN_POLLING 0
#endif

/** 默认波特率. */
#define DEFAULT_CAN_BR    (1000000)

/** 默认节点模式. */
#define DEFAULT_CAN_MODE  CAN_MODE_NORMAL

/**
 * @brief Initialize can peripheral.
 * @arg   None
 * @ret   None
 */
void can_gpio_init(void);

/**
 * @brief 配置 CAN 外设.
 * @arg   None
 * @ret   < 0, 出错
 */
int8_t can_config(void);

/**
 * @brief  获取 CAN1 句柄指针.
 * @param  None.
 * @retval CAN1 句柄地址.
 */
CAN_HandleTypeDef* get_can1_handle(void);

/**
 * @brief  获取 CAN1 句柄指针.
 * @param  None.
 * @retval CAN1 句柄地址.
 */
CAN_HandleTypeDef* get_can2_handle(void);

#endif
