#ifndef _HANDLE_RTK_DATA_H__
#define _HANDLE_RTK_DATA_H__

#include  <stdbool.h>
#include  "stm32f4xx.h"

void parse_rtcm_data(uint8_t data_buff[], uint16_t data_len);

#endif
