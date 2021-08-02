#ifndef __uavcan_allocatee_id_h
#define __uavcan_allocatee_id_h
#include <stdbool.h>
#include "canard.h"

#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID			1
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_NAME			"uavcan.protocol.dynamic_node_id.Allocation"
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_SIGNATURE	(0xB2A812620A11D40ULL)
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_SIZE		BIT_LEN_TO_SIZE(141)

#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_REQUEST_PERIOD_MS				1000
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_REQUEST_PERIOD_MS				600

#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_FOLLOWUP_DELAY_MS				400
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_FOLLOWUP_DELAY_MS				0

#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_FOLLOWUP_TIMEOUT_MS					500
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_UID_BIT_OFFSET            	        8
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_LENGTH_OF_UNIQUE_ID_IN_REQUEST	6
#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ANY_NODE_ID							0

#define UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_UNIQUE_ID_MAX_LENGTH					16

typedef struct
{
    uint8_t node_id;
    bool first_part_of_unique_id;

    struct
    {
        uint8_t len;
        uint8_t* data;
    } unique_id;
} uavcan_protocol_dynamic_node_id_Allocation;

/**
 * @brief  检查节点 ID 分配是否完成.
 * @param  None
 * @retval true, Already have a node ID.
 *         false, It's a anonymous ID.
 */
bool allocation_is_running(void);

/**
 * @brief  初始化发送延时.
 * @param  None
 * @retval None
 */
void allocation_init(void);

/**
 * @brief  获取动态 ID.
 * @param  None
 * @retval None
 */
void allocation_update(void);

/**
 * @brief  处理动态分配节点 ID 的数据类型.
 * @param  ins, UAVCAN 实例
 * @param  transfer, 打包后的数据存储缓冲区
 * @retval None
 */
void handle_allocation_data_broadcast(CanardInstance* ins, CanardRxTransfer* transfer);

#endif
