#include "gps_api.h"

/** Constructor.
*
*	The class is created with the port definition and the port is opened.
*	T
*/

gps_api::gps_api(std::string port="/dev/ttyS0", bool verbose) {
	// set verbose
	this->verbose = verbose;

	// set the port
	set_gps_port(port);
	
    file = fopen(gps_port.c_str(), "r");
//        FILE *file = fopen(gps_port.c_str(), "r");

    if(!file)
    {
        printf("Cannot open %s\n", gps_port.c_str());
        return;
		//return false;
    }

    setup_serial_port(file);
}

/** set up serial port
*
*   Set the  parameters for the I/O comm port the gps is attached.
*
*   @oaram pointer to the serial port 'file'.
*/
void gps_api::setup_serial_port(FILE *file)
{
    int fd;
    struct termios newtio;

    fd = fileno(file);

    memset(&newtio, 0, sizeof(newtio)); /* clear struct for new port settings */

    /* 
        B9600: Set bps rate. You could also use cfsetispeed and cfsetospeed.
        CS8     : 8n1 (8bit,no parity,1 stopbit)
        CLOCAL  : local connection, no modem contol
        CREAD   : enable receiving characters
     */
    newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;

    /*
        IGNPAR  : ignore bytes with parity errors
        otherwise make device raw (no other input processing)
     */
    newtio.c_iflag = IGNPAR;

    /*
        Raw output.
     */
    newtio.c_oflag = 0;

    /* 
        initialize all control characters 
        default values can be found in /usr/include/termios.h, and are given
        in the comments, but we don't need them here
     */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */

    /*
        ICANON  : enable canonical input
        disable all echo functionality, and don't send signals to calling program
     */
    newtio.c_lflag = 0;//ICANON;

    /* 
        now clean the modem line and activate the settings for the port
     */
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
}

/**  is report found
*
*   pass the command code and check to see if report has been found.
* 	if no report is returned in response to a command, then true is
* 	returned, if the command is in the list.
*
* 	@param cmd_stack_t command codereference to time fields
*   @return bool.
*/
bool gps_api::is_report_found(cmd_stack_t &cmd_stack) {
	bool isfound = false;

	switch (cmd_stack._cmd.report.code){
		
		// 0x1e
		case COMMAND_COLD_FACTORY_RESET :
			//0x45 report_sw_version
			if (sip.m_updated.report.sw_version) {
				isfound = true;
			}
			break;
			
		// 0x1f
		case COMMAND_REQUEST_SW_VERSION :
			//0x45 report_sw_version
			if (sip.m_updated.report.sw_version) {
				isfound = true;
			}
			break;
			
		// 0x25 
		case COMMAND_WARM_RESET_SELF_TEST :
			//0x45 report_sw_version
			if (sip.m_updated.report.sw_version) {
				isfound = true;
			}
			break;
			
		// 0x35
		case COMMAND_SET_IO_OPTIONS :
			// 0x55	 REPORT_IO_OPTIONS
			if (sip.m_updated.report.io_options) {
				isfound = true;
			}
			break;
			
		// 0x37
		case COMMAND_REQUEST_POSITION :
			// 0x42 REPORT_ECEF_POSITION_S
			if (sip.m_updated.report.ecef_position_s && sip.m_updated.report.ecef_position_d) {
				isfound = true;
			}
			// 0x43 REPORT_ECEF_VELOCITY
			
			// 0x83 REPORT_ECEF_POSITION_D
			//if (m_updated.report.ecef_position_d) {
			//	isfound = true;
			//}
			break;	
		//0x8E
		case COMMAND_SUPER_PACKET:
			switch (cmd_stack._cmd.extended.subcode) {
				// 0xa2
				case REPORT_SUPER_UTC_GPS_TIME :
					if (sip.m_updated.report.utc_gps_time) {
						isfound = true;
					}
					break;
				// 0xab
				case REPORT_SUPER_PRIMARY_TIME :
					if (sip.m_updated.report.primary_time) {
						isfound = true;
					}
					break;
				// 0xac
				case REPORT_SUPER_SECONDARY_TIME :
					if (sip.m_updated.report.secondary_time) {
						isfound = true;
					}	
					break;
			}
			break;
		default :
			isfound = false;
		}
	return isfound;
}
	// other reports
	// 0x42 REPORT_ECEF_POSITION_S:
	// 0x43 REPORT_ECEF_VELOCITY
	// 0x4a	REPORT_SINGLE_POSITION
	// 0x56 REPORT_ENU_VELOCITY				
	// 0x83 REPORT_ECEF_POSITION_D			
	// 0x84 REPORT_DOUBLE_POSITION
		
/** get the time
*
*   passes the time request to the gps unite and parses the returned info.
*
* 	@param int& reference to time fields
*   @return bool.
*/
//bool gps_api::get_time(std::string &gps_port, int &year, int &month, int &day, int &hour, int &minute, int &second)
bool gps_api::get_time(int &year, int &month, int &day, int &hour, int &minute, int &second)
{
   
//    FILE *file = fopen(gps_port.c_str(), "r");

//    if(!file)
//    {
//        //printf("Cannot open %s\n", SERIAL_DEVICE);
//        printf("Cannot open %s\n", gps_port.c_str());
//        return false;
//    }

//    setup_serial_port(file);

    unsigned char ch = 0;
RETRY:
    unsigned char buffer[256];

    // Request UTC timings
    buffer[0] = 0x10;
    buffer[1] = 0x8E;
    buffer[2] = 0xA2;
    buffer[3] = 0x3;
    buffer[4] = 0x10;
    buffer[5] = 0x03;
    fwrite(buffer, 6, 1, file);
    //printf("send A2 request\n");

    // Request 0x8F-AB packet
    buffer[0] = 0x10;
    buffer[1] = 0x8E;
    buffer[2] = 0xAB;
    buffer[3] = 0x1;
    buffer[4] = 0x10;
    buffer[5] = 0x03;
    fwrite(buffer, 6, 1, file);
    //printf("send AB request\n");


    while((ch = getc(file)) != 0x10) printf("get%x\n",  ch);
    if((ch = getc(file)) != 0x8F) 
        goto RETRY;
    if((ch = getc(file)) != 0xAB) 
        goto RETRY;

    int i = 0; 
    buffer[i++] = 0xAB;
    while(true)
    {
        if(i > 20) goto RETRY; // Packet shouldnt be that long anyway

        if ((ch = getc(file)) != 0x10)
            buffer[i++] = ch;
        else
        {
            if ((ch = getc(file)) == 0x03)
            {
                break;
            }
            else if(ch != 0x10)
            {
                printf("There is an error in the packet\n");
                goto RETRY;
            }
            else
            {
                buffer[i++] = 0x10;
            }
        }
    }
    int packet_len = i;

    printf("GPS packet received, length: %d\n", packet_len);
    if(packet_len != 17) 
    {
        printf("ERROR: Wrong sized packet received from GPS\n");
        goto RETRY;
    }

    // Check that we receive UTC time, and we are all stable
    int timing_flag = buffer[9];
    if((timing_flag & 0x3) != 0x3) 
    {
        printf("ERROR: The timing packet has not the bottom two bits set, meaning it is not in UTC time, should probably wait a little.\n");
        goto RETRY;
    }

    year = (buffer[15] << 8) + buffer[16];
    month = buffer[14];
    day = buffer[13];
    hour = buffer[12];
    minute = buffer[11];
    second = buffer[10];
    return true;
}

/** get gps time in utc seconds
*
*   updates the time_t referenced structure passed to method.
*
* 	@param time_t&  reference to time_t structure
*   @return bool 
*/
bool gps_api::get_gps_time_utc(time_t &seconds_since_epoch)
{
    struct tm time;
    if(get_time(time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec))
    {
		if (verbose) {
			printf("GPS time before edit Year: %d, Month: %d, Day: %d, Hour: %d, Minutes: %d, Seconds: %d\n",time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
		}
        time.tm_year = time.tm_year - 1900;
        time.tm_mon = time.tm_mon - 1;
        time.tm_zone = "UTC";
        time.tm_wday = -1;
        time.tm_yday = -1;
        time.tm_isdst = -1;

        seconds_since_epoch = timegm(&time);
        if (verbose) {
        printf("Got GPS time Year: %d, Month: %d, Day: %d, Hour: %d, Minutes: %d, Seconds: %d\n", 
            time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
		}
        return true;
    }
    else
		printf("****Failed to get GPS time****\n");
        return false;
}

/** set gps port
*
*   Set the variable gps_port with the port id.
*
*   @param string::gps_port  defaults to /dev/ttyS0.
*/
void gps_api::set_gps_port(std::string port="/dev/ttyS0") {
	gps_port = port;
}

/** get gps port
*
*   Get the gps_port setting.
*
*   @return string::gps_port  
*/
std::string gps_api::get_gps_port() {
	return (gps_port);
}

/** get xyz from gps
*
*   Get the xyz (lat, long, alt) from the gps.
*
*   @return bool  
*/
bool gps_api::get_xyz() {
	printf("XYZ rtn\n");
	bool rc;
	sip.m_command.report.code = COMMAND_REQUEST_POSITION;
	//sip.m_command.report.code = COMMAND_SET_IO_OPTIONS;
	//sip.m_command.report.data[0] = 0x3;
	for (int i=0; i<MAX_COMMAND;i++) {
		sip.m_command.report.data[i]='\0';
	}
	cmd_stack._cmd = sip.m_command;
	cmd_stack._byte_cnt = 1;
	cmd_stack._rpt_code = REPORT_ECEF_POSITION_D;
	cmd_stack._rpt_subcode = '\0';
	cmd_stack._rpt_desc = "ecef_position_d";
	//cmd_stack.push_back(w_stack);
	rc = send_gps_msg(cmd_stack);
	return rc;
}

/** send message to gps
*
*   send a sequence of commands to the gps.  The loop is continued until
*   the correct message id is returned.  The msg id is defined in the
* 	the first command struture of the cmd_stack.
*
*   @return bool  
*/
bool gps_api::send_gps_msg(cmd_stack_t cmd_stack) {
	//clear report flags
	sip.init_rpt();
	
	// init local var
    unsigned char ch = 0;
    unsigned char rpt_code = cmd_stack._rpt_code;
    unsigned char rpt_subcode = '\0';
    if (rpt_code == REPORT_SUPER) {
		rpt_subcode = cmd_stack._rpt_subcode;
	}
    
    
RETRY:
	// loop through each command in stack
    unsigned char buffer[256];
	buffer[0] = DLE;
	int x = 1;
	for (int j=0; j < cmd_stack._byte_cnt; j++,x++){
		buffer[x] = cmd_stack._cmd.raw.data[j];
	}
	buffer[x] = DLE;
	buffer[x+1] = ETX;
	fwrite(buffer, 1, x+1, file);
	if (verbose) {
		printf("Sending Request: ");
		for (int k=0;k<=x+1;k++){printf(" %x",buffer[k]);}
		printf("  looking for rpt %x %x\n",rpt_code,rpt_subcode);

	}

	if (cmd_stack._cmd.report.code == COMMAND_SUPER_PACKET && cmd_stack._byte_cnt > 2) {
		printf("----NO Report for cmd: %x-%x \n\n",cmd_stack._cmd.report.code,cmd_stack._cmd.extended.subcode);
		return true;
	}
	
	// read stream and pass to encode routine untile packet complete 
	
	int loop_cnt=0;
	while (!is_report_found(cmd_stack)) {
		int rc = 0;
		while (rc == 0) {	
			rc = sip.encode(getc(file));
		}
		loop_cnt++;
		if (loop_cnt >20) break;
	}
	bool rpt_fnd = is_report_found(cmd_stack);
	//printf("send msg: rpt_fnd: %i cmd: %x-%x \n",rpt_fnd,cmd_stack._cmd.report.code,cmd_stack._cmd.extended.subcode);
	if (cmd_stack._cmd.report.code == COMMAND_SUPER_PACKET) {
		//if (sip.m_report.report.code == rpt_code && sip.m_report.extended.subcode == rpt_subcode) {
		if (is_report_found(cmd_stack)) {
			//printf("Packet %x  found - returned %x-%x\n",cmd_stack[0]._rpt_code,sip.m_report.report.code,sip.m_report.extended.subcode);
			printf("Packet %x %x found \n",sip.m_report.report.code,sip.m_report.extended.subcode);
				
		}else {
			//printf("Packet %x not found - returned %x-%x\n",cmd_stack[0]._rpt_code,sip.m_report.report.code,sip.m_report.extended.subcode);
			printf("Packet %x %x not found - returned %x-%x\n",rpt_code,rpt_subcode,sip.m_report.report.code,sip.m_report.extended.subcode);
			//goto RETRY;
		}
	} else {
		//if (sip.m_report.report.code == rpt_code ) {
		if (is_report_found(cmd_stack)) {
			//printf("Packet %x  found - returned %x-%x\n",cmd_stack[0]._rpt_code,sip.m_report.report.code,sip.m_report.extended.subcode);
			printf("Packet %x  found \n",sip.m_report.report.code);
		} else {
			//printf("Packet %x not found - returned %x-%x\n",cmd_stack[0]._rpt_code,sip.m_report.report.code,sip.m_report.extended.subcode);
			printf("Packet %x not found - returned %x\n",rpt_code,sip.m_report.report.code);
			//goto RETRY;
		}
	}
    
	
	return true;
}	

/** get msg from gps
*
*   the response from the gps
*
*   @return bool  
*/
bool gps_api::recv_gps_msg() {
	return true;
}

bool gps_api::send_get_time() {
	bool rc;
	
	//build a2 request - set UTC
	sip.m_command.extended.code = COMMAND_SUPER_PACKET;
	sip.m_command.extended.subcode = REPORT_SUPER_UTC_GPS_TIME;
	sip.m_command.extended.data[0] = 0x3;
	cmd_stack._cmd = sip.m_command;
	cmd_stack._byte_cnt = 3;
	
	printf("****send 8e-a2, data -0x3\n");
	rc = send_gps_msg(cmd_stack);
	printf("****return from 8e-a2\n");
	//print_report();
	
	//cmd_stack.clear();
	//build ab request - request time packet
	sip.m_command.extended.code = COMMAND_SUPER_PACKET;
	sip.m_command.extended.subcode = REPORT_SUPER_PRIMARY_TIME;
	//sip.m_command.extended.data[0] = 0x1;
	cmd_stack._cmd = sip.m_command;
	cmd_stack._byte_cnt = 2;
	cmd_stack._rpt_code = REPORT_SUPER;
	cmd_stack._rpt_subcode = REPORT_SUPER_PRIMARY_TIME;
	cmd_stack._rpt_desc = "primary_time";
	//cmd_stack.push_back(w_stack);
	rc = send_gps_msg(cmd_stack);

	print_report();
}
void gps_api::print_report() {
	
	if (sip.m_updated.report.primary_time) {
				
		printf("\n");
		printf("Primary Time\n");
		printf("Seconds of week: %li %x\n",sip.m_primary_time.report.seconds_of_week,sip.m_primary_time.report.seconds_of_week);
		printf("    Week Number: %i %x\n",sip.m_primary_time.report.week_number,sip.m_primary_time.report.week_number);
		printf("     UTC Offset: %i %x\n",sip.m_primary_time.report.utc_offset,sip.m_primary_time.report.utc_offset);
		printf("       Bit flag: %u\n",sip.m_primary_time.report.flags.value);
		printf("        Seconds: %i\n",sip.m_primary_time.report.seconds);
		printf("        Minutes: %i\n",sip.m_primary_time.report.minutes);
		printf("          Hours: %i\n",sip.m_primary_time.report.hours);
		printf("            Day: %i\n",sip.m_primary_time.report.day);
		printf("          Month: %i\n",sip.m_primary_time.report.month);
		printf("           Year: %i %x\n",sip.m_primary_time.report.year,sip.m_primary_time.report.year);
	}
	//ac
	if (sip.m_updated.report.secondary_time) {
				
		printf("\n");
		printf("Secondary Time\n");
		printf("       receiver mode: %x \n",sip.m_secondary_time.report.receiver_mode);
		printf("   Disciplining mode: %x\n",sip.m_secondary_time.report.disciplining_mode);
		printf("Self Survey Progress: %i \n",sip.m_secondary_time.report.self_survey_progress);
		printf("   Holdover Duration: %i \n",sip.m_secondary_time.report.holdover_duration);
		printf("     Critical Alarms: %x\n",sip.m_secondary_time.report.critical_alarms.value);
		printf("       Minor Allarms: %x\n",sip.m_secondary_time.report.minor_alarms.value);
		printf(" gps decoding status: %x\n",sip.m_secondary_time.report.gps_decoding_status);
		printf(" disciplining status: %x\n",sip.m_secondary_time.report.disciplining_activity);
		printf("          pps offset: %f\n",sip.m_secondary_time.report.pps_offset);
		printf("              10 MHz: %f\n",sip.m_secondary_time.report.tenMHz_offset);
		printf("           DAC Value: %x\n",sip.m_secondary_time.report.dac_value);
		printf("         dac Voltage: %f\n",sip.m_secondary_time.report.dac_voltage);
		printf("         Temperature: %f\n",sip.m_secondary_time.report.temperature);
		printf("            Latitude: %f\n",sip.m_secondary_time.report.latitude);
		double _cf = 180/M_PI;
		double cf = 180/boost::math::constants::pi<double>();
		double _xl = sip.m_secondary_time.report.latitude * _cf;
		double xl = sip.m_secondary_time.report.latitude * cf;
		printf("      BOOST      Latitude: %.9f\n",xl);
		printf("      C++n       Latitude: %.9f\n",_xl);
		printf("           Longitude: %f\n",sip.m_secondary_time.report.longitude);
		double _yl = sip.m_secondary_time.report.longitude * _cf;
		double yl = sip.m_secondary_time.report.longitude * cf;
		printf("      BOOST          Long: %.9f\n",yl);
		printf("      C++n           Long: %.9f\n",_yl);
		printf("            Altitude: %f\n",sip.m_secondary_time.report.altitude);
	}
	if (sip.m_updated.report.ecef_position_s) {
				
		printf("\n");
		printf("   Latitude: %f %x\n",sip.m_ecef_position_s.report.x,sip.m_ecef_position_s.report.x);
		printf("  Longitude: %f %x\n",sip.m_ecef_position_s.report.y,sip.m_ecef_position_s.report.y);
		printf("   Altitude: %f %x\n",sip.m_ecef_position_s.report.z,sip.m_ecef_position_s.report.z);
		printf("time of fix: %f\n",sip.m_ecef_position_s.report.time_of_fix);
		
	}
	if (sip.m_updated.report.ecef_position_d) {
				
		printf("\n");
		printf("ecef Position D\n");
		printf("   Latitude: %d %x\n",sip.m_ecef_position_d.report.x,sip.m_ecef_position_d.report.x);
		printf("  Longitude: %d %x\n",sip.m_ecef_position_d.report.y,sip.m_ecef_position_d.report.y);
		printf("   Altitude: %d %x\n",sip.m_ecef_position_d.report.z,sip.m_ecef_position_d.report.z);
		printf(" clock bias: %d\n",sip.m_ecef_position_d.report.clock_bias);
		printf("time of fix: %d\n",sip.m_ecef_position_d.report.time_of_fix);
		
	}
	if (sip.m_updated.report.utc_gps_time) {
				
		printf("\n");
		printf("UTC GPS Time\n");
		printf("Seconds of week: %li %x\n",sip.m_primary_time.report.seconds_of_week,sip.m_primary_time.report.seconds_of_week);
		printf("    Week Number: %i %x\n",sip.m_primary_time.report.week_number,sip.m_primary_time.report.week_number);
		printf("     UTC Offset: %i %x\n",sip.m_primary_time.report.utc_offset,sip.m_primary_time.report.utc_offset);
		printf("       Bit flag: %u\n",sip.m_primary_time.report.flags.value);
		printf("        Seconds: %i\n",sip.m_primary_time.report.seconds);
		printf("        Minutes: %i\n",sip.m_primary_time.report.minutes);
		printf("          Hours: %i\n",sip.m_primary_time.report.hours);
		printf("            Day: %i\n",sip.m_primary_time.report.day);
		printf("          Month: %i\n",sip.m_primary_time.report.month);
		printf("           Year: %i %x\n",sip.m_primary_time.report.year,sip.m_primary_time.report.year);
	}

	//printf("\n\n=========================================\n");
	//union _t1 {
		//UINT8 data[sizeof(DOUBLE)];
		//DOUBLE lat;
	//} t1;
	//union _t2 {
		//UINT8 data[sizeof(DOUBLE)];
		//DOUBLE lng;
	//} t2;
	//union _t3 {
		//UINT8 data[sizeof(SINGLE)];
		//SINGLE pps;
	//} t3;
	//union _t4 {
		//UINT8 data[sizeof(SINGLE)];
		//SINGLE hmz;
	//} t4;

	//t1.lat = 35.11914;
	//t2.lng = -106.68122;
	//t3.pps = 1.22;
	//t4.hmz = -0.07;
	
	//printf(" lat: %lf - %lx\n",t1.lat,t1.lat);
	////printf("hex of lat_time:\n");
	//for (int k=0;k<=sizeof(DOUBLE);k++){printf(" %x",t1.data[k]);}
	//printf("\n");
	//printf("long: %lf - %lx\n",t2.lng,t2.lng);
	////printf("hex of pri_time:\n");
	//for (int k=0;k<=sizeof(DOUBLE);k++){printf(" %x",t2.data[k]);}
	//printf("\n");
	//printf(" pps: %f - %lx\n",t3.pps,t3.pps);
	////printf("hex of pri_time:\n");
	//for (int k=0;k<=sizeof(SINGLE);k++){printf(" %x",t3.data[k]);}
	//printf("\n");
	//printf(" pps: %f - %lx\n",t4.hmz,t4.hmz);
	////printf("hex of pri_time:\n");
	//for (int k=0;k<=sizeof(SINGLE);k++){printf(" %x",t4.data[k]);}
	//printf("\n");
	//printf("==============================================\n");
}
