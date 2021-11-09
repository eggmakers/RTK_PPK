#include "io.h"
#include "system.h"
#include "uavcan.h"
#include "time.h"
#include "light.h"
#include "boot.h"
#include "led.h"


int main()
{

    sys_init();
    boot();
    indicator();
	
	LED1_ON();
	LED2_ON();
	LED3_ON();
	
    /** UAVCAN Initialization. */
    while (uavcan_init() != CAN_BUS_OK);

    while(1)
    {
        if (bl_timeout())
        {
            /** Timeout. */
			LED1_ON();
			LED2_OFF();
			LED3_ON();
            jump_to_app();
        }
        else
        {
            indicator_task();
            uavcan_task();
        }	
    }
}
