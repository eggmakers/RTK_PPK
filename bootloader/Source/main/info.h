#ifndef __INFO_H
#define __INFO_H
#include <stdint.h>

/** 只用 CAN2, 将 0 改成 1. */
#ifndef USE_CAN2_ONLY
#define USE_CAN2_ONLY    0
#endif

/**
 * @note UAVCAN node information.
 */
#define HARDWARE_VERSION_MAJOR   1
#define HARDWARE_VERSION_MINOR   0

#define SOFTWARE_VERSION_MAJOR   1
#define SOFTWARE_VERSION_MINOR   2
#define SOFTWARE_GIT_HASH        0x20200320

#define APP_NODE_CERTIFICATE     "China MMC Tech"
#define APP_NODE_NAME            "com.mmc.bootloader"

/**
 * @note FLASH definition.
 */
#define FLASH_SIZE_KB     (2048)                    /** Flash size. */
#define BL_START_ADDR     (FLASH_BASE)              /** Bootloader start address. */
#define APP_DESC_ADDR     (0x0800C000)              /** App descriptor start address. */
#define APP_START_ADDR    (0x08010000)              /** App firmware start address. */
//#define APP_START_ADDR    (0x08020000)              /** App firmware start address. */
#define APP_END_ADDR      (0x081E0000 + 128 * 1024) /** App firmware end address. */
#define APP_MAX_SIZE      (APP_END_ADDR - APP_START_ADDR + 1) /** Max app firmware start address. */

/**
 * @brief  Get uavcan node ID.
 * @param  None
 * @retval UAVCAN node ID.
 */
uint8_t get_can_node_id(void);

/**
 * @brief  Set uavcan node ID.
 * @param  id: The uavcan node ID to be set (0 - 127).
 * @retval None
 */
void set_can_node_id(uint8_t id);

/**
 * @brief  Get can baudrate.
 * @param  None
 * @retval Baudrate.
 */
uint32_t get_can_baudrate(void);

/**
 * @brief  Set can baudrate.
 * @param  baudrate: The baudrate to be set.
 * @retval None
 */
void set_can_baudrate(uint32_t baudrate);

#endif
