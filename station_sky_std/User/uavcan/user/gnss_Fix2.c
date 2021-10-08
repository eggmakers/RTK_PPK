/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: D:\WorkProject\uavcan\github\public_regulated_data_types\uavcan\equipment\gnss\1063.Fix2.uavcan
 */
#include "Fix2.h"
#include "canard.h"
#include "uavcan.h"

/** UAVCAN 实例 */
extern CanardInstance g_canard1;
uavcan_equipment_gnss_Fix2 fix2;


#ifndef CANARD_INTERNAL_SATURATE
#define CANARD_INTERNAL_SATURATE(x, max) ( ((x) > max) ? max : ( (-(x) > max) ? (-max) : (x) ) );
#endif

#define CANARD_INTERNAL_ENABLE_TAO  ((uint8_t) 1)
#define CANARD_INTERNAL_DISABLE_TAO ((uint8_t) 0)

/**
  * @brief uavcan_equipment_gnss_Fix2_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t uavcan_equipment_gnss_Fix2_encode_internal(uavcan_equipment_gnss_Fix2* source, void* msg_buf, uint32_t offset, uint8_t root_item)
{
    uint32_t c = 0;
#ifndef CANARD_USE_FLOAT16_CAST
    uint16_t tmp_float = 0;
#else
    CANARD_USE_FLOAT16_CAST tmp_float = 0;
#endif

    // Compound
    offset = uavcan_Timestamp_encode_internal((void*)&source->timestamp, msg_buf, offset, 0);

    // Compound
    offset = uavcan_Timestamp_encode_internal((void*)&source->gnss_timestamp, msg_buf, offset, 0);
    source->gnss_time_standard = CANARD_INTERNAL_SATURATE(source->gnss_time_standard, 7)
    canardEncodeScalar(msg_buf, offset, 3, (void*)&source->gnss_time_standard); // 7
    offset += 3;

    // Void13
    offset += 13;
    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->num_leap_seconds); // 255
    offset += 8;

    source->longitude_deg_1e8 = CANARD_INTERNAL_SATURATE(source->longitude_deg_1e8, 68719476735)
    canardEncodeScalar(msg_buf, offset, 37, (void*)&source->longitude_deg_1e8); // 68719476735
    offset += 37;

    source->latitude_deg_1e8 = CANARD_INTERNAL_SATURATE(source->latitude_deg_1e8, 68719476735)
    canardEncodeScalar(msg_buf, offset, 37, (void*)&source->latitude_deg_1e8); // 68719476735
    offset += 37;

    source->height_ellipsoid_mm = CANARD_INTERNAL_SATURATE(source->height_ellipsoid_mm, 67108863)
    canardEncodeScalar(msg_buf, offset, 27, (void*)&source->height_ellipsoid_mm); // 67108863
    offset += 27;

    source->height_msl_mm = CANARD_INTERNAL_SATURATE(source->height_msl_mm, 67108863)
    canardEncodeScalar(msg_buf, offset, 27, (void*)&source->height_msl_mm); // 67108863
    offset += 27;

    // Static array (ned_velocity)
    for (c = 0; c < 3; c++)
    {
        canardEncodeScalar(msg_buf, offset, 32, (void*)(source->ned_velocity + c)); // 2147483647
        offset += 32;
    }

    source->sats_used = CANARD_INTERNAL_SATURATE(source->sats_used, 63)
    canardEncodeScalar(msg_buf, offset, 6, (void*)&source->sats_used); // 63
    offset += 6;

    source->status = CANARD_INTERNAL_SATURATE(source->status, 3)
    canardEncodeScalar(msg_buf, offset, 2, (void*)&source->status); // 3
    offset += 2;

    source->mode = CANARD_INTERNAL_SATURATE(source->mode, 15)
    canardEncodeScalar(msg_buf, offset, 4, (void*)&source->mode); // 15
    offset += 4;

    source->sub_mode = CANARD_INTERNAL_SATURATE(source->sub_mode, 63)
    canardEncodeScalar(msg_buf, offset, 6, (void*)&source->sub_mode); // 63
    offset += 6;

    // Dynamic Array (covariance)
    // - Add array length
    canardEncodeScalar(msg_buf, offset, 6, (void*)&source->covariance.len);
    offset += 6;

    // - Add array items
    for (c = 0; c < source->covariance.len; c++)
    {
        canardEncodeScalar(msg_buf, offset, 16, (void*)(source->covariance.data + c));// 32767
        offset += 16;
    }

    // float16 special handling
#ifndef CANARD_USE_FLOAT16_CAST
    tmp_float = canardConvertNativeFloatToFloat16(source->pdop);
#else
    tmp_float = (CANARD_USE_FLOAT16_CAST)source->pdop;
#endif
    canardEncodeScalar(msg_buf, offset, 16, (void*)&tmp_float); // 32767
    offset += 16;

    // Dynamic Array (ecef_position_velocity)
    if (! root_item)
    {
        // - Add array length
        canardEncodeScalar(msg_buf, offset, 1, (void*)&source->ecef_position_velocity.len);
        offset += 1;
    }

    // - Add array items
    for (c = 0; c < source->ecef_position_velocity.len; c++)
    {
        offset += uavcan_equipment_gnss_ECEFPositionVelocity_encode_internal((void*)&source->ecef_position_velocity.data[c], msg_buf, offset, 0);
    }

    return offset;
}

/**
  * @brief uavcan_equipment_gnss_Fix2_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t uavcan_equipment_gnss_Fix2_encode(uavcan_equipment_gnss_Fix2* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = uavcan_equipment_gnss_Fix2_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief uavcan_equipment_gnss_Fix2_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_equipment_gnss_Fix2 dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @param tao: is tail array optimization used
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_equipment_gnss_Fix2_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_equipment_gnss_Fix2* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao)
{
    int32_t ret = 0;
    uint32_t c = 0;
#ifndef CANARD_USE_FLOAT16_CAST
    uint16_t tmp_float = 0;
#else
    CANARD_USE_FLOAT16_CAST tmp_float = 0;
#endif

    // Compound
    offset = uavcan_Timestamp_decode_internal(transfer, 0, (void*)&dest->timestamp, dyn_arr_buf, offset, tao);
    if (offset < 0)
    {
        ret = offset;
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }

    // Compound
    offset = uavcan_Timestamp_decode_internal(transfer, 0, (void*)&dest->gnss_timestamp, dyn_arr_buf, offset, tao);
    if (offset < 0)
    {
        ret = offset;
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }

    ret = canardDecodeScalar(transfer, offset, 3, false, (void*)&dest->gnss_time_standard);
    if (ret != 3)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 3;

    // Void13
    offset += 13;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->num_leap_seconds);
    if (ret != 8)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 37, true, (void*)&dest->longitude_deg_1e8);
    if (ret != 37)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 37;

    ret = canardDecodeScalar(transfer, offset, 37, true, (void*)&dest->latitude_deg_1e8);
    if (ret != 37)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 37;

    ret = canardDecodeScalar(transfer, offset, 27, true, (void*)&dest->height_ellipsoid_mm);
    if (ret != 27)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 27;

    ret = canardDecodeScalar(transfer, offset, 27, true, (void*)&dest->height_msl_mm);
    if (ret != 27)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 27;

    // Static array (ned_velocity)
    for (c = 0; c < 3; c++)
    {
        ret = canardDecodeScalar(transfer, offset, 32, false, (void*)(dest->ned_velocity + c));
        if (ret != 32)
        {
            goto uavcan_equipment_gnss_Fix2_error_exit;
        }
        offset += 32;
    }

    ret = canardDecodeScalar(transfer, offset, 6, false, (void*)&dest->sats_used);
    if (ret != 6)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 6;

    ret = canardDecodeScalar(transfer, offset, 2, false, (void*)&dest->status);
    if (ret != 2)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 2;

    ret = canardDecodeScalar(transfer, offset, 4, false, (void*)&dest->mode);
    if (ret != 4)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 4;

    ret = canardDecodeScalar(transfer, offset, 6, false, (void*)&dest->sub_mode);
    if (ret != 6)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 6;

    // Dynamic Array (covariance)
    //  - Array length, not last item 6 bits
    ret = canardDecodeScalar(transfer, offset, 6, false, (void*)&dest->covariance.len); // 32767
    if (ret != 6)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
    offset += 6;

    //  - Get Array
    if (dyn_arr_buf)
    {
        dest->covariance.data = (float*)*dyn_arr_buf;
    }

    for (c = 0; c < dest->covariance.len; c++)
    {
        if (dyn_arr_buf)
        {
            ret = canardDecodeScalar(transfer, offset, 16, false, (void*)*dyn_arr_buf); // 32767
            if (ret != 16)
            {
                goto uavcan_equipment_gnss_Fix2_error_exit;
            }
            *dyn_arr_buf = (uint8_t*)(((float*)*dyn_arr_buf) + 1);
        }
        offset += 16;
    }

    // float16 special handling
    ret = canardDecodeScalar(transfer, offset, 16, false, (void*)&tmp_float);

    if (ret != 16)
    {
        goto uavcan_equipment_gnss_Fix2_error_exit;
    }
#ifndef CANARD_USE_FLOAT16_CAST
    dest->pdop = canardConvertFloat16ToNativeFloat(tmp_float);
#else
    dest->pdop = (float)tmp_float;
#endif
    offset += 16;

    // Dynamic Array (ecef_position_velocity)
    //  - Last item in struct & Root item & (Array Size > 8 bit), tail array optimization
    if (payload_len && tao == CANARD_INTERNAL_ENABLE_TAO)
    {
        //  - Calculate Array length from MSG length
        dest->ecef_position_velocity.len = ((payload_len * 8) - offset ) / 792; // 792 bit array item size
    }
    else
    {
        // - Array length 1 bits
        ret = canardDecodeScalar(transfer, offset, 1, false, (void*)&dest->ecef_position_velocity.len); // 0
        if (ret != 1)
        {
            goto uavcan_equipment_gnss_Fix2_error_exit;
        }
        offset += 1;
    }

    //  - Get Array
    if (dyn_arr_buf)
    {
        dest->ecef_position_velocity.data = (uavcan_equipment_gnss_ECEFPositionVelocity*)*dyn_arr_buf;
    }

    for (c = 0; c < dest->ecef_position_velocity.len; c++)
    {
        offset += uavcan_equipment_gnss_ECEFPositionVelocity_decode_internal(transfer, 0, (void*)&dest->ecef_position_velocity.data[c], dyn_arr_buf, offset, tao);
    }
    return offset;

uavcan_equipment_gnss_Fix2_error_exit:
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
  * @brief uavcan_equipment_gnss_Fix2_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_equipment_gnss_Fix2 dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_equipment_gnss_Fix2_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_equipment_gnss_Fix2* dest, uint8_t** dyn_arr_buf)
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
        for (uint32_t c = 0; c < sizeof(uavcan_equipment_gnss_Fix2); c++)
        {
            ((uint8_t*)dest)[c] = 0x00;
        }

        ret = uavcan_equipment_gnss_Fix2_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset, tao);

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

bool update_fix2_data(nova_msg_parser *fix2_data)
{
    if(NULL == fix2_data)
    {
        return false;
    }
    
    fix2.gnss_timestamp.usec = fix2_data->header.nova_headeru.tow * 1000; // 毫秒转微秒  
    fix2.gnss_time_standard = UAVCAN_EQUIPMENT_GNSS_FIX2_GNSS_TIME_STANDARD_GPS; //时间标准 
    fix2.num_leap_seconds = UAVCAN_EQUIPMENT_GNSS_FIX2_NUM_LEAP_SECONDS_UNKNOWN; //无闰秒  
    
    fix2.longitude_deg_1e8 = fix2_data->data.bestposu.lng * 1e8;
    fix2.latitude_deg_1e8 = fix2_data->data.bestposu.lat * 1e8;
	
    fix2.height_msl_mm = fix2_data->data.bestposu.hgt * 100; //m 转 mm
    fix2.height_ellipsoid_mm = (fix2_data->data.bestposu.hgt * 100) + (fix2_data->data.bestposu.undulation * 100);	// wgs84 = hgt - undulation
	
	
	fix2.status = fix2_data->data.bestposu.postype;	
	fix2.sats_used = fix2_data->data.bestposu.svsused;
    
    if(fix2_data->data.bestposu.postype > 16 && fix2_data->data.bestposu.postype <= 34)
    {
        fix2.mode = UAVCAN_EQUIPMENT_GNSS_FIX2_MODE_SINGLE;
        fix2.sub_mode = UAVCAN_EQUIPMENT_GNSS_FIX2_SUB_MODE_RTK_FLOAT;
    }
    else if(fix2_data->data.bestposu.postype > 34 && fix2_data->data.bestposu.postype <= 48)
    {
        fix2.mode = UAVCAN_EQUIPMENT_GNSS_FIX2_MODE_RTK;
        fix2.sub_mode = UAVCAN_EQUIPMENT_GNSS_FIX2_SUB_MODE_RTK_FLOAT;
    }
    else if(fix2_data->data.bestposu.postype > 48)
    {
        fix2.mode = UAVCAN_EQUIPMENT_GNSS_FIX2_MODE_RTK;
        fix2.sub_mode = UAVCAN_EQUIPMENT_GNSS_FIX2_SUB_MODE_RTK_FIXED;
    }
			    
	
	
    return true;
}

bool update_fix2_bestxyz(nova_msg_parser *fix2_data)
{			
	
	
	fix2.covariance.len = 6;
	
    fix2.covariance.data[0] = canardConvertNativeFloatToFloat16(fix2_data->data.bestxyzb.pos_x_delta);
    fix2.covariance.data[1] = canardConvertNativeFloatToFloat16(fix2_data->data.bestxyzb.pos_y_delta);
    fix2.covariance.data[2] = canardConvertNativeFloatToFloat16(fix2_data->data.bestxyzb.pos_z_delta);
	
    fix2.covariance.data[3] = canardConvertNativeFloatToFloat16(fix2_data->data.bestxyzb.vel_x_delta);
    fix2.covariance.data[4] = canardConvertNativeFloatToFloat16(fix2_data->data.bestxyzb.vel_y_delta);
    fix2.covariance.data[5] = canardConvertNativeFloatToFloat16(fix2_data->data.bestxyzb.vel_z_delta);
	
	
	fix2.ecef_position_velocity.data->position_xyz_mm[0] = fix2_data->data.bestxyzb.pos_x * 1000;
	fix2.ecef_position_velocity.data->position_xyz_mm[1] = fix2_data->data.bestxyzb.pos_y * 1000;
	fix2.ecef_position_velocity.data->position_xyz_mm[2] = fix2_data->data.bestxyzb.pos_z * 1000;
		
	fix2.ecef_position_velocity.data->velocity_xyz[0] = fix2_data->data.bestxyzb.vel_x;
	fix2.ecef_position_velocity.data->velocity_xyz[1] = fix2_data->data.bestxyzb.vel_y;
	fix2.ecef_position_velocity.data->velocity_xyz[2] = fix2_data->data.bestxyzb.vel_z;
	
	
	
	
	return true;
}

bool update_fix2_bestvel(nova_msg_parser *fix2_data)
{
	fix2.ned_velocity[0] = fix2_data->data.bestvelu.horspd;
	fix2.ned_velocity[1] = fix2_data->data.bestvelu.trkgnd;
	fix2.ned_velocity[1] = fix2_data->data.bestvelu.vertspd;
	
	return true;
}

bool update_fix2_psrdop(nova_msg_parser *fix2_data)
{
	fix2.pdop = fix2_data->data.psrdop.pdop;
//	
//	
//	fix2.covariance.len = 6;
//    fix2.covariance.data[0] = fix2_data->data.bestxyzb.pos_x_delta;
//    fix2.covariance.data[1] = fix2_data->data.bestxyzb.pos_y_delta;
//    fix2.covariance.data[2] = fix2_data->data.bestxyzb.pos_z_delta;
//    fix2.covariance.data[3] = fix2_data->data.bestxyzb.vel_x_delta;
//    fix2.covariance.data[4] = fix2_data->data.bestxyzb.vel_y_delta;
//    fix2.covariance.data[5] = fix2_data->data.bestxyzb.vel_z_delta;
	
	
	
	
   
//	fix2.ecef_position_velocity.len = 1;

//	fix2.ecef_position_velocity.data->covariance.len = 6;

//	fix2.ecef_position_velocity.data->covariance.data[0] = fix2_data->data.psrdop.pdop;
//	fix2.ecef_position_velocity.data->covariance.data[1] = fix2_data->data.psrdop.hdop;
//	fix2.ecef_position_velocity.data->covariance.data[2] = fix2_data->data.psrdop.pdop;
//	fix2.ecef_position_velocity.data->covariance.data[3] = fix2_data->data.psrdop.hdop;
//	fix2.ecef_position_velocity.data->covariance.data[4] = fix2_data->data.psrdop.pdop;
//	fix2.ecef_position_velocity.data->covariance.data[5] = fix2_data->data.psrdop.hdop;
	
	
	
	return true;
}

/**
   * @brief 发送fix2信息
   * @param
   * @param
   *
   * @return
*/
void send_fix2(void)
{
    if(is_update_fix2() == true)
    {        
        uint8_t buf[UAVCAN_EQUIPMENT_GNSS_FIX2_MAX_SIZE];
        const uint16_t offset = uavcan_equipment_gnss_Fix2_encode(&fix2,buf);
        static uint8_t transferid = 0;
        
        int16_t res =  canardBroadcast(&g_canard1,
                                        UAVCAN_EQUIPMENT_GNSS_FIX2_SIGNATURE,
                                        UAVCAN_EQUIPMENT_GNSS_FIX2_ID,
                                        &transferid,
                                        CANARD_TRANSFER_PRIORITY_MEDIUM,
                                        buf,
                                        offset);
        if(res < 0)
        {
            /**error */            
        }
        set_status_fix2();
    }
}

float covariance_data[6];
uavcan_equipment_gnss_ECEFPositionVelocity gnss_ECEFPositionVelocity;

void g_init_fix2_struct(void)
{
	fix2.covariance.data = &(covariance_data[0]);
	
//	fix2.ecef_position_velocity.data = &gnss_ECEFPositionVelocity;
//	gnss_ECEFPositionVelocity.covariance.data = covariance_data;
}

