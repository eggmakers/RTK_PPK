#ifndef __boot_h
#define __boot_h

/**
 * @brief  Set compat flag (Support uavcan firmware update).
 * @param  None
 * @retval None
 */
void set_compat_flag(void);

/**
 * @brief  重映射中断向量表.
 * @param  None
 * @retval None
 */
void sys_remap(void);

#endif
