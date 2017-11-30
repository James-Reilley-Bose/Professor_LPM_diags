#include "project.h"
#include "UpdateModuleBleRadio.h"
#include "RemoteUpdate.h"
#include "UpdateManagerTask.h"
#include "RemoteTask.h"


//static void UpdateModuleBleRadio_UpdateDoneCallback(uint32_t*, uint8_t);
static void HandleWatchdog(void);

//todo: Use another logger? not really part of remote here
SCRIBE_DECL(remote);


void UpdateModuleBleRadio_Enter(void)
{
    LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "UpdateModuleBleRadio_Enter");
    /*
    RemoteTaskPostCallbackMsg(REMOTE_MESSAGE_DO_UPDATE,
                              UpdateModuleBleRadio_UpdateDoneCallback,
                              0);
    */
    RemoteTaskPostMsg(REMOTE_MESSAGE_DO_UPDATE, 0);
    timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
}


// returns TRUE if message was handled, FALSE otherwise
BOOL UpdateModuleBleRadio_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "Watchdog expired");
            HandleWatchdog();
            return TRUE;
    }
    return FALSE;
}


void UpdateModuleBleRadio_Exit(void)
{
    LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "UpdateModuleBleRadio_Exit");
}


static void HandleWatchdog(void)
{
    if(Remote_Update_Status() == REMOTE_UPDATE_STATUS_COMPLETED_SUCCESS ||
       Remote_Update_Status() == REMOTE_UPDATE_STATUS_COMPLETED_FAIL )
    {
        LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "Done updating");
        timerStop(UpdateManagerTask_GetWDT(), 0);
        UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
    }
}

/*
// todo: receive arg about update result: error?
static void UpdateModuleBleRadio_UpdateDoneCallback(uint32_t* args, uint8_t foo)
{
    LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "UpdateModuleBleRadio_UpdateDoneCallback");
    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
}
*/