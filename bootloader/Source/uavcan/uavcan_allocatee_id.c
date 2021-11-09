#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "time.h"
#include "uavcan_allocatee_id.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

extern CanardInstance g_canard1;
extern CanardInstance g_canard2;

/** 节点唯一序列号. */
static uint8_t node_unique_id[UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_UNIQUE_ID_MAX_LENGTH];

/**
 * @note 动态分配的节点状态.
 */
static struct
{
    uint32_t request_timer_begin_ms; /** 请求的开始时间. */
    uint32_t request_delay_ms;       /** 延时. */
    uint32_t unique_id_offset;       /** 偏移. */
} allocation_state;

/**
 * @brief  获取随机码.
 * @param  None
 * @retval 随机比例值.
 */
static float get_random_float(void)
{
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;

        /** 获取唯一ID码. */
        get_unique_id(node_unique_id);

        const uint32_t* unique_32 = (uint32_t*)&node_unique_id[0];

        srand(micros() ^ *unique_32);
    }

    return ((float)rand() / (float)RAND_MAX);
}

/**
 * @brief  定时器: 请求时间.
 * @param  None
 * @retval None
 */
static void allocation_start_request_timer(void)
{
    if (!allocation_is_running())
    {
        return;
    }

    /** 获取当前时间. */
    allocation_state.request_timer_begin_ms = millis();

    /** 随机延时, 避免与其他请求分配 ID 的节点冲突. */
    allocation_state.request_delay_ms = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_REQUEST_PERIOD_MS +
                                        (uint32_t)(get_random_float() * (UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_REQUEST_PERIOD_MS - UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_REQUEST_PERIOD_MS));
}

/**
 * @brief  当 allocator 回应后, 缩短延时的时间.
 * @param  None
 * @retval None
 */
static void allocation_start_follow_up_timer(void)
{
    if (!allocation_is_running())
    {
        return;
    }

    /** 获取当前时间. */
    allocation_state.request_timer_begin_ms = millis();

    allocation_state.request_delay_ms = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_FOLLOWUP_DELAY_MS +
                                        (uint32_t)(get_random_float() * (UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_FOLLOWUP_DELAY_MS - UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_FOLLOWUP_DELAY_MS));
}

/**
 * @brief  检查节点 ID 分配是否完成.
 * @param  None
 * @retval true, It's a anonymous ID.
 *         false, Already have a node ID.
 */
bool allocation_is_running(void)
{
    return canardGetLocalNodeID(&g_canard1) == CANARD_BROADCAST_NODE_ID; /** 匿名节点的 ID 为 0. */
}

/**
 * @brief  初始化发送延时.
 * @param  None
 * @retval None
 */
void allocation_init(void)
{
    /** 判断是否已经分配 ID. */
    if (!allocation_is_running())
    {
        return;
    }

    /** 获取随机延时时间. */
    allocation_start_request_timer();
}

/**
 * @brief  获取动态 ID.
 * @param  None
 * @retval None
 */
void allocation_update(void)
{
    /** 判断是否已经分配 ID. */
    if (!allocation_is_running())
    {
        return;
    }

    /** 随机延时一段时间. */
    if (allocation_state.request_timer_begin_ms + allocation_state.request_delay_ms > millis())
    {
        return;
    }

    /** 获取随机延时时间. */
    allocation_start_request_timer();

    /** 发送请求分配 ID 消息. */
    uint8_t allocation_request[CANARD_CAN_FRAME_MAX_DATA_LEN - 1];

    /** 每次发送的长度. */
    const uint8_t uid_size = MIN(UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_UNIQUE_ID_MAX_LENGTH - allocation_state.unique_id_offset,
                                 UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_LENGTH_OF_UNIQUE_ID_IN_REQUEST);

    /** 设置第一阶段标记位. */
    allocation_request[0] = (allocation_state.unique_id_offset == 0) ? 1 : 0;

    memcpy(&allocation_request[1], &node_unique_id[allocation_state.unique_id_offset], uid_size);

    static uint8_t transfer_id;

    canardBroadcast(&g_canard1,
                    UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_SIGNATURE,
                    UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    allocation_request,
                    uid_size + 1); /** node id(7 bit) + first_stage_flag(1 bit) + uid_size. */

    allocation_state.unique_id_offset = 0;
}

/**
 * @brief  处理动态分配节点 ID 的数据类型.
 * @param  ins, UAVCAN 实例
 * @param  transfer, 打包后的数据存储缓冲区
 * @retval None
 */
void handle_allocation_data_broadcast(CanardInstance* ins, CanardRxTransfer* transfer)
{
    /** 判断是否已经分配 ID. */
    if (!allocation_is_running())
    {
        return;
    }

    /** Always start the allocation request timer and reset the UID offset. */
    allocation_start_request_timer();

    /** 重置 UID offset. */
    allocation_state.unique_id_offset = 0;

    /** 去掉同类竞争对手. */
    if (transfer->source_node_id == CANARD_BROADCAST_NODE_ID)
    {
        /** If source node ID is anonymous, return. */
        return;
    }

    uint8_t received_unique_id[UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_UNIQUE_ID_MAX_LENGTH];

    const uint8_t received_unique_id_len = transfer->payload_len - 1;

    /** 获取 UID , 第一个字节为 node_id 和 first_stage_flag. */
    for (uint8_t i = 0; i < received_unique_id_len; i++)
    {
        canardDecodeScalar(transfer, i * 8 + UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_UID_BIT_OFFSET, 8, false, &received_unique_id[i]);
    }

    /** 与自己的 UID 比较, 是否匹配. */
    if(memcmp(node_unique_id, received_unique_id, received_unique_id_len) != 0)
    {
        /** If unique ID does not match, return. */
        return;
    }

    if (received_unique_id_len < UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_UNIQUE_ID_MAX_LENGTH)
    {
        /** Unique ID partially matches - set the UID offset and start the followup timer. */
        allocation_state.unique_id_offset = received_unique_id_len;

        allocation_start_follow_up_timer();
    }
    else
    {
        /** Complete match received. */
        uint8_t allocated_node_id = 0;

        /** 获取分配后的 node id. */
        canardDecodeScalar(transfer, 0, 7, false, &allocated_node_id);

        /** 设置分配后的 node id. */
        if (allocated_node_id != 0)
        {
            canardSetLocalNodeID(ins, allocated_node_id);
        }
    }
}
