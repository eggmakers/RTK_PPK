#include "time.h"
#include "led.h"
#include "light.h"

/**
 * @brief  Indicator initialization.
 * @param  None
 * @retval None
 */
void indicator(void)
{
    led_init(); /** LED intialization. */
}

/**
 * @brief  Indicator task.
 * @param  None
 * @retval None
 */
void indicator_task(void)
{
#define INDICATOR_CYCLE_IN_USB_MSC_MODE (1000)

    static time_ms_t tick = 0;

    if (millis() - tick >= INDICATOR_CYCLE_IN_USB_MSC_MODE)
    {
        //LED1_ON();
		LED1_TOGGLE();
        LED2_TOGGLE();
		LED3_TOGGLE();
        tick = millis();
    }
}
