#ifndef __usart_dma2ringbuf_H
#define __usart_dma2ringbuf_H
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

#define UART_DMA_RX_UNIT_SIZE   512
#define UART_DMA_RX_BUFF_SIZE   512

/** 用于debug */
#define USART1_DMA_RX_BUFF_SIZE 512

/** 用于接收kx数据(本工程用不到,预留) */
#define USART6_DMA_RX_BUFF_SIZE 128

/** 用于存储rtk数据 */
#define USART2_DMA_RX_BUFF_SIZE (1024*6)

 /** 用于存储ppk数据,ppk的数据太大,有必要开那么大的缓冲.*/
#define UART3_DMA_RX_BUFF_SIZE  (1024*10)



typedef struct 
{
    UART_HandleTypeDef* huart; 		/* uart instance pointer */ 
    uint16_t uart_dma_rx_buff_size; /* DMA ring buffer size */
    uint16_t head; 					/* DMA ring buffer head */
    uint16_t tail; 					/* DMA ring buffer tail */
	uint16_t available;				/* number of bytes already get */
	uint16_t last_space;
}usart_dma2ringbuf_t ;

/**
  * @brief  接收字符串.
  * @param  uart_port: uart 端口处理结构的地址.
  * @param  buff: 接收缓冲区.
  * @param  read_len: 读取长度.
  * @retval 取出的数据长度.
  */
uint16_t uart_read_data_unit(UART_HandleTypeDef * huart, uint8_t * buff, uint16_t read_len);

uint16_t uart_read_data(UART_HandleTypeDef * huart, uint8_t * buff);
void USART_IrqHandler (UART_HandleTypeDef *huart);
void usart_update_ringbuf(UART_HandleTypeDef *huart);
#ifdef __cplusplus
}
#endif
#endif
