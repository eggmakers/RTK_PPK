#ifndef __read_write_h
#define __read_write_h

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/**
 * @note The structure of firmware update status.
 */
typedef struct
{
    bool read_file_req;         /** Read file request. */
    bool read_file;             /** Read file request. */
    uint32_t tick;              /** The time tick of the last read file request. */
    uint16_t timeout;           /** Waiting responce timeout interval. */
    uint8_t retried;            /** Timeout retried. */
    uint32_t offset;            /** Read file offset. */
    uint8_t transfer_id;        /** Message transfer ID. */
    uint8_t source_node_id;     /** File service ID. */
    int32_t last_erased_sector; /** Last erased sector. */
    char path[201];             /** File path. */
} fw_update_stat_t;

/**
 * @brief  处理接收到的数据.
 * @param  error:
 * @param  data: 需要写入的数据的地址.
 * @param  data_len: 需要写入的数据长度.
 * @param  finished:
 * @retval None
 */
void file_write_to_flash(int16_t error, const uint8_t* data, uint16_t data_len, bool finished);

#endif
