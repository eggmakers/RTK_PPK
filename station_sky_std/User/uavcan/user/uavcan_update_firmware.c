#include "can_bus.h"
#include "stm32f4xx_hal.h"
#include "uavcan_update_firmware.h"

#define RTC_BOOT_OFF    (0x0000) /** Stay in bootloader. */
#define RTC_BOOT_HOLD   (0x1001) /** Hold on bootloader. */
#define RTC_BOOT_FAST   (0x1002) /** Fast boot.*/
#define RTC_BOOT_UPDATE (0x1003) /** ORd with 8 bit local node ID. */
#define RTC_BOOT_FWOK   (0x1004) /** indicates FW ran for 30s. */



/**
 * @brief  固件更新请求.
 * @param  ins: The pointer of uavcan instance.
 * @param  transfer: The pointer of transfer buffer.
 * @retval None
 */
void handle_file_beginfirmwareupdate_request(CanardInstance* ins, CanardRxTransfer* transfer)
{
	uint32_t g_canbus_baud = DEFAULT_CAN_BR; /** Baudrate. */
    /** 固件更新. */
    HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR5, RTC_BOOT_UPDATE);

    /** Node ID. */
    HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR4, (uint32_t)ins->node_id);

    /** Baudrate (L). */
    HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR3, (g_canbus_baud & 0xFFFF));

    /** Baudrate (H). */
    HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR2, ((g_canbus_baud >> 16) & 0xFFFF));

    HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR5);
    HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR4);
    HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR3);
    HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR2);

    /** 重启. */
    HAL_NVIC_SystemReset();
}
