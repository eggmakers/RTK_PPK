#include <stddef.h>
#include "stm32f4xx_hal.h"
#include "boot.h"

#ifndef USE_BOOTLOADER
#define USE_BOOTLOADER 0
#endif

#ifndef USE_FASTBOOT
#define USE_FASTBOOT 0
#endif

/**
 * @note FLASH definition.
 */
#define FLASH_SIZE_KB     (2048)                    /** Flash size. */
#define BL_START_ADDR     (FLASH_BASE)              /** Bootloader start address. */
#define APP_DESC_ADDR     (0x0800C000)              /** App descriptor start address. */
#define APP_START_ADDR    (0x08010000)              /** App firmware start address. */
#define APP_END_ADDR      (0x081E0000 + 128 * 1024) /** App firmware end address. */
#define APP_MAX_SIZE      (APP_END_ADDR - APP_START_ADDR + 1) /** Max app firmware start address. */

#define APP_CAN_UPDATE    (0xCC) /** Application support uavcan firmware update. */

#if USE_BOOTLOADER
/**
 * @note Application descriptor, It's between BOOTLOADER and APPLICATION.
 */
typedef struct
{
    uint8_t magic[8];      /** Signature. */ //  = {'A', 'P', 'P', ' ', 'D', 'E', 'S', 'C'};
    uint32_t git_hash;     /** Git hash. */
    uint32_t image_size;   /** Image(app) size. */
    uint8_t version_major; /** Image major version. */
    uint8_t version_minor; /** Image minor version. */
    uint8_t compat_flag;   /** App support uavcan update. */
    uint16_t board_id;     /** Device ID. */
    uint32_t image_crc;    /** Image crc.(application) */
    uint32_t desc_crc;     /** Application description crc.(from magic to reserved) */
    uint8_t reserved[8];   /** Reserved. */
} app_descriptor_t;

__IO uint32_t VECTOR_TABLE[256] __attribute__((at(0x20000000)));

/**
 * @brief  Set compat flag (Support uavcan firmware update).
 * @param  None
 * @retval None
 */
void set_compat_flag(void)
{
#if USE_FASTBOOT
    app_descriptor_t desc = {0};

    /** Read application descriptor. */
    flash_read(APP_DESC_ADDR, (uint8_t *)&desc, sizeof(app_descriptor_t));

    /** Set support flag. */
    desc.compat_flag = APP_CAN_UPDATE;

    /** CRC compute length. */
    uint8_t desc_len = (uint8_t)(offsetof(app_descriptor_t, desc_crc) - offsetof(app_descriptor_t, magic));

    /** Compute crc. */
    desc.desc_crc = sw_crc32_calculate(0, (uint8_t*)&desc, desc_len);

    /** Erase application descriptor flash area. */
    if (flash_erase_page(get_sector(APP_DESC_ADDR)))
    {
        /** Write in flash. */
        flash_write(APP_DESC_ADDR, (uint8_t*)&desc, sizeof(app_descriptor_t));
    }
#endif
}

/**
 * @brief  重映射中断向量表.
 * @param  None
 * @retval None
 */
void sys_remap(void)
{
#define FIRMWARE_START_ADDR (uint32_t)APP_START_ADDR

    /** 复制中断向量表到 RAM 的首地址(0x20000000). */
    for(uint32_t vector_index = 0; vector_index < 256; vector_index++)
    {
        VECTOR_TABLE[vector_index] = *(__IO uint32_t*)(FIRMWARE_START_ADDR + (vector_index << 2));
    }

    __disable_irq();

    /** relocate vector table. */
    SCB->VTOR = (uint32_t)&VECTOR_TABLE;

    __DSB();

    __enable_irq();
}
#else
void set_compat_flag(void) {}

#endif
