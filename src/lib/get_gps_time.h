#ifndef GET_GPS_TIME_H
#define GET_GPS_TIME_H

//#define SERIAL_DEVICE "/dev/ttyS0"      
//#define SERIAL_DEVICE "/dev/ttyUSB0"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

void setup_serial_port(FILE *file)
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

bool get_time(std::string &gps_port, int &year, int &month, int &day, int &hour, int &minute, int &second)
{
    //FILE *file = fopen(SERIAL_DEVICE, "r");
    FILE *file = fopen(gps_port.c_str(), "r");

    if(!file)
    {
        //printf("Cannot open %s\n", SERIAL_DEVICE);
        printf("Cannot open %s\n", gps_port.c_str());
        return false;
    }

    setup_serial_port(file);

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

bool get_gps_time_utc(std::string &gps_port,time_t &seconds_since_epoch)
{
    struct tm time;
    if(get_time(gps_port,time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec))
    {
        time.tm_year = time.tm_year - 1900;
        time.tm_mon = time.tm_mon - 1;
        time.tm_zone = "UTC";
        time.tm_wday = -1;
        time.tm_yday = -1;
        time.tm_isdst = -1;

        seconds_since_epoch = timegm(&time);
        printf("Got GPS time Year: %d, Month: %d, Day: %d, Hour: %d, Minutes: %d, Seconds: %d\n", 
            time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
        return true;
    }
    else
        return false;
}

#if 0
int main()
{
    int year, month, day, hour, minute, second;
    if(get_time(year, month, day, hour, minute, second))
        printf("Year: %d, Month: %d, Day: %d, Hour: %d, Minutes: %d, Seconds: %d\n", year, month, day, hour, minute, second);
    return 0;
}
#endif

#endif //GET_GPS_TIME_H
