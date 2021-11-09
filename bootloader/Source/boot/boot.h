#ifndef __boot_h
#define __boot_h
#include <stdint.h>
#include <stdbool.h>

#define RTC_BOOT_OFF    (0x0000) /** Stay in bootloader. */
#define RTC_BOOT_HOLD   (0x1001) /** Hold on bootloader. */
#define RTC_BOOT_FAST   (0x1002) /** Fast boot.*/
#define RTC_BOOT_UPDATE (0x1003) /** ORd with 8 bit local node ID. */
#define RTC_BOOT_FWOK   (0x1004) /** indicates FW ran for 30s. */

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

/**
 * @note CAN bootloader update node id information.
 *       For those with limited memory space.
 */
typedef struct
{
    uint8_t node_id;   /** Node ID. */
    uint32_t baudrate; /** CAN baudrate. */
} app_node_info_t;

/**
 * @note CAN bootloader update information.
 *       For those with sufficient memory space.
 */
typedef struct
{
    app_node_info_t node_info; /** Application node information. */
    uint8_t node_id;   /** Node ID. */
    uint8_t src_id;    /** The node ID of the request to update the firmware. */
    uint8_t path[201]; /** Firmware path. */
} fw_update_info_t;

/**
 * @brief  Bootloader timeout time tick reset.
 * @param  None
 * @retval None
 */
void bl_timeout_reset(void);

/**
 * @brief  Whether to stay at bootloader timeout.
 * @param  None
 * @retval True if timeout.
 */
bool bl_timeout(void);

/**
 * @brief  Boot selection.
 * @param  None
 * @retval None
 */
void boot(void);

#endif
