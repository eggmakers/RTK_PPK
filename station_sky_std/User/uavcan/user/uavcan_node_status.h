#ifndef __UAVCAN_NODE_STATUS_H
#define __UAVCAN_NODE_STATUS_H
#include <stdint.h>
#include "canard.h"

#define PROTOCOL_NODESTATUS_MAX_BROADCASTING_PERIOD_MS  1000
#define PROTOCOL_NODESTATUS_MIN_BROADCASTING_PERIOD_MS  2

#define UAVCAN_PROTOCOL_NODESTATUS_ID                   341
#define UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE            (0x0f0868d0c1a7c6f1)
#define UAVCAN_PROTOCOL_NODESTATUS_MESSAGE_SIZE         7

/** @defgroup NODE_STATUS_HEALTH_DEFINITION */
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK            0
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING       1
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_ERROR         2
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_CRITICAL      3

/** @defgroup NODE_STATUS_MODE_DEFINITION */
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL     0
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION  1
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_MAINTENANCE     2
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_SOFTWARE_UPDATE 3
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_OFFLINE         7

/**
 * @brief 节点状态
 */
typedef struct
{
    uint32_t uptime_sec;                  /** bit len 32 */
    uint8_t  health;                      /** bit len 2 */
    uint8_t  mode;                        /** bit len 3 */
    uint8_t  sub_mode;                    /** bit len 3 */
    uint16_t vendor_specific_status_code; /** bit len 16 */
} uavcan_protocol_NodeStatus;

/**
 * @brief 发送 GET_NODE_INFO 请求
 */
typedef struct
{
    uint8_t canx;  /** 两条 CAN 总线, 0 代表 CAN1 总线, 1 代表 CAN2 总线. */
    uint8_t id;    /** 需要发送 GET_NODE_INFO 请求的节点. */
    uint8_t retry; /** 重试次数(最大 10 次, 每次 500 左右 - 随机) */
} node_info_request_mark_t;

/**
  * @brief protocol_NodeStatus_encode
  * @arg source : Pointer to source data struct
  * @arg msg_buf: Pointer to msg storage
  * @arg offset: bit offset to msg storage
  * @ret returns message length as bytes
  */
uint16_t uavcan_protocol_NodeStatus_encode(uavcan_protocol_NodeStatus *source, void *msg_buf, uint16_t offset);

/**
  * @brief protocol_NodeStatus_decode
  * @arg transfer: Pointer to CanardRxTransfer transfer
  * @arg dest: Pointer to destination struct
  * @arg offset: bit offset to msg storage
  * @ret offset or ERROR value if < 0
  */
int32_t uavcan_protocol_NodeStatus_decode(const CanardRxTransfer* transfer, uavcan_protocol_NodeStatus* dest, int32_t offset);

/**
 * @brief 设置节点健康状况.
 * @arg health, 健康状态. @ref NODE_STATUS_HEALTH_DEFINITION
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_ERROR
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_CRITICAL
 * @ret None
 */
void uavcan_set_node_health(uint8_t health);

/**
 * @brief 获取节点健康状况.
 * @arg None
 * @ret 节点健康状态. @ref NODE_STATUS_HEALTH_DEFINITION
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_ERROR
 *      UAVCAN_PROTOCOL_NODESTATUS_HEALTH_CRITICAL
 */
uint8_t uavcan_get_node_health(void);

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
void uavcan_set_node_mode(uint8_t mode);

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
uint8_t uavcan_get_node_mode(void);

/**
 * @brief 获取节点的状态信息.
 * @arg None
 * @ret 指向保存节点状态信息的结构体.
 */
uavcan_protocol_NodeStatus* uavcan_get_node_status(void);

/**
 * @brief 心跳广播, 500ms 发送一次节点状态.
 * @arg port, CAN1 or CAN2
 * @ret None
 */
void uavcan_send_heartbeat(can_port_e port);

/**
  * @brief 初始化节点的状态.
  * @arg None
  * @ret None
  */
void uavcan_node_status_init(void);

#endif
