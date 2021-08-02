#ifndef __uavcan_task_firmware_h
#define __uavcan_task_firmware_h

#include <stdint.h>
#include "canard.h"

#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID                 40
#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_NAME               "uavcan.protocol.file.BeginFirmwareUpdate"
#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_SIGNATURE          (0xB7D725DF72724126ULL)
#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_REQUEST_MAX_SIZE   BIT_LEN_TO_SIZE(1616)
#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_RESPONSE_MAX_SIZE	BIT_LEN_TO_SIZE(1031)

/**
 * @brief  固件更新请求.
 * @param  ins: The pointer of uavcan instance.
 * @param  transfer: The pointer of transfer buffer.
 * @retval None
 */
void handle_file_beginfirmwareupdate_request(CanardInstance* ins, CanardRxTransfer* transfer);

#endif
