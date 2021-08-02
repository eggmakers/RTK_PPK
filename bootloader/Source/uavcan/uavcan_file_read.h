#ifndef __uavcan_file_read_h
#define __uavcan_file_read_h
#include <stdint.h>
#include <string.h>
#include "canard.h"

#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID        40
#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_NAME      "uavcan.protocol.file.BeginFirmwareUpdate"
#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_SIGNATURE (0xB7D725DF72724126ULL)

#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_REQUEST_MAX_SIZE   BIT_LEN_TO_SIZE(1616)
#define UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_RESPONSE_MAX_SIZE	BIT_LEN_TO_SIZE(1031)

#define UAVCAN_PROTOCOL_FILE_READ_ID                       48
#define UAVCAN_PROTOCOL_FILE_READ_NAME                     "uavcan.protocol.file.Read"
#define UAVCAN_PROTOCOL_FILE_READ_SIGNATURE                (0x8DCDCA939F33F678ULL)
#define UAVCAN_PROTOCOL_FILE_READ_REQUEST_MAX_SIZE         BIT_LEN_TO_SIZE(1648)

/** firmware update 错误类型. */
#define UAVCAN_BEGINFIRMWAREUPDATE_ERROR_OK             0
#define UAVCAN_BEGINFIRMWAREUPDATE_ERROR_INVALID_MODE   1
#define UAVCAN_BEGINFIRMWAREUPDATE_ERROR_IN_PROGRESS    2
#define UAVCAN_BEGINFIRMWAREUPDATE_ERROR_UNKNOWN        255

typedef struct
{
    struct
    {
        uint8_t len;
        uint8_t data[200];
    } path;
} uavcan_protocol_file_Path;

typedef struct
{
    uint8_t source_node_id;
    uavcan_protocol_file_Path image_file_remote_path;
} uavcan_protocol_file_BeginFirmwareUpdateRequest;

typedef struct
{
    uint8_t error;

    struct
    {
        uint8_t len;
        uint8_t* data;
    } optional_error_message;
} uavcan_protocol_file_BeginFirmwareUpdateResponse;

typedef struct
{
    int16_t	value;
} uavcan_protocol_file_Error;

typedef struct
{
    uint64_t offset;
    uavcan_protocol_file_Path path;
} uavcan_protocol_file_ReadRequest;

typedef struct
{
    uavcan_protocol_file_Error error;

    struct
    {
        uint16_t len;
        uint8_t* data;
    } data;
} uavcan_protocol_file_ReadResponse;

/**
 * @brief  Set reading file status.
 * @param  source_node_id: Server node ID.
 * @param  transfer_id: transfer ID.
 * @param  path: File path.
 * @param  path_len: File path length.
 * @retval None
 */
void set_flash_state(uint8_t source_node_id, uint8_t transfer_id, const uint8_t * path, uint8_t path_len);

/**
 * @brief  第一步: 处理更新固件请求.
 * @param  ins, UAVCAN instance.
 * @param  transfer: Pointer to CanardRxTransfer transfer
 * @retval None
 */
void handle_begin_firmware_update(CanardInstance* ins, CanardRxTransfer* transfer);

/**
 * @brief  处理读文件.
 * @param  ins, UAVCAN instance.
 * @param  transfer: Pointer to CanardRxTransfer transfer
 * @retval None
 */
void handle_file_read_response(CanardInstance* ins, CanardRxTransfer* transfer);

/**
 * @brief  更新读请求(继续读请求).
 * @param  None
 * @retval None
 */
void upgrade_read_request(void);

#endif
