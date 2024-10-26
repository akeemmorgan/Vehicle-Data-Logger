/** @file vdl.cpp
 *  @author Akeem Morgan
 *  @date 31MAR22
 *  @brief Vehicle Data Logger main functions
 */
#include "stdafx.h"
#include "dlfirmata.h"
#include "logger.h"
#include <ofArduino.h>
#include "vdl.h"


/* Global objects */
ArduinoFirmata ard;

/** @brief Vehicle Data Logger main function
 *  @author Akeem Morgan
 *  @date 31MAR22
 *  @param void
 *  @return int program status
 */
int main(void)
{
    reading_s current = {0};
    int tc = 0;

    if(!ard.connect("ttyACM0", 57600)) // Serial Port and Baud Rate
    {
        fprintf(stdout,"\nFailed to connect to arduino!");
        return -1;
    }
    /* Need to do this to init the pins, get the firmware version, and  call setupArduino.
    Will stay in update loop looking for signal. When it arrives Setup will be called
    and we can start processing. */
    ard.sendReset();
    ard.sendProtocolVersionRequest();
    ard.sendFirmwareVersionRequest();

    ard.setupArduino(1);

    DlGpsInit();
    DlInitialization();
    DlDisplayLogo();
    sleep(3);

    while(1)
    {
        ard.update();
        ard.sendDigital(WHITELED, ARD_HIGH);
        current = DlGetLoggerReadings();
        DlUpdateLevel(current.xa, current.ya);
        if (tc == LOGCOUNT)
        {
            DlDisplayLoggerReadings(current);
            DlSaveLoggerData(current);

            if (ard.getDigital(LSWITCH) == ARD_HIGH)
            {
                ard.sendDigital(BEEPER, ARD_HIGH);
            }
            else
            {
                ard.sendDigital(REDLED, ARD_HIGH);
            }
            usleep(SLEEPTIME);

            ard.sendDigital(WHITELED, ARD_LOW);
            ard.sendDigital(REDLED, ARD_LOW);
            ard.sendDigital(BEEPER, ARD_LOW);
            usleep(SLEEPTIME);

            tc = 0;
        }
        else
        {
            usleep(SLEEPTIME);
            tc++;
        }
    }
    return 1;
}
