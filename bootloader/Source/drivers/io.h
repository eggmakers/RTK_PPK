#ifndef __IO_H
#define __IO_H
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

/** CAN1 for stm32f427*/
//#define CAN1_RX_PIN   GPIO_PIN_0
//#define CAN1_RX_GPIO  GPIOD
//#define CAN1_TX_PIN   GPIO_PIN_1
//#define CAN1_TX_GPIO  GPIOD

/** CAN2 for stm32f427 (in this programe,not used)*/
#define CAN2_RX_PIN   GPIO_PIN_12
#define CAN2_RX_GPIO  GPIOB
#define CAN2_TX_PIN   GPIO_PIN_13
#define CAN2_TX_GPIO  GPIOB

/** CAN1 for stm32f405 */
#define CAN1_RX_PIN   GPIO_PIN_8
#define CAN1_RX_GPIO  GPIOB
#define CAN1_TX_PIN   GPIO_PIN_9
#define CAN1_TX_GPIO  GPIOB 	


/** ICM20602 SPI1 */
#define ICM260602_INT_PIN      GPIO_PIN_2
#define ICM260602_INT_GPIO     GPIOA
#define ICM20602_SCK_PIN       GPIO_PIN_5
#define ICM20602_SCK_GPIO      GPIOA
#define ICM20602_MOSI_PIN      GPIO_PIN_7
#define ICM20602_MOSI_GPIO     GPIOA
#define ICM20602_SPI1_CSS_PIN  GPIO_PIN_1
#define ICM20602_SPI1_CSS_GPIO GPIOB
#define ICM20602_MISO_PIN      GPIO_PIN_4
#define ICM20602_MISO_GPIO     GPIOB

/** USART1. */
#define USART1_TX_PIN          GPIO_PIN_6
#define USART1_TX_GPIO         GPIOB

#define USART1_RX_PIN          GPIO_PIN_10
#define USART1_RX_GPIO         GPIOA

/** LED. */
#define LED1_GPIO_PIN          GPIO_PIN_1
#define LED1_GPIO              GPIOC
#define LED2_GPIO_PIN          GPIO_PIN_2
#define LED2_GPIO              GPIOC
#define LED3_GPIO_PIN          GPIO_PIN_3
#define LED3_GPIO              GPIOC

//#define LED1_GPIO_PIN          GPIO_PIN_9
//#define LED1_GPIO              GPIOE
//#define LED2_GPIO_PIN          GPIO_PIN_10
//#define LED2_GPIO              GPIOE
//#define LED3_GPIO_PIN          GPIO_PIN_11
//#define LED3_GPIO              GPIOE

#endif
