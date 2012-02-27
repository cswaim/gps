#include "gps_api.h"

int main()
{
	std::string port = "/dev/ttyUSB0";
    int year, month, day, hour, minute, second;
	time_t gps_time;
	gps_api gps(port);
    
    if(gps.get_gps_time_utc(gps_time)) {
        printf("\nYear: %d, Month: %d, Day: %d, Hour: %d, Minutes: %d, Seconds: %d\n", year, month, day, hour, minute, second);
        printf("seconds: %d\n", gps_time);
    }
    //test build cmd
    printf("---------getting time---------\n");
    gps.send_get_time();
    //return 0;
	//get zyz
	printf("---------getting xyz---------\n");
    //gps_api::xyz_t lla;
    bool rc = gps.get_xyz();
    //return 0;

    
	//get out
    return 0;
}

