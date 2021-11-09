/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: D:\UAVCANread\uavcan移植教程及参考示例\dsdl_compiler\uavcan\protocol\param\Empty.uavcan
 */

#ifndef __UAVCAN_PROTOCOL_PARAM_EMPTY
#define __UAVCAN_PROTOCOL_PARAM_EMPTY

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# Ex nihilo nihil fit.
#
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.protocol.param.Empty
******************************************************************************/

#define UAVCAN_PROTOCOL_PARAM_EMPTY_NAME                   "uavcan.protocol.param.Empty"
#define UAVCAN_PROTOCOL_PARAM_EMPTY_SIGNATURE              (0x6C4D0E8EF37361DFULL)

#define UAVCAN_PROTOCOL_PARAM_EMPTY_MAX_SIZE               ((0 + 7)/8)

typedef struct
{
    uint8_t empty;
} uavcan_protocol_param_Empty;

extern
uint32_t uavcan_protocol_param_Empty_encode(uavcan_protocol_param_Empty* source, void* msg_buf);

extern
int32_t uavcan_protocol_param_Empty_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_param_Empty* dest, uint8_t** dyn_arr_buf);

extern
uint32_t uavcan_protocol_param_Empty_encode_internal(uavcan_protocol_param_Empty* source, void* msg_buf, uint32_t offset, uint8_t root_item);

extern
int32_t uavcan_protocol_param_Empty_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_param_Empty* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_PROTOCOL_PARAM_EMPTY

