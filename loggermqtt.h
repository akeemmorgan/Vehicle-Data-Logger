#ifndef LOGGERMQTT_H_INCLUDED
#define LOGGERMQTT_H_INCLUDED
/** @file loggermqtt.h
 *  @author Akeem Morgan
 *  @date 14MAR22
 *  @brief Constants, structures, function prototypes for logger MQTT functions
 */

// Constants
#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "VehicleDataLogger"
#define TOPIC       "Logger Data"
#define QOS         1
#define TIMEOUT     10000L

// Function Prototypes
///\cond INTERNAL
int DlPublishLoggerData(const char *mqttdata);
///\endcond
#endif // LOGGERMQTT_H_INCLUDED
