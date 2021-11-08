#include "time.h"
#include "main.h"
#include "dma.h"
#include "gnss.h"
#include "gpio.h"
#include "usart.h"
#include "uavcan.h"
#include "usb_device.h"
#include "stm32f4xx_hal_rtc_ex.h"
#include "boot.h"
#include "param.h"
#include "fix2.h"

uint8_t SK3_STANDARD = 0;
/*sk3_select==0x01,SK3
	sk3_select==0x02,STD
*/
uint8_t SKmode_select;
/*SKmode_select = 0 :SK1模式
	SKmode_select = 1 :SK3/STD模式*/

/**
  * @brief System Clock Configuration
  * @retval None
  */
void clk_conf()
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

#if 1
/**
 * @brief  重映射中断向量表.
 * @param  None
 * @retval None
 */
#define APP_START_ADDR    (0x08010000)      /** App firmware start address. */
void sys_remap()
{
	#define FIRMWARE_START_ADDR (uint32_t)APP_START_ADDR
	__disable_irq();
	
    /** relocate vector table. */
    SCB->VTOR = (uint32_t)FLASH_BASE|FIRMWARE_START_ADDR;

    __DSB();

    __enable_irq();	
}

void system_reboot(void)
{
	__disable_irq();
    HAL_NVIC_SystemReset();
}
#endif



void bk_up_init()
{
    __HAL_RCC_BKPSRAM_CLK_ENABLE();

    HAL_PWR_EnableBkUpAccess();

    HAL_PWR_EnableBkUpReg();	
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	sys_remap();
	HAL_Init();
	clk_conf();
	time_init();
	gpio_init();
	
	dma_init();
	usart_init();
	led_init();
	
//	/*初始状态灯*/
//	LED1_OFF();
//	LED2_TOGGLE();
//	LED3_OFF();
	
	
	bk_up_init();
	g_init_fix2_struct();
//	/*检测到usb引脚为高电平,软重启,只运行usb 读卡器的功能.(注意检测对应硬件版本的usb插入检测引脚),否则,将有可能一直停留在这里 */
//	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
//	{
//		HAL_Delay(20);
//	 
//		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
//		{
//			usb_device_init();
//			
//			while(1)
//			{
//				printf("system running...\r\n");
//				led_task();
//			}
//		}
//	}
	printf("start delay\n");
	#if 1
	/* 等待板卡上电稳定(this is necessary.) **/
	HAL_Delay(5000);
	
	
	while(gnss_init(&huart3) == false)
	{
		/**设置超时等待时间*/
	}	
	
	#endif
	
	while (uavcan_init() != CAN_BUS_OK);
	
//	params_init();//放在加载参数之后，判断参数
	delay_ms(5000);
	uavcan_estimate_task();
	if(SKmode_select == 0)
	usart_init_without_usart2();
	if(SKmode_select == 1)
	usart_init();	
	pre_uavcan_task();
	while (1)	
	{
		led_task();
		/** 注意检查板卡rtk数据串口的电阻有没有接上,如果没有,将读不到rtk数据,
		*  对于sk1飞控来说,是不需要接的,对于sk3,则需要接。
		*/
		if(SKmode_select == 1)
		rtk_task();
		ppk_task();
		if(SKmode_select == 1)
		uavcan_task();
		usb_insert_task();
	}
}



/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

