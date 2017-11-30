#ifndef ETAPLIGHTSENSOR_H
#define  ETAPLIGHTSENSOR_H

#include "etapcoms.h"

/**
 * @DOCETAP 
 * COMMAND:   lightsensor
 *
 * ALIAS:     pt
 *
 * DESCRIP:   backlight tap command
 *
 * PARAM:     destination, command
 *
 * REPLY:     Depends
 *
 * EXAMPLE:   pt
 *
 * @DOCETAPEND
 */

#define LIGHT_SENSOR_HELP_TEXT "Get the light sensor LUX value\n\r\t Usage: ls\n\r"

void TAPLightSensor(CommandLine_t*);

#endif //  ETAPLIGHTSENSOR_H