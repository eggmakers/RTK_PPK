#ifndef __UAVCAN_H
#define __UAVCAN_H
#include "canard.h"
#include "gnss.h"
#include "GnssEcef.h"
/**
 * @brief CAN bus initialization status.
 */
typedef enum
{
    CAN_BUS_OK = 0,     /** Everything is ok. */
    CAN_UNSUPPORTED_BR, /** Unsupported bit rate or supplied clock was incorrect. */
    CAN_CONFIG_ERR,     /** The CAN clock and pins was configured incorrectly. */
    CAN_BUS_ERR,        /** There are no other nodes in the bus or the CAN chip is broken. */
    CAN_INVALID_ARG,    /** Invalid arguments. */
    CAN_MAILBOX_FULL,   /** All transfer mailboxes are full. */
    CAN_RESULT_RESERVED = 0xFF,
} can_result_e;

/**
 * @brief Get CAN status.
 * @arg port, CAN1 or CAN2
 * @ret @see can_result_e
 *     CAN_BUS_OK,         Everything is ok.
 *     CAN_UNSUPPORTED_BR, Unsupported bit rate or supplied clock was incorrect.
 *     CAN_CONFIG_ERR,     The CAN clock and pins was configured incorrectly.
 *     CAN_BUS_ERR,        There are no other nodes in the bus or the CAN chip is broken.
 *     CAN_INVALID_ARG     Invalid arguments.
 */
can_result_e get_can_stat(can_port_e port);

/**
 * @brief 初始化 UAVCAN
 * @arg None
 * @ret None
 */
can_result_e uavcan_init(void);

/**
 * @brief 采用轮询的方式定时调用，进行数据的收发
 * @arg None
 * @ret None
 */
void uavcan_task(void);
void uavcan_estimate_task(void);
void pre_uavcan_task(void);
void uavcan_test(void);
void uavcan_pre_handle_rx(void);

#define PI 3.1415926

extern uint8_t SKmode_select;

#endif
