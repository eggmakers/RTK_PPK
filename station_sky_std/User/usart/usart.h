/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart_dma2ringbuf.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN Private defines */
#define Usart2_TX_Pin GPIO_PIN_2
#define Usart2_TX_GPIO_Port GPIOA
#define Usart2_RX_Pin GPIO_PIN_3
#define Usart2_RX_GPIO_Port GPIOA
/* USER CODE END Private defines */
void usart_init(void);
void usart_init_without_usart2(void);
void MX_USART3_UART_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART6_UART_Init(void);
void uart_set_baud(UART_HandleTypeDef* huart, uint32_t baud_rate);

/* USER CODE BEGIN Prototypes */

/**
  * @brief  Get uart dma receive buffer
  * @param  huart @see UART_HandleTypeDef
  * @retval Receive buffer pointer
  */
uint8_t* get_uart_rx_buffer(UART_HandleTypeDef *huart);

/**
  * @brief  Get the length of data available for receipt.
  * @param  uart_port: uart 端口处理结构的地址.
  * @retval The length of data available for receipt.
  */
uint16_t get_available_data_len(UART_HandleTypeDef* huart);

usart_dma2ringbuf_t* get_usart_dma2ringbuf(UART_HandleTypeDef *huart);

/**
  * @brief  串口配置初始化
  * @param
  * @retval None
  */
void uart_config_init(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

