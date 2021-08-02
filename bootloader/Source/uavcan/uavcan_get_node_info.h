#ifndef __UAVCAN_GET_NODE_INFO_H
#define __UAVCAN_GET_NODE_INFO_H
#include <stdint.h>
#include "canard.h"
#include "uavcan_node_status.h"

#define UAVCAN_PROTOCOL_GETNODEINFO_ID                  1
#define UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE           (0xEE468A8121C46A9EULL)
#define UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE   BIT_LEN_TO_SIZE(3015)

/**
 * @brief 硬件版本
 */
typedef struct
{
    uint8_t major;         /* bit len 8 */
    uint8_t minor;         /* bit len 8 */
    uint8_t unique_id[16]; /* Static Array 8bit[16] max items */

    struct
    {
        uint8_t  len;      /* Dynamic array length */
        uint8_t* data;     /* Dynamic Array 8bit[255] max items */
    } certificate_of_authenticity;
} uavcan_protocol_HardwareVersion;

/**
 * @brief 软件版本
 */
typedef struct
{
    uint8_t  major;                          /* bit len 8 */
    uint8_t  minor;                          /* bit len 8 */
    uint8_t  optional_field_flags;           /* bit len 8 */
    uint32_t vcs_commit;                     /* bit len 32 */
    uint64_t image_crc;                      /* bit len 64 */
} uavcan_protocol_SoftwareVersion;

/**
 * @brief Get Node Info Request
 */
typedef struct
{
    uint8_t empty;
} uavcan_protocol_GetNodeInfoRequest;

/**
 * @brief Get Node Info Response
 */
typedef struct
{
    uavcan_protocol_NodeStatus status;
    uavcan_protocol_SoftwareVersion software_version;
    uavcan_protocol_HardwareVersion hardware_version;

    struct
    {
        uint8_t  len;  /* Dynamic array length */
        uint8_t* data; /* Dynamic Array 8bit[80] max items */
    } name;

} uavcan_protocol_GetNodeInfoResponse;

/**
 * @brief 初始化节点信息.
 * @arg None
 * @ret None
 */
void uavcan_node_info_init(void);

/**
 * @brief 获取节点信息应答.
 * @arg ins, UAVCAN 实例
 * @arg transfer, 打包后的数据存储缓冲区
 * @ret None
 */
void uavcan_get_node_info_respond(CanardInstance* ins, CanardRxTransfer* transfer);

#endif
