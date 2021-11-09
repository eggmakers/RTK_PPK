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
	param1, param2, PARAM_TOTAL,
} param_e;

int8_t g_load_flash_param(void);
void g_handle_param_get_set(CanardInstance *ins, CanardRxTransfer *transfer);
void g_handle_param_execute(CanardInstance *ins, CanardRxTransfer *transfer);

extern uavcan_protocol_param_GetSetRequest param[PARAM_TOTAL];
#endif /* PARAM_H_ */
