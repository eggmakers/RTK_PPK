#ifndef __uavcan_reboot_h
#define __uavcan_reboot_h
#include <stdint.h>
#include <stdbool.h>
#include "canard.h"

#define UAVCAN_PROTOCOL_RESTARTNODE_ID                     5
#define UAVCAN_PROTOCOL_RESTARTNODE_NAME                   "uavcan.protocol.RestartNode"
#define UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE              (0x569E05394A3017F0ULL)
#define UAVCAN_PROTOCOL_RESTARTNODE_REQUEST_MAX_SIZE       BIT_LEN_TO_SIZE(40)

#define UAVCAN_PROTOCOL_RESTARTNODE_RESPONSE_MAX_SIZE      BIT_LEN_TO_SIZE(1)

// Constants
#define UAVCAN_PROTOCOL_RESTARTNODE_REQ_MAGIC_NUMBER       0xACCE551B1E

typedef struct
{
    uint64_t magic_number;
} uavcan_protocol_RestartNodeRequest;

typedef struct
{
    bool ok;
} uavcan_protocol_RestartNodeResponse;

/**
 * @brief  处理节点重启请求.
 * @param  ins, UAVCAN 实例
 * @param  transfer, 打包后的数据存储缓冲区
 * @retval None
 */
void uavcan_handle_node_restart(CanardInstance* ins, CanardRxTransfer* transfer);

#endif
