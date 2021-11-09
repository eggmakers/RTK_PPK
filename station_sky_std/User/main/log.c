/*
 * log.c
 *
 *  Created on: Jun 4, 2020
 *      Author: fu
 */
//#include "main.h"
#include "LogMessage.h"
#include "uavcan.h"
#include "log.h"
#define SRC  "PayloadConnector"
void uavcan_log(uint8_t level_value, char *str, uint8_t str_len)
{
//	uint8_t msg_buf[UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_MAX_SIZE];
//	uavcan_protocol_debug_LogMessage msg;
//	msg.level.value = level_value;
//	msg.source.data = (uint8_t*) SRC;
//	msg.source.len = sizeof(SRC) - 1;

//	msg.text.data = (uint8_t*) str;
//	msg.text.len = str_len;
//	uint32_t len = uavcan_protocol_debug_LogMessage_encode(&msg, msg_buf);

//	static uint8_t transfer_id = 0;
//	canardBroadcast(&uavcan, UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_SIGNATURE,
//	UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_ID, &transfer_id,
//	CANARD_TRANSFER_PRIORITY_HIGHEST, msg_buf, len);
}
