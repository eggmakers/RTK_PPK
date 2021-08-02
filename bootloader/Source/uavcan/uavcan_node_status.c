#include <stdbool.h>
#include "time.h"
#include "uavcan_node_status.h"

/** UAVCAN 实例 */
extern CanardInstance g_canard1;
extern CanardInstance g_canard2;

/** 存储节点信息 */
static uavcan_protocol_NodeStatus node_stat;

/**
  * @brief protocol_NodeStatus_encode
  * @arg source : Pointer to source data struct
  * @arg msg_buf: Pointer to msg storage
  * @arg offset: bit offset to msg storage
  * @ret returns message length as bytes
  */
uint16_t uavcan_protocol_NodeStatus_encode(uavcan_protocol_NodeStatus *source, void *msg_buf, uint16_t offset)
{
    canardEncodeScalar(msg_buf, offset, 32, (void*)&source->uptime_sec);
    offset += 32;

    canardEncodeScalar(msg_buf, offset, 2, (void*)&source->health);
    offset += 2;

    canardEncodeScalar(msg_buf, offset, 3, (void*)&source->mode);
    offset += 3;

    canardEncodeScalar(msg_buf, offset, 3, (void*)&source->sub_mode);
    offset += 3;

    canardEncodeScalar(msg_buf, offset, 16, (void*)&source->vendor_specific_status_code);
    offset += 16;

    return offset;
}

/**
  * @brief protocol_NodeStatus_decode
  * @arg transfer: Pointer to CanardRxTransfer transfer
  * @arg dest: Pointer to destination struct
  * @arg offset: bit offset to msg storage
  * @ret offset or ERROR value if < 0
  */
int32_t uavcan_protocol_NodeStatus_decode(const CanardRxTransfer* transfer, uavcan_protocol_NodeStatus* dest, int32_t offset)
{
    int32_t ret = 0;

    ret = canardDecodeScalar(transfer, offset, 32, false, (void*)&dest->uptime_sec);
    if (ret != 32)
    {
        goto protocol_NodeStatus_error_exit;
    }
    offset += 32;

    ret = canardDecodeScalar(transfer, offset, 2, false, (void*)&dest->health);
    if (ret != 2)
    {
        goto protocol_NodeStatus_error_exit;
    }
    offset += 2;

    ret = canardDecodeScalar(transfer, offset, 3, false, (void*)&dest->mode);
    if (ret != 3)
    {
        goto protocol_NodeStatus_error_exit;
    }
    offset += 3;

    ret = canardDecodeScalar(transfer, offset, 3, false, (void*)&dest->sub_mode);
    if (ret != 3)
    {
        goto protocol_NodeStatus_error_exit;
    }
    offset += 3;

    ret = canardDecodeScalar(transfer, offset, 16, false, (void*)&dest->vendor_specific_status_code);
    if (ret != 16)
    {
        goto protocol_NodeStatus_error_exit;
    }
    offset += 16;
    return offset;

protocol_NodeStatus_error_exit:
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return -CANARD_ERROR_INTERNAL;
    }
}

/**
 * @brief 设置节点健康状况.
 * @arg health, 健康状态. @ref NODE_STATUS_HEALTH_DEFINITION
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_ERROR
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_CRITICAL
 * @ret None
 */
void uavcan_set_node_health(uint8_t health)
{
    node_stat.health = health;
}

/**
 * @brief 获取节点健康状况.
 * @arg None
 * @ret 节点健康状态. @ref NODE_STATUS_HEALTH_DEFINITION
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_ERROR
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_CRITICAL
 */
uint8_t uavcan_get_node_health(void)
{
    return node_stat.health;
}

/**
 * @brief 设置节点运行模式.
 * @arg mode, 模式. @ref NODE_STATUS_MODE_DEFINITION
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_MAINTENANCE
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_SOFTWARE_UPDATE
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_OFFLINE
 * @ret None
 */
void uavcan_set_node_mode(uint8_t mode)
{
    node_stat.mode = mode;
}

/**
 * @brief 获取节点运行模式.
 * @arg None
 * @ret 节点模式. @ref NODE_STATUS_MODE_DEFINITION
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_MAINTENANCE
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_SOFTWARE_UPDATE
 *      UAVCAN_PROTOCOL_NODESTATUS_MODE_OFFLINE
 */
uint8_t uavcan_get_node_mode(void)
{
    return node_stat.mode;
}

/**
 * @brief 获取节点的状态信息.
 * @arg None
 * @ret 指向保存节点状态信息的结构体.
 */
uavcan_protocol_NodeStatus* uavcan_get_node_status(void)
{
    return &node_stat;
}

/**
 * @brief 心跳广播, 500ms 发送一次节点状态.
 * @arg port, CAN1 or CAN2
 * @ret None
 */
void uavcan_send_heartbeat(can_port_e port)
{
#define CANARD_SPIN_PERIOD 500
    CanardInstance* ins;

    static uint32_t spin_time1 = 0;
    static uint32_t spin_time2 = 0;

    uint32_t* spin_time = NULL;
    /** Get UAVCAN instance. */
    if (port == CAN_P1)
    {
        ins = &g_canard1;
        spin_time = &spin_time1;
    }
    else if (port == CAN_P2)
    {
        ins = &g_canard2;
        spin_time = &spin_time2;
    }

    /** 检查是否达到 500ms */
    if(millis() < *spin_time + CANARD_SPIN_PERIOD)
    {
        return;
    }

    /** 获取当前的时间 */
    *spin_time = millis();

    uint8_t buffer[UAVCAN_PROTOCOL_NODESTATUS_MESSAGE_SIZE];

    node_stat.uptime_sec = sys_boot_time();
    const uint16_t offset = uavcan_protocol_NodeStatus_encode(&node_stat, buffer, 0);

    /** Note that the transfer ID variable MUST BE STATIC (or heap-allocated)! */
    static uint8_t transfer_id = 0;
    const int16_t bc_res = canardBroadcast(ins,
                                           UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                                           UAVCAN_PROTOCOL_NODESTATUS_ID,
                                           &transfer_id,
                                           CANARD_TRANSFER_PRIORITY_HIGH,
                                           buffer,
                                           BIT_LEN_TO_SIZE(offset));
    if (bc_res <= 0)
    {

    }
}

/**
  * @brief 初始化节点的状态.
  * @arg None
  * @ret None
  */
void uavcan_node_status_init(void)
{
    node_stat.uptime_sec = sys_boot_time();
    node_stat.health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
    node_stat.mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL;
    node_stat.sub_mode = 0;
    node_stat.vendor_specific_status_code = 1;
}
