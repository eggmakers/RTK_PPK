#include "io.h"
#include "info.h"
#include "boot.h"
#include "flash.h"
#include "led.h"
#include "system.h"
#include "sw_crc.h"
#include "read_write.h"

fw_update_stat_t fw_update_stat;

/** Application descriptor install. */
app_descriptor_t app_desc =
{
    {'A', 'P', 'P', ' ', 'D', 'E', 'S', 'C'}, /** Signature. */
    0,  /** Git hash. */
    0,  /** Image(app) size. */
    0,  /** Image major version. */
    0,  /** Image minor version. */
    0,  /** Compat flag. */
    0,  /** Device ID. */
    0,  /** Image crc.(application) */
    0,  /** Application description crc.(from magic to reserved) */
    {0} /** Reserved. */
};

/**
 * @brief  Update application descriptor.
 * @param  None
 * @retval None
 */
static void app_desc_update(void)
{
    app_desc.image_size = fw_update_stat.offset + 1; /** Imagine size. */

    const uint8_t desc_len = (uint8_t)(offsetof(app_descriptor_t, desc_crc) - offsetof(app_descriptor_t, magic));

    /** Compute crc. */
    app_desc.desc_crc = sw_crc32_calculate(0, (uint8_t*)&app_desc, desc_len);

    /** Erase application descriptor flash area. */
    if (flash_erase_page(get_sector(APP_DESC_ADDR)))
    {
        /** Write in flash. */
        flash_write(APP_DESC_ADDR, (uint8_t*)&app_desc, sizeof(app_descriptor_t));
    }
}

/**
 * @brief  升级完成.
 * @param  None
 * @retval None
 */
static void on_update_complete(void)
{
    fw_update_stat.read_file_req = false;

    /** Update application descriptor. */
    app_desc_update();
	
	__NOP();
    /** 跳转到 App. */
    if (!jump_to_app())
    {
        /** 跳转失败, 复位系统. */
        system_reboot();
    }
}

/**
 * @brief  擦除页.
 * @param  page_num: 第几页.
 * @retval None
 */
static bool erase_app_page(uint32_t sector)
{
    /** 擦除页. */
    if (flash_erase_page(sector) == false)
    {
        return false;
    }

    fw_update_stat.last_erased_sector = sector;

    return true;
}

/**
 * @brief  处理接收到的数据.
 * @param  error:
 * @param  data: 需要写入的数据的地址.
 * @param  data_len: 需要写入的数据长度.
 * @param  finished:
 * @retval None
 */
void file_write_to_flash(int16_t error, const uint8_t* data, uint16_t data_len, bool finished)
{
    static uint8_t count = 0;

    count++;

    if (count == 5)
    {
        fw_update_stat.read_file_req = false;
    }

    /** oops! 超出 app flash 可用长度了. */
    if (error != 0 || (fw_update_stat.offset + data_len) > APP_MAX_SIZE)
    {
        /** 升级失败. 将 flash 状态信息清除. */
        memset(&fw_update_stat, 0, sizeof(fw_update_stat));

        return;
    }

    /** 写满一个扇区后擦除下一扇区, 一个地址为 8 位. */
    const int32_t curr_sector = get_sector(APP_START_ADDR + fw_update_stat.offset + data_len);

    if (curr_sector > fw_update_stat.last_erased_sector)
    {
        for (int32_t i = fw_update_stat.last_erased_sector + 1; i <= curr_sector; i++)
        {
            /** 擦除. */
            if (erase_app_page(i) != true)
            {
                return;
            }
        }
    }

    /** 写 flash. */
    if (flash_write(APP_START_ADDR + fw_update_stat.offset, data, data_len) != HAL_OK)
    {
		__NOP();
        return;
    }

    /** 更新完成. */
    if (finished != true)
    {
		/** 更新 offset. */
		fw_update_stat.offset += data_len;
		count = 0;
		LED3_TOGGLE();
		
    }
    else
    {
        /** 更新 offset. */
        fw_update_stat.offset += data_len;
        on_update_complete();       
    }
}
