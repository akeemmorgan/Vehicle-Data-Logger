#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED
/** @file logger.h
 *  @author Akeem Morgan
 *  @date 16FEB22
 *  @brief Constants, structures, function prototypes for logger functions
 */

// Includes
#include <cinttypes>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "dlgps.h"

// Constants
#define SEARCHSTR "serial\t\t:"
#define SYSINFOBUFSZ 512
#define SENSEHAT 1
#define HB 0x00E7
#define HY 0xC4A0
#define HW 0xFFFF
#define GPSDEVICE 1
#define TIMESTRSZ 25
#define PAYLOADSTRSZ 400

// Default Logger Data Values
#define DTEMP 24.6
#define DHUMID 32
#define DPRESS 1013.5
#define DXA 1
#define DYA 1
#define DZA 1
#define DPITCH 10
#define DROLL 15
#define DYAW 20
#define DXM 1
#define DYM 1
#define DZM 1
#define DLAT 43.7289
#define DLONG -79.6074
#define DALT 166
#define DSPEED 99
#define DHEADING 320

// Structures
typedef struct readings
{
    time_t rtime;       ///<Reading time
    float temperature;  ///<Degrees Celsius
    float humidity;     ///<Per cent relative humidity
    float pressure;     ///<Kilo Pascals
    float xa;           ///<X-axis accelaration
    float ya;           ///<Y-axis accelaration
    float za;           ///<Z-axis accelaration
    float pitch;        ///<Pitch angle
    float roll;         ///<Roll angle
    float yaw;          ///<Yaw angle
    float xm;           ///<X axis micro Teslas
    float ym;           ///<Y axis micro Teslas
    float zm;           ///<Z axis micro Teslas
    float latitude;		///<Latitude
    float longitude;    ///<Longitude
    float altitude;     ///<Altitude
    float speed;        ///<Speed kph
    float heading;      ///<Heading degrees True
}reading_s;

// Function Prototypes
///\cond INTERNAL
int DlInitialization(void);
uint64_t DlGetSerial(void);
reading_s DlGetLoggerReadings(void);
void DlDisplayLoggerReadings(reading_s dreads);
int DlSaveLoggerData(reading_s creads);
void DlDisplayLogo(void);
void DlUpdateLevel(float xa, float ya);
///\endcond
#endif // LOGGER_H_INCLUDED

