#include "dma.h"

/**
  * @brief  Enable DMA controller clock
  * @param  None
  * @retval None
  */
void dma_init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();


    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
	
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

   /* usart3 tx and rx dma interrupt */
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);


    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}
