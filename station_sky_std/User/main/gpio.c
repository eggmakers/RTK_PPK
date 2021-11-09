/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include "fatfs.h"
#include "cmsis_armcc.h"
#include "usb_device.h"
#include "uavcan.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/

void sys_reset(void)
{
    //__disable_fault_irq();
	__set_FAULTMASK(1);
    NVIC_SystemReset();
}

void gpio_init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin : PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : USB 插入检测引脚 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);

}

/* USER CODE BEGIN 2 */

void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = LED1_GPIO_PIN;
    HAL_GPIO_Init(LED1_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED2_GPIO_PIN;
    HAL_GPIO_Init(LED2_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED3_GPIO_PIN;
    HAL_GPIO_Init(LED3_GPIO, &GPIO_InitStruct);

    LED1_ON();
    LED2_ON();
    LED3_ON();
}

void led_task(void)
{
	#define INDICATOR_CYCLE_IN_USB_MSC_MODE (1000)

    static time_ms_t tick = 0;

    if (millis() - tick >= INDICATOR_CYCLE_IN_USB_MSC_MODE)
    {
        LED1_TOGGLE();

        tick = millis();
    }
<<<<<<< Updated upstream
		if(SKmode_select == 0)
=======
		if(sk3_select == 0)
>>>>>>> Stashed changes
		{
			delay_ms(1000);
			printf("system running\r\n");
		}
			
}

//void SK1_USB_Task()
//{
//	uint8_t USB_Dect_CNT = 0;
//	uint8_t USB_TSK_Step = 0;
//	switch(USB_TSK_Step)
//	{
//		case 0:
//			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == GPIO_PIN_SET)
//			{
//				USB_Dect_CNT++;
//				HAL_Delay(10);
//			}
//			if(USB_Dect_CNT > 2)
//			{
//				USB_TSK_Step = 1;
//				
//				MX_FATFS_DeInit();
//				/*  本工程有一个bug,当初始化了sd卡并且往sd卡中写入了数据之后,
//				使用读卡器功能,就会一直不成功,
//				所以,目前的解决方法是如果让系统重启以便使用读卡器功能(后面再解决)  */
//				if(get_file_valid_or_not()==1)
//				{
//					sys_reset();
//				}
//			}
//			break;
//		
//		case 1:
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
//			HAL_Delay(10);
//			usb_device_init();
//			HAL_Delay(10);
//			USB_TSK_Step = 2;
//			break;
//		case 2:
//			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == RESET)
//			{
//				USB_Dect_CNT++;
//				HAL_Delay(100);
//			}
//			if(USB_Dect_CNT >= 10)
//			{
//				sys_reset();
//			}
//			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_3);
//			break;
//			
//		default :
//			break;
//	}
//	HAL_Delay(100);
//}

void usb_insert_task()
{
	/**检测到usb引脚为高电平,软重启,只运行usb 读卡器的功能. */
	 if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
	 {
		HAL_Delay(20);
		 
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
		{
			HAL_NVIC_SystemReset();
		}
	 }
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
