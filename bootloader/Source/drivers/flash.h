#ifndef __flash_h
#define __flash_h
#include <stdbool.h>
#include "io.h"

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t get_sector(uint32_t address);

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
uint32_t get_sector_size(uint32_t sector);

/**
 * @brief  Erase flash page.
 * @param  page_addr: The start address of flash page to be erased.
 * @retval Operation status.
 *     true:  Operation successful.
 *     false: Operation failes.
 */
bool flash_erase_page(uint32_t page_addr);

/**
 * @brief  Write data into flash.
 * @param  addr: The start address of flash to be written.
 * @param  data[]: The start address of data need to write into flash.
 * @param  len: The data length.
 * @retval Operation status.
 *     true:  Operation successful.
 *     false: Operation failes.
 */
bool flash_write(uint32_t addr, const uint8_t data[], uint16_t len);

/**
 * @brief  Read data from flash.
 * @param  addr: The start address of flash to be read.
 * @param  data[]: The start address of read buffer.
 * @param  len: The data length need to read.
 * @retval Operation status.
 *     true:  Operation successful.
 *     false: Operation failes.
 */
bool flash_read(uint32_t addr, const uint8_t *data, uint16_t len);

#endif
