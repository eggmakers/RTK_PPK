#include "GnssEcef.h"
#include "canard.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GnssEcef.h"
#include "uavcan.h"
#include "Timestamp.h"
#include "RTCMStream.h"
#include "parse_rtcm.h"

#ifndef CANARD_INTERNAL_SATURATE
#define CANARD_INTERNAL_SATURATE(x, max) ( ((x) > max) ? max : ( (-(x) > max) ? (-max) : (x) ) );
#endif

#define CANARD_INTERNAL_ENABLE_TAO  ((uint8_t) 1)
#define CANARD_INTERNAL_DISABLE_TAO ((uint8_t) 0)


/** user code start*/
extern CanardInstance g_canard1;
static mmc_GnssEcef gnss_msg;

uint8_t rtk_data[UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_DATA_MAX_LENGTH];

/** user code end*/

/**
  * @brief mmc_GnssEcef_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t mmc_GnssEcef_encode_internal(mmc_GnssEcef* source, void* msg_buf, uint32_t offset, uint8_t root_item)
{
    uint32_t c = 0;

    canardEncodeScalar(msg_buf, offset, 32, (void*)&source->devid); // 4294967295
    offset += 32;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->mode); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->fix_type); // 255
    offset += 8;

    // Static array (ecef_pos)
    for (c = 0; c < 3; c++)
    {
        canardEncodeScalar(msg_buf, offset, 64, (void*)(source->ecef_pos + c)); // 9223372036854775807
        offset += 64;
    }

    // Static array (ecef_vel)
    for (c = 0; c < 3; c++)
    {
        canardEncodeScalar(msg_buf, offset, 32, (void*)(source->ecef_vel + c)); // 2147483647
        offset += 32;
    }

    // Static array (ecef_pos_std)
    for (c = 0; c < 3; c++)
    {
        canardEncodeScalar(msg_buf, offset, 32, (void*)(source->ecef_pos_std + c)); // 2147483647
        offset += 32;
    }

    // Static array (ecef_vel_std)
    for (c = 0; c < 3; c++)
    {
        canardEncodeScalar(msg_buf, offset, 32, (void*)(source->ecef_vel_std + c)); // 2147483647
        offset += 32;
    }

    canardEncodeScalar(msg_buf, offset, 32, (void*)&source->heading); // 2147483647
    offset += 32;

    canardEncodeScalar(msg_buf, offset, 32, (void*)&source->heading_std); // 2147483647
    offset += 32;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->pos_sat); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->head_sat); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->pos_type); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->pos_sol); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->vel_type); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->vel_sol); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->head_type); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->head_sol); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 16, (void*)&source->hdop); // 65535
    offset += 16;

    canardEncodeScalar(msg_buf, offset, 16, (void*)&source->vdop); // 65535
    offset += 16;

    // Compound
    offset = uavcan_Timestamp_encode_internal((void*)&source->utc, msg_buf, offset, 0);

    return offset;
}

/**
  * @brief mmc_GnssEcef_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t mmc_GnssEcef_encode(mmc_GnssEcef* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = mmc_GnssEcef_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief mmc_GnssEcef_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     mmc_GnssEcef dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @param tao: is tail array optimization used
  * @retval offset or ERROR value if < 0
  */
int32_t mmc_GnssEcef_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, mmc_GnssEcef* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao)
{
    int32_t ret = 0;
    uint32_t c = 0;

    ret = canardDecodeScalar(transfer, offset, 32, false, (void*)&dest->devid);
    if (ret != 32)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 32;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->mode);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->fix_type);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    // Static array (ecef_pos)
    for (c = 0; c < 3; c++)
    {
        ret = canardDecodeScalar(transfer, offset, 64, false, (void*)(dest->ecef_pos + c));
        if (ret != 64)
        {
            goto mmc_GnssEcef_error_exit;
        }
        offset += 64;
    }

    // Static array (ecef_vel)
    for (c = 0; c < 3; c++)
    {
        ret = canardDecodeScalar(transfer, offset, 32, false, (void*)(dest->ecef_vel + c));
        if (ret != 32)
        {
            goto mmc_GnssEcef_error_exit;
        }
        offset += 32;
    }

    // Static array (ecef_pos_std)
    for (c = 0; c < 3; c++)
    {
        ret = canardDecodeScalar(transfer, offset, 32, false, (void*)(dest->ecef_pos_std + c));
        if (ret != 32)
        {
            goto mmc_GnssEcef_error_exit;
        }
        offset += 32;
    }

    // Static array (ecef_vel_std)
    for (c = 0; c < 3; c++)
    {
        ret = canardDecodeScalar(transfer, offset, 32, false, (void*)(dest->ecef_vel_std + c));
        if (ret != 32)
        {
            goto mmc_GnssEcef_error_exit;
        }
        offset += 32;
    }

    ret = canardDecodeScalar(transfer, offset, 32, false, (void*)&dest->heading);
    if (ret != 32)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 32;

    ret = canardDecodeScalar(transfer, offset, 32, false, (void*)&dest->heading_std);
    if (ret != 32)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 32;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->pos_sat);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->head_sat);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->pos_type);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->pos_sol);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->vel_type);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->vel_sol);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->head_type);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->head_sol);
    if (ret != 8)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 16, false, (void*)&dest->hdop);
    if (ret != 16)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 16;

    ret = canardDecodeScalar(transfer, offset, 16, false, (void*)&dest->vdop);
    if (ret != 16)
    {
        goto mmc_GnssEcef_error_exit;
    }
    offset += 16;

    // Compound
    offset = uavcan_Timestamp_decode_internal(transfer, 0, (void*)&dest->utc, dyn_arr_buf, offset, tao);
    if (offset < 0)
    {
        ret = offset;
        goto mmc_GnssEcef_error_exit;
    }
    return offset;

mmc_GnssEcef_error_exit:
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
  * @brief mmc_GnssEcef_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     mmc_GnssEcef dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t mmc_GnssEcef_decode(const CanardRxTransfer* transfer, uint16_t payload_len, mmc_GnssEcef* dest, uint8_t** dyn_arr_buf)
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
        for (uint32_t c = 0; c < sizeof(mmc_GnssEcef); c++)
        {
            ((uint8_t*)dest)[c] = 0x00;
        }

        ret = mmc_GnssEcef_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset, tao);

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




/** user code start*/

static bool gnss_updated = false;

mmc_GnssEcef gnss_ecef =
{
    .devid = 86787,
    .mode = 1,
};

void update_heading(mmc_GnssEcef *gnss_ecef,const heading headingb)
{
    gnss_ecef->head_type = headingb.pos_type;
    gnss_ecef->head_sol = headingb.sol_stat;
    gnss_ecef->heading = headingb.heading;
    gnss_ecef->heading_std = headingb.hdg_std_dev;
    gnss_ecef->head_sat = headingb.soln_svs;
}

void update_bestxyz(mmc_GnssEcef *gnss_ecef,const bestxyz bestxyzb)
{
    //	0-1: no fix, 2: 2D fix, 3: 3D fix, 4: RTCM code differential
    //	5: Real-Time Kinematic, float
    //	6: Real-Time Kinematic, fixed,
    if(bestxyzb.pos_type < SOL_TYPE_SINGLE)
    {
        gnss_ecef->fix_type = 0;
    }
    else
    {
        gnss_ecef->fix_type = 3;
        if(bestxyzb.pos_type >= SOL_TYPE_NARROW_FLOAT)
        {
            gnss_ecef->fix_type = 5;
        }
        if(bestxyzb.pos_type >= SOL_TYPE_NARROW_INT)
        {
            gnss_ecef->fix_type = 6;
        }
    }

    gnss_ecef->pos_type = bestxyzb.pos_type;
    gnss_ecef->pos_sol = bestxyzb.pos_sol_stat;
    gnss_ecef->vel_type = bestxyzb.vel_type;
    gnss_ecef->vel_sol = bestxyzb.vel_sol_status;

    gnss_ecef->ecef_pos[0] = bestxyzb.pos_x;
    gnss_ecef->ecef_pos[1] = bestxyzb.pos_y;
    gnss_ecef->ecef_pos[2] = bestxyzb.pos_z;

    gnss_ecef->ecef_vel[0] = bestxyzb.vel_x;
    gnss_ecef->ecef_vel[1] = bestxyzb.vel_y;
    gnss_ecef->ecef_vel[2] = bestxyzb.vel_z;

    gnss_ecef->ecef_pos_std[0] = bestxyzb.pos_x_delta;
    gnss_ecef->ecef_pos_std[1] = bestxyzb.pos_y_delta;
    gnss_ecef->ecef_pos_std[2] = bestxyzb.pos_z_delta;

    gnss_ecef->ecef_vel_std[0] = bestxyzb.vel_x_delta;
    gnss_ecef->ecef_vel_std[1] = bestxyzb.vel_y_delta;
    gnss_ecef->ecef_vel_std[2] = bestxyzb.vel_z_delta;

    gnss_ecef->pos_sat = bestxyzb.soln_svs;

    gnss_ecef->hdop = ( fabsf( bestxyzb.pos_x_delta ) + fabsf( bestxyzb.pos_y_delta )  ) * 100;
    gnss_ecef->vdop = fabsf( bestxyzb.pos_z_delta ) * 100;

    gnss_updated = true;
}
void update_psrdop2(mmc_GnssEcef *gnss_ecef,const psrdop2 psrdop2b)
{
//    gnss_ecef->hdop = (uint16_t)(psrdop2b.hdop * 100);
//    gnss_ecef->vdop = (uint16_t)(psrdop2b.vdop * 100);
}

void update_timestamp(mmc_GnssEcef *gnss_ecef,const nova_time timeb)
{
    static const uint8_t month_day[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    uint64_t usec;
    usec = 0;
    for(uint16_t year = 1970; year < timeb.utc_year; year++)
    {
        usec +=365;
        if((year % 4 == 0 && year%100 != 0) || (year % 400 == 0))
        {
            usec ++;
        }
    }
    for(uint8_t month = 1; month < timeb.utc_month; month++)
    {
        usec += month_day[month - 1];
        if(month == 2 && ((timeb.utc_year % 4 == 0 && timeb.utc_year%100 != 0) || (timeb.utc_year % 400 == 0)))
        {
            usec ++;
        }
    }
    usec += timeb.utc_day - 1;     // 得到所有天数
    usec *= 24;                    // 化为小时单位
    usec += timeb.utc_hour;        // 加上一天中剩余的小时数
    usec *= 60;                    // 化为分钟单位
    usec += timeb.utc_min;         // 加上一天中剩余的分钟数
    usec *= 60000;                 // 化为毫秒单位
    usec += timeb.utc_ms;          // 加上一天中剩余的毫秒数
    usec *= 1000;                  // 化为微秒单位

    gnss_ecef->utc.usec = usec;
}

/**
* @brief  外部获取utc_time结构体变量的接口，采用指针传递，减少结构体作为参数传递时的数据复制需要的时间
* @param  None
* @retval 指向utc_time变量的指针
  */
//utc_time_t* get_utc_time(void)
//{
//    return &utc_time;
//}

bool get_gnss_updated(void)
{
    return gnss_updated;
}

void set_gnss_updated(void)
{
    gnss_updated = false;
}

mmc_GnssEcef *get_gnss_ecef(void)
{
    return &gnss_ecef;
}


void send_gnss(void)
{
#if 1
	static uint32_t last_tick = 0;
	static uint32_t gnss_update_err_cnt = 0;

    if(get_gnss_updated() == true)
    {
    	last_tick = HAL_GetTick();
    	uint8_t gns_buffer[MMC_GNSSECEF_MAX_SIZE];
        gnss_msg = *get_gnss_ecef();
        const uint16_t offset = mmc_GnssEcef_encode(&gnss_msg,gns_buffer);
        static uint8_t transfer_id = 0;
        const int16_t bc_res = canardBroadcast(&g_canard1,
                                               MMC_GNSSECEF_SIGNATURE,
                                               MMC_GNSSECEF_ID,
                                               &transfer_id,
											   CANARD_TRANSFER_PRIORITY_MEDIUM,
											   gns_buffer,
                                               offset);
        if(bc_res <= 0)
        {
            // TODO
            // handle error
        }
        set_gnss_updated();
    }
    else
    {
    	if( HAL_GetTick()-last_tick >= 1000)
    	{
    		last_tick = HAL_GetTick();
    		printf(">>>>>>gnss_update_err_cnt:%d\r\n",gnss_update_err_cnt++);
    	}
    }
#endif
}


/** user code end*/


