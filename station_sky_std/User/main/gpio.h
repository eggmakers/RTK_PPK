/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#ifndef _BSP_GPIO_H__
#define _BSP_GPIO_H__

#include "stm32f4xx.h"

//v2版硬件对应的引脚
#define SD_DETECT_Pin           GPIO_PIN_15
#define SD_DETECT_GPIO_Port     GPIOA

#define USB_MARK_Pin            GPIO_PIN_0
#define USB_MARK_GPIO_PORT      GPIOA

//v3版硬件对应的引脚
//#define SD_DETECT_Pin           GPIO_PIN_15
//#define SD_DETECT_GPIO_Port     GPIOA
//
//#define USB_MARK_Pin            GPIO_PIN_8
//#define USB_MARK_GPIO_PORT      GPIOA

#define CARD_PWR_Pin            GPIO_PIN_4
#define CARD_PWR_PORT           GPIOA

#if 0
#define LED_RED_PIN         GPIO_PIN_1
#define LED_RED_PORT        GPIOC

#define LED_GREEN_PIN       GPIO_PIN_2
#define LED_GREEN_PORT      GPIOC

#define LED_YELLOW_PIN        GPIO_PIN_3
#define LED_YELLOW_PORT       GPIOC

#define LED_RED_TOGGLE()    HAL_GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN)
#define LED_RED_ON()        HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET)
#define LED_RED_OFF()       HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET)

#define LED_GREEN_TOGGLE()  HAL_GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN)
#define LED_GREEN_ON()      HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_SET)
#define LED_GREEN_OFF()     HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_RESET)

#define LED_YELLOW_TOGGLE()   HAL_GPIO_TogglePin(LED_YELLOW_PORT, LED_YELLOW_PIN)
#define LED_YELLOW_ON()       HAL_GPIO_WritePin(LED_YELLOW_PORT, LED_YELLOW_PIN, GPIO_PIN_SET)
#define LED_YELLOW_OFF()      HAL_GPIO_WritePin(LED_YELLOW_PORT, LED_YELLOW_PIN, GPIO_PIN_RESET)
#endif

/** LED. */
#define LED1_GPIO_PIN          GPIO_PIN_1
#define LED1_GPIO              GPIOC
#define LED2_GPIO_PIN          GPIO_PIN_2
#define LED2_GPIO              GPIOC
#define LED3_GPIO_PIN          GPIO_PIN_3
#define LED3_GPIO              GPIOC

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

#define CARD_PWR_DISABLE()     HAL_GPIO_WritePin(CARD_PWR_PORT, CARD_PWR_Pin, GPIO_PIN_SET)
#define CARD_PWR_ENABLE()      HAL_GPIO_WritePin(CARD_PWR_PORT, CARD_PWR_Pin, GPIO_PIN_RESET)

#define     USB_INSERT      0x01
#define     USB_NOT_INSERT  0x00

#define USB_DETECTED()      HAL_GPIO_ReadPin(USB_MARK_GPIO_PORT,USB_MARK_Pin)


void led_init(void);
void gpio_init(void);
uint8_t usb_is_inserted(void);

#endif

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
void usb_insert_task(void);

void gpio_init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
