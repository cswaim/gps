/**
 *	@file tsip.cpp
 * 	@brief class implementation for the simplified Trimble Standard Interface Protocol (TSIP) library
 *	@author 	(original) Brett Howard, Andrew Stern
 *	@author 	Criss Swaim
 * 	author uri 	http://www.tpginc.net/
 * 	@version	v1.0
 * 	@date		2012-02

 *  @copyright 	Copyright (c) 2011 N7MG Brett Howard
 *  @copyright	Copyright (c) 2011 Andrew Stern (N7UL)
 *
 *  This library is free software; you can redistribute it and/or
  *  modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  * $Id: Tsip.cpp 31 2011-06-12 17:33:08Z andrew@n7ul.com $
  *
  *
  *
  * Modified 2012-02 by Criss Swaim, The Pineridge Group, LLC
  * 	runs on linux 64bit
  * 	add routines for endian compatibility wiht x86 hardware
  * 	added file open function
  * 	added routines to issue commands without msg returned
  * 	added basic send message - get report routine
  * 	added std get time and get xyz
  *
  *
  * @todo
  * 	Add test for endian hardware to determine compatibility
  *
  * Usage:
  * @code
  * 	std::string prot = '/dev/ttyS0'
  * 	tsip::xyz_t xyz;
  *		time_t gps_time;
  *
  * 	tsip gps(port);
  *   or
  * 	tsip gps;
  * 	gps.set_gps_port(port);
  *
  * 	gps_time = gps.get_gps_time_utc();
  * 	xyz = gps.get_xyz();
  * @endcode
  *
  *
  *
  */

#include "tsip.h"

/** Constructor.
*
*	The class is created with the port definition and the port is opened.
*	The verbose flag defaults to true and the debug flag defaults to
*	false.  The gps file is opened on the specified port.
*
* 	@param string   port name  "/dev/ttyS0"
* 	@param bool     verbose - optional
*
*/
tsip::tsip(std::string _port, bool verbose) {
	// set verbose
	set_verbose(true);
	set_debug(false);

	//conversion factor to compute radians to degrees
	_rad = 180/M_PI;
	//init report fields
	init_rpt();

	if (_port != "") {
		open_gps_port(_port);
	}
	//// set the port
	//set_gps_port(port);

    //file = fopen(gps_port.c_str(), "r");
////        FILE *file = fopen(gps_port.c_str(), "r");

    //if(!file)
    //{
		////if open fails - terminate run
        //printf("Cannot open %s\n", gps_port.c_str());
        //exit;
    //}

    //setup_serial_port(file);
}

/** Destructor.
*
*	Close the gps file
*/
tsip::~tsip() {
	if (file != NULL) {
		printf("closing serial port\n");
		fclose(file);
	}
}

/** initilize command/report fields
*
*   This routine initialized the command/report fields for each
*	command request sent to the process.
*
*	@return  void
*/
void tsip::init_rpt()
{
	m_report_length = 0;
	m_state = START;
	m_updated.value = 0;
	m_ecef_position_s.valid = false;
	m_ecef_position_d.valid = false;
	m_ecef_velocity.valid = false;
	m_sw_version.valid = false;
	m_single_position.valid = false;
	m_double_position.valid = false;
	m_io_options.valid = false;
	m_enu_velocity.valid = false;
	m_utc_gps_time.valid = false;
	m_primary_time.valid = false;
	m_secondary_time.valid = false;
	m_unknown.valid = false;

	for (int i=0; i<MAX_DATA;i++) {
		m_report.raw.data[i]='\0';
	}
}
/** set verbose flag
*
*   If the verbose flag is false, then all messages from the tsip
* 	routine are suppressed.
* 	The default is true
*
* 	@param   bool  true/false
* 	@return  void
*/
void tsip::set_verbose(bool vb) {
	verbose = vb;
}

/** set debug flag
*
*   If the debug flag is false, then debug messages are suppressed.
* 	The default is false
*
* 	@param   bool  true/false
* 	@return  void
*/
void tsip::set_debug(bool db) {
	debug = db;
}

/** set gps port
*
*   Set the variable gps_port with the port id.
*
*   @param string::gps_port  defaults to /dev/ttyS0.
*/
void tsip::set_gps_port(std::string port="/dev/ttyS0") {
	gps_port = port;
}

/** get gps port
*
*   Get the gps_port setting.
*
*   @return string::gps_port
*/
std::string tsip::get_gps_port() {
	return (gps_port);
}

/** open serial port
*
*   Open the gps port and initialize.
*
*   @param string   port name  "/dev/ttyS0"
*   @return bool    true - success, false = fail
*/
bool tsip::open_gps_port(std::string port)
{
	if (port == "") {
		port = gps_port;
	}
	if (port == "") {
		printf("Port must be provided in call or in set_gps_port\n");
		return(false);
	}

	// set the port
	set_gps_port(port);

    file = fopen(gps_port.c_str(), "r");

	if (file != NULL) {
		setup_gps_port(file);
		port_status = true;
		return(true);
	} else {
		printf("Cannot open %s\n", gps_port.c_str());
		perror(gps_port.c_str());
		port_status = false;
        return (false);
	}
    //if(!file) {
		////if open fails - terminate run
        //printf("Cannot open %s\n", gps_port.c_str());
        //return (false);
    //}
	//printf("calling setup\n");
    //setup_gps_port(file);
    //printf("return from setup\n");
    //return(true);
}

/** set up serial port
*
*   Set the  parameters for the I/O comm port the gps is attached.
*
*   @param pointer to the serial port 'file'.
*/
void tsip::setup_gps_port(FILE *file)
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


/** convert 2 bytes to short int
*
*   Conversion routine to accept the bytes from m_command begining
*	with the byte referenced by the passed index.  The formatted value
* 	is returned.
*
* 	***note x86 is a low-endian machine, the trimble is high-endian
*      so the bytes must be flipped
*
* 	@param   int  the position of the beg byte in m_report
* 	@param   char the code r or e to  idenity which array to pull from
*	@return  UINIT16
*/
UINT16 tsip::b2_to_uint16(int bb, char r_code) {
	UINT16 x = 0;

	if (r_code == 'r') {
		x = (m_report.report.data[bb] << 8) + m_report.report.data[bb+1];
	} else if (r_code == 'e') {
		x = (m_report.extended.data[bb] << 8) + m_report.extended.data[bb+1];
	}

	return x;
}

/** convert 4 bytes to int
*
*   Conversion routine to accept the bytes from m_command begining
*	with the byte referenced by the passed index.  The formatted value
* 	is returned.
*
* 	***note x86 is a low-endian machine, the trimble is high-endian
*      so the bytes must be flipped
*
* 	@param   int  the position of the beg byte in m_report
* 	@param   char the code r or e to  idenity which array to pull from
*	@return  UINIT32
*/
UINT32 tsip::b4_to_uint32(int bb, char r_code) {
	UINT32 x = 0;

	if (r_code == 'r') {
		x = (m_report.report.data[bb] << 24) + (m_report.report.data[bb+1] << 16) + (m_report.report.data[bb+2] << 8) + m_report.report.data[bb+3];
	} else if (r_code == 'e') {
		x = (m_report.extended.data[bb] << 24) + (m_report.extended.data[bb+1] << 16) + (m_report.extended.data[bb+2] << 8) + m_report.extended.data[bb+3];
	}

	return x;
}

/** convert 4 bytes to single
*
*   Conversion routine to accept the bytes from m_command begining
*	with the byte referenced by the passed index.  The formatted value
* 	is returned.
*
* 	***note x86 is a low-endian machine, the trimble is high-endian
*      so the bytes must be flipped
*
* 	@param   int  the position of the first byte in m_report
* 	@param   char the code r or e to  idenity which array to pull from
*	@return  singel
*/
SINGLE tsip::b4_to_single(int bb, char r_code) {
	union _sgl_t {
		UINT8 data[sizeof(SINGLE)];
		char cdata[sizeof(SINGLE)];
		SINGLE value;
	} sgl;

	//must reverse order of bytes for endian compatability
	if (r_code == 'r') {
		for (int i=0, j=sizeof(SINGLE)-1; i<sizeof(SINGLE); i++, j--) {
			sgl.cdata[j] = m_report.report.data[bb+i];
		}
	} else if (r_code == 'e') {
		for (int i=0, j=sizeof(SINGLE)-1; i<sizeof(SINGLE); i++, j--) {
			sgl.cdata[j] = m_report.extended.data[bb+i];
		}
	}

	return sgl.value;


}

/** convert 8 bytes to double
*
*   Conversion routine to accept the bytes from m_command begining
*	with the byte referenced by the passed index.  The formatted value
* 	is returned.
*
* 	***note x86 is a low-endian machine, the trimble is high-endian
*      so the bytes must be flipped
*
* 	@param   int  the position of the first byte in m_report
* 	@param   char the code r or e to  idenity which array to pull from
*	@return  double
*/
DOUBLE tsip::b8_to_double(int bb, char r_code) {
	union _dbl_t {
		UINT8 data[sizeof(DOUBLE)];
		char cdata[sizeof(DOUBLE)];
		DOUBLE value;
	} dbl;

	//must reverse order of bytes for endian compatability
	if (r_code == 'r') {
		for (int i=0, j=sizeof(DOUBLE)-1; i<sizeof(DOUBLE); i++, j--) {
			dbl.cdata[j] = m_report.report.data[bb+i];
		}
	} else if (r_code == 'e') {
		for (int i=0, j=sizeof(DOUBLE)-1; i<sizeof(DOUBLE); i++, j--) {
			dbl.cdata[j] = m_report.extended.data[bb+i];
		}
	}

	return dbl.value;

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
bool tsip::is_report_found(_command_packet &_cmd) {
	bool isfound = false;

	switch (_cmd.report.code){

		// 0x1e
		case COMMAND_COLD_FACTORY_RESET :
			//0x45 report_sw_version
			if (m_updated.report.sw_version) {
				isfound = true;
			}
			break;

		// 0x1f
		case COMMAND_REQUEST_SW_VERSION :
			//0x45 report_sw_version
			if (m_updated.report.sw_version) {
				isfound = true;
			}
			break;

		// 0x25
		case COMMAND_WARM_RESET_SELF_TEST :
			//0x45 report_sw_version
			if (m_updated.report.sw_version) {
				isfound = true;
			}
			break;

		// 0x35
		case COMMAND_SET_IO_OPTIONS :
			// 0x55	 REPORT_IO_OPTIONS
			if (m_updated.report.io_options) {
				isfound = true;
			}
			break;

		// 0x37
		case COMMAND_REQUEST_POSITION :
			// 0x42 REPORT_ECEF_POSITION_S
			if (m_updated.report.ecef_position_s && m_updated.report.ecef_position_d) {
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
			switch (_cmd.extended.subcode) {
				// 0xa2
				case REPORT_SUPER_UTC_GPS_TIME :
					if (m_updated.report.utc_gps_time) {
						isfound = true;
					}
					break;
				// 0xab
				case REPORT_SUPER_PRIMARY_TIME :
					if (m_updated.report.primary_time) {
						isfound = true;
					}
					break;
				// 0xac
				case REPORT_SUPER_SECONDARY_TIME :
					if (m_updated.report.secondary_time) {
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

/** encode byte stream into TSIP packets
*
*   The TSIP packet encode state machine that collects a stream
*   of bytes into TSIP packets.Encodes byte steam from a serial
*   byte stream from a Trimble Thunderbolt GPSDO into TSIP
*   packets. When a packet has been completed the corresponding
*   report is updated.
*/
int tsip::encode(UINT8 c)
{
	switch (m_state) {

	case START:
		// search for start
		if (c == DLE)
			m_state = FRAME;
		break;

	case FRAME:
		// check if mis-framed
		if (c == DLE || c == ETX)
			m_state = START;
		else {
			m_state = DATA;
			m_report_length = 0;
			m_report.raw.data[m_report_length++] = c;
		}
		break;

	case DATA:
		// found data DLE
		if (c == DLE) {
			m_state = DATA_DLE;
		}
		// add byte to report packet
		else if (m_report_length < MAX_DATA) {
			m_report.raw.data[m_report_length++] = c;
		}
		break;

	case DATA_DLE:
		// escaped data
		if (c == DLE) {
			m_state = DATA;
			if (m_report_length < MAX_DATA) {
				m_report.raw.data[m_report_length++] = c;
			}
		}
		// end of frame
		else if (c == ETX) {
			m_state = START;
			return update_report();    //Whoohoo the moment we've been waitin for
		}
		// mis-framed
		else
			m_state = START;
			if (verbose) printf("waiting gps packet......\n");
		break;

	default:
		m_state = START;
		if (verbose) printf("waiting gps packet......\n");
		break;
	}

	return 0;
}


/** update received report
*
*   Update received report's property buffer.
*
*   @return m_updated to indicate which report was received.
*/
int tsip::update_report()
{
	void *src;
	void *dst;
	int  rlen = 0;

	if (verbose) printf("Found Report: %x-%x\n",m_report.report.code,m_report.extended.subcode);
	// save report
	switch (m_report.report.code) {

	case REPORT_ECEF_POSITION_S:
		m_updated.report.ecef_position_s = 1;
		m_ecef_position_s.valid = true;
		rlen = sizeof(m_ecef_position_s.report);

		m_ecef_position_s.report.x = (m_report.extended.data[0] << 24) + (m_report.extended.data[1] << 16) + (m_report.extended.data[2] << 8) + m_report.extended.data[3];
		m_ecef_position_s.report.y = (m_report.extended.data[4] << 24) + (m_report.extended.data[5] << 16) + (m_report.extended.data[6] << 8) + m_report.extended.data[7];
		m_ecef_position_s.report.z = (m_report.extended.data[8] << 24) + (m_report.extended.data[9] << 16) + (m_report.extended.data[10] << 8) + m_report.extended.data[11];
		m_ecef_position_s.report.time_of_fix = (m_report.extended.data[12] << 24) + (m_report.extended.data[13] << 16) + (m_report.extended.data[14] << 8) + m_report.extended.data[15];

		break;

	case REPORT_ECEF_POSITION_D:
		m_updated.report.ecef_position_d = 1;
		m_ecef_position_d.valid = true;
		src = m_report.report.data;
		dst = &m_ecef_position_d.report;
		rlen = sizeof(m_ecef_position_d.report);
		break;

	case REPORT_ECEF_VELOCITY:
		m_updated.report.ecef_velocity = 1;
		m_ecef_velocity.valid = true;
		src = m_report.report.data;
		dst = &m_ecef_velocity.report;
		rlen = sizeof(m_ecef_velocity.report);
		break;

	case REPORT_SW_VERSION:
		m_updated.report.sw_version = 1;
		m_sw_version.valid = true;
		src = m_report.report.data;
		dst = &m_sw_version.report;
		rlen = sizeof(m_sw_version.report);
		break;

	case REPORT_SINGLE_POSITION:
		m_updated.report.single_position = 1;
		m_single_position.valid = true;
		src = m_report.report.data;
		dst = &m_single_position.report;
		rlen = sizeof(m_single_position.report);
		break;

	case REPORT_DOUBLE_POSITION:
		m_updated.report.double_position = 1;
		m_double_position.valid = true;
		src = m_report.report.data;
		dst = &m_double_position.report;
		rlen = sizeof(m_single_position.report);
		break;

	case REPORT_IO_OPTIONS:
		m_updated.report.io_options = 1;
		m_io_options.valid = true;
		src = m_report.report.data;
		dst = &m_io_options.report;
		rlen = sizeof(m_io_options.report);
		break;

	case REPORT_ENU_VELOCITY:
		m_updated.report.enu_velocity = 1;
		m_enu_velocity.valid = true;
		src = m_report.report.data;
		dst = &m_enu_velocity.report;
		rlen = sizeof(m_enu_velocity.report);
		break;

	case REPORT_SUPER:
		switch (m_report.extended.subcode) {

		// 8f-a2
		case REPORT_SUPER_UTC_GPS_TIME:
			m_updated.report.utc_gps_time = 1;
			m_utc_gps_time.valid = true;
			rlen = sizeof(m_utc_gps_time.report);

			m_utc_gps_time.report.bits.value = m_report.extended.data[0];
			break;

		// 8f-ab
		case REPORT_SUPER_PRIMARY_TIME:
			m_updated.report.primary_time = 1;
			m_primary_time.valid = true;
			rlen = sizeof(m_primary_time.report);

			m_primary_time.report.seconds_of_week = b4_to_uint32(0,'e');
			m_primary_time.report.week_number = b2_to_uint16(4,'e');
			m_primary_time.report.utc_offset = b2_to_uint16(6,'e');
			m_primary_time.report.flags.value = m_report.extended.data[8];
			m_primary_time.report.seconds = m_report.extended.data[9];
			m_primary_time.report.minutes = m_report.extended.data[10];
			m_primary_time.report.hours = m_report.extended.data[11];
			m_primary_time.report.day = m_report.extended.data[12];
			m_primary_time.report.month = m_report.extended.data[13];
			m_primary_time.report.year = b2_to_uint16(14,'e');

			break;

		// 8f-ac
		case REPORT_SUPER_SECONDARY_TIME:
			m_updated.report.secondary_time = 1;
			m_secondary_time.valid = true;
			rlen = sizeof(m_secondary_time.report);

			m_secondary_time.report.receiver_mode = m_report.extended.data[0];
			m_secondary_time.report.disciplining_mode = m_report.extended.data[1];
			m_secondary_time.report.self_survey_progress = m_report.extended.data[2];
			m_secondary_time.report.holdover_duration = b4_to_uint32(3,'e');
			m_secondary_time.report.critical_alarms.value = b2_to_uint16(7,'e');
			m_secondary_time.report.minor_alarms.value = b2_to_uint16(9,'e');;
			m_secondary_time.report.gps_decoding_status = m_report.extended.data[11];
			m_secondary_time.report.disciplining_activity = m_report.extended.data[12];
			m_secondary_time.report.spare_status1 = m_report.extended.data[13];
			m_secondary_time.report.spare_status2 = m_report.extended.data[14];

			m_secondary_time.report.pps_offset = b4_to_single(15,'e');
			m_secondary_time.report.tenMHz_offset = b4_to_single(19,'e');
			m_secondary_time.report.dac_value = b4_to_uint32(23,'e');
			m_secondary_time.report.dac_voltage = b4_to_single(27,'e');
			m_secondary_time.report.temperature = b4_to_single(31,'e');

			m_secondary_time.report.latitude = b8_to_double(35,'e');
			m_secondary_time.report.longitude = b8_to_double(43,'e');
			m_secondary_time.report.altitude = b8_to_double(51,'e');

			m_secondary_time.report.spare[0] = m_report.extended.data[59];
			m_secondary_time.report.spare[1] = m_report.extended.data[60];
			m_secondary_time.report.spare[2] = m_report.extended.data[61];
			m_secondary_time.report.spare[3] = m_report.extended.data[62];
			m_secondary_time.report.spare[4] = m_report.extended.data[63];
			m_secondary_time.report.spare[5] = m_report.extended.data[64];
			m_secondary_time.report.spare[6] = m_report.extended.data[65];
			m_secondary_time.report.spare[7] = m_report.extended.data[66];
			break;

		default:
			m_updated.report.unknown = 1;
			m_unknown.valid = true;
		    src = m_report.raw.data;
			dst = m_unknown.report.raw.data;
			rlen = sizeof(m_unknown.report.raw.data);
			break;
		}

	default:
		m_updated.report.unknown = 1;
		m_unknown.valid = true;
	    src = m_report.raw.data;
		dst = m_unknown.report.raw.data;
		rlen = sizeof(m_unknown.report.raw.data);
        break;
	}

	// report strucute updated
	if (rlen > 0 ) {
		if (debug) {
			printf("command buffer:\n");
			for (int k=0;k<24;k++){printf(" %x",m_command.raw.data[k]);}
			printf("\n");
			printf("\nreport buffer:\n");
			for (int k=0;k<m_report_length;k++){printf(" %x",m_report.raw.data[k]);}
			printf("\n");
		}


		return 1;
	}


	return 0;
}

/** get_request_msg
*
*   send a sequence of commands to the gps.
*
*   @return bool
*/
bool tsip::send_request_msg(_command_packet _cmd) {

	unsigned char buffer[256];
	buffer[0] = DLE;
	int x = 1;
	for (int j=0; j < _cmd.raw.cmd_len; j++,x++){
		buffer[x] = _cmd.raw.data[j];
	}
	buffer[x] = DLE;
	buffer[x+1] = ETX;
	int byte_cnt = fwrite(buffer, 1, x+1, file);
	if (verbose) {
		printf("Sending Request: ");
		for (int k=0;k<=x+1;k++){printf(" %x",buffer[k]);}
		printf("\n");
	}

	return (byte_cnt == x+1 ? true : false);
}

/** get_report_msg
*
*   send a sequence of commands to the gps.  The loop is continued until
*   the correct message id is returned.
*
*   @return bool
*/
bool tsip::get_report_msg(_command_packet _cmd) {
	//clear report flags
	init_rpt();

	// init local var
    unsigned char ch = 0;


RETRY:
	// send the commmand
    send_request_msg(m_command);

	// read stream and pass to encode routine untile packet complete
	int loop_cnt=0;
	while (!is_report_found(_cmd)) {
		int rc = 0;
		while (rc == 0) {
			rc = encode(getc(file));
		}
		loop_cnt++;
		if (loop_cnt >20) break;
	}

	//set flag
	bool rpt_fnd = is_report_found(_cmd);


	if(verbose){
		if (rpt_fnd) {
				printf("Packet %x %x found \n",m_report.report.code,m_report.extended.subcode);
			}else {
				printf("Packet for  %x %x not found \n",m_report.report.code,m_report.extended.subcode);
		}
		printf("\n");
	}

	return (rpt_fnd);
}

/** get gps time in utc seconds
*
*   updates the time_t referenced structure passed to method.
*

*   @return time_t
*/
time_t tsip::get_gps_time_utc() {
	bool rc;

	//build a2 request - set UTC
	m_command.extended.code = COMMAND_SUPER_PACKET;
	m_command.extended.subcode = REPORT_SUPER_UTC_GPS_TIME;
	m_command.extended.data[0] = 0x3;
	m_command.extended.cmd_len = 3;

	rc = send_request_msg(m_command);

	//build ab request - request time packet
	m_command.extended.code = COMMAND_SUPER_PACKET;
	m_command.extended.subcode = REPORT_SUPER_PRIMARY_TIME;
	m_command.extended.cmd_len  = 2;
	rc = get_report_msg(m_command);
	if (!rc) {
		printf("****Failed to get GPS time****\n");
		exit (false);
    }


	struct tm time;

    time.tm_zone = "UTC";
    time.tm_wday = -1;
    time.tm_yday = -1;
    time.tm_isdst = -1;
    time.tm_year = m_primary_time.report.year - 1900;
    time.tm_mon = m_primary_time.report.month - 1;
    time.tm_mday = m_primary_time.report.day;
    time.tm_hour = m_primary_time.report.hours;
    time.tm_min = m_primary_time.report.minutes;
    time.tm_sec = m_primary_time.report.seconds;

    gps_time = timegm(&time);
    if (verbose) {
		printf("Got GPS time Year: %d, Month: %d, Day: %d, Hour: %d, Minutes: %d, Seconds: %d\n", time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
		printf("seconds: %d\n", gps_time);
    }

	return gps_time;
}

/** get xyz from gps
*
*   Get the xyz (lat, long, alt) from the gps.
*
*   @return xyz_t lat, long, alt
*/
tsip::xyz_t tsip::get_xyz() {

	bool rc;
	//build ab request - request time packet
	m_command.extended.code = COMMAND_SUPER_PACKET;
	m_command.extended.subcode = REPORT_SUPER_SECONDARY_TIME;
	m_command.extended.cmd_len  = 2;
	rc = get_report_msg(m_command);

	if (rc) {
		xyz.latitude= m_secondary_time.report.latitude * _rad;
		xyz.longitude= m_secondary_time.report.longitude * _rad;
		xyz.altitude= m_secondary_time.report.altitude;
	} else {
		xyz.latitude=0;
		xyz.longitude=0;
		xyz.altitude=0;
	}
	return xyz;
}


/** revert_to_default 8E-45
*
* revert a segment to factory default
* default is to revert all segments
*  valid segments are 0x03-0x09 inclusive
*     3 - receiver configuration
*     4 - packet i/o
*     5 - Serial port configuration
*     6 - Timing output configuration
*     7 - accurate position
*     8 - self-survey configuration
*     9 - disciplining configuration
*  0xff - all segments
*
*   @return bool rc
*/
bool tsip::revert_to_default(int seg_num=0xff) {
	bool rc;

	//build 8E- request - set autosave
	m_command.extended.code = COMMAND_SUPER_PACKET;
	m_command.extended.subcode = COMMAND_REVERT_TO_DEFAULT;
	m_command.extended.data[0] = seg_num;

	m_command.extended.cmd_len = 3;

	rc = send_request_msg(m_command);
	return rc;
}


/** save_to_eeprom  8E-4C
*
*   save all segments to eeprom
*      default is to save all segments
*      valid segments are 0x03-0x09 inclusive
*       3 - receiver configuration
*       4 - packet i/o
*       5 - Serial port configuration
*       6 - Timing output configuration
*       7 - accurate position
*       8 - self-survey configuration
*       9 - disciplining configuration
*    0xff - all segments
*
*   @return bool rc
*/
bool tsip::save_to_eeprom(int seg_num=0xff) {
	bool rc;

	//build 8E- request - set autosave
	m_command.extended.code = COMMAND_SUPER_PACKET;
	m_command.extended.subcode = COMMAND_SAVE_EEPROM ;
	m_command.extended.data[0] = seg_num;

	m_command.extended.cmd_len = 3;

	rc = send_request_msg(m_command);
	return rc;
}


/** start self survey  8E-A6
*
*   start the self_survey
*   data values
*    0 - restart self-survey
*    1 - save position to flash
*    2 - delete position from flash
*
*   @return bool rc
*/
bool tsip::start_self_survey() {
	bool rc;

	//build 8E-A6 request - start self survey
	m_command.extended.code = COMMAND_SUPER_PACKET;
	m_command.extended.subcode = COMMAND_SELF_SURVEY;
	m_command.extended.data[0] = 0;
	m_command.extended.cmd_len = 3;

	rc = send_request_msg(m_command);
	return rc;
}

/** set_survey_period  8E-A9
*
*   updates the survey count.  Default is 2000.
*
*   @return bool rc
*/
bool tsip::set_survey_params(int survey_cnt) {
	bool rc;

	//build 8E-A9 request - set survey period
	m_command.data_8ea9.code = COMMAND_SUPER_PACKET;
	m_command.data_8ea9.subcode = COMMAND_SET_SELF_SURVEY_PARAMS;

	//enable survey
	m_command.data_8ea9.enable_survey = 1;

	//save position
	m_command.data_8ea9.save_position = 0;

	//survey length
	m_command.data_8ea9.self_survey_length = survey_cnt;
	m_command.data_8ea9.reserved_8ea9 = 0;
	m_command.data_8ea9.cmd_len = 12;


	rc = send_request_msg(m_command);
	return rc;
}
