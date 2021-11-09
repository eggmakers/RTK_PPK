#ifndef __watchdog_h
#define __watchdog_h
#include <stdbool.h>

/**
 * @brief  检查是否是看门狗复位重启.
 * @param  None
 * @retval True if reset from watchdong.
 */
bool was_watchdag_reset(void);

#endif
