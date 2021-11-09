#ifndef __TIME_H
#define __TIME_H
#include <stdint.h>

/** Millisecond time */
typedef uint32_t time_ms_t;

/** Microsecond time */
typedef uint64_t time_us_t;

typedef struct
{
    uint32_t clock_stat; /** Clock Model Status. */
    double offset;       /** Receiver clock offset in seconds from GPS reference time. */
    double offset_std;   /** Receiver clock offset standard deviation. */
    double utc_offset;   /** The offset of GPS reference time from UTC time. */
    uint32_t utc_year;   /** UTC year. */
    uint8_t utc_month;   /** UTC month. */
    uint8_t utc_day;     /** UTC day. */
    uint8_t utc_hour;    /** UTC hour. */
    uint8_t utc_min;     /** UTC minute. */
    uint32_t utc_ms;     /** UTC millisecond. */
    uint32_t utc_stat;   /** UTC status. */
} utc_time_t;

/**
 * @brief Delay for 'us' microsecond.
 * @arg   us, The amount of delay.
 * @ret   None
 */
void delay_us(time_us_t us);

/**
 * @brief Delay for 'ms' millisecond.
 * @arg   ms, The amount of delay.
 * @ret   None
 */
void delay_ms(time_ms_t ms);

/**
 * @brief Get the current count value incremented by one microsecond.
 * @arg   None
 * @ret   The curren count value (microsecond).
 */
time_us_t micros(void);

/**
 * @brief Get the current count value incremented by one millisecond.
 * @arg   None
 * @ret   The curren count value (millisecond).
 */
time_ms_t millis(void);

/**
 * @brief 获取系统开机时长，开机时需调用一次.
 * @arg None
 * @ret None
 */
uint32_t sys_boot_time(void);

/**
 * @brief  Get UTC time.
 * @param  None
 * @retval UTC time.
 */
utc_time_t get_utc_time(void);

/**
 * @brief  Set UTC time.
 * @param  time: The new utc time.
 * @retval None
 */
void set_utc_time(utc_time_t time);

/**
 * @brief Initialize the initial time value.
 * @arg   None
 * @ret   None.
 */
void time_init(void);

#endif
