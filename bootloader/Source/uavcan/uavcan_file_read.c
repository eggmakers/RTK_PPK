#include "info.h"
#include "boot.h"
#include "time.h"
#include "flash.h"
#include "read_write.h"
#include "uavcan_file_read.h"

/** UAVCAN 实例 */
extern CanardInstance g_canard1;

extern fw_update_stat_t fw_update_stat;

/**
 * @brief  UAVCAN file read request.
 * @param  remote_node_id: Server node id
 * @param  offset: File offset.
 * @param  path: File path.
 * @retval Transfer ID.
 */
static uint8_t uavcan_send_file_read_request(uint8_t remote_node_id, const uint64_t offset, const char* path)
{
    uint8_t buffer[UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_RESPONSE_MAX_SIZE];

    /** 编码文件所在路径. */
    canardEncodeScalar(buffer, 0, 40, &offset);

    /** File path length. */
    const size_t path_len = strlen(path);

    memcpy(&buffer[5], path, path_len);

    const size_t total_size = path_len + 5;

    static uint8_t transfer_id = 0;

    /** Reading file transfer ID. */
    uint8_t read_file_transfer_id = transfer_id;

    canardRequestOrRespond(&g_canard1,
                           remote_node_id,
                           UAVCAN_PROTOCOL_FILE_READ_SIGNATURE,
                           UAVCAN_PROTOCOL_FILE_READ_ID,
                           &transfer_id,
                           CANARD_TRANSFER_PRIORITY_LOW,
                           CanardRequest,
                           buffer,
                           total_size);

    return read_file_transfer_id;
}

/**
 * @brief  Set reading file status.
 * @param  source_node_id: Server node ID.
 * @param  transfer_id: transfer ID.
 * @param  path: File path.
 * @param  path_len: File path length.
 * @retval None
 */
void set_flash_state(uint8_t source_node_id, uint8_t transfer_id, const uint8_t * path, uint8_t path_len)
{
    /** 初始化 flag state. */
    fw_update_stat.offset             = 0;
    fw_update_stat.last_erased_sector = get_sector(APP_START_ADDR) - 1;
    fw_update_stat.source_node_id     = source_node_id;
    fw_update_stat.transfer_id        = transfer_id;

    /** File path. */
    memcpy(fw_update_stat.path, path, path_len);

    fw_update_stat.read_file_req      = true;
    fw_update_stat.read_file          = true;
    fw_update_stat.timeout            = 1000;
    fw_update_stat.retried            = 3;
    fw_update_stat.tick               = millis();
}

/**
 * @brief  第一步: 处理更新固件请求.
 * @param  ins, UAVCAN instance.
 * @param  transfer: Pointer to CanardRxTransfer transfer
 * @retval None
 */
void handle_begin_firmware_update(CanardInstance* ins, CanardRxTransfer* transfer)
{
    /** 获取 source node id. */
    uint8_t source_node_id;

    canardDecodeScalar(transfer, 0, 8, false, &source_node_id);

    /** 获取 path 长度. */
    const uint8_t path_len = transfer->payload_len - 1;

    /** len + path centent. */
    char path[201];

    /** 获取 path 内容. */
    for(uint8_t i = 0; i < path_len; i++)
    {
        canardDecodeScalar(transfer, 8 + i * 8, 8, false, (uint8_t *)&path[i]);
    }
    path[path_len] = '\0';

    /** 响应请求. */
    uint8_t buffer[UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_RESPONSE_MAX_SIZE];

    const char* error_message = "go_to_update";
    const size_t error_message_len = strlen(error_message);

    buffer[0] = (uint8_t)UAVCAN_BEGINFIRMWAREUPDATE_ERROR_IN_PROGRESS;
    memcpy(&buffer[1], error_message, error_message_len);

    canardRequestOrRespond(ins,
                           transfer->source_node_id,
                           UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_SIGNATURE,
                           UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID,
                           &transfer->transfer_id,
                           transfer->priority,
                           CanardResponse,
                           buffer,
                           error_message_len + 1);

    /** 设置状态机. */
    set_flash_state(source_node_id, transfer->transfer_id, (const uint8_t *)path, path_len);
}

/**
 * @brief  处理读文件.
 * @param  ins, UAVCAN instance.
 * @param  transfer: Pointer to CanardRxTransfer transfer
 * @retval None
 */
void handle_file_read_response(CanardInstance* ins, CanardRxTransfer* transfer)
{
    /** 时刻需要判断更新源是否匹配. */
    if (transfer->transfer_id != fw_update_stat.transfer_id || transfer->source_node_id != fw_update_stat.source_node_id)
    {
        return;
    }

    /** 解码error code. */
    int16_t error;

    canardDecodeScalar(transfer, 0, 16, true, &error);

    /** payload_len - (CRC_H + CRC_L). */
    const size_t data_len = transfer->payload_len - 2;
    uint8_t data[256];

    for(uint16_t i = 0; i < data_len; i++)
    {
        canardDecodeScalar(transfer, 16 + i * 8, 8, false, &data[i]);
    }

    /** 处理接收到的数据(传入缓冲区). */
    file_write_to_flash(error, data, data_len, data_len < 256); /** 如果数据长度少于256, 则可以认为已经更新完成. */

    fw_update_stat.read_file_req = true;
}

/**
 * @brief  File read request.
 * @param  None
 * @retval True, if there is a reading request.
 */
static bool read_req(void)
{
    if (fw_update_stat.read_file_req == false)
    {
        if (fw_update_stat.read_file == true)
        {
            /**
             * @note In the stage of read file.
             */
            if (millis() - fw_update_stat.tick >= fw_update_stat.timeout)
            {
                if (fw_update_stat.retried != 0)
                {
                    /** Try again. */
                    fw_update_stat.retried--;
                    return true;
                }
                else
                {
                    /** Timeout, end to read. */
                    fw_update_stat.read_file = false;
                }
            }
        }
    }
    else
    {
        return true;
    }

    return false;
}

/**
 * @brief  更新读请求(继续读请求).
 * @param  None
 * @retval None
 */
void upgrade_read_request(void)
{
    if (read_req() == false)
    {
        return;
    }

    /** Make sure won't jump to application while update firmware. */
    bl_timeout_reset();

    fw_update_stat.transfer_id = uavcan_send_file_read_request(fw_update_stat.source_node_id, fw_update_stat.offset, fw_update_stat.path);
    fw_update_stat.read_file_req = false;
    fw_update_stat.retried = 3;
    fw_update_stat.tick = millis();
}
