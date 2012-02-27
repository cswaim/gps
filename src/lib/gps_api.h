#ifndef GPS_API_H
#define GPS_API_H
#define _USE_MATH_DEFINES
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <termios.h>
//#include <string.h>
#include <cstring>
//#include <time.h>
#include <ctime>
//#include <stdio.h>
#include <cstdio>
#include <string>
#include <vector>
#include <boost/math/constants/constants.hpp>
#include <cmath>
#include "tsip.h"

/***************************************************************
 *	gps_api - defines the access to the thunderbolt GPS
 * 
 * *************************************************************/
class gps_api {
private:
	std::string gps_port;
	bool verbose;
	time_t gps_time;
	FILE *file;
	struct cmd_stack_t {
		_command_packet _cmd;            	// cmd request string
		int _byte_cnt;       				// bytes in cmd string
		unsigned char _rpt_code;		// return report code
		unsigned char _rpt_subcode;		// return report subcode
		std::string _rpt_desc;
	};
	//_cmd_stack w_stack;
	cmd_stack_t cmd_stack;
	
	//initialize TSIP state processor
	tsip sip;

public:
	struct xyz_t {
		double x;
		double y;
		double z;
	};

	gps_api(std::string port, bool verbose=true); 
	bool get_gps_time_utc(time_t &seconds_since_epoch);
	void set_gps_port(std::string gps_port);
	std::string get_gps_port();
	bool get_xyz();
	bool send_get_time();
	
private:
	void setup_serial_port(FILE *file);
	bool get_time(int &year, int &month, int &day, int &hour, int &minute, int &second);
	bool is_report_found(cmd_stack_t &cmd_stack);
	bool send_gps_msg(cmd_stack_t cmd_stack);
	bool recv_gps_msg();
	void print_report();

};
#endif //GET_GPS_TIME_H
