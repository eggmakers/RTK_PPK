/*
 * param.c
 *
 *  Created on: Jun 4, 2020
 *      Author: fu
 */

#include "stm32f4xx.h"
//#include "stm32f4xx_conf.h"

#include "param.h"
#include "log.h"
#include "string.h"

uint8_t dyn_arr_buf[260];

uavcan_protocol_param_GetSetRequest param[PARAM_TOTAL];
uavcan_protocol_param_GetSetResponse scope_value[PARAM_TOTAL];

int8_t g_save_param_to_flash(uint8_t *data, uint8_t len);
int8_t g_erase_param_flash(void);

static void init_param(void)
{
	param[0].index = param1;
	param[0].name.data = (uint8_t*) "test_param1";
	param[0].name.len = sizeof("test_param1") - 1;
	param[0].value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
	param[0].value.integer_value = 200;
	
	scope_value[0].default_value.integer_value = 100;
	scope_value[0].min_value.integer_value = 0;
	scope_value[0].max_value.integer_value = 65536;

	scope_value[0].default_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
	scope_value[0].max_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
	scope_value[0].min_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
	

	param[1].index = param2;
	param[1].name.data = (uint8_t*) "test_param2";
	param[1].name.len = sizeof("test_param2") - 1;
	param[1].value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
	param[1].value.real_value = 3.14;
	
	scope_value[1].default_value.real_value = 9.99;
	scope_value[1].max_value.real_value = 1000.0;
	scope_value[1].min_value.real_value = 0.0;
	
	scope_value[1].default_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
	scope_value[1].max_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
	scope_value[1].min_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;


}

void g_handle_param_get_set(CanardInstance *ins, CanardRxTransfer *transfer)
{
	uint8_t *tmp = dyn_arr_buf;
	uint16_t index = 0;
	/*decode*/
	uavcan_protocol_param_GetSetRequest param_GetSetRequest;
	uavcan_protocol_param_GetSetRequest_decode(transfer, transfer->payload_len,
			&param_GetSetRequest, &tmp);
	index = param_GetSetRequest.index;
	/*如果是有效参数*/
	if (param_GetSetRequest.value.union_tag != UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY
			&& param_GetSetRequest.index < PARAM_TOTAL)
	{
		for (index = 0; index < PARAM_TOTAL; index++)
		{
			/*参数名称匹配，则修改参数值*/
			if (strncmp((const char*) param[index].name.data,
					(const char*) param_GetSetRequest.name.data,
					param[index].name.len) == 0)
			{
				memcpy(&(param[index].value), &(param_GetSetRequest.value),
						sizeof(param_GetSetRequest.value));
				break;
			}
		}
	}

	uint8_t msg_buf[UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_MAX_SIZE];
	uavcan_protocol_param_GetSetResponse param_GetSetResponse;
	if (index < PARAM_TOTAL)
	{
		//响应参数请求
		param_GetSetResponse.name.data = param[index].name.data;
		param_GetSetResponse.name.len = param[index].name.len;
		memcpy(&(param_GetSetResponse.value), &(param[index].value),sizeof(param_GetSetResponse.value));
		memcpy(&(param_GetSetResponse.min_value), &(scope_value[index].min_value), sizeof(param_GetSetResponse.min_value));
		memcpy(&(param_GetSetResponse.max_value), &(scope_value[index].max_value), sizeof(param_GetSetResponse.max_value));
		memcpy(&(param_GetSetResponse.default_value), &(scope_value[index].default_value), sizeof(param_GetSetResponse.default_value));
		
		param_GetSetResponse.default_value.union_tag = scope_value[index].default_value.union_tag;
		param_GetSetResponse.min_value.union_tag = scope_value[index].min_value.union_tag;
		param_GetSetResponse.max_value.union_tag = scope_value[index].max_value.union_tag;
	}
	else
	{
		param_GetSetResponse.name.data = "";
		param_GetSetResponse.name.len = 0;
		param_GetSetResponse.value.union_tag =
				UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
		param_GetSetResponse.default_value.union_tag =
				UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
		param_GetSetResponse.min_value.union_tag =
				UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
		param_GetSetResponse.max_value.union_tag =
				UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
	}
	uint32_t len = uavcan_protocol_param_GetSetResponse_encode(
			&param_GetSetResponse, msg_buf);
	canardRequestOrRespond(ins, transfer->source_node_id,
	UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE,
	UAVCAN_PROTOCOL_PARAM_GETSET_ID, &transfer->transfer_id, transfer->priority,
			CanardResponse, msg_buf, len);
}

void g_handle_param_execute(CanardInstance *ins, CanardRxTransfer *transfer)
{
	int ret = 0;
	/*decode*/
	uavcan_protocol_param_ExecuteOpcodeRequest param_ExecuteOpcodeRequest;
	uavcan_protocol_param_ExecuteOpcodeRequest_decode(transfer,
			transfer->payload_len, &param_ExecuteOpcodeRequest, NULL);
	if (param_ExecuteOpcodeRequest.opcode
			== UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_REQUEST_OPCODE_SAVE)
	{
		ret = g_save_param_to_flash((uint8_t*) param, sizeof(param));
	}
	else if (param_ExecuteOpcodeRequest.opcode
			== UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_REQUEST_OPCODE_ERASE)
	{
		ret = g_erase_param_flash();
	}
	/*
	 * response
	 */
	uavcan_protocol_param_ExecuteOpcodeResponse param_ExecuteOpcodeResponse;
	uint8_t msg_buf[UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_RESPONSE_MAX_SIZE];
	if (ret == 0)
	{
		param_ExecuteOpcodeResponse.argument = 0;
		param_ExecuteOpcodeResponse.ok = true;
	}
	else
	{
		param_ExecuteOpcodeResponse.argument = ret;
		param_ExecuteOpcodeResponse.ok = false;
	}
	uint32_t len = uavcan_protocol_param_ExecuteOpcodeResponse_encode(
			&param_ExecuteOpcodeResponse, msg_buf);
	canardRequestOrRespond(ins, transfer->source_node_id,
	UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE,
	UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID, &transfer->transfer_id,
			transfer->priority, CanardResponse, msg_buf, len);
}

/***************************************FLASH OPERATION******************************/
//bootloader 17KB, APP在64KB处
#define FLASH_START_ADD         0x0800C000   //Sector 3 0x0800C000 - 0x0800FFFF 16 Kbyte
#define PARAM_START_MAGIC_NUM   0x5678ABCD
#define PARAM_END_MAGIC_NUM     0xAAAABCDE
#define END_OF_FLASH            0x0800FFF0

static void find_last_param_addr(uint32_t *validAddr)
{
	uint16_t i = 0;
	uint32_t tmp;
//find PARAM_END_MAGIC_NUM from 0x080E0000 to end of Flash,step = 4 Byte
	for (i = 0; i <= (END_OF_FLASH - FLASH_START_ADD) / 4; i++)
	{
		tmp = *(uint32_t*) (FLASH_START_ADD + 4 * i);
		if (tmp == PARAM_START_MAGIC_NUM)
		{
			*validAddr = FLASH_START_ADD + 4 * i + 4;
		}
	}
}

static void find_empty_addr(uint32_t *validAddr)
{
	uint16_t i = 0;
	uint32_t tmp;
//find PARAM_END_MAGIC_NUM from 0x080E0000 to end of Flash,step = 4 Byte
	for (i = 0; i <= 127 * 1024 / 4; i++)
	{
		tmp = *(uint32_t*) (FLASH_START_ADD + 4 * i);
		if (tmp == PARAM_END_MAGIC_NUM)
		{
			*validAddr = FLASH_START_ADD + 4 * i + 4;
		}
	}
}

/*
 *
 */

int8_t g_save_param_to_flash(uint8_t *data, uint8_t len)
{
	int32_t i;
	HAL_StatusTypeDef status;
	uint32_t emptyAddress;

	emptyAddress = FLASH_START_ADD;
	find_empty_addr(&emptyAddress);
//if no EmptyAddr ,EraseSector
	__HAL_FLASH_CLEAR_FLAG(
			FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
					| FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	if ((emptyAddress == FLASH_START_ADD) || emptyAddress > END_OF_FLASH)
	{
		//can not find valid address, erase the flash
		HAL_FLASH_Unlock();
		FLASH_Erase_Sector( FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
		HAL_FLASH_Lock();

		emptyAddress = FLASH_START_ADD;
	}
// Program PARAM_START_MAGIC_NUM
	HAL_FLASH_Unlock();
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, emptyAddress, PARAM_START_MAGIC_NUM);
	if (status != HAL_OK)
	{
		g_warning_log("PARAM_START_MAGIC_NUM ERROR");
		HAL_FLASH_Lock();
		return -2;
	}

	for (i = 0; i < len / 4; i++)
	{
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, emptyAddress + 4 + 4 * i,
				*(uint32_t*) (data + 4 * i));
		if (status != HAL_OK)
		{
			g_warning_log("Program ERROR");
			HAL_FLASH_Lock();
			return -3;
		}
	}
	for (int j = 0; j < len % 4; j++)
	{
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, emptyAddress + 4 + 4 * i + j,
				*(data + 4 * i + j));
		if (status != HAL_OK)
		{
			g_warning_log("Program ERROR");
			HAL_FLASH_Lock();
			return -3;
		}
	}
// Program PARAM_END_MAGIC_NUM
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, emptyAddress + 8 + 4 * i, PARAM_END_MAGIC_NUM);
	if (status != HAL_OK)
	{
		g_warning_log("PARAM_END_MAGIC_NUM ERROR");
		HAL_FLASH_Lock();
		return -2;
	}

	HAL_FLASH_Lock();

	for (i = 0; i < len; i++)
	{
		if (*(uint8_t*) (emptyAddress + 4 + i) != *(data + i))
		{
			g_warning_log("Check ERROR");
			return -4;
		}
	}
	g_info_log("saveParamToFlash success");
	return 0;
}

int8_t g_load_flash_param(void)
{
	int32_t i;
	uint32_t LastParamAddress = 0;
	find_last_param_addr(&LastParamAddress);
	if (LastParamAddress == 0)
	{
		init_param();
		g_info_log("bootloader default param");
		return -1;
	}

	uint8_t *tmp = (uint8_t*) param;
	for (i = 0; i < sizeof(param); i++)
	{
		*tmp = *(uint8_t*) (LastParamAddress + i);
		tmp++;
	}
	g_info_log("LoadParam success");
	return 0;
}

int8_t g_erase_param_flash(void)
{

	HAL_StatusTypeDef status;
	__HAL_FLASH_CLEAR_FLAG(
			FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
					| FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	HAL_FLASH_Unlock();
	FLASH_Erase_Sector( FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
	if (status != HAL_OK)
	{
		g_warning_log("Erase sector error");
		return -1;
	}
	return 0;
}
