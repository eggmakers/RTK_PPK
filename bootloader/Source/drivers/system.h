#ifndef __SYSTEM_H
#define __SYSTEM_H
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 初始化 HAL 库、系统时钟.
 * @arg   None
 * @ret   None.
 */
void sys_init(void);

void sys_init1(void);
void SystemClock_Config(void);
/**
 * @brief 获取 UID.
 * @arg buf, 获取到的 UID 存储地址.
 * @ret None.
 */
void get_unique_id(uint8_t* buf);

/**
 * @brief  System reboot.
 * @param  None
 * @retval None
 */
void system_reboot(void);

/**
 * @brief  Check for application in user flash.
 * @param  None
 * @retval None
 */
bool bl_check_app_exist(void);

/**
 * @brief  Jump to application
 * @param  None
 * @retval None
 */
bool jump_to_app(void);

#endif
