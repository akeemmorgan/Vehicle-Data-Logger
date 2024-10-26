/** @file logger.cpp
 *  @author Akeem Morgan
 *  @date 24JAN22
 *  @brief Vehicle Data Logger functions
 */
#include "logger.h"
#include "loggermqtt.h"
#include "sensehat.h"

/* Global objects */
SenseHat Sh;

/** @brief Prints the logger program header
 *  @author Akeem Morgan
 *  @date 14FEB2022
 *  @param void
 *  @return 1
 */
int DlInitialization(void)
{
    fprintf(stdout, "\nAkeem Morgan's CENG252 Vehicle Data Logger\n"
            "Data Logger Initialization\n");
    return 1;
}

/** @brief Obtain serial number for display
 *  @author Akeem Morgan
 *  @date 24JAN2022
 *  @param void
 *  @return unsigned integer 'serial' with the hw serial no
 */
uint64_t DlGetSerial(void)
{
    static uint64_t serial = 0;
	FILE *fp;
	char buf[SYSINFOBUFSZ];
	char searchstring[] = SEARCHSTR;
	fp = fopen ("/proc/cpuinfo", "r");
	if (fp != NULL)
	{
        while (fgets(buf, sizeof(buf), fp) != NULL)
		{
			if (!strncasecmp(searchstring, buf, strlen(searchstring)))
			{
				sscanf(buf+strlen(searchstring), "%Lx", &serial);
			}
		}
		fclose(fp);
	}
    if (serial==0)
    {
        system("uname -a");
        system("ls --fu /usr/lib/codeblocks | grep -Po '\\.\\K[^ ]+'>stamp.txt");
        fp = fopen ("stamp.txt", "r");
        if (fp != NULL)
        {
            while (fgets(buf, sizeof(buf), fp) != NULL)
            {
                sscanf(buf, "%Lx", &serial);
            }
            fclose(fp);
        }
    }
	return serial;
}

/** @brief Assign values
 *  @author Akeem Morgan
 *  @date 14FEB2022
 *  @param void
 *  @return creads
 */
reading_s DlGetLoggerReadings(void)
{
        reading_s creads = {0};
        loc_t gpsdata = {0.0};

        creads.rtime = time(NULL);

    #if SENSEHAT
        creads.temperature = Sh.GetTemperature();
        creads.humidity = Sh.GetHumidity();
        creads.pressure = Sh.GetPressure();
        usleep(IMUDELAY);
        Sh.GetAcceleration(creads.xa, creads.ya, creads.za);
        usleep(IMUDELAY);
        Sh.GetOrientation(creads.pitch, creads.roll, creads.yaw);
        usleep(IMUDELAY);
        Sh.GetMagnetism(creads.xm, creads.ym, creads.zm);
    #else
        creads.temperature = DTEMP;
        creads.humidity = DHUMID;
        creads.pressure = DPRESS;
        creads.xa = DXA;
        creads.ya = DYA;
        creads.za = DZA;
        creads.pitch = DPITCH;
        creads.roll = DROLL;
        creads.yaw = DYAW;
        creads.xm = DXM;
        creads.ym = DYM;
        creads.zm = DZM;
    #endif // SENSEHAT

    #if GPSDEVICE
        gpsdata = DlGpsLocation();
        creads.latitude = gpsdata.latitude;
        creads.longitude = gpsdata.longitude;
        creads.altitude = gpsdata.altitude;
        creads.speed = gpsdata.speed;
        creads.heading = gpsdata.course;
    #else
        creads.latitude = DLAT;
        creads.longitude = DLONG;
        creads.altitude = DALT;
        creads.speed = DSPEED;
        creads.heading = DHEADING;
    #endif // GPSDEVICE

        return creads;
}

/** @brief Prints the time of the readings and the sensor values to the console
 *  @author Akeem Morgan
 *  @date 24JAN2022
 *  @param readings_s datatype 'dreads' to pass simulated sensor values and the time
 *  @return void
 */
void DlDisplayLoggerReadings(reading_s dreads)
{
    fprintf(stdout, "\nUnit: %LX %sT: %2.1fC\tH: %2.0f%%\tP: %4.1fkPa"
            "\nXa: %fg\tYa: %fg\tZa: %fg"
            "\nPitch: %f\tRoll: %f\tYaw: %f"
            "\nXm: %f\tYm: %f\tZm: %f"
            "\nLatitude: %f\tLongitude: %f\tAltitude: %f"
            "\nSpeed: %f\tHeading: %f\n",
            DlGetSerial(), ctime(&dreads.rtime),
            dreads.temperature, dreads.humidity, dreads.pressure,
            dreads.xa, dreads.ya, dreads.za,
            dreads.pitch, dreads.roll, dreads.yaw,
            dreads.xm, dreads.ym, dreads.zm,
            dreads.latitude, dreads.longitude, dreads.altitude,
            dreads.speed, dreads.heading);
}

/** @brief Save readings to CSV and JSON
 *  @author Akeem Morgan
 *  @date 14MAR2022
 *  @param readings_s datatype "creads"
 *  @return int rc MQTT status
 */
int DlSaveLoggerData(reading_s creads)
{
    FILE *fp;
    char ltime[TIMESTRSZ];
    char jsondata[PAYLOADSTRSZ];
    int rc;

    // Write data to CSV file
    fp = fopen("loggerdata.csv", "a");
    if (fp == NULL)
    {
        return 0;
    }
    strcpy(ltime, ctime(&creads.rtime));
    ltime[3] = ',';
    ltime[7] = ',';
    ltime[10] = ',';
    ltime[19] = ',';
    fprintf(fp, "%.24s,%3.1f,%3.0f,%3.1f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                ltime,creads.temperature, creads.humidity, creads.pressure, creads.xa,
                creads.ya, creads.za, creads.pitch, creads.roll, creads.yaw, creads.xm,
                creads.ym, creads.zm, creads.latitude, creads.longitude, creads.altitude,
                creads.speed, creads.heading);
    fclose(fp);

    // Write data to JSON file
    sprintf(jsondata, "{\"temperature\":%-3.1f,\"humidity\":%-3.0f,\"pressure\":%-3.1f,"
                        "\"xa\":%-f,\"ya\":%-f,\"za\":%-f,\"pitch\":%-f,\"roll\":%-f,"
                        "\"yaw\":%-f,\"xm\":%-f,\"ym\":%-f,\"zm\":%-f,\"latitude\":%-f,"
                        "\"longitude\":%-f,\"altitude\":%-f,\"speed\":%-f,\"heading\":%-f,\"active\": true}",
                        creads.temperature, creads.humidity, creads.pressure, creads.xa, creads.ya, creads.za,
                        creads.pitch, creads.roll, creads.yaw, creads.xm, creads.ym, creads.zm, creads.latitude,
                        creads.longitude, creads.altitude,creads.speed, creads.heading);
    fp = fopen("loggerdata.json", "w");
    if (fp == NULL)
    {
        return -1;
    }
    fprintf(fp, jsondata);
    fclose(fp);

    rc = DlPublishLoggerData(jsondata);

    return rc;
}

/** @brief Display logo on RGB565
 *  @author Akeem Morgan
 *  @date 02FEB2022
 *  @param void
 *  @return void
 */
void DlDisplayLogo(void)
{
    uint16_t logo[8][8] = {
        HB,HB,HB,HB,HB,HB,HB,HB,
        HB,HB,HW,HB,HB,HW,HB,HY,
        HB,HB,HW,HB,HB,HW,HY,HY,
        HB,HB,HW,HB,HB,HW,HY,HY,
        HB,HB,HW,HW,HW,HW,HY,HY,
        HB,HB,HW,HY,HY,HW,HY,HY,
        HB,HY,HW,HY,HY,HW,HY,HY,
        HY,HY,HY,HY,HY,HY,HY,HY,
    };
    Sh.WipeScreen();
    Sh.ViewPattern(logo);
}

/** @brief Shifts a group of four pixels on the LED matrix in response to variation in accelerations in the X and Y planes
 *  @author Akeem Morgan
 *  @date 02FEB2022
 *  @param float xa, floatya
 *  @return void
 */
void DlUpdateLevel(float xa, float ya)
{
    int x, y;
    Sh.WipeScreen();
    y = (int) (xa * -30.0 + 4);
    x = (x < 0) ? 0 : ((x > 6) ? 6 : x);
    y = (y < 0) ? 0 : ((y > 6) ? 6 : y);
    Sh.LightPixel(x, y, HY);
    Sh.LightPixel(x + 1, y, HY);
    Sh.LightPixel(x, y + 1, HY);
    Sh.LightPixel(x + 1, y + 1, HY);
}
