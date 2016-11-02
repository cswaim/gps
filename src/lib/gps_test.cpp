#include <tsip.h>
#include <iostream>
//#include "get_gps_time.h"
void print_report();
std::string port = "/dev/ttyUSB0";
//std::string port = "/dev/ttyS5";
tsip::xyz_t xyz;
time_t gps_time;
tsip gps;

int main(int argc,char **argv)
{
	//check for port
	if (argc > 1) {
		std::cout << "  argc: " << argc << std::endl;
		std::cout << "argv 0: " << argv[0] << std::endl;
		std::cout << "argv 1: " << argv[1] << std::endl;
		port = argv[1];
	}
	
	//std::string port = "/dev/ttyUSB0";
    int year, month, day, hour, minute, second;


	//printf("----quiet---getting time--TSIP-------\n");
   // gps.set_verbose(false);
   // gps_time = gps.get_gps_time_utc();
    //gps.set_verbose(true);

	//tsip get time
    printf("---------getting time--TSIP-------\n");
    std::cout << "opening port " << port << std::endl;
    gps.set_gps_port(port);
    bool rc = gps.open_gps_port();
    printf("open rc: %i \n",rc);
    if (!rc){
		printf("--open failed, terminating job\n");
		exit(1);
	}
	std::cout << "get utc time"<<std::endl;
	gps_time = gps.get_gps_time_utc();
    if (gps_time) {
        printf("\nYear: %d, Month: %d, Day: %d, Hour: %d, Minutes: %d, Seconds: %d\n", year, month, day, hour, minute, second);
        printf("seconds: %d\n", gps_time);
    }
    print_report();

    //orig get time
	printf("-----------getting gps time .h----\n");
	//time_t seconds_since_epoch;
	//get_gps_time_utc(port,seconds_since_epoch);
	//printf("Orig seconds: %d\n", seconds_since_epoch);

	//get zyz
	printf("---------getting xyz---------\n");
    xyz = gps.get_xyz();
    print_report();

    //set self survey parameters
    rc = gps.set_survey_params(60);
    std::cout << "self survey params rc: " << rc << std::endl;
    //clear position & start self-survey
    rc = gps.revert_to_default(7);
    std::cout << "revert to default rc: " << rc << std::endl;
	//start self survey
    rc = gps.start_self_survey();
    std::cout << "self survey rc: " << rc << std::endl;

	//get out
    return 0;
}

void print_report() {

	if (gps.m_updated.report.primary_time) {

		printf("\n");
		printf("Primary Time\n");
		printf("Seconds of week: %li\n",gps.m_primary_time.report.seconds_of_week);
		printf("    Week Number: %i\n",gps.m_primary_time.report.week_number);
		printf("     UTC Offset: %i\n",gps.m_primary_time.report.utc_offset);
		printf("       Bit flag: %u\n",gps.m_primary_time.report.flags.value);
		printf("        Seconds: %i\n",gps.m_primary_time.report.seconds);
		printf("        Minutes: %i\n",gps.m_primary_time.report.minutes);
		printf("          Hours: %i\n",gps.m_primary_time.report.hours);
		printf("            Day: %i\n",gps.m_primary_time.report.day);
		printf("          Month: %i\n",gps.m_primary_time.report.month);
		printf("           Year: %i\n",gps.m_primary_time.report.year);
	}
	//ac
	if (gps.m_updated.report.secondary_time) {

		printf("\n");
		printf("Secondary Time\n");
		printf("       receiver mode: %x \n",gps.m_secondary_time.report.receiver_mode);
		printf("   Disciplining mode: %x\n",gps.m_secondary_time.report.disciplining_mode);
		printf("Self Survey Progress: %i \n",gps.m_secondary_time.report.self_survey_progress);
		printf("   Holdover Duration: %i \n",gps.m_secondary_time.report.holdover_duration);
		printf("     Critical Alarms: %x\n",gps.m_secondary_time.report.critical_alarms.value);
		printf("       Minor Allarms: %x\n",gps.m_secondary_time.report.minor_alarms.value);
		printf(" gps decoding status: %x\n",gps.m_secondary_time.report.gps_decoding_status);
		printf(" disciplining status: %x\n",gps.m_secondary_time.report.disciplining_activity);
		printf("          pps offset: %f\n",gps.m_secondary_time.report.pps_offset);
		printf("              10 MHz: %f\n",gps.m_secondary_time.report.tenMHz_offset);
		printf("           DAC Value: %x\n",gps.m_secondary_time.report.dac_value);
		printf("         dac Voltage: %f %x\n",gps.m_secondary_time.report.dac_voltage,gps.m_secondary_time.report.dac_voltage);
		printf("         Temperature: %f %x\n",gps.m_secondary_time.report.temperature,gps.m_secondary_time.report.temperature);
		printf("        Rad Latitude: %f\n",gps.m_secondary_time.report.latitude);
		printf("       Rad Longitude: %f\n",gps.m_secondary_time.report.longitude);
		printf("            Altitude: %f\n",gps.m_secondary_time.report.altitude);

		double _cf = 180/M_PI;
		double _xl = gps.m_secondary_time.report.latitude * _cf;
		printf(" C++        Latitude: %.9f\n",_xl);
		double _yl = gps.m_secondary_time.report.longitude * _cf;
		printf(" C++            Long: %.9f\n",_yl);
		printf(" rtn            Lat : %.9f\n",xyz.latitude);
		printf(" rtn            Long: %.9f\n",xyz.longitude);
		printf(" rtn             Alt: %.9f\n",xyz.altitude);
	}
	if (gps.m_updated.report.ecef_position_s) {

		printf("\n");
		printf("   Latitude: %f %x\n",gps.m_ecef_position_s.report.x,gps.m_ecef_position_s.report.x);
		printf("  Longitude: %f %x\n",gps.m_ecef_position_s.report.y,gps.m_ecef_position_s.report.y);
		printf("   Altitude: %f %x\n",gps.m_ecef_position_s.report.z,gps.m_ecef_position_s.report.z);
		printf("time of fix: %f\n",gps.m_ecef_position_s.report.time_of_fix);

	}
	if (gps.m_updated.report.ecef_position_d) {

		printf("\n");
		printf("ecef Position D\n");
		printf("   Latitude: %d %x\n",gps.m_ecef_position_d.report.x,gps.m_ecef_position_d.report.x);
		printf("  Longitude: %d %x\n",gps.m_ecef_position_d.report.y,gps.m_ecef_position_d.report.y);
		printf("   Altitude: %d %x\n",gps.m_ecef_position_d.report.z,gps.m_ecef_position_d.report.z);
		printf(" clock bias: %d\n",gps.m_ecef_position_d.report.clock_bias);
		printf("time of fix: %d\n",gps.m_ecef_position_d.report.time_of_fix);

	}
	if (gps.m_updated.report.utc_gps_time) {

		printf("\n");
		printf("UTC GPS Time\n");
		printf("Seconds of week: %li %x\n",gps.m_primary_time.report.seconds_of_week,gps.m_primary_time.report.seconds_of_week);
		printf("    Week Number: %i %x\n",gps.m_primary_time.report.week_number,gps.m_primary_time.report.week_number);
		printf("     UTC Offset: %i %x\n",gps.m_primary_time.report.utc_offset,gps.m_primary_time.report.utc_offset);
		printf("       Bit flag: %u\n",gps.m_primary_time.report.flags.value);
		printf("        Seconds: %i\n",gps.m_primary_time.report.seconds);
		printf("        Minutes: %i\n",gps.m_primary_time.report.minutes);
		printf("          Hours: %i\n",gps.m_primary_time.report.hours);
		printf("            Day: %i\n",gps.m_primary_time.report.day);
		printf("          Month: %i\n",gps.m_primary_time.report.month);
		printf("           Year: %i %x\n",gps.m_primary_time.report.year,gps.m_primary_time.report.year);
	}
}
