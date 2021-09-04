#include "gpio.h"
#include "parse_rtcm.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;

static enum {
	RTCM_HEAD = 0,
	RTCM_RESERVE,
	RTCM_LENGTH,
	RTCM_DATA,
	RTCM_CRC,
}rtcm_state;

const int table[] =
{
	0x000000, 0x864CFB, 0x8AD50D, 0x0C99F6, 0x93E6E1, 0x15AA1A, 0x1933EC, 0x9F7F17,
	0xA18139, 0x27CDC2, 0x2B5434, 0xAD18CF, 0x3267D8, 0xB42B23, 0xB8B2D5, 0x3EFE2E, 0xC54E89, 0x430272,
	0x4F9B84, 0xC9D77F, 0x56A868, 0xD0E493, 0xDC7D65, 0x5A319E, 0x64CFB0, 0xE2834B, 0xEE1ABD, 0x685646,
	0xF72951, 0x7165AA, 0x7DFC5C, 0xFBB0A7, 0x0CD1E9, 0x8A9D12, 0x8604E4, 0x00481F, 0x9F3708, 0x197BF3,
	0x15E205, 0x93AEFE, 0xAD50D0, 0x2B1C2B, 0x2785DD, 0xA1C926, 0x3EB631, 0xB8FACA, 0xB4633C, 0x322FC7,
	0xC99F60, 0x4FD39B, 0x434A6D, 0xC50696, 0x5A7981, 0xDC357A, 0xD0AC8C, 0x56E077, 0x681E59, 0xEE52A2,
	0xE2CB54, 0x6487AF, 0xFBF8B8, 0x7DB443, 0x712DB5, 0xF7614E, 0x19A3D2, 0x9FEF29, 0x9376DF, 0x153A24,
	0x8A4533, 0x0C09C8, 0x00903E, 0x86DCC5, 0xB822EB, 0x3E6E10, 0x32F7E6, 0xB4BB1D, 0x2BC40A, 0xAD88F1,
	0xA11107, 0x275DFC, 0xDCED5B, 0x5AA1A0, 0x563856, 0xD074AD, 0x4F0BBA, 0xC94741, 0xC5DEB7, 0x43924C,
	0x7D6C62, 0xFB2099, 0xF7B96F, 0x71F594, 0xEE8A83, 0x68C678, 0x645F8E, 0xE21375, 0x15723B, 0x933EC0,
	0x9FA736, 0x19EBCD, 0x8694DA, 0x00D821, 0x0C41D7, 0x8A0D2C, 0xB4F302, 0x32BFF9, 0x3E260F, 0xB86AF4,
	0x2715E3, 0xA15918, 0xADC0EE, 0x2B8C15, 0xD03CB2, 0x567049, 0x5AE9BF, 0xDCA544, 0x43DA53, 0xC596A8,
	0xC90F5E, 0x4F43A5, 0x71BD8B, 0xF7F170, 0xFB6886, 0x7D247D, 0xE25B6A, 0x641791, 0x688E67, 0xEEC29C,
	0x3347A4, 0xB50B5F, 0xB992A9, 0x3FDE52, 0xA0A145, 0x26EDBE, 0x2A7448, 0xAC38B3, 0x92C69D, 0x148A66,
	0x181390, 0x9E5F6B, 0x01207C, 0x876C87, 0x8BF571, 0x0DB98A, 0xF6092D, 0x7045D6, 0x7CDC20, 0xFA90DB,
	0x65EFCC, 0xE3A337, 0xEF3AC1, 0x69763A, 0x578814, 0xD1C4EF, 0xDD5D19, 0x5B11E2, 0xC46EF5, 0x42220E,
	0x4EBBF8, 0xC8F703, 0x3F964D, 0xB9DAB6, 0xB54340, 0x330FBB, 0xAC70AC, 0x2A3C57, 0x26A5A1, 0xA0E95A,
	0x9E1774, 0x185B8F, 0x14C279, 0x928E82, 0x0DF195, 0x8BBD6E, 0x872498, 0x016863, 0xFAD8C4, 0x7C943F,
	0x700DC9, 0xF64132, 0x693E25, 0xEF72DE, 0xE3EB28, 0x65A7D3, 0x5B59FD, 0xDD1506, 0xD18CF0, 0x57C00B,
	0xC8BF1C, 0x4EF3E7, 0x426A11, 0xC426EA, 0x2AE476, 0xACA88D, 0xA0317B, 0x267D80, 0xB90297, 0x3F4E6C,
	0x33D79A, 0xB59B61, 0x8B654F, 0x0D29B4, 0x01B042, 0x87FCB9, 0x1883AE, 0x9ECF55, 0x9256A3, 0x141A58,
	0xEFAAFF, 0x69E604, 0x657FF2, 0xE33309, 0x7C4C1E, 0xFA00E5, 0xF69913, 0x70D5E8, 0x4E2BC6, 0xC8673D,
	0xC4FECB, 0x42B230, 0xDDCD27, 0x5B81DC, 0x57182A, 0xD154D1, 0x26359F, 0xA07964, 0xACE092, 0x2AAC69,
	0xB5D37E, 0x339F85, 0x3F0673, 0xB94A88, 0x87B4A6, 0x01F85D, 0x0D61AB, 0x8B2D50, 0x145247, 0x921EBC,
	0x9E874A, 0x18CBB1, 0xE37B16, 0x6537ED, 0x69AE1B, 0xEFE2E0, 0x709DF7, 0xF6D10C, 0xFA48FA, 0x7C0401,
	0x42FA2F, 0xC4B6D4, 0xC82F22, 0x4E63D9, 0xD11CCE, 0x575035, 0x5BC9C3, 0xDD8538 };


int get_crc24(uint8_t data[], uint16_t data_len)
{
	int crc_res = 0;
	uint16_t i = 0;

	for (i = 0; i< data_len; i++)
	{
		crc_res = ((crc_res << 8) & 0xFFFFFF) ^ table[(crc_res >> 16) ^ (data[i] & 0xFF)];
	}

	return crc_res;
}

/**
* @brief  rtcm消息帧解析状态机
* @param  None
* @retval true:完成消息读取并校验,flase：还未接收完成或校验不通过
  */

/*******************************************************************************
rtcmv3数据包,其数据帧格式如下：
   -------------------------------------------------------------------
   |     head     |   reserved   |  length  |   userdata   |   crc   |
   |--------------|--------------|--------- |--------------|---------|
   | 0xD3(8bit)   |   0(6bit)    |  10bit   | 0--1023 Byte |  3 Byte |
   -------------------------------------------------------------------
说明：
	1. head为数据包的包头0XD3;

	2. reserved为预留的字段,内容是6bit的0;

	3. length 为userdata字段的有效负载长度,仅仅只是userdata的长度

	4. crc 为校验和，校验的内容从包头到userdata

*******************************************************************************/
static uint16_t  parse_rtk_data_ok_cnt = 0;
static uint16_t  send_rtk_data_ok_cnt = 0;
static HAL_StatusTypeDef ret = HAL_ERROR;

static bool rtcm_parse(uint8_t temp)
{
	static uint16_t  data_cnt = 0;
	static uint16_t  data_len = 0;
	static uint8_t   send_buf[1024]={0};
	bool   status = false;
	int  crc_num = 0;
	int  crc_num1 = 0;

	switch (rtcm_state)
	{
	
	case RTCM_HEAD:
		data_cnt = 0;
		data_len = 0;
		if (temp == 0XD3)
		{
			send_buf[data_cnt++] = temp;
			rtcm_state = RTCM_RESERVE;
		}
		break;

	case RTCM_RESERVE:
		if (temp >> 2 == 0x00)
		{
			rtcm_state = RTCM_LENGTH;
			// 后2个bit是长度信息
			data_len = temp;
			send_buf[data_cnt++] = temp;
		}
		/* reserved field 6 bit */
		else
			rtcm_state = RTCM_HEAD;
		break;

	case RTCM_LENGTH:
		send_buf[data_cnt++] = temp;
		data_len = ((uint16_t)data_len << 8) + temp;
		if(data_len <= 0)
		{
			rtcm_state = RTCM_HEAD;
		}
		else
		{
			rtcm_state = RTCM_DATA;
		}

		break;

	case RTCM_DATA:
		send_buf[(data_cnt++)] = temp;
		if (data_cnt >= data_len)
			rtcm_state = RTCM_CRC;
		break;

	case RTCM_CRC:
		send_buf[(data_cnt++)] = temp;
		// userdata字段的后3个字节是CRC校验,注意data_len是从userdata开始计数的
		if (data_cnt >= (data_len+6))
		{
			// rtcm V3 协议的CRC校验是从包头到userdata的,所以,长度是data_len+3
			crc_num = get_crc24(&send_buf[0], data_len + 3);
			crc_num1 = (int)(send_buf[data_len + 5])
				+ (int)(send_buf[data_len + 4] << 8)
				+ (int)(send_buf[data_len + 3] << 16);
			// 校验成功
			if (crc_num == crc_num1)
			{
				if( ((parse_rtk_data_ok_cnt++)%20) ==0 )
				{
					/** 每接收并且解析成功20次的rtcm数据 */
					printf("\r\n>>>>> parse_rtk_data_ok = %d <<<<<\r\n",parse_rtk_data_ok_cnt);
				}
				
				/** 等待串口可用 */ 
				while((huart2.gState != HAL_UART_STATE_READY));
				
				/** 发送地面端的rtcm数据到板卡 */ 
				ret = HAL_UART_Transmit_DMA(&huart2, &send_buf[0], data_cnt);
				if(ret == HAL_OK)
				{
					if( ((send_rtk_data_ok_cnt++)%20) ==0 )
					{
						printf("\r\n>>>>> send_rtk_data_ok = %d <<<<<\r\n",send_rtk_data_ok_cnt);
					}
				}
			}
			rtcm_state = RTCM_HEAD;
		}
		break;

	default:
		break;
	}
	// CRC检验通过,status = 1;
	return status;
}


/**
* @brief  处理rtk接收到的DMA数据,解析数据
* @param data 需要处理的数据缓冲
* @param data_len 数据缓冲的长度
* @retval None
  */
void parse_rtcm_data(uint8_t data_buff[], uint16_t data_len)
{
	for (uint16_t index = 0; index < data_len; index++)
	{
		if(rtcm_parse(data_buff[index]) == true)
		{
			/** do something */
			printf("rtcm parse done\r\n");
		}
	}
}

