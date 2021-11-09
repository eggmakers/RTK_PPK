#ifndef __uavcan_autobaud_h
#define __uavcan_autobaud_h

#include "canard_stm32.h"

/**
 * @note Auto-baud detection status.
 */
typedef struct
{
    bool success;          /** 是否成功获取到波特率. */
    uint32_t start_ms;     /** 开始的时间. */
    uint8_t crnt_baud_idx; /** 切换索引号. */
} canbus_autobaud_state_t;

/**
 * @brief  初始化CAN总线.
 * @param  target_bitrate: The bitrate to be set. The bitrate must be in "valid_baudrates".
 * @param  mode: The can mode to be set.
 *     @arg CanardSTM32IfaceModeNormal: Used to the typical data transmission.
 *     @arg CanardSTM32IfaceModeSilent: Used to auto-baudrate detection.
 * @retval False if the baudrate invalid.
 */
bool canbus_init(const uint32_t target_bitrate, CanardSTM32IfaceMode mode);

/**
 * @brief  Start automatic bitrate detection.
 * @param  state: The instance of auto-baud detection status. @see canbus_autobaud_state_t
 * @param  baud: The bitrate to be set.
 * @retval None
 */
void canbus_autobaud_start(canbus_autobaud_state_t* state, uint32_t baud);

/**
 * @brief  更新波特率, 检查是否正确.
 * @param  state: The instance of auto-baud detection status. @see canbus_autobaud_state_t
 * @retval The detected can bus bit rate.
 */
int32_t canbus_autobaud_update(canbus_autobaud_state_t* state);

#endif
