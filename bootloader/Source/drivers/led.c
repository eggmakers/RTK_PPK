#include "led.h"

/**
 * @brief  led hardware initialization.
 * @param  None
 * @retval None
 */
void led_init(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
    /** Led off. */
    LED1_OFF();
    LED2_OFF();
    LED3_OFF();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = LED1_GPIO_PIN;
    HAL_GPIO_Init(LED1_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED2_GPIO_PIN;
    HAL_GPIO_Init(LED2_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED3_GPIO_PIN;
    HAL_GPIO_Init(LED3_GPIO, &GPIO_InitStruct);
}
