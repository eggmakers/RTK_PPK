#ifndef __gnss_H
#define __gnss_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "usart.h"
#include "gps.h"
#include "usart_dma2ringbuf.h"

#define PRASE_RTK 0
#define PRASE_PPK 1

typedef enum
{
	GNSS_PORT1 = 0,
	GNNS_PORT2,
} gnss_port_e;

bool is_update_fix2(void);
void set_status_fix2(void);

bool is_update_auxiliary(void);
void set_auxiliary_status(void);

bool is_update_MagneticFieldStrength(void);
void set_MagneticFieldStrength_status(void);


/**
 * @brief 获取GGA数据
 * @param 接收GGA数据地址
 * @retval len长度
*/
uint16_t get_gga(uint8_t * gga_msg);
bool get_rtk_status(void);
void send_rtcm(uint8_t * rtcm, uint16_t len);

/**
  * @brief  GNSS Initialization.
  * @param  port @see UART_HandleTypeDef.
  * @retval None
  */
bool create_ppk_log(void);
void log_ppk_data(void);

/**
  * @brief  验证串口是否稳定.
  * @param  None
  * @retval None
  */
void check_gnss_data(void);

/**
  * @brief  Get data count.
  * @param  None
  * @retval Data counter pointer.
  */
void* get_data_counter(void);

bool gnss_update(void);
void inject_data(const uint8_t *data, uint16_t len);
bool parse(uint8_t data);
bool process_message(void);
uint32_t CRC32Value(uint32_t icrc);
uint32_t CalculateBlockCRC32(uint32_t length, uint8_t *buffer, uint32_t crc);

GPS_Status get_fix_status(void);
uint8_t *get_position(void);
uint8_t get_num_of_satellites(void);

typedef enum
{
    NOVATEL_CALIBRATION = 0,
    QX_CALIBRATION,
    MANUAL_CALIBRATION,
    RESERVED_CALIBRATION = 0xFFFF,
} rtk_calibration_type_e;

typedef struct
{
    enum {
        ROVER_MODE = 0,
        BASE_MODE,
        CMD_MODE,
    } rtk_mode;

    enum {
        ROVER_INIT = 0,
        BASE_INIT,
        CMD_MODE_INIT,
        RTK_UPDATE,
    } rtk_status;

    rtk_calibration_type_e rtk_type;

} rtk_base_status_t;

#pragma pack(4)
typedef struct
{
    // 0
    uint8_t preamble[3];
    // 3
    uint8_t headerlength;
    // 4
    uint16_t messageid;
    // 6
    uint8_t messagetype;
    //7
    uint8_t portaddr;
    //8
    uint16_t messagelength;
    //10
    uint16_t sequence;
    //12
    uint8_t idletime;
    //13
    uint8_t timestatus;
    //14
    uint16_t week;
    //16
    uint32_t tow;
    //20
    uint32_t recvstatus;
    // 24
    uint16_t resv;
    //26
    uint16_t recvswver;
} nova_header;

#pragma pack(4)
typedef struct
{
    float gdop;
    float pdop;
    float hdop;
    float htdop;
    float tdop;
    float cutoff;
    uint32_t svcount;
    // extra data for individual prns
} psrdop;

#pragma pack(4)
typedef struct
{
    uint32_t solstat;      ///< Solution status
    uint32_t postype;      ///< Position type
    double lat;            ///< latitude (deg)
    double lng;            ///< longitude (deg)
    double hgt;            ///< height above mean sea level (m)
    float undulation;      ///< relationship between the geoid and the ellipsoid (m)
    uint32_t datumid;      ///< datum id number
    float latsdev;         ///< latitude standard deviation (m)
    float lngsdev;         ///< longitude standard deviation (m)
    float hgtsdev;         ///< height standard deviation (m)
    // 4 bytes
    uint8_t stnid[4];      ///< base station id
    float diffage;         ///< differential position age (sec)
    float sol_age;         ///< solution age (sec)
    uint8_t svstracked;    ///< number of satellites tracked
    uint8_t svsused;       ///< number of satellites used in solution
    uint8_t svsl1;         ///< number of GPS plus GLONASS L1 satellites used in solution
    uint8_t svsmultfreq;   ///< number of GPS plus GLONASS L2 satellites used in solution
    uint8_t resv;          ///< reserved
    uint8_t extsolstat;    ///< extended solution status - OEMV and greater only
    uint8_t galbeisigmask;
    uint8_t gpsglosigmask;
} bestpos;

#if NVTEL_MODE
#pragma pack(4)
typedef struct
{
    uint32_t sol_status;
    uint32_t pos_type;
    double latitude;
    double lontitude;
    double height;
    float undulation;
    uint32_t datum_id;
    float lat_de;
    float lon_de;
    float hgt_de;
    char stn_id[4];
    float diff_age;
    float sol_age;
    uint8_t sv_s;
    uint8_t soln_sv_s;
    uint8_t ggl1;
    uint8_t soln_multisv_s;
    uint8_t reverse;
    uint8_t ext_sol_stat;
    uint8_t gal_bds_mask;
    uint8_t gps_glo_mask;
}markpos;
#pragma pack()/** 取消字节对齐. */

#else
#pragma pack(4)
typedef struct
{
	uint8_t  event_id;
	uint8_t  event_status;
	uint8_t  reserved0;
	uint8_t  reserved1;
	uint32_t week;
	uint32_t second;
	uint32_t sub_second;
	uint32_t reserved2;
	uint32_t offset_second;
	uint32_t offset_subsecond;

    uint32_t sol_status;
    uint32_t pos_type;
    double latitude;
    double lontitude;
    double height;
    float undulation;
    uint32_t datum_id;
    float lat_de;
    float lon_de;
    float hgt_de;
    char stn_id[4];
    float diff_age;
    float sol_age;
    uint8_t sv_s;
    uint8_t soln_sv_s;
    uint8_t ggl1;
    uint8_t soln_multisv_s;
    uint8_t reverse;
    uint8_t ext_sol_stat;
    uint8_t gal_bds_mask;
    uint8_t gps_glo_mask;
}markpos;
#pragma pack()/** 取消字节对齐. */
#endif

#pragma pack(4)
typedef struct
{
    uint32_t prn;      ///卫星 PRN 编号（BDS 1 到 63）
    double   tow;      ///子帧 1 的时间标识（基于 GPS时间），s
    uint32_t health;   ///健康状态–在北斗 ICD 中定义的 一个 1 比特的健康代码
    uint32_t adoe;     ///星历数据龄期
    uint32_t adoe_1;   ///星历数据龄期（同字段 5）
    uint32_t week;	   ///GPS 周计数（GPS Week）
    uint32_t z_week;   ///基于 GPS 周的 Z 计数周数，为星历子帧 1 的周数。

    double   toe;	   ///星历参考时刻*（北斗时的周内秒），s
    double   a;		   ///轨道长半轴
    double   n;		   ///卫星平均角速度的改正值，rad/s
    double   m0;	   ///参考时间的平近点角，rad
    double   ecc;	   ///偏心率
    double   w;		   ///近地点幅角，rad

    double   cuc;	   ///纬度幅角（余弦振幅，rad）
    double   cus;	   ///纬度幅角（正弦振幅，rad）
    double   crc;	   ///轨道半径（余弦振幅，m）
    double   crs;	   ///轨道半径（正弦振幅，m）
    double   cic;	   ///倾角（余弦振幅，rad）
    double   cis;	   ///倾角（正弦振幅，rad）

    double   I0;	   ///参考时时刻轨道倾角，rad
    double   idot;	   ///轨道倾角变化率，rad/s
    double   o_0;	   ///升交点赤经，rad
    double   o_dot;	   ///升交点赤经变化率，rad/s
    uint32_t   aodc;	   ///时钟数据龄期
    double   toc;	   ///卫星钟差参考时间（基于 GPS 时间），s

    double   tgd1;	   ///B1群延迟（B1星上设备时延差），s
    double   tgd2;	   ///B2 群延迟（B2 星上设备时延差），s
    double   af0;	   ///卫星钟差参数，s
    double   af1;	   ///卫星钟速参数，s/s
    double   af2;	   ///卫星钟漂参数，s/s/s
    uint32_t     anti_fraud;///反欺骗
    double   N;	       ///改正平均角速度，rad/s
    double   ura;	   ///用户距离精度，m2。

} bd2ephem;


#pragma pack(4)
typedef struct
{
    uint32_t prn;      ///卫星 PRN 编号（BDS 1 到 63）
    double   tow;      ///子帧 1 的时间标识（基于 GPS时间），s
    uint32_t health;   ///健康状态–在北斗 ICD 中定义的 一个 1 比特的健康代码
    uint32_t iode1;     ///星历数据龄期
    uint32_t iode2;   ///星历数据龄期（同字段 5）
    uint32_t week;	   ///GPS 周计数（GPS Week）
    uint32_t z_week;   ///基于 GPS 周的 Z 计数周数，为星历子帧 1 的周数。

    double   toe;	   ///星历参考时刻*（北斗时的周内秒），s
    double   a;		   ///轨道长半轴
    double   n;		   ///卫星平均角速度的改正值，rad/s
    double   m0;	   ///参考时间的平近点角，rad
    double   ecc;	   ///偏心率
    double   w;		   ///近地点幅角，rad

    double   cuc;	   ///纬度幅角（余弦振幅，rad）
    double   cus;	   ///纬度幅角（正弦振幅，rad）
    double   crc;	   ///轨道半径（余弦振幅，m）
    double   crs;	   ///轨道半径（正弦振幅，m）
    double   cic;	   ///倾角（余弦振幅，rad）
    double   cis;	   ///倾角（正弦振幅，rad）

    double   I0;	   ///参考时时刻轨道倾角，rad
    double   idot;	   ///轨道倾角变化率，rad/s
    double   o_0;	   ///升交点赤经，rad
    double   o_dot;	   ///升交点赤经变化率，rad/s
    uint32_t   iodc;	   ///时钟数据龄期
    double   toc;	   ///卫星钟差参考时间（基于 GPS 时间），s

    double   tgd;	   ///群延迟，s
    double   af0;	   ///卫星钟差参数，s
    double   af1;	   ///卫星钟速参数，s/s
    double   af2;	   ///卫星钟漂参数，s/s/s
    uint32_t anti_fraud;///反欺骗
    double   N;	       ///改正平均角速度，rad/s
    double   ura;	   ///用户距离精度，m2。

} gpsephem;

//typedef struct
//{
//    float gdop;
//    float pdop;
//    float hdop;
//    float htdop;
//    float tdop;
//    uint32_t cutoff;
//    uint32_t prn_num;
//    
//    uint32_t prn1;
//    uint32_t prn2;
//    uint32_t prn3;
//    uint32_t prn4;
//}psrdop;

typedef struct
{
    float gdop;
    float pdop;
    float hdop;
    float vdop;
    uint32_t num_sys;

    uint32_t sys_1;
    uint32_t tdop_1;

    uint32_t sys_2;
    uint32_t tdop_2;

    uint32_t sys_3;
    uint32_t tdop_3;

    uint32_t sys_4;
    uint32_t tdop_4;
}psrdop2;

typedef struct
{
    uint32_t pos_sol_stat;
    uint32_t pos_type;
    double pos_x;
    double pos_y;
    double pos_z;
    float pos_x_delta;
    float pos_y_delta;
    float pos_z_delta;
    uint32_t vel_sol_status;
    uint32_t vel_type;
    double vel_x;
    double vel_y;
    double vel_z;
    float vel_x_delta;
    float vel_y_delta;
    float vel_z_delta;
    char stn_id[4];
    float vel_latency;
    float diff_age;
    float sol_age;
    uint8_t svs;
    uint8_t soln_svs;
    uint8_t ggl1;
    uint8_t soln_multi_svs;
    char reserved;
    uint8_t ext_sol_stat;
    uint8_t gal_bds_sig_mask;
    uint8_t gps_glo_sig_mask;
}bestxyz;


typedef struct
{
    uint32_t sol_stat;
    uint32_t pos_type;
    float length;
    float heading;
    float pitch;
    float reserved;
    float hdg_std_dev;
    float pitch_std_dev;
    char stn_id[4];
    uint8_t svs;
    uint8_t soln_svs;
    uint8_t obs;
    uint8_t multi;
    uint8_t sol_source;
    uint8_t ext_sol_stat;
    uint8_t gal_bds_sig_mask;
    uint8_t gps_glo_sig_mask;
}heading;


#pragma pack(4)
typedef struct
{
    uint32_t solstat;
    uint32_t veltype;
    float latency;
    float age;
    double horspd;
    double trkgnd;
    // + up
    double vertspd;
    float resv;
} bestvel;

#pragma pack(4)
typedef struct
{
    uint32_t clock_stat;        ///< Clock Model Status
    double offset;              ///< Receiver clock offset in seconds from GPS reference time
    double offset_std;          ///< Receiver clock offset standard deviation
    double utc_offset;          ///< The offset of GPS reference time from UTC time
    uint32_t utc_year;          ///< UTC year
    uint8_t utc_month;          ///< UTC month
    uint8_t utc_day;            ///< UTC day
    uint8_t utc_hour;           ///< UTC hour
    uint8_t utc_min;            ///< UTC minute
    uint32_t utc_ms;            ///< UTC millisecond
    uint32_t utc_stat;          ///< UTC status
} nova_time;

#pragma pack(4)
typedef struct
{
    uint32_t satellite_id;      ///< ID/ranging code
    uint32_t week;              ///< week number
    double ura;                 ///< user range accuracy(meters).this is the evaluated URAI/URA lookup-table value
    uint32_t health;            ///< autonomous satellite health flag.
    double tgd1;                ///< equipment group delay differential for the B1 signal(seconds)
    double tgd2;                ///< equipment group delay differential for the B2 signal(seconds)
    uint32_t aodc;              ///< age of data,clock
    uint32_t toc;               ///< reference time of clock parameters
    double a0;                  ///< constant term of clock correction polynomial
    double a1;                  ///< Linear term of clock correction polynomial
    double a2;                  ///< quadratic term of clock correction polynomial
    uint32_t aode;              ///< age of data,ephemeris
    uint32_t toe;               ///< reference time of ephemeris parameters
    double root_a;              ///< square root of semi-major axis
    double ecc;                 ///< eccentricityO(sqrt(metres))
    double argu_of_perigee;     ///< argument of perigee
    double mean_motion_diff;    ///< mean motion defference from computed value
    double mean_anomaly;        ///< mean anomaly at reference time
    double longtd_of_ascend;    ///< longitude of ascending node of orbital of plane computed according to reference time
    double rate_right_ascen;    ///< rate of right ascension
    double inclination_angle;   ///< inclination angle at reference time
    double idot;                ///< rate of inclination angle
    double c_uc;                ///< amplitude of cosine harmonic correction term to the argument of latitude
    double c_us;                ///< amplitude of sine harmonic correction term to the argument of latitude
    double c_rc;                ///< amplitude of cosine harmonic correction term to the orbit radius
    double c_rs;                ///< amplitude of sine harmonic correction term to the orbit radius
    double c_ic;                ///< amplitude of cosine harmonic correction term to the angle of inclination
    double c_is;                ///< amplitude of sine harmonic correction term to the angle of inclination
} bdsephemeris;

#pragma pack(4)
typedef struct
{
    uint16_t slot_offset;       ///< slot information offset
    uint16_t freq_offset;       ///< frequency channel offset for satellite in the range 0 to 20;
    uint8_t sat_type;           ///< satellite type
    uint8_t reserved1;
    uint16_t e_week;            ///< reference week of ephemeris
    uint32_t e_time;            ///< reference time of ephemeris
    uint32_t t_offset;          ///< integer seconds between GPS and GLONASS time.
    uint16_t n_t;               ///< calendar number of day within 4 year interval starting at Jan 1 of a leap year.
    uint8_t reserved2;
    uint8_t reserved3;
    uint32_t issue;             ///< 15 minute interval number conrresponding to ephemeris reference time
    uint32_t health;            ///< ephemeris health where
    double pos_x;               ///< X coordinate for satelllite at reference time
    double pos_y;               ///< Y coordinate for satellite at reference time
    double pos_z;               ///< Z coordinate for satellite at reference time
    double vel_x;               ///< X coordinate for satellite celocity ar reference time
    double vel_y;               ///< Y coordinate for satellite celocity ar reference time
    double vel_z;               ///< Z coordinate for satellite celocity ar reference time
    double ls_acc_x;            ///< X coordinate for lunisolar acceleration at reference time
    double ls_acc_y;            ///< Y coordinate for lunisolar acceleration at reference time
    double ls_acc_z;            ///< Z coordinate for lunisolar acceleration at reference time
    double tau_n;               ///< correction to the nth satellite time t_n relative to glonass tine t_c
    double delta_tau_n;         ///< time defference between navigation RF signal reansmitted in L2 sub-band and navigation RF signal trnasmitted in L1 sub-band by nth satellite
    double gamma;               ///< frequency correction
    uint32_t time_of_frame;     ///< time of frame start
    uint32_t tech_param;        ///< technological parameter
    uint32_t user_range;        ///< user range
    uint32_t age;               ///< age of data
    uint32_t flags;             ///< information flags
} gloephemeris;

// #pragma pack(1)
#pragma pack(4)
typedef struct
{
    uint32_t prn;               ///< satellite prn number
    uint32_t ref_week;          ///< ephemeris reference week number
    uint32_t ref_secs;          ///< ephemeris reference time
    uint8_t subframe1[30];      ///< subframe 1 data
    uint8_t subframe2[30];      ///< subframe 2 data
    uint8_t subframe3[30];      ///< subframe 3 data
} rawephem;

#pragma pack(4)
typedef struct
{
	uint32_t sol_status;
	uint32_t vel_type;
	uint32_t latency;
	float age;
	double hor_spd;
	double trk_gnd;
	double vert_spd;
	uint32_t reserved;
	uint32_t crc;
}psrvel;

#pragma pack(4)
typedef struct
{
    uint8_t hex[24];
} rangecmp_single_fram;

#pragma pack(4)
typedef struct
{
    uint32_t num_of_obs;
    rangecmp_single_fram single_fram[128+128];
} rangecmp;

#pragma pack(4)
typedef union {
    bestvel bestvelu;
    bestpos bestposu;
    psrdop psrdopu;
    nova_time time;
    bdsephemeris bds;
    gloephemeris glo;
    rawephem     gps;
    rangecmp     observer;
    gpsephem     gpse;
    bd2ephem     bd2e;
    markpos      mkpos;
    bestxyz      bestxyzb;
    heading      headingb;
    psrdop2      psrdop2b;
    psrdop       psrdop;
	psrvel		 psrvelb;
    uint8_t 	 bytes[sizeof(rangecmp)];
    //uint8_t bytes[UART3_DMA_RX_BUFF_SIZE];
} msgbuffer;

#pragma pack(4)
typedef union {
    nova_header nova_headeru;
    uint8_t data[28];
} msgheader;

#pragma pack(4)
typedef struct
{
    enum
    {
        PREAMBLE1 = 0,
        PREAMBLE2,
        PREAMBLE3,
        HEADERLENGTH,
        HEADERDATA,
        DATA_,
        CRC1,
        CRC2,
        CRC3,
        CRC4,
    } nova_state;

    msgheader header;
    msgbuffer data;
    uint32_t crc;
    uint16_t read;
} nova_msg_parser;



bool is_gnss_initialized(void);

GPS_State* get_gnss_state(void);

rtk_base_status_t* get_rtk_base_status(void);

/**
  * @brief  get calibration type from flash
  * @param  None
  * @retval None
  */
void sync_calibration_type(void);

/**
  * @brief  set calibration type and save to flash
  * @param  type: rtk_calibration_type_e
  * @retval None
  */
void set_calibration_type_to_flash(rtk_calibration_type_e type);

/**
  * @brief  get calibration type
  * @param  None
  * @retval The address of calibration type
  */
rtk_calibration_type_e * get_calibration_type(void);

/**
  * @brief  获取当前Novatel命令模式
  * @param  is_enter true：表示已进入命令模式, false：表示未在命令模式
  * @retval None
*/
void enter_cmd_mode(bool is_enter);

/**
  * @brief  设置fix position 的精度、纬度和高度
  * @param  lat: 纬度
  * @param  lng: 精度
  * @param  lat: 高度
  * @retval None
*/
void set_fix_position(float lat, float lng, float alt);

/**
 * @brief 移动站与基站切换
 * @param type: calibration type
 * @retval None
*/
void switch_base_mode(rtk_calibration_type_e type);

/**
  * @brief  Enter or exit rover mode.
  * @param  None
  * @retval false, fail
  *         true, success
  */
bool rover_mode_toggle(void);

/**
  * @brief  Lock position imformation in rover mode.
  * @param  None
  * @retval false, fail
  *         true, success
  */
bool rover_mode_lock_pos(void);

/**
  * @brief  Get rover mode status.
  * @param  None
  * @retval true, in rover mode.
  *         false, out of rover mode.
  */
bool get_rover_mode_stat(void);

/**
  * @brief  Get status of the lock information.
  * @param  None
  * @retval true, lock.
  *         false, unlock.
  */
bool get_rover_mode_lock_stat(void);

/**
* @brief 询问当前的时间是否可用
*/
bool is_time_valid(void);

// 获取当前的UTC时间
nova_time* get_utc_time(void);
bool gnss_init(UART_HandleTypeDef *port);
void ppk_task(void);
void rtk_task(void);
#endif
