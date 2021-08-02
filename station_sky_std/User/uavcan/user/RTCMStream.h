/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: D:\UAVCANread\uavcan移植教程及参考示例\dsdl_compiler\uavcan\equipment\gnss\1062.RTCMStream.uavcan
 */

#ifndef __UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM
#define __UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# GNSS RTCM SC-104 protocol raw stream container.
# RTCM messages that are longer than max data size can be split over multiple consecutive messages.
#

uint8 PROTOCOL_ID_UNKNOWN = 0
uint8 PROTOCOL_ID_RTCM2   = 2
uint8 PROTOCOL_ID_RTCM3   = 3
uint8 protocol_id

uint8[<=256] data
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.equipment.gnss.RTCMStream
saturated uint8 protocol_id
saturated uint8[<=256] data
******************************************************************************/

#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_ID                1062
#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_NAME              "uavcan.equipment.gnss.RTCMStream"
#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_SIGNATURE         (0x1F56030ECB171501ULL)
//(0x1F56030ECB171501ULL)
#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_MAX_SIZE         	((1040 + 7)/8) 
//((1040 + 7)/8) ((2065 + 7)/8)
// Constants
#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_PROTOCOL_ID_UNKNOWN                  0 // 0
#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_PROTOCOL_ID_RTCM2                    2 // 2
#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_PROTOCOL_ID_RTCM3                    3 // 3

//#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_DATA_MAX_LENGTH                                 256
#define UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_DATA_MAX_LENGTH                                 	128

typedef struct
{
    // FieldTypes
    uint8_t    protocol_id;                   // bit len 8
    struct
    {
        uint16_t    len;                       // Dynamic array length
        uint8_t*   data;                      // Dynamic Array 8bit[256] max items
    } data;

} uavcan_equipment_gnss_RTCMStream;

extern
uint32_t uavcan_equipment_gnss_RTCMStream_encode(uavcan_equipment_gnss_RTCMStream* source, void* msg_buf);

extern
int32_t uavcan_equipment_gnss_RTCMStream_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_equipment_gnss_RTCMStream* dest, uint8_t** dyn_arr_buf);

extern
uint32_t uavcan_equipment_gnss_RTCMStream_encode_internal(uavcan_equipment_gnss_RTCMStream* source, void* msg_buf, uint32_t offset, uint8_t root_item);

extern
int32_t uavcan_equipment_gnss_RTCMStream_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_equipment_gnss_RTCMStream* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM

void handle_rtcm(CanardRxTransfer *transfer);

