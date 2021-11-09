#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include <parse_rtcm.h>
#include "sdio.h"
#include "gnss.h"
#include "fatfs.h"
#include "uavcan.h"
#include "fix2.h"
#include "Auxiliary.h"
#include "MagneticFieldStrength.h"
#include "gpio.h"

#define NOVA_DEBUGGING 0

/* rtk模式下选择当前是sk1（1）
				飞控还是sk3（0）飞控 */
#define SK1_MODE 0
/*NVTEL_MODE==1，诺瓦泰板卡；NVTEL_MODE==0，和芯星通板卡*/
#define NVTEL_MODE 0
/* ppk模式下选择当前是单天线还是双天线 */
#define SINGLE_ANTENNA 1
#define RTK_MODE 0
#define PPK_MODE 1
#define RTK_TO_SK1_Mode 2
#define BDS_SINGLEONLY 3

// 消息帧对应的ID
#define BESTPOS_ID      0x2A
#define TIME_ID         0x65
#define BDSEPHEMERIS_ID 0x6A0
#define GLOEPHEMERIS_ID 0x2D3
#define RAWEPHEM_ID     0x29
#define RANGE_ID        0x2B
#define RANGECMP_ID     0x8C

#define BD2EPHEM_ID     (1047) /** 压缩的"北斗星历"数据 ID. */
#define GPSEPHEM_ID     (0x07)    /** 压缩的"GPS 星历"数据 ID. */


#define MARKPOS_EVENT1  0xB5    /** novatel event1 消息ID*/
#define MARKPOS_EVENTALL  0x134 /** 和芯星通eventall 消息ID 308*/
#define HEADING_ID      0x3CB
#define PSRDOP2_ID      0x48B
#define BESTXYZ_ID      0xF1

bool rtk_parse(uint8_t data);
//#define NOVA_PREAMBLE1  0xAA
//#define NOVA_PREAMBLE2  0x44
//#define NOVA_PREAMBLE3  0x12 
#define CRC_LENGTH      4

/* RTK Base status */
static rtk_base_status_t rtk_base_status;

static UART_HandleTypeDef* novtel_uart;
static GPS_State state;


static uint8_t rtk_buffer[UART_DMA_RX_UNIT_SIZE];
static uint8_t ppk_buffer[UART_DMA_RX_UNIT_SIZE];
//static uint8_t rtcm_buffer[UART_DMA_RX_BUFF_SIZE];


static uint32_t last_vel_time;
static uint32_t crc_suc_counter = 0;



static nova_msg_parser nova_msg;
static nova_msg_parser nova_msg1;

static bool gnss_initialized = false;
static bool new_position;
static bool new_speed;
static bool fix2_data_update;
static bool auxiliary_update;
static bool MagneticFieldStrength_update;

static bool is_fixed = false;

/* ppk start */
volatile bool utc_time_valid = false;
nova_time utc_time = {1};
uint16_t tim_counter = 0;
uint16_t bds_counter = 0;
uint16_t bd2_counter = 0;
uint16_t gpseph_counter = 0;
uint16_t glo_counter = 0;
uint16_t gps_counter = 0;
uint16_t rangecmp_counter = 0;
uint16_t mark_counter = 0;
uint16_t markevent_counter = 0;
static bool parse_ok_cmd(uint8_t temp_data);
bool rtk_process_message(void);
bool ppk_process_message(void);
#if NOVA_DEBUGGING

#include <cstdio>
# define Debug(fmt, args ...)

do {
    printf("%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args);
    hal.scheduler->delay(1);
} while(0)

#else

# define Debug(fmt, args ...)

#endif

#if NVTEL_MODE
const char* _initialisation_ppk_cmd[8] =
{
	"\r\nunlogall com2\r\n",
	"interfacemode com2 novatel novatel on\r\n",
	"log com2 timeb ontime 0.2\r\n",
	"log com2 rawephemb onnew\r\n",
	"log com2 bdsephemerisb onchanged\r\n",
	"log com2 gloephemerisb onchanged\r\n",
	"log com2 markposb onnew\r\n",

#if SINGLE_ANTENNA
	"log com2 rangecmpb ontime 1\r\n",
#elif
	"log com2 rangeb ontime 1\r\n",
#endif
};

#if SK1_MODE
const char* _initialisation_rtk_cmd[6] =
{
	// 关闭 RTK 的所有消息帧
	"\r\nunlogall com1\r\n",
	"serialconfig com1 115200\r\n",
	"log com1 bestposb ontime 0.2 0 nohold\r\n",
	"log com1 bestvelb ontime 0.2 0 nohold\r\n",
	"log com1 psrdopb  ontime 0.2 0 nohold\r\n",
	 // 配置 RTK 的数据格式
	"interfacemode com1 rtcmv3 novatel off\r\n",
};

#else
const char* _initialisation_rtk_cmd[7] =
{
	// 关闭 RTK 的所有消息帧
	"\r\nunlogall com1\r\n",
	"serialconfig com1 115200\r\n",
	"log com1 bestxyzb  ontime 0.2\r\n",
	"log com1 psrdop2b onchanged\r\n",
	"log com1 timeb ontime 1\r\n",
	"log com1 headingb onchanged\r\n",
	 // 配置 RTK 的数据格式
	"interfacemode com1 rtcmv3 novatel off\r\n",
};
#endif

const char* _initialisation_baud_rate[3] =
{
	"\r\nunlogall true\r\n",
	"com com2 115200 n 8 1 n off\r\n",
	"saveconfig\r\n",
};

#else
const char* _initialisation_ppk_cmd[] =
{
	"\r\nunlogall com2\r\n",
	"CONFIG EVENT ENABLE NEGATIVE 10\r\n",
	//"log com2 timeb ontime 1\r\n",
	/** 和芯板卡eventall 必须配合gga来使用,gga可以不用记录 */
	"log com2 gpgga ontime 0.2\r\n",
	/** 核芯星通天空端板卡的星历数据有问题,需要共用基站端的星历数据 */
	//"log com2 bd2ephemb ontime 60\r\n",     /** 压缩的"北斗星历"数据. */
	//"log com2 gpsephemb ontime 60\r\n",     /** 压缩的"GPS 星历"数据. */
	//"log com2 gloephemerisb ontime 60\r\n", /** 压缩的"格洛纳斯星历"数据. */
	"log com2 timeb ontime 1\r\n",
	"log com2 eventallb onchanged\r\n",

#if SINGLE_ANTENNA
	"log com2 rangecmpb ontime 1\r\n",/** 压缩的"观测"数据. */
#else
	"log com2 rangeb ontime 1\r\n",
#endif
};

#if SK1_MODE
/** sk1需要的数据帧 */
const char* _initialisation_rtk_cmd[7] =
{
	// 关闭 RTK 的所有消息帧
	"\r\nunlogall com1\r\n",
	"com com1 115200 n 8 1 n off\r\n",
	 // 配置成移动站,接收rtcm数据
	"mode rover\r\n",
	"fix none\r\n",
	"log com1 bestposb  ontime 0.2\r\n",
	"log com1 bestvelb  ontime 0.2\r\n",
	"log com1 psrdopb   onchanged\r\n",
};

#else

/*SK1需要的数据帧*/
const char* _initialisation_rtk_to_flight_cmd[8] =
{
	// 关闭 RTK 的所有消息帧
	"\r\nunmask BDS\r\n",
	"unmask glo\r\n",
	 // 配置成移动站,接收rtcm数据
	"unmask gal\r\n",
	"unmask gps\r\n",
	"unmask qzss\r\n",
	"log com3 bestvelb  ontime 0.2\r\n",
	"log com3 psrdopb   onchanged\r\n",
	"saveconfig\r\n",
};

/** sk3需要的数据帧 */
const char* _initialisation_rtk_cmd[] =
{
	// 关闭 RTK 的所有消息帧
	"\r\nunlogall com1\r\n",
	"com com1 115200 n 8 1 n off\r\n",
	//"com com1 460800 n 8 1 n off\r\n",
	 // 配置成移动站,接收rtcm数据
	"mode rover\r\n",
	"fix none\r\n",
	"log com1 bestxyzb  ontime 0.1\r\n",	//241, Best available cartesian position and velocity
	"log com1 psrvelb  ontime 0.1\r\n"
    "log com1 bestposb  ontime 0.1\r\n",	//42, Best position
	"log com1 psrdopb  onchanged\r\n",		//174, Pseudorange DOP
	//"log com1 timeb ontime 1\r\n",
	"log com1 headingb ontime 0.05\r\n",	//971,heading
	"log com1 eventallb onchanged\r\n",
};

#endif

const char* _initialisation_baud_rate[3] =
{
	"\r\n\r\nunlogall com2\r\n",
	"com com2 115200 n 8 1 n off\r\n",
	"saveconfig\r\n",
};
#endif



static const uint8_t NOVA_PREAMBLE1 = 0xaa;
static const uint8_t NOVA_PREAMBLE2 = 0x44;
static const uint8_t NOVA_PREAMBLE3 = 0x12;

/* command mode flag */
static bool novatel_cmd_mode = false;

static Location fix_position = {0, 0, 0};

/* calibration type */
static rtk_calibration_type_e rtk_calibration_type = QX_CALIBRATION;

bool is_gnss_initialized(void)
{
    return gnss_initialized;
}

static enum {
	CMD_O = 0,
	CMD_K,
}novatel_cmd;

static bool parse_ok_cmd(uint8_t temp_data)
{
	 bool status = false;

	switch (novatel_cmd)
    {
		case CMD_O :
				if(temp_data == 'O')
					novatel_cmd = CMD_K;
			break;

		case CMD_K :
				if(temp_data == 'K')
					status = true;
				else
					novatel_cmd = CMD_O;
			break;

		default:
			break;
	}
	return status;
}

/**
  * @brief  get calibration type
  * @param  None
  * @retval The address of calibration type
  */
rtk_calibration_type_e * get_calibration_type(void)
{
    return &rtk_calibration_type;
}

/**
  * @brief  获取当前Novatel命令模式
  * @param  is_enter true：表示已进入命令模式, false：表示未在命令模式
  * @retval None
  */
void enter_cmd_mode(bool is_enter)
{
    novatel_cmd_mode = is_enter;
}

/**
  * @brief  设置fix position 的精度、纬度和高度
  * @param  lat: 纬度
  * @param  lng: 精度
  * @param  lat: 高度
  * @retval None
  */
void set_fix_position(float lat, float lng, float alt)
{
    fix_position.lat = lat;
    fix_position.lng = lng;
    fix_position.alt = alt;
}

/**
  * @brief 获取 RTK 的校准状态
  * @param None
  * @retval 校准状态
  */
GPS_Status get_fix_status(void)
{
    return state.status;
}

/**
  * @brief 判断fix2数据是否更新
  * @param None
  * @retval 
  */
bool is_update_fix2(void)    
{
    return fix2_data_update;
}

void set_status_fix2(void)
{
    fix2_data_update = false;
}

/**
  * @brief 判断Auxiliary数据是否更新
  * @param None
  * @retval 
  */
bool is_update_auxiliary(void)
{
    return auxiliary_update;
}

void set_auxiliary_status(void)
{
    auxiliary_update = false;
}

/**
  * @brief 判断MagneticFieldStrength数据是否更新
  * @param None
  * @retval 
  */
bool is_update_MagneticFieldStrength(void)
{
    return MagneticFieldStrength_update;
}

void set_MagneticFieldStrength_status(void)
{
    MagneticFieldStrength_update = false;
}

/**
  * @brief 获取 RTK 当前位置
  * @param None
  * @retval 经度，维度，高度
  */
uint8_t *get_position(void)
{
    return (uint8_t*)&state.location;
}

GPS_State* get_gnss_state(void)
{
    return &state;
}

/**
  * @brief 获取 Novatel 的卫星数量
  * @param None
  * @retval 卫星数量
  */
uint8_t get_num_of_satellites(void)
{
    return state.num_sats;
}

/**
  * @brief 移动站与基站切换
  * @param type: calibration type
  * @retval None
  */
void switch_base_mode(rtk_calibration_type_e type)
{
    /* switch mode, when type modes are different */
    if (type == NOVATEL_CALIBRATION && rtk_base_status.rtk_type != NOVATEL_CALIBRATION)
    {
        /* switch to novaltel calibration mode */
        rtk_base_status.rtk_type = NOVATEL_CALIBRATION;
        rtk_base_status.rtk_mode = BASE_MODE;
        rtk_base_status.rtk_status = BASE_INIT;
    }
    else if (type == QX_CALIBRATION && rtk_base_status.rtk_type != QX_CALIBRATION)
    {
        /* switch to qianxun service mode */
        rtk_base_status.rtk_type = QX_CALIBRATION;
        rtk_base_status.rtk_mode = ROVER_MODE;
        rtk_base_status.rtk_status = ROVER_INIT;
    }
    else if (type == MANUAL_CALIBRATION && rtk_base_status.rtk_type != MANUAL_CALIBRATION)
    {
        /* switch to command mode */
        rtk_base_status.rtk_type = MANUAL_CALIBRATION;
        rtk_base_status.rtk_mode = CMD_MODE;
    }
}

rtk_base_status_t* get_rtk_base_status(void)
{
    return &rtk_base_status;
}

/**
  * @brief 获取GGA数据
  * @param 接收GGA数据地址
  * @retval len长度
  */
//uint16_t get_gga(uint8_t * gga_msg)
//{
//    memcpy(gga_msg, gga_buffer, gga_len);
//    return gga_len;
//}

/**
  * @brief  获取rtk是否校准成功
  * @retval true:成功，false:未成功
  */
bool get_rtk_status(void)
{
    return is_fixed;
}

/**
  * @brief Rover mode status.
  */
struct
{
    bool in_rover_mode;
    bool is_lock_pos;
} rover_mode_stat;

/**
  * @brief  Enter or exit rover mode.
  * @param  None
  * @retval false, fail
  *         true, success
  */
bool rover_mode_toggle(void)
{
    if ((uint32_t)state.status == true)
    {
        rover_mode_stat.in_rover_mode = false;
        return false;
    }

    if (rover_mode_stat.in_rover_mode == true)
    {
        /* Exit rover mode */
        rover_mode_stat.in_rover_mode = false;
    }
    else
    {
        /* Enter rover mode */
        rover_mode_stat.in_rover_mode = true;
    }

    return true;
}

/**
  * @brief  Lock position imformation in rover mode.
  * @param  None
  * @retval false, fail
  *         true, success
  */
bool rover_mode_lock_pos(void)
{
    if (rover_mode_stat.in_rover_mode == false)
    {
        return false;
    }

    if (rover_mode_stat.is_lock_pos == true)
    {
        /* Unlock position info. */
        rover_mode_stat.is_lock_pos = false;
    }
    else
    {
        /* Lock position info. */
        rover_mode_stat.is_lock_pos = true;
    }

    return true;
}

/**
  * @brief  Get rover mode status.
  * @param  None
  * @retval true, in rover mode.
  *         false, out of rover mode.
  */
bool get_rover_mode_stat(void)
{
    return rover_mode_stat.in_rover_mode;
}

/**
  * @brief  Get status of the lock information.
  * @param  None
  * @retval true, lock.
  *         false, unlock.
  */
bool get_rover_mode_lock_stat(void)
{
    return rover_mode_stat.is_lock_pos;
}
/**
  *  @brief  外部调用接口，用来获取千寻下发的RTCM
  *  @param  len,rtcm的数据长度
  *  @retval None
  */
void send_rtcm(uint8_t * rtcm, uint16_t len)
{
	while(huart2.gState != HAL_UART_STATE_READY)
	{
		/** 当前串口不可用,暂时让出cpu使用权 */
		//HAL_Delay(1);
	}
    HAL_UART_Transmit_DMA(&huart2, rtcm, len);
}

/**
  *  @brief  板卡自动波特率配置
  *  @param  None
  *  @retval None
  */
bool init_baudrate(UART_HandleTypeDef *port)
{

    static uint8_t step = 0;
    static uint8_t index = 0;
    // static uint8_t time_out = 0;
    uint8_t  status = false;
    uint16_t len = 0;
    uint32_t baud_rate_table[] = {115200, 9600, 57600, 43000, 38400, 19200, 14400};
	//uint32_t baud_rate_table[] = {115200};
    novtel_uart = port;

    nova_msg.nova_state = PREAMBLE1;

    const char *init_str1 = _initialisation_baud_rate[0];
    const char *init_str2 = _initialisation_baud_rate[1];
    const char *init_str3 = _initialisation_baud_rate[2];

    switch(step)
    {
    case 0:
        /* Automatic uart baud rate detection*/

        /* Polling switching uart baud rate */
        uart_set_baud(novtel_uart, baud_rate_table[index]);
        /** 等待串口波特率转换完成 */
		HAL_Delay(10);
        if (++index >= sizeof(baud_rate_table)/sizeof(baud_rate_table[0]))
        {
            index = 0;
        }

        /** 等待上一次发送完成 */
        while(novtel_uart->gState != HAL_UART_STATE_READY)
        {
        }
        HAL_UART_Transmit_DMA(novtel_uart, (uint8_t*)init_str1, strlen(init_str1));
        /** 等待DMA接收数据完毕 */
        HAL_Delay(300);
		
		while(1)
		{
			len = uart_read_data_unit(novtel_uart, ppk_buffer,UART_DMA_RX_UNIT_SIZE);
			if(len == 0)
			{
				break;
			}
			else if(len > 0)
			{
				for(uint16_t j = 0; j < len; j++)
				{
					if( parse_ok_cmd(ppk_buffer[j]) == true )
					{
						step++;
						printf("case 0 ok\n");
						/*跳出for循环*/
						break;
					}
				}
				
			}		
		}
        break;

    case 1:
        /* set com2 baud rate to 115200 */
        while(novtel_uart->gState != HAL_UART_STATE_READY)
        {

        }
        HAL_UART_Transmit_DMA(novtel_uart, (uint8_t*)init_str2, strlen(init_str2));
        HAL_Delay(300);

		while(1)
		{
			len = uart_read_data_unit(novtel_uart, ppk_buffer,UART_DMA_RX_UNIT_SIZE);
			if(len == 0)
			{
				break;
			}
			else if(len > 0)
			{
				for(uint16_t j = 0; j < len; j++)
				{
					if( parse_ok_cmd(ppk_buffer[j]) == true )
					{
						step++;
						printf("case 1 ok\n");
						// time_out = 0;
						/*跳出for循环*/
						break;
					}
				}
			}
		}
		
//        if (++time_out == 2)
//        {
//            step = 0;
//            time_out = 0;
//        }
        break;

    case 2:
        /* Reset the baud rate to 115200 */
        uart_set_baud(novtel_uart, 115200);
        HAL_Delay(10);
        /* save config */
        while(novtel_uart->gState != HAL_UART_STATE_READY)
        {
//        	HAL_Delay(1);
        }
        HAL_UART_Transmit_DMA(novtel_uart, (uint8_t*)init_str3, strlen(init_str3));
        HAL_Delay(300);

		while(1)
		{
			len = uart_read_data_unit(novtel_uart, ppk_buffer,UART_DMA_RX_UNIT_SIZE);
			if(len == 0)
			{
				break;
			}
			else if(len > 0)
			{
				for(uint16_t j = 0; j < len; j++)
				{
					if( parse_ok_cmd(ppk_buffer[j]) == true )
					{
						status = true;
						step++;
						printf("case 2 ok\n");
						// time_out = 0;
						/*跳出for循环*/
						break;
					}
				}
			}
	    }
        /* Reset the baud rate to 115200 */
        uart_set_baud(novtel_uart, 115200);
		printf("668:uart_set_baud(novtel_uart, 115200);\n");
        //gga_len = 0;
        //gnss_initialized = true;
        break;

    default:
        break;
    }

    return status;
}

bool init_work_cmd(uint8_t work_mode)
{
	bool status = false;
	const char *init_str = NULL;
	uint8_t j = 0;
	if(work_mode == RTK_MODE)
	{
		j = (sizeof(_initialisation_rtk_cmd) / sizeof(_initialisation_rtk_cmd[0]));
		
	}
	else if(work_mode == PPK_MODE)
	{
		j = (sizeof(_initialisation_ppk_cmd) / sizeof(_initialisation_ppk_cmd[0]));
	}
	else if(work_mode == RTK_TO_SK1_Mode)
	{
		j = (sizeof(_initialisation_rtk_to_flight_cmd) / sizeof(_initialisation_rtk_to_flight_cmd[0]));
	}

    for (uint8_t i = 0; i < j; )
    {
    	if(work_mode == RTK_MODE)
    	{
    		init_str = _initialisation_rtk_cmd[i];
			printf("  rtk_cmd[%d] %s  \n",i,_initialisation_rtk_cmd[i]);
    	}
    	else if(work_mode == PPK_MODE)
    	{
    		init_str = _initialisation_ppk_cmd[i];
			printf("  ppk_cmd[%d] %s  \n",i,_initialisation_ppk_cmd[i]);
    	}
			else if(work_mode == RTK_TO_SK1_Mode)
			{
				init_str = _initialisation_rtk_to_flight_cmd[i];
				printf("  rtk_to_flight_cmd[%d] %s  \n",i,_initialisation_rtk_to_flight_cmd[i]);
			}

        while(novtel_uart->gState != HAL_UART_STATE_READY)
        {
        	/** 当前串口不可用，暂时让出cpu使用权 */
        	//HAL_Delay(1);
        }
        HAL_UART_Transmit_DMA(novtel_uart,(uint8_t*)init_str, strlen(init_str));
        /** 等待串口数据发送完成和板卡返回反馈信息 */
        HAL_Delay(100);
        uint16_t len = uart_read_data(novtel_uart, ppk_buffer);
        if (len > 0)
        {
            for(uint16_t k = 0;k < len; k++)
            {
            	if( parse_ok_cmd(ppk_buffer[k])==true )
            	{
            		/** 判断板卡返回的指令是否存在“OK”，如果存在，说明当前的指令已经生效，接着进行下一条指令的配置 */
					if(work_mode == RTK_MODE)
					{
					printf("  %s   is ok\n\n",_initialisation_rtk_cmd[i]);
					}else if(work_mode == PPK_MODE)
					{
					printf("  %s   is ok\n\n",_initialisation_ppk_cmd[i]);
					}
					else if(work_mode == RTK_TO_SK1_Mode)
					{
						printf("  %s   is ok\n\n",_initialisation_rtk_to_flight_cmd[i]);
					}
            		i++;
            		/** 跳出内层循环 */
            		break;
            	}
            }
        }
        HAL_Delay(20);
    }

    status = true;
    return status;
}

/**
  *  @brief  初始化板卡.
  *  @param  None
  *  @retval None
  */
bool gnss_init(UART_HandleTypeDef *port)
{
	static uint8_t step = 0;
	uint8_t status = false;
	switch(step)
	{
		case 0:/** 第一步:板卡自动波特率检测 */
			if( init_baudrate(port)==true )
			{
				step++;
			}
			break;

		case 1:/** 第二步:配置板卡的rtk指令 */
			if( init_work_cmd(RTK_MODE)==true )
			{
				step++;
			}
			break;

		case 2:/** 第三步:配置板卡的ppk指令 */
			
		
			if( init_work_cmd(PPK_MODE)==true )
			{
				step++;
			}
			break;
		case 3:
			if(init_work_cmd(RTK_TO_SK1_Mode)==true)
			{
				gnss_initialized = true;
				status = true;
			}
			break;
		default :
			break;
	}
	
	
	
	
	
	return status;
}

/**
  *  @brief  创建ppk日志.
  *  @param  None
  *  @retval None
  */
bool create_ppk_log(void)
{
	uint8_t status = false;
	uint16_t len = 0;
	
	while(1)
	{
		len = uart_read_data_unit(novtel_uart, ppk_buffer,UART_DMA_RX_UNIT_SIZE);
		if(len == 0)
		{
			break;
		}
		else if(len > 0)
		{
			for(uint16_t j = 0 ; j < len ; j++)
			{
				parse(ppk_buffer[j]);
			}
		}
	}
	
	if(is_time_valid()==true)
	{
	    MX_SDIO_SD_Init();
		
	    status = MX_FATFS_Init();
		
	    if(status == true)
	    {
	    	status = true;
	    	set_file_valid_or_not(true);
	    }
	}

	return status;
}

/**
  *  @brief  解析ppk数据并且写入到sd卡
  *  @param  None
  *  @retval None
  */
void log_ppk_data()
{
	uint16_t len = 0;

	while(1)
	{
		len = uart_read_data_unit(novtel_uart, ppk_buffer,UART_DMA_RX_UNIT_SIZE);
		if(len == 0)
		{
			break;
		}
		else if(len > 0)
		{
			for(uint16_t j = 0 ; j < len ; j++)
			{
				parse(ppk_buffer[j]);
			}
		}
	}
}

/**
  *  @brief  解析板卡的rtk数据,并且通过uavcan转发给飞控
  *  @param  None
  *  @retval None
  */
void rtk_task()
{
	uint16_t len = 0;

	while(1)
	{
		len = uart_read_data_unit(&huart2, rtk_buffer,UART_DMA_RX_UNIT_SIZE);
		if(len == 0)
		{
			break;
		}
		else if(len > 0)
		{
			for(uint16_t j = 0 ; j < len ; j++)
			{
				rtk_parse(rtk_buffer[j]);
			}
		}
	}
}



bool parse(uint8_t data)
{
	static uint16_t crc_error_counter = 0;
    switch (nova_msg.nova_state)
    {
    default:

    case PREAMBLE1:
        if (data == NOVA_PREAMBLE1)
        {
            nova_msg.nova_state = PREAMBLE2;
        }
        nova_msg.read = 0;
        break;

    case PREAMBLE2:
        if (data == NOVA_PREAMBLE2)
        {
            nova_msg.nova_state = PREAMBLE3;
        }
        else
        {
            nova_msg.nova_state = PREAMBLE1;
        }
        break;

    case PREAMBLE3:
        if (data == NOVA_PREAMBLE3)
        {
            nova_msg.nova_state = HEADERLENGTH;
        }
        else
        {
            nova_msg.nova_state = PREAMBLE1;
        }
        break;

    case HEADERLENGTH:
        nova_msg.header.data[0] = NOVA_PREAMBLE1;
        nova_msg.header.data[1] = NOVA_PREAMBLE2;
        nova_msg.header.data[2] = NOVA_PREAMBLE3;
        nova_msg.header.data[3] = data;
        nova_msg.header.nova_headeru.headerlength = data;
        nova_msg.nova_state = HEADERDATA;
        nova_msg.read = 4;
        break;

    case HEADERDATA:
        if (nova_msg.read >= sizeof(nova_msg.header.data))
        {
            nova_msg.nova_state = PREAMBLE1;
            break;
        }
        nova_msg.header.data[nova_msg.read] = data;
        nova_msg.read++;
        if (nova_msg.read >= nova_msg.header.nova_headeru.headerlength)
        {
            nova_msg.nova_state = DATA_;
        }
        break;

    case DATA_:
        if (nova_msg.read >= sizeof(nova_msg.data))
        {
            nova_msg.nova_state = PREAMBLE1;
            break;
        }
        nova_msg.data.bytes[nova_msg.read - nova_msg.header.nova_headeru.headerlength] = data;
        nova_msg.read++;
        if (nova_msg.read >= (nova_msg.header.nova_headeru.messagelength + nova_msg.header.nova_headeru.headerlength))
        {
            nova_msg.nova_state = CRC1;
        }
        break;

    case CRC1:
        nova_msg.data.bytes[nova_msg.read - nova_msg.header.nova_headeru.headerlength] = data;
        nova_msg.read++;
        nova_msg.crc = (uint32_t) (data << 0);
        nova_msg.nova_state = CRC2;
        break;

    case CRC2:
        nova_msg.data.bytes[nova_msg.read - nova_msg.header.nova_headeru.headerlength] = data;
        nova_msg.read++;
        nova_msg.crc += (uint32_t) (data << 8);
        nova_msg.nova_state = CRC3;
        break;

    case CRC3:
        nova_msg.data.bytes[nova_msg.read - nova_msg.header.nova_headeru.headerlength] = data;
        nova_msg.read++;
        nova_msg.crc += (uint32_t) (data << 16);
        nova_msg.nova_state = CRC4;
        break;

    case CRC4:
        nova_msg.data.bytes[nova_msg.read - nova_msg.header.nova_headeru.headerlength] = data;
        nova_msg.read++;
        nova_msg.crc += (uint32_t) (data << 24);
        nova_msg.nova_state = PREAMBLE1;

        uint32_t crc = CalculateBlockCRC32((uint32_t)nova_msg.header.nova_headeru.headerlength, (uint8_t *)&nova_msg.header.data, (uint32_t)0);
        crc = CalculateBlockCRC32((uint32_t)nova_msg.header.nova_headeru.messagelength, (uint8_t *)&nova_msg.data, crc);

        if (nova_msg.crc == crc)
        {
            crc_suc_counter++;
            return ppk_process_message();
        }
        else
        {
			crc_error_counter++;
			printf(">>>>>>ppk crc failed:%d,len:%d\r\n",crc_error_counter,nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH);
        }
        break;
    }

    return false;
}

bool rtk_parse(uint8_t data)
{
	static uint16_t crc_error_counter = 0;
    switch (nova_msg1.nova_state)
    {
    default:

    case PREAMBLE1:
        if (data == NOVA_PREAMBLE1)
        {
            nova_msg1.nova_state = PREAMBLE2;
        }
        nova_msg1.read = 0;
        break;

    case PREAMBLE2:
        if (data == NOVA_PREAMBLE2)
        {
            nova_msg1.nova_state = PREAMBLE3;
        }
        else
        {
            nova_msg1.nova_state = PREAMBLE1;
        }
        break;

    case PREAMBLE3:
        if (data == NOVA_PREAMBLE3)
        {
            nova_msg1.nova_state = HEADERLENGTH;
        }
        else
        {
            nova_msg1.nova_state = PREAMBLE1;
        }
        break;

    case HEADERLENGTH:
        nova_msg1.header.data[0] = NOVA_PREAMBLE1;
        nova_msg1.header.data[1] = NOVA_PREAMBLE2;
        nova_msg1.header.data[2] = NOVA_PREAMBLE3;
        nova_msg1.header.data[3] = data;
        nova_msg1.header.nova_headeru.headerlength = data;
        nova_msg1.nova_state = HEADERDATA;
        nova_msg1.read = 4;
        break;

    case HEADERDATA:
        if (nova_msg1.read >= sizeof(nova_msg1.header.data))
        {
            nova_msg1.nova_state = PREAMBLE1;
            break;
        }
        nova_msg1.header.data[nova_msg1.read] = data;
        nova_msg1.read++;
        if (nova_msg1.read >= nova_msg1.header.nova_headeru.headerlength)
        {
            nova_msg1.nova_state = DATA_;
        }
        break;

    case DATA_:
        if (nova_msg1.read >= sizeof(nova_msg1.data))
        {
            nova_msg1.nova_state = PREAMBLE1;
            break;
        }
        nova_msg1.data.bytes[nova_msg1.read - nova_msg1.header.nova_headeru.headerlength] = data;
        nova_msg1.read++;
        if (nova_msg1.read >= (nova_msg1.header.nova_headeru.messagelength + nova_msg1.header.nova_headeru.headerlength))
        {
            nova_msg1.nova_state = CRC1;
        }
        break;

    case CRC1:
        nova_msg1.data.bytes[nova_msg1.read - nova_msg1.header.nova_headeru.headerlength] = data;
        nova_msg1.read++;
        nova_msg1.crc = (uint32_t) (data << 0);
        nova_msg1.nova_state = CRC2;
        break;

    case CRC2:
        nova_msg1.data.bytes[nova_msg1.read - nova_msg1.header.nova_headeru.headerlength] = data;
        nova_msg1.read++;
        nova_msg1.crc += (uint32_t) (data << 8);
        nova_msg1.nova_state = CRC3;
        break;

    case CRC3:
        nova_msg1.data.bytes[nova_msg1.read - nova_msg1.header.nova_headeru.headerlength] = data;
        nova_msg1.read++;
        nova_msg1.crc += (uint32_t) (data << 16);
        nova_msg1.nova_state = CRC4;
        break;

    case CRC4:
        nova_msg1.data.bytes[nova_msg1.read - nova_msg1.header.nova_headeru.headerlength] = data;
        nova_msg1.read++;
        nova_msg1.crc += (uint32_t) (data << 24);
        nova_msg1.nova_state = PREAMBLE1;

        uint32_t crc = CalculateBlockCRC32((uint32_t)nova_msg1.header.nova_headeru.headerlength, (uint8_t *)&nova_msg1.header.data, (uint32_t)0);
        crc = CalculateBlockCRC32((uint32_t)nova_msg1.header.nova_headeru.messagelength, (uint8_t *)&nova_msg1.data, crc);

        if (nova_msg1.crc == crc)
        {
            crc_suc_counter++;
            return rtk_process_message();
        }
        else
        {

			crc_error_counter++;
			printf(">>>>>>rtk crc failed:%d,len=%d.\r\n",crc_error_counter,nova_msg1.header.nova_headeru.headerlength + nova_msg1.header.nova_headeru.messagelength + CRC_LENGTH);
        }
        break;
    }

    return false;
}



bool ppk_process_message(void)
{
	uint16_t length = 0;
	mmc_GnssEcef* gnss_ecef = get_gnss_ecef();
    uint16_t messageid = nova_msg.header.nova_headeru.messageid;
//	printf("PPK Message ID = %d\r\n",messageid);
    switch(messageid)
    {

        case 42:/* bestpos msg*/
            state.time_week = nova_msg.header.nova_headeru.week;
            state.time_week_ms = (uint32_t) nova_msg.header.nova_headeru.tow;
            state.last_gps_time_ms = HAL_GetTick();

            state.location.lat = nova_msg.data.bestposu.lat;
            state.location.lng = nova_msg.data.bestposu.lng;
            state.location.alt = nova_msg.data.bestposu.hgt;

            state.num_sats = nova_msg.data.bestposu.svsused;

            state.horizontal_accuracy = (float) ((nova_msg.data.bestposu.latsdev + nova_msg.data.bestposu.lngsdev)/2);
            state.vertical_accuracy = (float) nova_msg.data.bestposu.hgtsdev;
            state.have_horizontal_accuracy = true;
            state.have_vertical_accuracy = true;
            state.rtk_age_ms = nova_msg.data.bestposu.diffage * 1000;
            state.rtk_num_sats = nova_msg.data.bestposu.svsused;

            state.status = (GPS_Status)nova_msg.data.bestposu.postype;

            new_position = true;
            break;
        case 99: //bestvel
            state.ground_speed = (float) nova_msg.data.bestvelu.horspd;
            state.ground_course = (float) nova_msg.data.bestvelu.trkgnd;
            state.velocity.z = -(float) nova_msg.data.bestvelu.vertspd;
            state.have_vertical_velocity = true;

            last_vel_time = (uint32_t) nova_msg.header.nova_headeru.tow;		
            new_speed = true;
            break;

        case 174:
            state.hdop = (uint16_t) (nova_msg.data.psrdopu.hdop*100);
            state.vdop = (uint16_t) (nova_msg.data.psrdopu.htdop*100);
            break;

        /** PPK start */
        case TIME_ID://101
            if(nova_msg.data.time.clock_stat == 0 && nova_msg.data.time.utc_stat == 1)
            {
                utc_time_valid = true;
                utc_time = nova_msg.data.time;
				
				length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
				update_timestamp(gnss_ecef,nova_msg.data.time);

            }
#if 0
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
#endif
            tim_counter ++;
            break;

        case BDSEPHEMERIS_ID:
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            bds_counter ++;
            break;

        case BD2EPHEM_ID:
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            bd2_counter ++;
            break;

        case GPSEPHEM_ID:
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            gpseph_counter ++;
            break;

        case GLOEPHEMERIS_ID:
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            glo_counter ++;
            break;

        case RAWEPHEM_ID:
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            gps_counter ++;
            break;

        case RANGECMP_ID://140
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            rangecmp_counter ++;
            break;

        case MARKPOS_EVENT1:
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            mark_counter ++;
						LED3_TOGGLE();
            break;
        case MARKPOS_EVENTALL:
            length = nova_msg.header.nova_headeru.headerlength + nova_msg.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg.header.data),length);
            markevent_counter ++;
						LED3_TOGGLE();
            break;
				
        default:

            break;
    }


    /** PPK end **/
    static uint32_t last_time = 0;

    if( (HAL_GetTick()-last_time) >= 5000 )
    {
    	printf("\r\n>>>>>>>>ppk_cnt:tim:%4d,bd2:%4d,gpse:%4d,bds:%4d,glo:%4d,gps:%4d,rangecmp:%4d,eventall_id:%4d,mark:%4d.<<<<<<<<<\r\n"
			,tim_counter,bd2_counter,gpseph_counter,bds_counter,glo_counter,gps_counter,rangecmp_counter,markevent_counter,mark_counter);
    	last_time = HAL_GetTick();
    }
    /* ensure out position and velocity stay insync */
    if (new_position && new_speed && last_vel_time == state.time_week_ms)
    {
        new_speed = new_position = false;
        return true;
    }

    return false;
}

bool rtk_process_message()
{
	static uint16_t best_id = 0;
	static uint16_t psrp2_id = 0;
	static uint16_t head_id = 0;
	static uint16_t rtkdata_id = 0;
	
	uint16_t length = 0;

    uint16_t messageid = nova_msg1.header.nova_headeru.messageid;
    mmc_GnssEcef* gnss_ecef = get_gnss_ecef();
	if(messageid == 174)
	printf("RTK Message ID = %d\r\n",messageid);

    switch(messageid)
    {
				
				case 42:/* bestpos msg*/
            state.time_week = nova_msg1.header.nova_headeru.week;
            state.time_week_ms = (uint32_t) nova_msg1.header.nova_headeru.tow;
            state.last_gps_time_ms = HAL_GetTick();

            state.location.lat = nova_msg1.data.bestposu.lat;
            state.location.lng = nova_msg1.data.bestposu.lng;
            state.location.alt = nova_msg1.data.bestposu.hgt;

            state.num_sats = nova_msg1.data.bestposu.svsused;

            state.horizontal_accuracy = (float) ((nova_msg1.data.bestposu.latsdev + nova_msg1.data.bestposu.lngsdev)/2);
            state.vertical_accuracy = (float) nova_msg1.data.bestposu.hgtsdev;
            state.have_horizontal_accuracy = true;
            state.have_vertical_accuracy = true;
            state.rtk_age_ms = nova_msg1.data.bestposu.diffage * 1000;
            state.rtk_num_sats = nova_msg1.data.bestposu.svsused;

            state.status = (GPS_Status)nova_msg1.data.bestposu.postype;
            state.postype = nova_msg1.data.bestposu.postype;
        
            update_fix2_data(&nova_msg1);
			
            new_position = true;			//位置更新标志
			
            fix2_data_update = true;
            break;
        case 99:		//speed---bestvelb
            state.ground_speed = (float) nova_msg1.data.bestvelu.horspd;
            state.ground_course = (float) nova_msg1.data.bestvelu.trkgnd;
            state.velocity.z = -(float) nova_msg1.data.bestvelu.vertspd;
            state.have_vertical_velocity = true;

            last_vel_time = (uint32_t) nova_msg1.header.nova_headeru.tow;
		
			update_fix2_bestvel(&nova_msg1);
            new_speed = true;
            break;

        case 174:		//auxiliary---PSRDOP
            state.hdop = (uint16_t) (nova_msg1.data.psrdopu.hdop*100);
            state.vdop = (uint16_t) (nova_msg1.data.psrdopu.htdop*100);
            
            update_auxiliary(&nova_msg1);
						update_fix2_psrdop(&nova_msg1);
            auxiliary_update = true;
            break;
			

        /** RTK start */
        case TIME_ID://101
            if(nova_msg1.data.time.clock_stat == 0 && nova_msg1.data.time.utc_stat == 1)
            {
                utc_time_valid = true;
                utc_time = nova_msg1.data.time;

//                if( is_gnss_init_ok()==1 )
//                {
//                	length = nova_msg1.header.nova_headeru.headerlength + nova_msg1.header.nova_headeru.messagelength + CRC_LENGTH;

//                	update_timestamp(gnss_ecef,nova_msg1.data.time);
//                }
            }
#if 0
            length = nova_msg1.header.nova_headeru.headerlength + nova_msg1.header.nova_headeru.messagelength + CRC_LENGTH;
            fatfs_log_message((nova_msg1.header.data),length);
#endif
            tim_counter ++;
            break;

            // 以下是SK3 RTK需要的功能
			
			//bestxyz；GnssEcef
			
		case BESTXYZ_ID://241
			length = nova_msg1.header.nova_headeru.headerlength + nova_msg1.header.nova_headeru.messagelength + CRC_LENGTH;
			best_id++;
			update_bestxyz(gnss_ecef, nova_msg1.data.bestxyzb);
		update_fix2_bestxyz(&nova_msg1);
			break;

		case 215:/*RTKDATA*/
			length = nova_msg1.header.nova_headeru.headerlength + nova_msg1.header.nova_headeru.messagelength + CRC_LENGTH;
      rtkdata_id++;
			update_fix2_rtkdata(&nova_msg1);
			break;
		
		case PSRDOP2_ID:	//nova板卡配置，和星芯通没有
			length = nova_msg1.header.nova_headeru.headerlength + nova_msg1.header.nova_headeru.messagelength + CRC_LENGTH;
			psrp2_id++;
			update_psrdop2(gnss_ecef, nova_msg1.data.psrdop2b);
			break;

		case HEADING_ID:	//MagneticFieldStrength--971
			length = nova_msg1.header.nova_headeru.headerlength + nova_msg1.header.nova_headeru.messagelength + CRC_LENGTH;

			head_id++;
			update_heading(gnss_ecef, nova_msg1.data.headingb);
		
			if (nova_msg1.data.headingb.pos_type == 50)		
			{
				update_MagneticFieldStrength_data(&nova_msg1);
				MagneticFieldStrength_update = true;
			}
			
			break;

        default:

            break;
    }


    /** RTK end **/
    static uint32_t last_time = 0;

    if( (HAL_GetTick()-last_time) >= 5000 )
    {
    	printf("\r\n>>>>>>>rtk_cnt:time_id = %d ,bestxyz_id = %d , psrp2_id = %d , heading_id = %d.<<<<<<<<<\r\n",tim_counter,best_id,psrp2_id,head_id);
    	last_time = HAL_GetTick();
    }
    /* ensure out position and velocity stay insync */
    if (new_position && new_speed && last_vel_time == state.time_week_ms)
    {
        new_speed = new_position = false;
        return true;
    }

    return false;
}


#define CRC32_POLYNOMIAL 0xEDB88320L

uint32_t CRC32Value(uint32_t icrc)
{
    int i;
    uint32_t crc = icrc;
    for ( i = 8 ; i > 0; i-- )
    {
        if ( crc & 1 )
        {
            crc = ( crc >> 1 ) ^ CRC32_POLYNOMIAL;
        }
        else
        {
            crc >>= 1;
        }
    }
    return crc;
}

uint32_t CalculateBlockCRC32(uint32_t length, uint8_t *buff, uint32_t crc)
{
    while ( length-- != 0 )
    {
        crc = ((crc >> 8) & 0x00FFFFFFL) ^ (CRC32Value(((uint32_t) crc ^ *buff++) & 0xff));
    }
    return( crc );
}

/**
* @brief 询问当前的时间是否可用
*/
bool is_time_valid(void)
{
    return utc_time_valid;
}

/**
* @brief 获取当前时间
* @retval 时间指针
*/
nova_time* get_utc_time(void)
{
    return &utc_time;
}

void ppk_task()
{
	static uint8_t step = 0;
//	uint8_t status = false;
	
	switch(step)
	{
		case 0:
			if(create_ppk_log() == true)
			{
				step = 1;
			}
			break;
		
		case 1:
			log_ppk_data();
			break;	
		
		default:
			break;		
	}
}
