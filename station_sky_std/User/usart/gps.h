#ifndef __gps_H
#define __gps_H

#include "stm32f4xx.h"
#include "stdbool.h"

typedef enum GPS_FIX_TYPE
{
   GPS_FIX_TYPE_NO_GPS=0, /* No GPS connected | */
   GPS_FIX_TYPE_NO_FIX=1, /* No position information, GPS is connected | */
   GPS_FIX_TYPE_2D_FIX=2, /* 2D position | */
   GPS_FIX_TYPE_3D_FIX=3, /* 3D position | */
   GPS_FIX_TYPE_DGPS=4, /* DGPS/SBAS aided 3D position | */
   GPS_FIX_TYPE_RTK_FLOAT=5, /* RTK float, 3D position | */
   GPS_FIX_TYPE_RTK_FIXED=6, /* RTK Fixed, 3D position | */
   GPS_FIX_TYPE_STATIC=7, /* Static fixed, typically used for base stations | */
   GPS_FIX_TYPE_PPP=8, /* PPP, 3D position. | */
   GPS_FIX_TYPE_ENUM_END=9, /*  | */
} GPS_FIX_TYPE;

/// GPS status codes
typedef enum  {
		NO_GPS = GPS_FIX_TYPE_NO_GPS,                     ///< No GPS connected/detected
		NO_FIX = GPS_FIX_TYPE_NO_FIX,                     ///< Receiving valid GPS messages but no lock
		GPS_OK_FIX_2D = GPS_FIX_TYPE_2D_FIX,              ///< Receiving valid messages and 2D lock
		GPS_OK_FIX_3D = GPS_FIX_TYPE_3D_FIX,              ///< Receiving valid messages and 3D lock
		GPS_OK_FIX_3D_DGPS = GPS_FIX_TYPE_DGPS,           ///< Receiving valid messages and 3D lock with differential improvements
		GPS_OK_FIX_3D_RTK_FLOAT = GPS_FIX_TYPE_RTK_FLOAT, ///< Receiving valid messages and 3D RTK Float
		GPS_OK_FIX_3D_RTK_FIXED = GPS_FIX_TYPE_RTK_FIXED, ///< Receiving valid messages and 3D RTK Fixed
}GPS_Status;
typedef struct {
		double lat;
		double lng;
		double alt;
}Location;

typedef struct {
		int16_t x;
		int16_t y;
		int16_t z;
}Velocity_t;

typedef enum {
    SOL_COMPUTED = 0,
    INSUFFICIENT_OBS,
    NO_CONVERGENCE,
    SINGULARITY,
    COV_TRACE,
    TEST_DIST,
    COLD_START,
    V_H_LIMIT,
    RESIDUALS,
    INTEGRITY_WARNING,
    PENDING,
    INVALID_FIX,
    UNAUTHORIZED,
    SOL_STAT_REVERSE = 0xFFFF,
} sol_stat_e;

typedef struct  {
		uint8_t instance; // the instance number of this GPS

		// all the following fields must all be filled by the backend driver
        uint8_t postype;
		GPS_Status status;                  ///< driver fix status
        sol_stat_e sol_stat;                ///< Solution status
		uint32_t time_week_ms;              ///< GPS time (milliseconds from start of GPS week)
		uint16_t time_week;                 ///< GPS week number
		Location location;                  ///< last fix location
		float ground_speed;                 ///< ground speed in m/sec
		float ground_course;                ///< ground course in degrees
		uint16_t hdop;                      ///< horizontal dilution of precision in cm
		uint16_t vdop;                      ///< vertical dilution of precision in cm
		uint8_t num_sats;                   ///< Number of visible satellites
		Velocity_t velocity;                  ///< 3D velocity in m/s, in NED format
		float speed_accuracy;               ///< 3D velocity RMS accuracy estimate in m/s
		float horizontal_accuracy;          ///< horizontal RMS accuracy estimate in m
		float vertical_accuracy;            ///< vertical RMS accuracy estimate in m
		bool have_vertical_velocity:1;      ///< does GPS give vertical velocity? Set to true only once available.
		bool have_speed_accuracy:1;         ///< does GPS give speed accuracy? Set to true only once available.
		bool have_horizontal_accuracy:1;    ///< does GPS give horizontal position accuracy? Set to true only once available.
		bool have_vertical_accuracy:1;      ///< does GPS give vertical position accuracy? Set to true only once available.
		uint32_t last_gps_time_ms;          ///< the system time we got the last GPS timestamp, milliseconds

		// all the following fields must only all be filled by RTK capable backend drivers
		uint32_t rtk_time_week_ms;         ///< GPS Time of Week of last baseline in milliseconds
		uint16_t rtk_week_number;          ///< GPS Week Number of last baseline
		uint32_t rtk_age_ms;               ///< GPS age of last baseline correction in milliseconds  (0 when no corrections, 0xFFFFFFFF indicates overflow)
		uint8_t  rtk_num_sats;             ///< Current number of satellites used for RTK calculation
		uint8_t  rtk_baseline_coords_type; ///< Coordinate system of baseline. 0 == ECEF, 1 == NED
		int32_t  rtk_baseline_x_mm;        ///< Current baseline in ECEF x or NED north component in mm
		int32_t  rtk_baseline_y_mm;        ///< Current baseline in ECEF y or NED east component in mm
		int32_t  rtk_baseline_z_mm;        ///< Current baseline in ECEF z or NED down component in mm
		uint32_t rtk_accuracy;             ///< Current estimate of 3D baseline accuracy (receiver dependent, typical 0 to 9999)
		int32_t  rtk_iar_num_hypotheses;   ///< Current number of integer ambiguity hypotheses
}GPS_State;

#endif
