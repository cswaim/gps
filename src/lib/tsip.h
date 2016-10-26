/*
  tsip.h - A simplified Trimble Standard Interface Protocol (TSIP) library
            based on the work for the Arduino platform.

           The library configured to work with the Trimble Thunderbolt GPS
           Disciplined Oscillator (GPSDO) and is based on the
           "ThunderBolt GPS Disciplined Clock User Guide" Version 5.0
           Part Number: 35326-30  November 2003

  Modified 2012-02 by Criss Swaim, The Pineridge Group, LLC for Linux env

  Copyright (c) 2011 N7MG Brett Howard
  Copyright (c) 2011 Andrew Stern (N7UL)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  $Id: Tsip.h 31 2011-06-12 17:33:08Z andrew@n7ul.com $

*/

#ifndef _tsip_h
#define _tsip_h
#define _USE_MATH_DEFINES

#include <algorithm>
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>
#include <cmath>
#include <termios.h>
#include <ctime>

#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

#define _GPS_VERSION 0x00 // software version of this library
#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001

#define MAX_DATA     1024			// report buffer size
#define MAX_COMMAND  64				// command buffer size

//#define DLE		0x10
//#define ETX		0x03
const unsigned char DLE = 0x10;
const unsigned char ETX = 0x03;

#define TRUE	1
#define FALSE	0

//match Trimble Documentation Datatypes to Arduino Datatypes
typedef unsigned char UINT8;
typedef char SINT8;
typedef short int SINT16;
typedef unsigned short int UINT16;
typedef unsigned int UINT32;
typedef int SINT32;
typedef float SINGLE;
typedef double DOUBLE;

// supported reports and super-reports
const UINT8 REPORT_ECEF_POSITION_S			= 0x42;
const UINT8 REPORT_ECEF_VELOCITY			= 0x43;
const UINT8 REPORT_SW_VERSION				= 0x45;
const UINT8 REPORT_SINGLE_POSITION			= 0x4a;
const UINT8 REPORT_IO_OPTIONS				= 0x55;
const UINT8 REPORT_ENU_VELOCITY				= 0x56;
const UINT8 REPORT_ECEF_POSITION_D			= 0x83;
const UINT8 REPORT_DOUBLE_POSITION			= 0x84;
const UINT8 REPORT_SUPER					= 0x8f;
const UINT8 REPORT_SUPER_UTC_GPS_TIME		= 0xa2;
const UINT8 REPORT_SUPER_PRIMARY_TIME		= 0xab;
const UINT8 REPORT_SUPER_SECONDARY_TIME		= 0xac;

//supported super-command subcommands
const UINT8 COMMAND_SAVE_EEPROM             = 0x4c;
const UINT8 COMMAND_SELF_SURVEY             = 0xa6;
const UINT8 COMMAND_SET_SELF_SURVEY_PARAMS  = 0xa9;

// supported commands and super-commands
const UINT8 COMMAND_COLD_FACTORY_RESET		= 0x1e;
const UINT8 COMMAND_REQUEST_SW_VERSION		= 0x1f;
const UINT8 COMMAND_WARM_RESET_SELF_TEST	= 0x25;
const UINT8 COMMAND_SET_IO_OPTIONS			= 0x35;
const UINT8 COMMAND_REQUEST_POSITION		= 0x37;
const UINT8 COMMAND_SUPER_PACKET			= 0x8E;

/****************************
 * Request packet structures *
 ****************************/
// generic command TSIP packet
union _command_packet {
	struct _raw {
		UINT8 data[MAX_COMMAND];
		UINT8 cmd_len;
	} raw;
	struct _report {
		UINT8 code;
		UINT8 data[MAX_COMMAND-1];
		UINT8 cmd_len;
	} report;
	struct _extended {
		UINT8 code;
		UINT8 subcode;
		UINT8 data[MAX_COMMAND-2];
		UINT8 cmd_len;
	} extended;
	//struct _extended_8EA9 {
		//UINT8 code;
		//UINT8 subcode;
		//UINT8 enable_survey;
		//UINT8 save_position;
		//UINT32 self_survey_length;
		//UINT32 reserved_8Ea9;
		//UINT8 cmd_len;
	//} extended_8eA9;
	struct _data_8ea9 {
		UINT8 code;
		UINT8 subcode;
		UINT8 enable_survey;
		UINT8 save_position;
		UINT32 self_survey_length;
		UINT32 reserved_8ea9;
		UINT8 cmd_len;
	} data_8ea9;
};

/****************************
 * Report packet structures *
 ****************************/

// generic report TSIP packet
union _report_packet {
	struct _raw {
		UINT8 data[MAX_DATA];
	} raw;
	struct _report {
		UINT8  code;
		UINT8  data[MAX_DATA-1];
	} report;
	struct _extended {
		UINT8  code;
		UINT8  subcode;
		UINT8  data[MAX_DATA-2];
	} extended;
};

// Single precision XYZ Earth Centered Earth Fixed (ECEF) Position Packet
struct _ecef_position_s {
	bool  valid;
	struct _0x42 {
		SINGLE x;				// X meters
		SINGLE y; 				// Y meters
		SINGLE z;				// Z meters
		SINGLE time_of_fix;		// time of fix in GPS or UTC seconds
	} report;
};

// XYZ Earth Centered Earth Fixed (ECEF) Velocity Packet
struct _ecef_velocity {
	bool  valid;
	struct _0x43 {
		SINGLE x;				// X meters/second
		SINGLE y;				// Y meters/second
		SINGLE z;				// Z meters/second
		SINGLE bias_rate;		// meters/second
		SINGLE time_of_fix;		// time of fix in GPS or UTC seconds
	} report;
};

// Software Version Packet
struct _sw_version {
	bool  valid;
	struct _0x45 {
		UINT8 app_major;
		UINT8 app_minor;
		UINT8 app_month;
		UINT8 app_day;
		UINT8 app_year;			// year - 1900
		UINT8 gps_major;
		UINT8 gps_minor;
		UINT8 gps_month;
		UINT8 gps_day;
		UINT8 gps_year;			// year - 1900
	} report;
};

// Single precision LLA position fix
struct _single_position {
	bool  valid;
	struct _0x4A {
		SINGLE latitude;		// radians + north, - south
		SINGLE longitude;		// radians + east, - west
		SINGLE altitude;		// meters
		SINGLE clock_bias;		// meters relative to GPS
		SINGLE time_of_fix;		// seconds (GPS/UTC)
	} report;
};

// I/O options
struct _io_options {
	bool  valid;
	struct _report {
		union _position {
			UINT8 value;						// position options
			struct _bits {
				UINT8 ecef				: 1;
				UINT8 lla				: 1;
				UINT8 msl				: 1;
				UINT8 reserved_0		: 1;
				UINT8 double_precision	: 1;
				UINT8 reserved_1		: 3;
			} bits;
		} position;
		union _velocity {						// velocity options
			UINT8 value;
			struct _bits {
				UINT8 ecef				: 1;
				UINT8 enu				: 1;
				UINT8 reserved			: 6;
			} bits;
		} velocity;
		union _timing {							// timing options
			UINT8 value;
			struct _bits {
				UINT8 utc				: 1;
				UINT8 reserved			: 7;
			} bits;
		} timing;
		union _auxiliary {						// auxiliary options
			UINT8 value;
			struct _bits {
				UINT8 packet_5A			: 1;
				UINT8 filtered_PRs		: 1;
				UINT8 reserved_0			: 1;
				UINT8 dbhz				: 1;
				UINT8 reserved_1			: 4;
			} bits;
		} auxiliary;
	} report;
};

// Single precision East-North-Up (ENU) velocity fix
struct _enu_velocity {
	bool  valid;
	struct _0x56 {
		SINGLE east;			// m/s + east, - west
		SINGLE north;			// m/s + north, - south
		SINGLE up;				// m/s + up, - down
		SINGLE clock_bias;		// meters/second
		SINGLE time_of_fix;		// seconds (GPS/UTC)
	} report;
};

// Double precision XYZ Earth Centered Earth Fixed (ECEF) Position Packet
struct _ecef_position_d {
	bool  valid;
	struct _0x83 {
		DOUBLE x;				// X meters
		DOUBLE y; 				// Y meters
		DOUBLE z;				// Z meters
		DOUBLE clock_bias;		// clock bias meters
		SINGLE time_of_fix;		// time of fix in GPS or UTC seconds
	} report;
};

// Double precision LLA position fix
struct _double_position {
	bool  valid;
	struct _0x84 {
		DOUBLE latitude;		// radians + north, - south
		DOUBLE longitude;		// radians + east, - west
		DOUBLE altitude;		// meters
		DOUBLE clock_bias;		// meters relative to GPS
		DOUBLE time_of_fix;		// seconds (GPS/UTC)
	} report;
};

// 8F-A2 UTC GPS Time
struct _utc_gps_time {
	bool  valid;
	struct _0x8FA2 {
		union _flags {
			UINT8 value;
			UINT8 date_time_fmt		: 1;	// 0-gps time, 1-utc time
			UINT8 pps_reference		: 1;	// 0-gps time, 1-utc time
			UINT8  unused			: 6;	// unused bits
		} bits;
	} report;
};


//8F-AB Primary Timing Packet
struct _primary_time {
	bool  valid;
	struct _0x8FAB {
		UINT32  seconds_of_week; // GPS seconds since GPS Sunday 00:00:00
		UINT16  week_number;	// GPS week number
		SINT16  utc_offset;		// GPS-UTC seconds difference
		union _flags {
			UINT8 value;
			struct _bits {
				UINT8  utc_time		: 1;	// UTC/GPS time
				UINT8  utc_pps		: 1;	// UTC/GPS PPS
				UINT8  time_not_set	: 1;	// time set/not set
				UINT8  no_utc_info	: 1;	// have/do not have UTC info
				UINT8  test_mode_time	: 1;	// time from GPS/test mode time
				UINT8  unused			: 3;	// unused bits
			} bits;
		} flags;				// timing flags
		UINT8   seconds;		// 0-59  (UTC/GPS flags bit0 = 1/0)
		UINT8   minutes;		// 0-59
		UINT8   hours;			// 0-23
		UINT8   day;			// 1-31
		UINT8   month;			// 1-12
		UINT16  year;			// four digit year
	} report;
};

//8F-AC Secondary Timing Packet
struct _secondary_time {
	bool  valid;
	struct _0x8FAC {
		UINT8  receiver_mode;
			#define RECEIVE_MODE_AUTO_2D_3D					0
			#define RECEIVE_MODE_SINGLE_STATELLITE			1
			#define RECEIVE_MODE_HORIZONTAL_2D				3
			#define RECEIVE_MODE_FULL_POSITION_2D			4
			#define RECEIVE_MODE_DGPS_REFERENCE				5
			#define RECEIVE_MODE_CLOCK_HOLD					6
			#define RECEIVE_MODE_OVERDETERMINDE_CLOCK		7
		UINT8  disciplining_mode;
			#define DISCIPLINING_MODE_NORMAL				0
			#define DISCIPLINING_MODE_POWER_UP				1
			#define DISCIPLINING_MODE_AUTO_HOLDOVER			2
			#define DISCIPLINING_MODE_MANUAL_HOLDOVER		3
			#define DISCIPLINING_MODE_RECOVERY				4
			#define DISCIPLINING_MODE_NOT_USED				5
			#define DISCIPLINING_MODE_DISCIPLINING_DISABLED	6
		UINT8   self_survey_progress;	// 0-100%
		UINT32  holdover_duration;		// seconds
		union _critical_alarms {
			UINT16 value;
			struct _bits {
				UINT16 rom_checksum_error		: 1;
				UINT16 ram_check_failed			: 1;
				UINT16 power_supply_failure		: 1;
				UINT16 fpga_check_failed			: 1;
				UINT16 control_voltage_at_rail	: 1;
				UINT16 unused					: 11;
			} bits;
		} critical_alarms;
		union _minor_alarms {
			UINT16 value;
			struct _bits {
				UINT16 control_voltage_near_rail		: 1;
				UINT16 antenna_open					: 1;
				UINT16 antenna_shorted				: 1;
				UINT16 not_tracking_satellites		: 1;
				UINT16 oscillator_not_disciplined	: 1;
				UINT16 self_survey_in_progress		: 1;
				UINT16 no_accurate_stored_position	: 1;
				UINT16 leap_second_pending			: 1;
				UINT16 in_test_mode					: 1;
				UINT16 inaccurate_position			: 1;
				UINT16 eeprom_segment_corrupt		: 1;
				UINT16 almanac_not_current			: 1;
				UINT16 unused						: 4;
			} bits;
		} minor_alarms;
		UINT8   gps_decoding_status;
			#define GPS_DECODING_STATUS_DOING_FIXES			0
			#define GPS_DECODING_STATUS_NO_GPS_TIME			1
			#define GPS_DECODING_STATUS_PDOP_TOO_HIGH		3
			#define GPS_DECODING_STATUS_NO_SATELLITES		8
			#define GPS_DECODING_STATUS_ONE_SATELLITE		9
			#define GPS_DECODING_STATUS_TWO_SATELLITES		0x0A
			#define GPS_DECODING_STATUS_THREE_SATELLITES	0x0B
			#define GPS_DECODING_STATUS_CHOSEN_SAT_UNUSABLE	0x0C
			#define GPS_DECODING_STATUS_TRAIM_REJECTED_FIX	0x10
		UINT8   disciplining_activity;
			#define DISCIPLINING_ACTIVITY_PHASE_LOCKING		0
			#define DISCIPLINING_ACTIVITY_OSC_WARMING_UP	1
			#define DISCIPLINING_ACTIVITY_FREQUENCY_LOCKING	2
			#define DISCIPLINING_ACTIVITY_PLACING_PPS		3
			#define DISCIPLINING_ACTIVITY_INIT_LOOP_FILTER	4
			#define DISCIPLINING_ACTIVITY_COMPENSATING_OCXO	5
			#define DISCIPLINING_ACTIVITY_INACTIVE			6
			#define DISCIPLINING_ACTIVITY_NOT_USED			7
			#define DISCIPLINING_ACTIVITY_RECOVERY_MODE		8
		UINT8   spare_status1;
		UINT8   spare_status2;
		SINGLE  pps_offset;				// estimate of UTC/GPS offset (ns)
		SINGLE  tenMHz_offset;			// estimate of UTC/GPS offset (ppb)
		UINT32  dac_value;
		SINGLE  dac_voltage;			// voltage
		SINGLE  temperature;			// degress C
		DOUBLE  latitude;			// DOUBLE radians
		DOUBLE  longitude;			// DOUBLE radians
		DOUBLE  altitude;			// DOUBLE meters
		UINT8   spare[8];
	} report;
};

// unknown report packet
struct _unknown {
	bool  valid;
	union _report_packet report;
};


// Trimble Standard Interface Protocol (TSIP) class
class tsip {
	public:
		struct xyz_t {
			double latitude;			//decimal degrees
			double longitude;			//decimal degrees
			double altitude;			//meters
		};

		//return parms
		time_t gps_time;
		xyz_t xyz;
		double _rad;            // (180/pi) to convert radians to degree

		// received reports
		struct _ecef_position_s		m_ecef_position_s;
		struct _ecef_position_d		m_ecef_position_d;
		struct _ecef_velocity		m_ecef_velocity;
		struct _sw_version			m_sw_version;
		struct _single_position		m_single_position;
		struct _double_position		m_double_position;
		struct _io_options			m_io_options;
		struct _enu_velocity		m_enu_velocity;
		struct _utc_gps_time        m_utc_gps_time;
		struct _primary_time		m_primary_time;
		struct _secondary_time		m_secondary_time;
		struct _unknown				m_unknown;

		// report updated flags
		// there is an update bit per implemented report
		union _update_report {
			int value;
			struct _bits {
				int ecef_position_s : 1;
				int ecef_position_d : 1;
				int ecef_velocity   : 1;
				int sw_version      : 1;
				int single_position : 1;
				int double_position : 1;
				int io_options      : 1;
				int enu_velocity    : 1;
				int primary_time    : 1;
				int secondary_time  : 1;
				int utc_gps_time    : 1;
				int unused          : 4;	// future expansion
				int unknown			: 1;	// unknown report
			} report;
		} m_updated;

		// TSIP command packet buffer
		union _command_packet m_command;

		// TSIP report packet buffer
		union _report_packet  m_report;
		int   m_report_length;

		//public methods
		tsip(std::string port="", bool verbose=true);
		~tsip(void);
		int encode(UINT8 c);			// encode byte stream into packets
		void init_rpt(void); 			// initialize the report fields
		void set_verbose(bool);         // set verbose
		void set_debug(bool);        	// set debug
		void set_gps_port(std::string gps_port);
		bool set_survey_params(int survey_cnt);
		bool set_auto_save();
		bool store_position();
		bool start_self_survey();
		bool open_gps_port(std::string port="");
		std::string get_gps_port();
		bool send_request_msg(_command_packet _cmd);
		bool get_report_msg(_command_packet _cmd);

		//gps_api(std::string port, bool verbose=true);
		//bool get_gps_time_utc(time_t &seconds_since_epoch);
		time_t get_gps_time_utc();
		xyz_t get_xyz();
		bool send_get_time();


	private:
		bool verbose;
		bool debug;

		std::string gps_port;
		FILE *file;

		// packet decoder states
		enum t_state {
			START=1,
			FRAME,
			DATA,
			DATA_DLE
		} m_state;

		//methods
		void setup_gps_port(FILE *file);
		int update_report(void);		// update report with packet data
		bool is_report_found(_command_packet &_cmd);
		UINT16 b2_to_uint16(int bb, char r_code);	// convert 2 bytes to short integer
		UINT32 b4_to_uint32(int bb, char r_code);	// convert 4 bytes to integer
		SINGLE b4_to_single(int bb, char r_code);	// convert 4 bytes to float
		DOUBLE b8_to_double(int bb, char r_code);	// convert 8 bytes to double
};


#endif
