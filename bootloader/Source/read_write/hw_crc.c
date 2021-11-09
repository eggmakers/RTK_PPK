#include "io.h"
#include "hw_crc.h"

#if 0
static CRC_HandleTypeDef CrcHandle;

/**
 * @brief  Hardware crc initialization.
 * @param  None
 * @retval None
 */
void crc_init(void)
{
    CrcHandle.Instance = CRC;

    if (HAL_CRC_Init(&CrcHandle) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
}

/**
 * @brief  Hardware crc calculation.
 * @param  data: The pointer of data to be computed.
 * @param  len: data length(4 bytes).
 * @retval The result of computing.
 */
uint32_t hw_crc_calculate(uint32_t data[], uint32_t len)
{
    return HAL_CRC_Accumulate(&CrcHandle, data, len);
}
#endif
