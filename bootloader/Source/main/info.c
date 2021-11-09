#include "info.h"

/**
 * @note UAVCAN node ID.
 */
static uint8_t can_node_id = 0;

/**
 * @note UAVCAN baudrate.
 *       defualt value: 0bps.
 */
static uint32_t can_baudrate = 0;

/**
 * @brief  Get can baudrate.
 * @param  None
 * @retval Baudrate.
 */
uint32_t get_can_baudrate(void)
{
    return can_baudrate;
}

/**
 * @brief  Set can baudrate.
 * @param  baudrate: The baudrate to be set.
 * @retval None
 */
void set_can_baudrate(uint32_t baudrate)
{
    can_baudrate = baudrate;
}

/**
 * @brief  Get uavcan node ID.
 * @param  None
 * @retval UAVCAN node ID.
 */
uint8_t get_can_node_id(void)
{
    return can_node_id;
}

/**
 * @brief  Set uavcan node ID.
 * @param  id: The uavcan node ID to be set (0 - 127).
 * @retval None
 */
void set_can_node_id(uint8_t id)
{
    if (id <= 127)
    {
        can_node_id = id;
    }
}
