/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: D:\UAVCANread\uavcan移植教程及参考示例\dsdl_compiler\uavcan\equipment\gnss\1062.RTCMStream.uavcan
 */
#include "RTCMStream.h"
#include "canard.h"
#include "parse_rtcm.h"


#ifndef CANARD_INTERNAL_SATURATE
#define CANARD_INTERNAL_SATURATE(x, max) ( ((x) > max) ? max : ( (-(x) > max) ? (-max) : (x) ) );
#endif

#define CANARD_INTERNAL_ENABLE_TAO  ((uint8_t) 1)
#define CANARD_INTERNAL_DISABLE_TAO ((uint8_t) 0)

/**
  * @brief uavcan_equipment_gnss_RTCMStream_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t uavcan_equipment_gnss_RTCMStream_encode_internal(uavcan_equipment_gnss_RTCMStream* source, void* msg_buf, uint32_t offset, uint8_t root_item)
{
    uint32_t c = 0;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->protocol_id); // 255
    offset += 8;

    // Dynamic Array (data)
    if (! root_item)
    {
        // - Add array length
        canardEncodeScalar(msg_buf, offset, 9, (void*)&source->data.len);
        offset += 9;
    }

    // - Add array items
    for (c = 0; c < source->data.len; c++)
    {
        canardEncodeScalar(msg_buf, offset, 8, (void*)(source->data.data + c));// 255
        offset += 8;
    }

    return offset;
}

/**
  * @brief uavcan_equipment_gnss_RTCMStream_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t uavcan_equipment_gnss_RTCMStream_encode(uavcan_equipment_gnss_RTCMStream* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = uavcan_equipment_gnss_RTCMStream_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief uavcan_equipment_gnss_RTCMStream_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_equipment_gnss_RTCMStream dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @param tao: is tail array optimization used
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_equipment_gnss_RTCMStream_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_equipment_gnss_RTCMStream* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao)
{
    int32_t ret = 0;
    uint32_t c = 0;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->protocol_id);
    if (ret != 8)
    {
        goto uavcan_equipment_gnss_RTCMStream_error_exit;
    }
    offset += 8;

    // Dynamic Array (data)
    //  - Last item in struct & Root item & (Array Size > 8 bit), tail array optimization
    if (payload_len && tao == CANARD_INTERNAL_ENABLE_TAO)
    {
        //  - Calculate Array length from MSG length
        dest->data.len = ((payload_len * 8) - offset ) / 8; // 8 bit array item size
    }
    else
    {
        // - Array length 9 bits
        ret = canardDecodeScalar(transfer, offset, 9, false, (void*)&dest->data.len); // 255
        if (ret != 9)
        {
            goto uavcan_equipment_gnss_RTCMStream_error_exit;
        }
        offset += 9;
    }

    //  - Get Array
    if (dyn_arr_buf)
    {
        dest->data.data = (uint8_t*)*dyn_arr_buf;
    }

    for (c = 0; c < dest->data.len; c++)
    {
        if (dyn_arr_buf)
        {
            ret = canardDecodeScalar(transfer, offset, 8, false, (void*)*dyn_arr_buf); // 255
            if (ret != 8)
            {
                goto uavcan_equipment_gnss_RTCMStream_error_exit;
            }
            *dyn_arr_buf = (uint8_t*)(((uint8_t*)*dyn_arr_buf) + 1);
        }
        offset += 8;
    }
    return offset;

uavcan_equipment_gnss_RTCMStream_error_exit:
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return -CANARD_ERROR_INTERNAL;
    }
}

/**
  * @brief uavcan_equipment_gnss_RTCMStream_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_equipment_gnss_RTCMStream dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_equipment_gnss_RTCMStream_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_equipment_gnss_RTCMStream* dest, uint8_t** dyn_arr_buf)
{
    const int32_t offset = 0;
    int32_t ret = 0;

    /* Backward compatibility support for removing TAO
     *  - first try to decode with TAO DISABLED
     *  - if it fails fall back to TAO ENABLED
     */
    uint8_t tao = CANARD_INTERNAL_DISABLE_TAO;

    while (1)
    {
        // Clear the destination struct
        for (uint32_t c = 0; c < sizeof(uavcan_equipment_gnss_RTCMStream); c++)
        {
            ((uint8_t*)dest)[c] = 0x00;
        }

        ret = uavcan_equipment_gnss_RTCMStream_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset, tao);

        if (ret >= 0)
        {
            break;
        }

        if (tao == CANARD_INTERNAL_ENABLE_TAO)
        {
            break;
        }
        tao = CANARD_INTERNAL_ENABLE_TAO;
    }

    return ret;
}


void handle_rtcm(CanardRxTransfer *transfer)
{
	uint8_t protocol_id = 0;
	uint8_t rtcm_len = 0;
	static uint8_t rtcm_buf[UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_DATA_MAX_LENGTH];
	
	/** 解码protocol_id,现在用的rtcm数据协议都是V3版本的 */
	canardDecodeScalar(transfer, 0, 8, false,&protocol_id);
	
	/** 按字节解码rtcm数据,transfer->payload_len-1,是因为前面的第一个字节是protocol_id,第二个开始才是rtcm数据 */
	for(uint16_t i = 0;i < transfer->payload_len - 1; i++)
	{
		canardDecodeScalar(transfer, 8+8*i, 8, false,&rtcm_buf[i]);
	}
	
    parse_rtcm_data(&rtcm_buf[0], transfer->payload_len - 2);
}

