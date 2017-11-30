#include "UpdateManagerTask.h"
#include "UpdateModuleAuthenticate.h"
#include "IpcRouterTask.h"
#ifdef LPM_HAS_USER_INTERFACE
#include "UITask.h"
#endif

static void UpdateModule_Authenticate_HandleTimerExpired(GENERIC_MSG_t* msg);
static BOOL UpdateModule_Authenticate_HandlePacketAvailable(GENERIC_MSG_t* msg);

SCRIBE_DECL(update);

void UpdateModule_Authenticate_Enter(void)
{
    timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Authenticating download...");
}

void UpdateModule_Authenticate_Exit(void)
{
    timerStop(UpdateManagerTask_GetWDT(), 0);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Authenticating download complete");
}

/*
 * @func UpdateModuleBase_HandleMessage_AuthenticatE
 *
 * @brief Handles messages while in UPDATE_AUTHENTICATE state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled FALSE if it wasn't
 */
BOOL UpdateModule_Authenticate_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            UpdateModule_Authenticate_HandleTimerExpired(msg);
            break;
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            handled = UpdateModule_Authenticate_HandlePacketAvailable(msg);
            break;
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

/*
 * @func UpdateModule_Authenticate_HandleTimerExpired
 *
 * @brief Receives IPC packets in the UPDATE_AUTHENTICATE state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return none
 */
static void UpdateModule_Authenticate_HandleTimerExpired(GENERIC_MSG_t* msg)
{
}

/*
 * @func UpdateModule_Authenticate_HandlePacketAvailable
 *
 * @brief Receives IPC packets in the UPDATE_AUTHENTICATE state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled FALSE if it wasn't
 */
static BOOL UpdateModule_Authenticate_HandlePacketAvailable(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;
    IpcPacket_t* p = (IpcPacket_t*) msg->params[0];

    switch (p->s.opcode)
    {
        case IPC_UPDATE_AUTHENTICATE:
            {
                UpdateBlobHeader upd;

                uint32_t status = REV_WORD_BYTES(UPDATE_STATUS_AUTH_FAIL);

                IpcSendPacket_t response = 
                {
                    .dest = IPC_DEVICE_SOUNDTOUCH,
                    .op = IPC_UPDATE_AUTHENTICATE,
                    .sequence = p->s.sequence,
                    .connectionID = p->s.connectionID,
                    .data = (void*)&status,
                    .length = sizeof(status),
                };
        
                if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
                {
                    status = REV_WORD_BYTES(UPDATE_STATUS_AUTH_OK);
                    
                    IPCRouter_SendResponse(&response);
                    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
                }
                else
                {
                    // TODO: retries?
                    IPCRouter_SendResponse(&response);
                    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Authentication failed, aborting update.");
                    UpdateManagerTask_AbortUpdate();
                }
                break;
            }
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

