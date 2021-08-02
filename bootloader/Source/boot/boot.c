#include <string.h>
#include <stddef.h>
#include "info.h"
#include "io.h"
#include "time.h"
#include "system.h"
#include "flash.h"
#include "sw_crc.h"
#include "watchdog.h"
#include "boot.h"

/**
 * RTC backup register definition.
 * +-----------------------------+
 * |Register     |    Use for    |
 * +-----------------------------+
 * | RTC_BKP_DR5 |  Boot option  |
 * | RTC_BKP_DR4 |  App node ID  |
 * | RTC_BKP_DR3 |  Baudrate(L)  |
 * | RTC_BKP_DR2 |  Baudrate(H)  |
 * +-----------------------------+
 *
 * Firmware update RAM shared space definition.
 * {
 *     uint8_t  node_id;
 *     uint32_t baudrate;
 *     uint8_t  src_id;
 *     uint8_t  path[201];
 * }
 *
 * Firmware descriptor definition.
 * {
 *     uint8_t magic[8];
 *     uint32_t git_hash;
 *     uint32_t image_size;
 *     uint8_t version_major;
 *     uint8_t version_minor;
 *     uint16_t board_id;
 *     uint8_t reserved[8];
 *     uint32_t desc_crc;
 *     uint32_t image_crc;
 * }
 */
#define APP_CAN_UPDATE     (0xCC) /** Application support uavcan firmware update. */
#define BOOTLOADER_TIMEOUT (3000)

static bool try_boot = false; /** Whether to jump to APP or not. */
static uint16_t timeout = BOOTLOADER_TIMEOUT; /** Timeout for jumping to APP. */

static app_descriptor_t desc; /** App descriptor. */

/**
 * @brief  Check firmware update.
 * @param  None
 * @retval None
 */
void check_update(void)
{
    /** Get node ID. */
    uint32_t id = HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR4);

    /** Get Baud rate. */
    uint32_t baudrate = HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR3);
    baudrate |= (HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR2) << 16);

    /** Set uavcan node ID. */
    set_can_node_id((id & 0xFF));

    /** Set uavcan baudrate. */
    set_can_baudrate(baudrate);
}

/**
 * @brief  Clear shared area.
 * @param  None
 * @retval None
 */
static void clear_shared_area(void)
{
    HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR5, 0u);
    HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR4, 0u);
    HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR3, 0u);
}

/**
 * @brief  Check firmware crc32.
 * @param  None
 * @retval The result of crc32 computing.
 */
static uint32_t check_firmware_crc(void)
{
    /** i - desc.image_size. */
    return 0;
}

/**
 * @brief  检查固件是否 OK.
 * @param  None
 * @retval None
 */
static bool check_firmware(void)
{
	#if 0
	/** 去掉以下的判断程序 */
    const char sig[8] = {'A', 'P', 'P', ' ', 'D', 'E', 'S', 'C'};

    flash_read(APP_DESC_ADDR, (uint8_t *)&desc, sizeof(app_descriptor_t));

    /** Check magic number. */
    if (memcmp(desc.magic, sig, 8) != 0)
    {
        return false;
    }

    /** Check descriptor crc. */
    const uint8_t desc_len = (uint8_t)(offsetof(app_descriptor_t, desc_crc) - offsetof(app_descriptor_t, magic));
    if (desc.desc_crc != sw_crc32_calculate(0, (uint8_t*)&desc, desc_len))
    {
        return false;
    }

    /** Check firmware crc32. */
    if (desc.image_crc != check_firmware_crc())
    {
        return false;
    }
	#endif
	
    /** Check stack address. */
    return bl_check_app_exist();
}

/**
 * @brief  启动项选择.
 * @param  None
 * @retval None
 */
static void boot_option(void)
{
    /** Get status from RTC register. */
    uint32_t option = HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR5);

    switch (option)
    {
    case RTC_BOOT_OFF:
        try_boot = false;
        timeout = 3000; /** 防止烧错固件, 有重新更新固件的余地. */
        break;

    case RTC_BOOT_HOLD:
        /** Hold on bootloader. */
        try_boot = false;
        timeout = 0;
        break;

    case RTC_BOOT_FAST:
        /** Fast boot. */
        try_boot = true;
        timeout  = 0;
        break;

    case RTC_BOOT_UPDATE:
        /** Firmware update. */
        try_boot = false;
        timeout  = 10000;

        /** Check firmware update infomation. */
        check_update();
        break;

    case RTC_BOOT_FWOK:
        break;

    default:
        break;
    }
}

/**
 * @note Timeout time tick.
 */
static time_ms_t timeout_tick = 0;

/**
 * @brief  Bootloader timeout time tick reset.
 * @param  None
 * @retval None
 */
void bl_timeout_reset(void)
{
    timeout_tick = millis();
}

/**
 * @brief  Whether to stay at bootloader timeout.
 * @param  None
 * @retval True if timeout.
 */
bool bl_timeout(void)
{
    if (timeout_tick == 0)
    {
        /** Record time tick for the first time. */
        timeout_tick = millis();
    }

    if (timeout != 0)
    {
        if (millis() - timeout_tick >= timeout)
        {
            /** Timeout. */
            timeout_tick = 0;
            return true;
        }
    }
    return false;
}

/**
 * @brief  Boot selection.
 * @param  None
 * @retval None
 */
void boot(void)
{
    /** 检查是否是看门狗复位导致系统重启, 如果是就立即重新进入 APP. */
    if (was_watchdag_reset() == true)
    {
        try_boot = true; /** 要求启动 APP. */
        timeout  = 0;
    }
    else
    {
        /** 启动选择. */
        boot_option();
    }

    /** Clean. */
    clear_shared_area();

    /** 检查固件是否 OK. */
    if (check_firmware() != true)
    {
        try_boot = false;
        timeout = 0;
    }
    else if ((desc.compat_flag == APP_CAN_UPDATE) && (timeout != 0))
    {
        /** fast boot for good firmware. */
        try_boot = true;
        timeout = 1000;
    }

    /** Try boot. */
    if (try_boot == true)
    {
        jump_to_app();
    }
}
