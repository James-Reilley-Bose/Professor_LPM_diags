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
#include "etapBackLight.h"
#include "etapIpc.h"
#include "BackLightTask.h"
 
void TAPBackLight(CommandLine_t* CommandLine)
{
    switch (CommandLine->numArgs)
    {
        case 0:
        {
            char message[256];
            
            memset (message, '\0', sizeof(message));
            sprintf(message, "back light intensity: %u", BackLightTask_Get());
                    
            TAP_PrintString(message);
            break;
        }// 0
      
        case 1:
        {
            ipc_uint32_t intensity = atoi(CommandLine->args[0]);
            
            if (intensity > 100)
            {
                char message[256];
            
                memset (message, '\0', sizeof(message));
                sprintf(message, "error: invalid intensity: %s, possible values: [0..100]", CommandLine->args[0]);
                TAP_PrintString(message);
            }
            else 
            {
                BackLightTask_Set(intensity);
            }
            break;
        }// 1
        
        default:
        {
             TAP_PrintString(BACK_LIGHT_HELP_TEXT);
             break;
        }
    }
}// TAPBackLight
