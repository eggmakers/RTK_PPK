/*
 * log.h
 *
 *  Created on: Jun 4, 2020
 *      Author: fu
 */

#ifndef LOG_H_
#define LOG_H_
#include "stm32f4xx.h"
#include "stdio.h"

void uavcan_log(uint8_t level_value, char *str, uint8_t len);

#define g_debug_log(format,...){\
				uint8_t len;\
				char msg[90];\
				len = snprintf(msg, 90, format, ##__VA_ARGS__);\
				uavcan_log(0,msg,len);\
		}
#define g_info_log(format,...){\
				uint8_t len;\
				char msg[90];\
				len = snprintf(msg, 90, format, ##__VA_ARGS__);\
				uavcan_log(1,msg,len);\
		}
#define g_warning_log(format,...){\
				uint8_t len;\
				char msg[90];\
				len = snprintf(msg, 90, format, ##__VA_ARGS__);\
				uavcan_log(2,msg,len);\
		}
#define g_error_log(format,...){\
				uint8_t len;\
				char msg[90];\
				len = snprintf(msg, 90, format, ##__VA_ARGS__);\
				uavcan_log(3,msg,len);\
		}

#endif /* LOG_H_ */
