#ifndef __led_h
#define __led_h
#include "io.h"

/** LED1 */
#define LED1_TOGGLE()  HAL_GPIO_TogglePin(LED1_GPIO, LED1_GPIO_PIN)
#define LED1_OFF()     HAL_GPIO_WritePin(LED1_GPIO, LED1_GPIO_PIN, GPIO_PIN_RESET)
#define LED1_ON()      HAL_GPIO_WritePin(LED1_GPIO, LED1_GPIO_PIN, GPIO_PIN_SET)

/** LED2 */
#define LED2_TOGGLE()  HAL_GPIO_TogglePin(LED2_GPIO, LED2_GPIO_PIN)
#define LED2_OFF()     HAL_GPIO_WritePin(LED2_GPIO, LED2_GPIO_PIN, GPIO_PIN_RESET)
#define LED2_ON()      HAL_GPIO_WritePin(LED2_GPIO, LED2_GPIO_PIN, GPIO_PIN_SET)

/** LED3 */
#define LED3_TOGGLE()  HAL_GPIO_TogglePin(LED3_GPIO, LED3_GPIO_PIN)
#define LED3_OFF()     HAL_GPIO_WritePin(LED3_GPIO, LED3_GPIO_PIN, GPIO_PIN_RESET)
#define LED3_ON()      HAL_GPIO_WritePin(LED3_GPIO, LED3_GPIO_PIN, GPIO_PIN_SET)

/**
 * @brief  led hardware initialization.
 * @param  None
 * @retval None
 */
void led_init(void);

#endif
