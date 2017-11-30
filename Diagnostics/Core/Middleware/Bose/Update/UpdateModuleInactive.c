//
// UpdateModuleInactive.c
//

#include "UpdateModuleInactive.h"
#include "UpdateManagerTask.h"
#include "IpcProtocolLPM.h"
#include "IPCRouterTask.h"

static BOOL UpdateModuleBase_Inactive_HandlePacketAvailable(GENERIC_MSG_t* msg);

void UpdateModuleInactive_Init(void)
{

}

BOOL UpdateModuleInactive_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            handled = UpdateModuleBase_Inactive_HandlePacketAvailable(msg);
            break;
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

void UpdateModuleInactive_Enter(void)
{

}

void UpdateModuleInactive_Exit(void)
{
    // TODO - dj1005472 - I don't think we need this anymore... Take it out once sure.
    //UpdateState.ReceivedRebootPlease = FALSE; // reset to FALSE, we can end up back here after an abort
}

static BOOL UpdateModuleBase_Inactive_HandlePacketAvailable(GENERIC_MSG_t* msg)
{
    BOOL handled = FALSE;
    IpcPacket_t* p = (IpcPacket_t*) msg->params[0];

    if ((p->s.opcode == IPC_UPDATE_ENTER) && (p->s.sender == IPC_DEVICE_SOUNDTOUCH))
    {
        // TODO - dj1005472 - Check if update variant allows update

        // For devices that do no have an SM2, this is where we change to the UPDATE source.
        // For devices with an SM2, this is too close for the source switch to write to the NV.
        // A NV erase will stall the system for too long and we drop IPC packets from the SM2. Now
        // the SM2 will tell us to switch to the UPDATE source before they begin the update process which
        // should be enough time to keep us from missing any IPC packets
#if 0 // TODO - dj1005472 - how do we want to handle this? how much notion of source do we have on the LPM?
        if (SourceChangeAPI_GetCurrentSource() != SOURCE_UPDATE)
        {
            UI_SetUnifySource(UI_GetUnifySourceIndex(SOURCE_UPDATE));
        }
#endif
        handled = TRUE;
        uint32_t status = REV_WORD_BYTES(UPDATE_STATUS_READY_OK);

        IpcSendPacket_t response = 
        {
            .dest = IPC_DEVICE_SOUNDTOUCH,
            .op = IPC_UPDATE_ENTER,
            .sequence = p->s.sequence,
            .connectionID = p->s.connectionID,
            .data = (void*)&status,
            .length = sizeof(status),
        };
        
        IPCRouter_SendResponse(&response);

        UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
    }

    return handled;
}
