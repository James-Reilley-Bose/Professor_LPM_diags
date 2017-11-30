#include "UpdateManagerTask.h"
#include "UpdateModuleWaitInstall.h"
#include "IpcRouterTask.h"
#ifdef LPM_HAS_USER_INTERFACE
#include "UITask.h"
#endif

SCRIBE_DECL(update);

static void UpdateModule_WaitInstall_HandleTimerExpired(GENERIC_MSG_t* msg);
static BOOL UpdateModule_WaitInstall_HandlePacketAvailable(GENERIC_MSG_t* msg);

void UpdateModule_WaitInstall_Enter(void)
{
    timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Waiting for the install signal...");
}

void UpdateModule_WaitInstall_Exit(void)
{
    timerStop(UpdateManagerTask_GetWDT(), 0);
}

/*
 * @func UpdateModule_WaitInstall_HandleMessage
 *
 * @brief Handles messages while in UPDATE_SM2_WAIT_ state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled FALSE if it wasn't
 */
BOOL UpdateModule_WaitInstall_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            UpdateModule_WaitInstall_HandleTimerExpired(msg);
            break;
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            handled = UpdateModule_WaitInstall_HandlePacketAvailable(msg);
            break;
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

/*
 * @func UpdateModule_WaitInstall_HandlePacketAvailable
 *
 * @brief Receives IPC packets in the UPDATE_SM2WaitInstall state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled FALSE if it wasn't
 */
static BOOL UpdateModule_WaitInstall_HandlePacketAvailable(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;
    IpcPacket_t* p = (IpcPacket_t*) msg->params[0];

    switch (p->s.opcode)
    {
        case IPC_UPDATE_START_FLASH:
            {
                UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
                break;
            }
        default:
            handled = FALSE;
            break;
    }

    return handled;
}


/*
 * @func UpdateModule_Wait_Install_HandleTimerExpired
 *
 * @brief Receives IPC packets in the UPDATE_WAIT_INSTALL state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return none
 */
static void UpdateModule_WaitInstall_HandleTimerExpired(GENERIC_MSG_t* msg)
{
}

