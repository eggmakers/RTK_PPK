/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#if 0

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */



bool fatfs_test()
{
	uint32_t btw = 0;
	static uint8_t i = 0;
	static uint8_t step = 0;
	char test_buf[] = {"这是一个sd卡 fatfs 测试文件.\r\n"};
		
	switch(step)
	{
		case 0:
			if((f_mount(&SDFatFS, (TCHAR const*)SDPath, 1)==0))
			{
				step++;
			}
			break;
			
		case 1: 
			if( f_mkdir("sd_test")== FR_OK || f_mkdir("sd_test")== FR_EXIST)
			{
				step++;
			}
			break;	

		case 2: 
			if( f_open(&SDFile, "sd_test/tes.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ)==FR_OK )
			{
				step++;
			}
			break;	
			
		case 3:
		if(i <=100)
		{
			if(f_write(&SDFile,&test_buf[0],strlen(test_buf),&btw) == FR_OK)
			{
				i++;
				f_sync(&SDFile);
			}
			
		}
		else
		{
			f_close(&SDFile);
			step++;
		}			
		break;
		
		default:
			
			break;	
	}
	
	if(step > 3)
	{
		return true;
	}
	else
	{
		return false;
	}	
}
/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */     
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}
#endif
/* USER CODE BEGIN Application */

/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"
#include "gnss.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include "sdio.h"
 

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

static bool file_valid = false;

int8_t create_folder(void);
int8_t create_file(void);
void get_file_name(char *p_name,char const *p_ext_name);
uint8_t num2char(uint8_t num);

/**
  * @brief  根据板卡的utc时间创建ppk日志
  * @param  None
  * @retval true is ok, otherwise is fail
  */
//bool MX_FATFS_Init(void) 
//{
//  /*## FatFS: Link the SD driver ###########################*/
//  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

//  /* USER CODE BEGIN Init */
//  /* additional user code for init */     
//  /* USER CODE END Init */
//}

bool MX_FATFS_Init(void)
{

    /*## FatFS: Link the SD driver ###########################*/
//	 f_res |= FATFS_LinkDriver(&SD_Driver, SDPath);
	static uint8_t step = 0;
	uint8_t status = false;
	switch(step)
	{
		case 0:
			if(FATFS_LinkDriver(&SD_Driver, SDPath)!=0)
			{
			}
			else
			{
				step++;
			}				
			break;
		
		case 1:
			if(create_folder()!=0)// 挂载SD卡并创建文件夹
			{
			}
			else
			{
				step++;
			}				
			break;	
		
		case 2:
			if(create_file()!=0)// 挂载SD卡并创建文件夹
			{
			}
			else
			{
				status = true;
				step++;
			}				
			break;	
			
		default:
			break;
	}
	
	return status;
}

extern SD_HandleTypeDef hsd;
void MX_FATFS_DeInit(void)
{
	if(file_valid == true)
	{
		close_file();
		HAL_SD_MspDeInit(&hsd);
		HAL_SD_DeInit(&hsd);
		FATFS_UnLinkDriver(SDPath);
		f_mount(NULL, (TCHAR const*)SDPath, 1);
	}

}
/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
    /* USER CODE BEGIN get_fattime */
    return 0;
    /* USER CODE END get_fattime */
}

/**
  * @brief  挂载SD卡并创建工作目录
  * @param  None
  * @retval RF_OK:成功，否则不成功
  */
int8_t create_folder(void)
{
    FRESULT f_res;

    f_res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1);     // 挂载SD卡

    if(f_res == FR_NO_FILESYSTEM)                           // 如果没有文件系统
    {
        BYTE work[_MAX_SS];

        f_res = f_mkfs((TCHAR const*)SDPath, FM_FAT, 0, work, sizeof(work));    // 格式化创建文件系统

        if(f_res == FR_OK)
        {
            f_res = f_mount(NULL, (TCHAR const*)SDPath, 1);                     // 格式化后需要取消挂载，并重新挂载一次

            f_res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1);
        }
    }
    return f_res;
}

/**
  * @brief  通过系统时间创建文件
  * @param  None
  * @retval 0:成功，否则不成功
  */
int8_t create_file(void)
{
    const char dir[] = "ppk_sky";                           // 工作目录
    char file_name[PPK_FILE_NAME_SIZE];                     // 文件名
    char file_path[PPK_FILE_NAME_SIZE + sizeof(dir) + 1];   // 文件路径

    f_mkdir(dir);                                           // 创建工作目录，若存在，则不创建

    get_file_name(file_name,".bin");                    // 通过系统时间创建文件名
    snprintf(file_path,sizeof(file_path),"%s/%s",dir,file_name);    // 连接文件名和工作目录

    if(FR_OK != f_open(&SDFile,file_path,FA_CREATE_ALWAYS | FA_WRITE | FA_READ))    // 新建/打开文件
    {
    	return 1;
    }
    return 0;
}

/**
  * @brief  获取文件名
  * @param  p_name:文件名返回
  * @param  p_ext_name:扩展名设定
  * @retval None
  */

void get_file_name(char *p_name,char const *p_ext_name)
{
	nova_time *utc_time = get_utc_time();                  // 获取系统时间结构体
    memcpy(p_name,"sky_",4);                                // 文件名前缀

    p_name[4] =  num2char(utc_time->utc_year / 1000);
    p_name[5] =  num2char(utc_time->utc_year % 1000 / 100);
    p_name[6] = num2char(utc_time->utc_year % 100 / 10);
    p_name[7] = num2char(utc_time->utc_year % 10);

    p_name[8] = num2char(utc_time->utc_month / 10);
    p_name[9] = num2char(utc_time->utc_month % 10);

    p_name[10] = num2char(utc_time->utc_day / 10);
    p_name[11] = num2char(utc_time->utc_day % 10);

    p_name[12] = num2char(utc_time->utc_hour / 10);
    p_name[13] = num2char(utc_time->utc_hour % 10);

    p_name[14] = num2char(utc_time->utc_min / 10);
    p_name[15] = num2char(utc_time->utc_min % 10);

    memcpy(&p_name[16],p_ext_name,4);                       // 文件扩展名
}

/**
  * @brief  把数字转换成对应的字符
  * @param  num；需要转换的数字
  * @retval None
  */
uint8_t num2char(uint8_t num)
{
    return num + '0';
}

/**
  * @brief  关闭文件
  * @param  None
  * @retval None
  */
void close_file(void)
{
	if(file_valid == true)
	{
		f_close(&SDFile);
	}
}

/**
  * @brief  把指定地址和长度的数据写入到SD卡
  * @param  data；需要写入的数据地址
  * @param  size：数据的长度
  * @retval None
  */
static uint16_t write_sd_ok_cnt = 0;

void fatfs_log_message(uint8_t* data,uint16_t size)
{
	bool led_flag = false;
	static uint32_t sd_err = 0;
  UINT fnum;
  FRESULT result = FR_DISK_ERR;
  if(file_valid == true )      // 判断文件是否可用
  {
    uint8_t i = 0;
    for(; i < size / 511; i++)          // 每次最多写入511个字节，否则会出错
    {
      result = f_write(&SDFile,&data[SIGLE_WRITE_DATA_SIZE * i],SIGLE_WRITE_DATA_SIZE,&fnum);
			if(result==0 && fnum==SIGLE_WRITE_DATA_SIZE) //写入成功
			{
					led_flag = true;
			}
    }

		if( (size % SIGLE_WRITE_DATA_SIZE) > 0 )
		{
			result = f_write(&SDFile, &data[SIGLE_WRITE_DATA_SIZE * i], size % SIGLE_WRITE_DATA_SIZE, &fnum);    // 写入剩余的字节数

			if(result==0 && fnum==(size % SIGLE_WRITE_DATA_SIZE) ) //写入成功
			{
				led_flag = true;
			}
		}

		if(led_flag == true)
		{
			write_sd_ok_cnt++;
			f_sync(&SDFile);
			/** 每写卡成功10次,就让绿灯闪烁*/
			if(write_sd_ok_cnt%10==0)
			{
				printf(">>>>>write_sd_ok_cnt:%d<<<<<\r\n",write_sd_ok_cnt);
				//LED_GREEN_TOGGLE();
				LED2_TOGGLE();
			}
		}
		else
		{
			printf(">>>>>write sd err:%d<<<<<\r\n",++sd_err);
		}
	}
}

/**
  * @brief  返回write_sd_ok_cnt的值,在led_task中会根据这个值进行led闪烁
  * @param  None
  * @retval None
  */
uint16_t get_write_sd_ok_value()
{
	return write_sd_ok_cnt;
}

/**
  * @brief  设置文件可用，在创建文件之前，系统能接收到数据帧，并执行写入操作，但是此时无法正常写入，需要系统获取到稳定的时间和创建文件之后才能正确写入数据
  * @param  None
  * @retval None
  */

//void set_file_valid(void)
//{
//    file_valid = true;
//}

void set_file_valid_or_not(bool file_mode)
{
    file_valid = file_mode;
}

bool get_file_valid_or_not()
{
	return file_valid;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
