#ifndef __hw_crc_h
#define __hw_crc_h
#include <stdint.h>

/**
 * @brief  Hardware crc initialization.
 * @param  None
 * @retval None
 */
void crc_init(void);

/**
 * @brief  Hardware crc calculation.
 * @param  data: The pointer of data to be computed.
 * @param  len: data length(4 bytes).
 * @retval The result of computing.
 */
uint32_t hw_crc_calculate(uint32_t data[], uint32_t len);

#endif
