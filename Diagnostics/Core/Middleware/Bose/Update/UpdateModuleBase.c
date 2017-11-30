//
// UpdateModuleBase.c
//

#include "UpdateModuleBase.h"
#include "IpcRouterTask.h"
#include "ProductSystemParams.h"
#include "IpcRouterTask.h"

SCRIBE_DECL(update);

/* Private functions */
static void UpdateModuleBase_HandlePacketAvailable(GENERIC_MSG_t* msg);

static const char* UpdateDisplayStatusNames[] =
{
    "Idle",
    "Downloading",
    "Authenticating",
    "Installing",
    "Completed"
};

/*
 * @func UpdateModuleBase_HandleMessage
 *
 * @brief Handles messages that aren't handled in device modules. This
 *        function is not related to a specific state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled (and it has to be at this point!)
 */
BOOL UpdateModuleBase_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            UpdateModuleBase_HandlePacketAvailable(msg);
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferRequest:
            IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge, msg->params[0], BULK_RESPONSE_DECLINE);
            break;
        case UPDATE_MESSAGE_ID_RebootRequestReceived:
            UpdateState.ReceivedRebootPlease = TRUE;
            break;
        case UPDATE_MESSAGE_ID_SetState:
            UpdateManagerTask_SetState(msg->params[0]);
            break;
        case UPDATE_MESSAGE_ID_SetAutoUpdateMode:
            UpdateManagerTask_SetAutoUpdateMode((BOOL) msg->params[0]);
            break;
        case UPDATE_MESSAGE_ID_SetWaitForExtPeriphMode:
            UpdateManagerTask_SetWaitForExtPeriphMode(msg->params[0]);
            break;
        case UPDATE_MESSAGE_ID_DoEnterState:
            UpdateManagerTask_doEnterState();
            break;
        case UPDATE_MESSAGE_ID_SetForceUpdate:
            UpdateManagerTask_SetForceUpdate((BOOL) msg->params[0]);
            break;
        case UPDATE_MESSAGE_ID_DoSingleUpdate:
            UpdateManagerTask_DoSingleUpdate((UpdateBaseState_t) msg->params[0], (BOOL) msg->params[1]);
            break;
        default:
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled message: %s (Current State: %s)", GetEventString(msg->msgID), GetUpdateManagerStateString(UpdateState.State));
            break;
    }

    return TRUE;
}

/*
 * @func UpdateModuleBase_HandlePacketAvailable
 *
 * @brief The last stop for IPC packets. If state specific
 *        handlers didn't handle the packet it will be passed
 *        here.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return n/a
 */
static void UpdateModuleBase_HandlePacketAvailable(GENERIC_MSG_t* msg)
{
    IpcPacket_t* p = (IpcPacket_t*) msg->params[0];
    switch (p->s.opcode)
    {
        case IPC_RIVIERA_CONTROL_UPDATE:
            switch (p->s.params[0])
            {
                case IPC_ST_CONTROL__DISPLAY_UPDATE_STATUS:
                    {
                        uint8_t state = p->s.params[1];
                        // TODO - the doc would seem to indicate that this is actually a uint32_t in the
                        // new message layout, but I'm not sure this is actually the intent since it seems
                        // to be a uint8_t in the old message
                        uint8_t progress = p->s.data.b[0];
                        if ((state >= IPC_UPDATE_DISPLAY_STATUS_IDLE) &&
                                (state <= IPC_UPDATE_DISPLAY_STATUS_COMPLETED))
                        {
                            LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "SM2 Update State: %s is %d%% complete",
                                UpdateDisplayStatusNames[state - 1], progress);
                        }
                    }
                    break;
                default:
                    LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "Unknown IPC_ST_CONTROL sub-command: %d",
                        p->s.params[0]);
                    break;
            }
            break; // IPC_RIVIERA_CONTROL_UPDATE
        case IPC_GET_VERSION:
        {
            IpcBlobVersion_t versionStruct;
            NV_GetBlobVersion(versionStruct.version);
            IpcSendPacket_t response = 
            {
                .dest = IPC_DEVICE_SOUNDTOUCH,
                .op = IPC_GET_VERSION,
                .sequence = p->s.sequence,
                .connectionID = p->s.connectionID,
                .data = (void*)&versionStruct,
                .length = sizeof(versionStruct),
            };
            IPCRouter_SendResponse(&response);
            break;
        }
        default:
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled packet: 0x%02X (Current State: %s)",
                p->s.opcode, GetUpdateManagerStateString(UpdateState.State));
            break;
    }
}

void UpdateModuleBase_Begin_Enter(void)
{
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Beginning to update the subsystems...");
    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
}

/*
 * @func UpdateModuleBase_Reboot_Enter
 *
 * @brief Reboot the system.
 *
 * @param none
 *
 */
void UpdateModuleBase_Reboot_Enter(void)
{
    BOOL updateSuccess = TRUE;
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "%-30s%10s\t%-20s", "Update State", "Duration", "Result");
    for (UpdateBaseState_t i = (UpdateBaseState_t) 0; i < UPDATE_NUM_STATES; i++)
    {
        // Update results for UPDATE_INACTIVE & UPDATE_REBOOT states are irrlevent
        if ((i == UPDATE_INACTIVE) || (i == UPDATE_REBOOT))
        {
            continue;
        }
        uint32_t duration = UpdateState.results[i].endTime - UpdateState.results[i].startTime;
        UpdateResult_t r = UpdateState.results[i].result;
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "%-30s%10d\t%-20s", GetUpdateManagerStateString(i), duration, GetUpdateManagerResultString(r));

        if ((r != UPDATE_COMPLETED) &&
                (r != UPDATE_SKIPPED_VERSION) &&
                (r != UPDATE_SKIPPED) &&
                (r != UPDATE_FILE_NOT_FOUND))
        {
            updateSuccess = FALSE;
        }
    }

    if (updateSuccess)
    {
        NV_SetBlobVersionFromStoredBlob();
    }
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Rebooting...");

    // Leave time to flush any logs
    vTaskDelay(5000);
    system_reboot();
}
