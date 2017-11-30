#include "UpdateManagerTask.h"
#include "UpdateModuleWaitExtPeriph.h"
#include "IpcRouterTask.h"

static void UpdateModule_WaitExtPeriph_HandleTimerExpired(GENERIC_MSG_t* msg);

SCRIBE_DECL(update);

/*
 * @func UpdateModuleBase_Wait_Ext_Periph_Enter
 *
 * @brief If somehow we already received the reboot request,
 *        let's go ahead and do it.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModule_WaitExtPeriph_Enter(void)
{
    //We are done with all other peripherals, now we need to wait until the SM2 gives us the ok by asking for a reboot.
    //Then we can update the F0
    uint32_t status = REV_WORD_BYTES(UPDATE_REBOOT_REQUEST);
    IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_UPDATE_STATUS, NULL, &status, sizeof(status));
    timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Waiting on Ext. Peripherals to finish updating...");
}


void UpdateModule_WaitExtPeriph_Exit(void)
{
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Ext. Peripherals have finished updating");
    timerStop(UpdateManagerTask_GetWDT(), 0);
}


/*
 * @func UpdateModuleBase_HandleMessage_Wait_Ext_Periph
 *
 * @brief Handles messages while in UPDATE_WAIT_EXT_PERIPHERALS state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled FALSE if it wasn't
 */
BOOL UpdateModule_WaitExtPeriph_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            UpdateModule_WaitExtPeriph_HandleTimerExpired(msg);
            break;
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            {
                IpcPacket_t* p = (IpcPacket_t*) msg->params[0];
                if(p->s.opcode == IPC_UPDATE_REBOOT)
                {
                    UpdateState.ReceivedRebootPlease = TRUE;
                    handled = TRUE;
                }
            }
            break;
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

/*
 * @func UpdateModuleBase_Wait_Ext_Periph_HandleTimerExpired
 *
 * @brief Receives IPC packets in the UPDATE_WAIT_EXT_PERIPHERALS state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return none
 */
static void UpdateModule_WaitExtPeriph_HandleTimerExpired(GENERIC_MSG_t* msg)
{
    if (UpdateState.ReceivedRebootPlease)
    {
        UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
        return;
    }
    if (!UpdateState.waitForExtPeriph)
    {
        UpdateManagerTask_CompleteState(UPDATE_SKIPPED);
        return;
    }
}
