#include "flash.h"

/** Base address of the Flash sectors Bank 1. */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /** Base @ of Sector 0, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /** Base @ of Sector 1, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /** Base @ of Sector 2, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /** Base @ of Sector 3, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /** Base @ of Sector 4, 64 Kbytes. */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /** Base @ of Sector 5, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /** Base @ of Sector 6, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /** Base @ of Sector 7, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /** Base @ of Sector 8, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /** Base @ of Sector 9, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /** Base @ of Sector 10, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /** Base @ of Sector 11, 128 Kbytes. */

/** Base address of the Flash sectors Bank 2. */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x08100000) /** Base @ of Sector 0, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_13    ((uint32_t)0x08104000) /** Base @ of Sector 1, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08108000) /** Base @ of Sector 2, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_15    ((uint32_t)0x0810C000) /** Base @ of Sector 3, 16 Kbytes. */
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x08110000) /** Base @ of Sector 4, 64 Kbytes. */
#define ADDR_FLASH_SECTOR_17    ((uint32_t)0x08120000) /** Base @ of Sector 5, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_18    ((uint32_t)0x08140000) /** Base @ of Sector 6, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_19    ((uint32_t)0x08160000) /** Base @ of Sector 7, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_20    ((uint32_t)0x08180000) /** Base @ of Sector 8, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_21    ((uint32_t)0x081A0000) /** Base @ of Sector 9, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_22    ((uint32_t)0x081C0000) /** Base @ of Sector 10, 128 Kbytes. */
#define ADDR_FLASH_SECTOR_23    ((uint32_t)0x081E0000) /** Base @ of Sector 11, 128 Kbytes. */

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t get_sector(uint32_t address)
{
    uint32_t sector = 0;

    if((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;
    }
    else if((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;
    }
    else if((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;
    }
    else if((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;
    }
    else if((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;
    }
    else if((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;
    }
    else if((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;
    }
    else if((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_SECTOR_7;
    }
    else if((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_SECTOR_8;
    }
    else if((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_SECTOR_9;
    }
    else if((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_SECTOR_10;
    }
    else if((address < ADDR_FLASH_SECTOR_12) && (address >= ADDR_FLASH_SECTOR_11))
    {
        sector = FLASH_SECTOR_11;
    }
//    else if((address < ADDR_FLASH_SECTOR_13) && (address >= ADDR_FLASH_SECTOR_12))
//    {
//        sector = FLASH_SECTOR_12;
//    }
//    else if((address < ADDR_FLASH_SECTOR_14) && (address >= ADDR_FLASH_SECTOR_13))
//    {
//        sector = FLASH_SECTOR_13;
//    }
//    else if((address < ADDR_FLASH_SECTOR_15) && (address >= ADDR_FLASH_SECTOR_14))
//    {
//        sector = FLASH_SECTOR_14;
//    }
//    else if((address < ADDR_FLASH_SECTOR_16) && (address >= ADDR_FLASH_SECTOR_15))
//    {
//        sector = FLASH_SECTOR_15;
//    }
//    else if((address < ADDR_FLASH_SECTOR_17) && (address >= ADDR_FLASH_SECTOR_16))
//    {
//        sector = FLASH_SECTOR_16;
//    }
//    else if((address < ADDR_FLASH_SECTOR_18) && (address >= ADDR_FLASH_SECTOR_17))
//    {
//        sector = FLASH_SECTOR_17;
//    }
//    else if((address < ADDR_FLASH_SECTOR_19) && (address >= ADDR_FLASH_SECTOR_18))
//    {
//        sector = FLASH_SECTOR_18;
//    }
//    else if((address < ADDR_FLASH_SECTOR_20) && (address >= ADDR_FLASH_SECTOR_19))
//    {
//        sector = FLASH_SECTOR_19;
//    }
//    else if((address < ADDR_FLASH_SECTOR_21) && (address >= ADDR_FLASH_SECTOR_20))
//    {
//        sector = FLASH_SECTOR_20;
//    }
//    else if((address < ADDR_FLASH_SECTOR_22) && (address >= ADDR_FLASH_SECTOR_21))
//    {
//        sector = FLASH_SECTOR_21;
//    }
//    else if((address < ADDR_FLASH_SECTOR_23) && (address >= ADDR_FLASH_SECTOR_22))
//    {
//        sector = FLASH_SECTOR_22;
//    }
//    else /** (address < FLASH_END_ADDR) && (address >= ADDR_FLASH_SECTOR_23) */
//    {
//        sector = FLASH_SECTOR_23;
//    }
    return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
uint32_t get_sector_size(uint32_t sector)
{
    uint32_t sectorsize = 0x00;

    if((sector == FLASH_SECTOR_0) || (sector == FLASH_SECTOR_1) || (sector == FLASH_SECTOR_2) ||\
            (sector == FLASH_SECTOR_3) )
    {
        sectorsize = 16 * 1024;
    }
    else if(sector == FLASH_SECTOR_4)
    {
        sectorsize = 64 * 1024;
    }
    else
    {
        sectorsize = 128 * 1024;
    }
    return sectorsize;
}

/**
 * @brief  Erase flash page.
 * @param  page_addr: The start address of flash page to be erased.
 * @retval Operation status.
 *     true:  Operation successful.
 *     false: Operation failes.
 */
bool flash_erase_page(uint32_t sector)
{
    /** Unlock flash. */
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        return false;
    }

    FLASH_EraseInitTypeDef FLASH_EraseInitType;

    FLASH_EraseInitType.TypeErase    = FLASH_TYPEERASE_SECTORS; /** Erase sector operation. */
    FLASH_EraseInitType.VoltageRange = FLASH_VOLTAGE_RANGE_3;   /** Device operating range: 2.7V to 3.6V. */
    FLASH_EraseInitType.Sector       = sector;                  /** Sector start address. */
    FLASH_EraseInitType.NbSectors    = 1;                       /** Erase one section. */

    /** Receive the error address while doing reasing operation. */
    uint32_t page_error_addr = 0;

    /** Erase. */
    if (HAL_FLASHEx_Erase(&FLASH_EraseInitType, &page_error_addr) != HAL_OK)
    {
        return false;
    }

    /** Lock flash. */
    if (HAL_FLASH_Lock() != HAL_OK)
    {
        return false;
    }

    return true;
}

/**
 * @brief  Write data into flash.
 * @param  addr: The start address of flash to be written.
 * @param  data[]: The start address of data need to write into flash.
 * @param  len: The data length.
 * @retval Operation status.
 *     true:  Operation successful.
 *     false: Operation failes.
 */
bool flash_write(uint32_t addr, const uint8_t data[], uint16_t len)
{
    /** Unlock flash. */
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        return false;
    }

    for (uint16_t i = 0; i < len; i += 2)
    {
        /** Write half-word. */
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, *((uint16_t *)(&data[i])));
    }

    /** Lock flash. */
    if (HAL_FLASH_Lock() != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
 * @brief  Read data from flash.
 * @param  addr: The start address of flash to be read.
 * @param  data[]: The start address of read buffer.
 * @param  len: The data length need to read.
 * @retval Operation status.
 *     true:  Operation successful.
 *     false: Operation failes.
 */
bool flash_read(uint32_t addr, const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i += 2)
    {
        *(uint16_t *)&data[i] = *(__IO uint16_t*)(addr + i);
    }

    return HAL_OK;
}
