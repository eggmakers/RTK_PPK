#include "io.h"
#include "watchdog.h"

#define STM32F4

/**
 * @note IWDG_BASE definition.
 */
#ifndef IWDG_BASE
#if defined(STM32H7)
#define IWDG_BASE             0x58004800
#elif defined(STM32F7) || defined(STM32F4)
#define IWDG_BASE             0x40003000
#elif defined(STM32F1) || defined(STM32F3)
#define IWDG_BASE             0x40003000
#else
#endif
#endif

/**
 * @note RCC_BASE definition.
 */
#ifndef RCC_BASE
#error "Unsupported IWDG RCC MCU config"
#endif

/**
 * @note defines for working out if the reset was from the watchdog
 */
#if defined(STM32H7)
#define WDG_RESET_STATUS (*(__IO uint32_t *)(RCC_BASE + 0xD0))
#define WDG_RESET_CLEAR (1U<<16)
#define WDG_RESET_IS_IWDG (1U<<26)
#elif defined(STM32F7) || defined(STM32F4)
#define WDG_RESET_STATUS (*(__IO uint32_t *)(RCC_BASE + 0x74))
#define WDG_RESET_CLEAR (1U<<24)
#define WDG_RESET_IS_IWDG (1U<<29)
#elif defined(STM32F1) || defined(STM32F3)
#define WDG_RESET_STATUS (*(__IO uint32_t *)(RCC_BASE + 0x24))
#define WDG_RESET_CLEAR (1U<<24)
#define WDG_RESET_IS_IWDG (1U<<29)
#else
#endif

/**
 * @brief  检查是否是看门狗复位重启.
 * @param  None
 * @retval True if reset from watchdong.
 */
bool was_watchdag_reset(void)
{
    /** Default return false. */
    return false;

    // return (WDG_RESET_STATUS & WDG_RESET_IS_IWDG);
}
