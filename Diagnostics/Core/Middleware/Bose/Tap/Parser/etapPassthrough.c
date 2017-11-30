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
#include "etapPassThrough.h"
#include "etapIpc.h"
#include "IpcPassthrough.h"

/*
BOOL destinationAvailable(uint8_t dest)
{
  return TRUE;
}
*/

void TAP_PtCommand(CommandLine_t* CommandLine)
{
    BOOL valid = FALSE;
    uint8_t dest;
    int32_t length;

    if (CommandLine->numArgs < 2)
    {
        TAP_PrintString(PASS_THROUGH_HELP_TEXT);
        return;
    }

    length = ( strlen(CommandLine->args[1]) - 2) ;

    valid = stringToDestination(CommandLine->args[0], &dest);
    if (valid == FALSE || !(dest == IPC_DEVICE_DSP || dest == IPC_DEVICE_AP || dest == IPC_DEVICE_SOUNDTOUCH))
    {
        TAP_PrintString("Invalid Destination");
        return;
    }

    //Derek Richardson TODO - Check if destination is available/present before posting
    /*
    if(!destinationAvailable(uint8_t dest))
    {
      //TAP_PrintString("Destination Unavailable");
      //return
    }
    */

    if (length > 0 && CommandLine->args[1][0] ==  '\"' && CommandLine->args[1][strlen(CommandLine->args[1]) - 1] == '\"')
    {
        //Little hack to remove quotes
        CommandLine->args[1] = CommandLine->args[1] + 1;
        CommandLine->args[1][strlen(CommandLine->args[1]) - 1] = '\0';

        unsigned char* data = BufferManagerGetBuffer(length + 1);
        debug_assert(data);
        memcpy(data, CommandLine->args[1], length + 1);

        IpcPassThrough_Send(dest, data, length + 1);
        BufferManagerFreeBuffer(data);
    }
    else
    {
        TAP_PrintString("Please wrap command in quotations!");
    }
}
