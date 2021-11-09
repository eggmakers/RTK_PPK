#ifndef __sw_crc_h
#define __sw_crc_h
#include <stdint.h>

/**
 * @brief  Software crc32 calculating.
 * @param  crc: Default crc value.
 * @param  data: The pointer of data to be computed.
 * @param  len: data length(1 bytes).
 * @retval The result of computing.
 */
uint32_t sw_crc32_calculate(uint32_t crc, uint8_t data[], uint32_t len);

#endif
