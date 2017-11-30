#include "UpdateManagerTask.h"
#include "UpdateModuleWaitDownload.h"
#include "IpcRouterTask.h"
#ifdef LPM_HAS_USER_INTERFACE
#include "UITask.h"
#endif

static void UpdateModule_WaitDownload_HandleBulkRequest(GENERIC_MSG_t* msg);
static void UpdateModule_WaitDownload_HandleTimerExpired(void);

SCRIBE_DECL(update);

/*
 * @func UpdateModule_WaitDownload_HandleMessage
 *
 * @brief Handles messages while in UPDATE_WAIT_DOWNLOAD state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled FALSE if it wasn't
 */
BOOL UpdateModule_WaitDownload_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            UpdateModule_WaitDownload_HandleTimerExpired();
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferRequest:
            UpdateModule_WaitDownload_HandleBulkRequest(msg);
            break;
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

/*
 * @func UpdateModule_WaitDownload_HandleTimerExpired
 *
 * @brief Cancels the update if we've waited for the file for
 *        too long.
 *
 * @param n/a
 *
 * @return n/a
 */
static void UpdateModule_WaitDownload_HandleTimerExpired(void)
{
}

void UpdateModule_WaitDownload_Enter(void)
{
    timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Waiting for download to start...");
}

void UpdateModule_WaitDownload_Exit(void)
{
    timerStop(UpdateManagerTask_GetWDT(), 0);
}

/*
 * @func UpdateModule_WaitDownload_HandleBulkRequest
 *
 * @brief Handles bulk requests in the UPDATE_WAIT_DOWNLOAD state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return n/a
 */
static void UpdateModule_WaitDownload_HandleBulkRequest(GENERIC_MSG_t* msg)
{
    IPCBulk_ActivateRxEndpoint(&UpdateState.bulkUpdateEndpoint, msg->params[0], msg->params[1]);

    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
}
