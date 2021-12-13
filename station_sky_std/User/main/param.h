/*
 * param.h
 *
 *  Created on: Jun 4, 2020
 *      Author: fu
 */

#ifndef PARAM_H_
#define PARAM_H_

#include "GetSet.h"
#include "ExecuteOpcode.h"
//#include "main.h"

typedef enum
{
	param1, PARAM_TOTAL,
} param_e;

typedef struct uavcan_protocol_param
{
	uavcan_protocol_param_GetSetRequest param[PARAM_TOTAL];
	uavcan_protocol_param_GetSetResponse scope_value[PARAM_TOTAL];

}uavcan_protocol_param;


int8_t g_load_flash_param(void);
void g_handle_param_get_set(CanardInstance *ins, CanardRxTransfer *transfer);
void g_handle_param_execute(CanardInstance *ins, CanardRxTransfer *transfer);

extern uint8_t sk3_select;
extern uavcan_protocol_param_GetSetRequest param[PARAM_TOTAL];
//extern uint8_t sk3_select;
#endif /* PARAM_H_ */
