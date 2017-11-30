#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "etap.h"
#include "EventDefinitions.h"
#include "IpcTxTask.h"
#include "IpcProtocolLpm.h"  
#include "IPCRouterTask.h"
#include "IPCBulk.h"
#include "rottenlog.h"
#include "buffermanager.h"
#include <stdlib.h>
#include <string.h>
#include "etapLightSensor.h"
#include "etapIpc.h"
#include "LightSensorTask.h"

void TAPLightSensor(CommandLine_t* CommandLine)
{
    switch (CommandLine->numArgs)
    {
        case 0:
        {
            char     message[256];
            uint16_t lux_decimal     = 0;
            uint16_t lux_fractional = 0;
            
            if (LightSensorTask_Read(&lux_decimal, &lux_fractional) != SUCCESS)
            {
                TAP_PrintString("error: failed to read light sensor LUX");
                break;
            }
            
            memset (message, '\0', sizeof(message));
            sprintf(message, "light sensor LUX: %d.%d", lux_decimal, lux_fractional);
                    
            TAP_PrintString(message);
            break;
        }// 0
       
        default:
        {
             TAP_PrintString(LIGHT_SENSOR_HELP_TEXT);
             break;
        }
    }
}// TAPLightSensor
