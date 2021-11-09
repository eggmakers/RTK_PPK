#include "stm32f4xx_hal.h"
#include "time.h"

static time_us_t ticks_per_us = 0;
static volatile time_ms_t sys_tick_uptime = 0;
static volatile uint32_t sys_tick_val_stamp = 0;

static volatile int sys_tick_pending = 0;

/** UTC real time. */
static utc_time_t utc_time = {0};

/**
 * @brief SysTick interrupt handler.
 * @arg   None
 * @ret   None
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
	
	#if 1
    HAL_SYSTICK_IRQHandler();

    if (sys_tick_uptime > 4233600000ll)
    {
        /** 49 days non-reset would cuase a tick reset. */
        sys_tick_uptime = 0;
    }

    sys_tick_uptime++;
    sys_tick_val_stamp = SysTick->VAL;
    sys_tick_pending = 0;
    (void)(SysTick->CTRL);
	#endif
}

/**
 * @brief Delay for 'us' microsecond.
 * @arg   us, The amount of delay.
 * @ret   None
 */
void delay_us(time_us_t us)
{
    time_us_t now = micros();
    while (micros() - now < us);
}

/**
 * @brief Delay for 'ms' millisecond.
 * @arg   ms, The amount of delay.
 * @ret   None
 */
void delay_ms(time_ms_t ms)
{
    HAL_Delay(ms);
}

static time_us_t micros_isr(void)
{
    register uint32_t ms, pending, cycle_cnt;

    cycle_cnt = SysTick->VAL;

    /**
     * 由于 systick 中断优先级是 15, 也就是它处于最低的优先级, 所以, 如果是在高优先级的中断函数里面调用 micros(),
     * 然后再接着调用 micros_isr() 的话, 那么, 此时又刚好产生了一次 sysTick_ Handler 中断, 就必须等到高优先级的
     * 中断函数执行完毕以后, SysTick_Handler() 才能执行, sys_tick_uptime 才能 ++, 但是, 这么做的话, 就会丧失掉
     * 一次 ms 的计数, 因此, 必须要做一次检查, 检查 SysTick 是否已经计数到 0, 计数到 0 的话, 就让 ms++.
     */

    /** 检查 systick ctrl 寄存器的第 16 位是否被置位, 如果被置位, 说明 SysTick 已经计数到 0. */
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    {
        sys_tick_pending = 1;
        cycle_cnt = SysTick->VAL;
    }
    ms = sys_tick_uptime;
    pending = sys_tick_pending;

    /** 将 systick 寄存器的值转换成 us 值. */
    return ((time_us_t)(ms + pending) * 1000LL) + (ticks_per_us * 1000LL - (time_us_t)cycle_cnt) / ticks_per_us;
}

/**
 * @brief Get the current count value incremented by one microsecond.
 * @arg   None
 * @ret   The curren count value (microsecond).
 */
time_us_t micros(void)
{
    register uint32_t ms, cycle_cnt;

    /** 判断当前的 micros() 是在中断里面调用的, 还是在 while(1) 里面调用的. */
    if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk))
    {
        return micros_isr();
    }
    else
    {
        /** 如果是在 while(1) 里面调用的, 分别获取当前系统 ms 的计数值和当前 syeTick->VAL 的值. */
        do
        {
            ms = sys_tick_uptime;
            cycle_cnt = SysTick->VAL;
        }
        /**
         * 上面已经进行了 ms = sys_tick_uptime 的操作, 因此 ms != sys_tick_uptime 能成立的条件只有一个,
         * 就是在 ms = sys_tick_uptime 执行完之后, 刚好产生了一次 sysTick_ Handler 中断, 在中断中让
         * sys_tick_uptime++, cycle_cnt > sys_tick_val_stamp 同理.
         */
        while (ms != sys_tick_uptime || cycle_cnt > sys_tick_val_stamp);
        /**
         * (ticks_per_us * 1000LL - (time_us_t)cycle_cnt) / ticks_per_us --> 将当前 SysTick->VAL 的值换算成 us,
         * 换算的方法: 如果是从零开始的向上计数器, 则直接用当前的计数值 / 每 us 应计的数目 = cycle_cnt / ticks_per_us
         * 即可, 但是 systick 是一个 24 位的向下计算器, 所以, 当前的计数值 = 初始计数值 - SysTick->VAL, 而 hal 库中默认
         * 设置的是 1ms 产生一次 systick 中断, 所以, 初始计数值 = HAL_RCC_GetSysClockFreq() / 1000 = ticks_per_us * 1000
         * 因此当前的计数值 = ticks_per_us * 1000LL - (time_us_t)cycle_cnt, 再 / ticks_per_us, 就是将当前的计数值换算成 us 值.
         */
        return ((time_us_t)ms * 1000LL) + (ticks_per_us * 1000LL - (time_us_t)cycle_cnt) / ticks_per_us;
    }
}

/**
 * @brief Get the current count value incremented by one millisecond.
 * @arg   None
 * @ret   The curren count value (millisecond).
 */
time_ms_t millis(void)
{
    return sys_tick_uptime;
}

/**
 * @brief 获取系统开机时长，开机时需调用一次.
 * @arg None
 * @ret None
 */
uint32_t sys_boot_time(void)
{
    static uint32_t started_at_sec = 0;

    if (started_at_sec == 0)
    {
        started_at_sec = (uint32_t)(micros() / 1000000U);
    }

    return (uint32_t)((micros() / 1000000U) - started_at_sec);
}

/**
 * @brief  Get UTC time.
 * @param  None
 * @retval UTC time.
 */
utc_time_t get_utc_time(void)
{
    return utc_time;
}

/**
 * @brief  Set UTC time.
 * @param  time: The new utc time.
 * @retval None
 */
void set_utc_time(utc_time_t time)
{
    utc_time = time;
}

/**
 * @brief Initialize the initial time value.
 * @arg   None
 * @ret   None.
 */
void time_init(void)
{
    /** 根据当前的系统时钟, 计算出每 us 所应该计数的计数值.(比如计了 168 个数, 代表 1us) */
    ticks_per_us = HAL_RCC_GetSysClockFreq() / 1000000;

    /** Initialize the system boot time. */
    sys_boot_time();
}
