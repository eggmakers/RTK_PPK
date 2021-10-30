#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include "usart_dma2ringbuf.h"

//Uart1, for debug
UART_HandleTypeDef huart1;
//Uart2, for ppk,硬件上和板卡的com1相连,用于ppk数据输出和板卡指令配置
UART_HandleTypeDef huart2;
//Uart3, for rtk,硬件上和板卡的com2相连,只是作为最终的rtk数据输出
UART_HandleTypeDef huart3;
//Uart4, for hisi
//UART_HandleTypeDef huart4;
//Uart6, for kx,用于接收地面端的rtcm数据
UART_HandleTypeDef huart6;

/**
  * @brief  Uart dma buffer
  */

#pragma pack(4)//按一个字节对齐
struct
{
    uint8_t usart1_buff[USART1_DMA_RX_BUFF_SIZE];
    uint8_t usart2_buff[USART2_DMA_RX_BUFF_SIZE];
    uint8_t usart3_buff[UART3_DMA_RX_BUFF_SIZE];
    uint8_t usart6_buff[USART6_DMA_RX_BUFF_SIZE];
} uart_dma_rx_buff;
#pragma pack()    //取消自定义字节对齐方式

/* initial dma ringbuffer for each uart */
usart_dma2ringbuf_t husart_dma2ringbuf1 =
{
    &huart1,
    USART1_DMA_RX_BUFF_SIZE,
    0,
    0,
    0,
    USART1_DMA_RX_BUFF_SIZE
};

usart_dma2ringbuf_t	husart_dma2ringbuf2  =
{
    &huart2,
    USART2_DMA_RX_BUFF_SIZE,
    0,
    0,
    0,
    USART2_DMA_RX_BUFF_SIZE
};

usart_dma2ringbuf_t husart_dma2ringbuf3  =
{
    &huart3,
    UART3_DMA_RX_BUFF_SIZE,
    0,
    0,
    0,
    UART3_DMA_RX_BUFF_SIZE
};

usart_dma2ringbuf_t husart_dma2ringbuf6  =
{
    &huart6,
    USART6_DMA_RX_BUFF_SIZE,
    0,
    0,
    0,
    USART6_DMA_RX_BUFF_SIZE
};

void start_usart_ringbuf(UART_HandleTypeDef * uart_port);

DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

/**
  * @brief  UART4 init function
  * @param  None
  * @retval None
  */
void MX_USART3_UART_Init(void)
{

    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
    	////_//Error_Handler(__FILE__, __LINE__);
    }
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    start_usart_ringbuf(&huart3);
}

/**
  * @brief  USART1 init function
  * @param  None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
    	////_//Error_Handler(__FILE__, __LINE__);
    }
    /* Enable idle line interrupt */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    start_usart_ringbuf(&huart1);
}

/**
  * @brief  USART2 init function
  * @param  None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
    	////_//Error_Handler(__FILE__, __LINE__);
    }
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    start_usart_ringbuf(&huart2);
}

/**
  * @brief  USART2 shut down init function
  * @param  None
  * @retval None
  */
void MX_USART2_INPUT_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = Usart2_TX_Pin|Usart2_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/**
  * @brief  USART6 init function
  * @param  None
  * @retval None
  */
void MX_USART6_UART_Init(void)
{

    huart6.Instance = USART6;
    huart6.Init.BaudRate = 115200;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX_RX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart6) != HAL_OK)
    {
    	////_//Error_Handler(__FILE__, __LINE__);
    }
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
    start_usart_ringbuf(&huart6);
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(uartHandle->Instance == USART3)
    {
        /* USER CODE BEGIN USART3_MspInit 0 */

        /* USER CODE END USART3_MspInit 0 */
        /* USART3 clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**USART3 GPIO Configuration
        PB10     ------> UART3_TX
        PB11     ------> UART3_RX
        */
		GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USART3 DMA Init */
        /* USART3_TX Init */
        hdma_usart3_tx.Instance = DMA1_Stream3;
        hdma_usart3_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart3_tx.Init.Mode = DMA_NORMAL;
        hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);
        }

        __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart3_tx);

        /* USART3_RX Init */
        hdma_usart3_rx.Instance = DMA1_Stream1;
        hdma_usart3_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart3_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);;
        }
        __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);
        //HAL_UART_Receive_DMA ( uartHandle, &usart3_buff[0], sizeof(usart3_buff) );


        /* USART3 interrupt Init */
        HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        /* USER CODE BEGIN USART3_MspInit 1 */

        /* USER CODE END USART3_MspInit 1 */
    }
    else if(uartHandle->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
		GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate =  GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 DMA Init */
        /* USART1_RX Init */
        hdma_usart1_rx.Instance = DMA2_Stream2;
        hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);;
        }
        __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);
#if 1
        /* USART1_TX Init */
        hdma_usart1_tx.Instance = DMA2_Stream7;
        hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);;
        }

        __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);
#endif

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspInit 1 */
        /* USER CODE END USART1_MspInit 1 */
    }
    else if(uartHandle->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspInit 0 */

        /* USER CODE END USART2_MspInit 0 */
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
		GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate =  GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART2 DMA Init */
        /* USART2_RX Init */
        hdma_usart2_rx.Instance = DMA1_Stream5;
        hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);
        }
        __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);



#if 1
        /* USART2_TX Init */
        hdma_usart2_tx.Instance = DMA1_Stream6;
        hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart2_tx.Init.Mode = DMA_NORMAL;
        hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);;
        }

        __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);
#endif

        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspInit 1 */
        /* USER CODE END USART2_MspInit 1 */
    }
    else if(uartHandle->Instance == USART6)
    {
        /* USER CODE BEGIN USART3_MspInit 0 */

        /* USER CODE END USART3_MspInit 0 */
        /* USART6 clock enable */
        __HAL_RCC_USART6_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**USART3 GPIO Configuration
        PC6      ------> USART6_TX
        PC7      ------> USART6_RX
        */
		GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate =  GPIO_AF8_USART6;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* USART3 DMA Init */
        /* USART3_RX Init */
        hdma_usart6_rx.Instance = DMA2_Stream1;
        hdma_usart6_rx.Init.Channel = DMA_CHANNEL_5;
        hdma_usart6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart6_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart6_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart6_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart6_rx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);;
        }
        __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart6_rx);


#if 0
        /* USART6_TX Init */
        hdma_usart6_tx.Instance = DMA2_Stream6;
        hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
        hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart6_tx.Init.Mode = DMA_NORMAL;
        hdma_usart6_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
        {
            ////_//Error_Handler(__FILE__, __LINE__);;
        }

        __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart6_tx);
#endif
        /* USART3 interrupt Init */
        HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
        /* USER CODE BEGIN USART3_MspInit 1 */
        /* USER CODE END USART3_MspInit 1 */
    }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

    if(uartHandle->Instance == UART4)
    {
        /* USER CODE BEGIN UART4_MspDeInit 0 */

        /* USER CODE END UART4_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_UART4_CLK_DISABLE();

        /**UART4 GPIO Configuration
        PA0     ------> UART4_TX
        PA1     ------> UART4_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

        /* UART4 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmatx);
        HAL_DMA_DeInit(uartHandle->hdmarx);

        /* UART4 interrupt Deinit */
        HAL_NVIC_DisableIRQ(UART4_IRQn);
        /* USER CODE BEGIN UART4_MspDeInit 1 */

        /* USER CODE END UART4_MspDeInit 1 */
    }
    else if(uartHandle->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

        /* USART1 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmarx);
        HAL_DMA_DeInit(uartHandle->hdmatx);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
    else if(uartHandle->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspDeInit 0 */

        /* USER CODE END USART2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

        /* USART2 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmarx);
        HAL_DMA_DeInit(uartHandle->hdmatx);

        /* USART2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspDeInit 1 */

        /* USER CODE END USART2_MspDeInit 1 */
    }
    else if(uartHandle->Instance == USART6)
    {
        /* USER CODE BEGIN USART3_MspDeInit 0 */

        /* USER CODE END USART3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART6_CLK_DISABLE();

        /**USART3 GPIO Configuration
        PB10     ------> USART3_TX
        PB11     ------> USART3_RX
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

        /* USART3 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmarx);
        HAL_DMA_DeInit(uartHandle->hdmatx);

        /* USART3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART3_IRQn);
        /* USER CODE BEGIN USART3_MspDeInit 1 */

        /* USER CODE END USART3_MspDeInit 1 */
    }

}

/**
  * @brief  start usart DMA rx
  * @param
  * @retval None
  */
void start_usart_ringbuf(UART_HandleTypeDef * uart_port)
{
    /* Enable DMA rx complete interupt */
    if(uart_port->hdmarx !=NULL )
    {
		//__HAL_DMA_ENABLE_IT(uart_port->hdmarx, DMA_IT_TC);

		/* Disable Half rx Interrupt */
		__HAL_DMA_DISABLE_IT(uart_port->hdmarx, DMA_IT_HT);

		usart_dma2ringbuf_t *usart_dma2ringbuf = get_usart_dma2ringbuf(uart_port);

		uint8_t *rx_buffer = get_uart_rx_buffer(uart_port);

		/* Set rx ringbuffer */
		HAL_UART_Receive_DMA(uart_port, rx_buffer,usart_dma2ringbuf->uart_dma_rx_buff_size);
    }

    /* Enable DMA tx complete interupt */
    if(uart_port->hdmatx !=NULL )
    {
    	__HAL_DMA_DISABLE_IT(uart_port->hdmatx, DMA_IT_TC);
        /* Disable Half tx Interrupt */
        __HAL_DMA_DISABLE_IT(uart_port->hdmatx, DMA_IT_HT);
    }
}

static void disable_uart(UART_HandleTypeDef* huart)
{
    __HAL_UART_DISABLE(huart);
}

static void enable_uart(UART_HandleTypeDef* huart)
{
    __HAL_UART_ENABLE(huart);
}

void uart_set_baud(UART_HandleTypeDef* huart, uint32_t baud_rate)
{
    disable_uart(huart);
    huart->Init.BaudRate = baud_rate;
    if (HAL_UART_Init(huart) != HAL_OK)
    {
        ////_//Error_Handler(__FILE__, __LINE__);;
    }
    enable_uart(huart);
}

/**
  * @brief  Get the length of data available for receipt.
  * @param  uart_port: uart 端口处理结构的地址.
  * @retval The length of data available for receipt.
  */
uint16_t get_available_data_len(UART_HandleTypeDef* huart)
{
    if(huart->Instance == USART1)
    {
        return husart_dma2ringbuf1.available;
    }
    else if(huart->Instance == USART2)
    {
        return husart_dma2ringbuf2.available;
    }
    else if(huart->Instance == USART6)
    {
        return husart_dma2ringbuf6.available;
    }
    else if(huart->Instance == USART3)
    {
        return husart_dma2ringbuf3.available;
    }
    else
    {
        return 0;
    }
}

/**
  * @brief  Get uart dma receive buffer
  * @param  huart @see UART_HandleTypeDef
  * @retval Receive buffer pointer
  */
uint8_t* get_uart_rx_buffer(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        return uart_dma_rx_buff.usart1_buff;

    }
    else if(huart->Instance == USART2)
    {
        return uart_dma_rx_buff.usart2_buff;
    }
    else if(huart->Instance == USART6)
    {
        return uart_dma_rx_buff.usart6_buff;
    }
    else if(huart->Instance == USART3)
    {
        return uart_dma_rx_buff.usart3_buff;
    }
    else
    {
        return NULL;
    }
}

/**
  * @brief  init_ringbuf_usart
  * @param
  * @retval None
  */
usart_dma2ringbuf_t* get_usart_dma2ringbuf(UART_HandleTypeDef *huart) {
    if(huart->Instance == USART1)
    {
        return &husart_dma2ringbuf1;
    }
    else if(huart->Instance == USART2)
    {
        return &husart_dma2ringbuf2;
    }
    else if(huart->Instance == USART3)
    {
        return &husart_dma2ringbuf3;
    }
    else if(huart->Instance == USART6)
    {
        return &husart_dma2ringbuf6;
    }
    return 0;
}

void usart_init(void)
{
	//Uart1, for debug
	MX_USART1_UART_Init();
	//Uart2, for rtk
	MX_USART2_UART_Init();
	//Uart3, for ppk
	MX_USART3_UART_Init();
	//Uart6, for kx
	MX_USART6_UART_Init();
	printf("UAVCAN Message Received\r\n");
}

void usart_init_without_usart2(void)
{
	//Uart1, for debug
	MX_USART1_UART_Init();
	//Uart2, for rtk(shut down)
	MX_USART2_INPUT_Init();
	//Uart3, for ppk
	MX_USART3_UART_Init();
	//Uart6, for kx
	MX_USART6_UART_Init();	
	printf("UAVCAN Message Rejected\r\n");	
}

int fputc(int ch,FILE *file)
{
    while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TXE) == RESET);
    huart1.Instance->DR = ch;
    return ch;
}

#if 0

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    while ((USART1->SR & 0X40) == 0);
    USART1->DR = (uint8_t) ch;
    return ch;
}

int _write(int file, char *ptr, int len)
{
      int DataIdx;
      for (DataIdx = 0; DataIdx < len;DataIdx++)
     {
           __io_putchar(*ptr++);
     }
      return len;
}
#endif




