#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "info.h"
#include "time.h"
#include "system.h"

/**
 * @brief System Clock Configuration.
 * @arg   None
 * @ret   None
 */
static void clk_conf(void)
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
    //RCC_OscInitStruct.PLL.PLLM = 12;
	RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Activate the Over-Drive mode
    */
//    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
//    {
//        Error_Handler();
//    }
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

    /** Configure the Systick interrupt time */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    /** Configure the Systick */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /** SysTick_IRQn interrupt configuration. */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void SystemClock_Config(void)
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

/**
 * @brief 初始化 HAL 库、系统时钟.
 * @arg   None
 * @ret   None.
 */
void sys_init(void)
{
    /** Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /** Configure the system clock. */
    clk_conf();
	
    /** Initialize initial time value. */
    time_init();

    __HAL_RCC_BKPSRAM_CLK_ENABLE();

    HAL_PWR_EnableBkUpAccess();

    HAL_PWR_EnableBkUpReg();
}


/**
 * @brief 获取 UID.
 * @arg buf, 获取到的 UID 存储地址.
 * @ret None.
 */
void get_unique_id(uint8_t* buf)
{
    uint32_t part_of_uid;

    part_of_uid = HAL_GetUIDw0();
    memcpy(buf + 0, (uint8_t*)&part_of_uid, sizeof(uint32_t));

    part_of_uid = HAL_GetUIDw1();
    memcpy(buf + 4, (uint8_t*)&part_of_uid, sizeof(uint32_t));

    part_of_uid = HAL_GetUIDw2();
    memcpy(buf + 8, (uint8_t*)&part_of_uid, sizeof(uint32_t));
}

/**
 * @brief  System reboot.
 * @param  None
 * @retval None
 */
void system_reboot(void)
{
    HAL_NVIC_SystemReset();
}

typedef void (*p_function)(void);

/** MCU RAM size, used for checking accurately whether flash contains valid application. */
#define RAM_SIZE    (uint32_t)(0x20000)

/**
 * @brief  Check for application in user flash.
 * @param  None
 * @retval None
 */
bool bl_check_app_exist(void)
{
    return (((*(__IO uint32_t*)APP_START_ADDR) & ~(RAM_SIZE - 1)) == 0x20000000); /** Check stack address. */
}

/**
 * @brief  Jump to application
 * @param  None
 * @retval None
 */
bool jump_to_app(void)
{
    /** Check if there is application in user flash area. */
    if(bl_check_app_exist())
    {
        /** Jump to user application. */

        /**
         * The second word in the user code area is the program start address (reset address).
         * You can see from the interrupt vector table here.
         */
        uint32_t jump_address = *(__IO uint32_t*)(APP_START_ADDR + 4);
        p_function jump_to_application = (p_function)jump_address;

        HAL_RCC_DeInit();
        HAL_DeInit();

        SysTick->CTRL = 0;
        SysTick->LOAD = 0;
        SysTick->VAL  = 0;

        /** Initialize user application's Stack Pointer. */

        /**
         * Initialize the APP stack pointer (
         * the first word in the user code area is used to store the top address of the stack).
         * The top address of the user code will be set to the top pointer of the stack.
         */
        __set_MSP(*(__IO uint32_t*)APP_START_ADDR);

        /** Here we go. */
        jump_to_application();
    }

    return false;
}
