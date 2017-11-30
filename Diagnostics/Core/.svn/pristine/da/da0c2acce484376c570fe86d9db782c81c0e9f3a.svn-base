// UpdateTask.c
//

#include "UpdateManagerTask.h"
#include "IPCRouterTask.h"
#include "genTimer.h"
#include "IpcProtocolLpm.h"
#include "IpcTxTask.h"
#include "viewNotify.h"
#include "UpdateVariant.h"
#include "UpdateModuleBase.h"
#include "nv_system.h"

SCRIBE_DECL(update);
SCRIBE_DECL(ipc_update);

/* Private functions */
static void UpdateManagerTask_SM2ReportUpdateProgress(void);

/* Globals */
ManagedTask* ManagedUpdateTask = NULL;

UpdateState_t UpdateState =
{
    .State = (UpdateBaseState_t) 0,
    .watchDogTimer = NULL,
    .autoUpdateAll = TRUE,
    .waitForExtPeriph = TRUE,
};

/*
 * @func UpdateManagerTask_Init
 *
 * @brief Initializes the update manager task and related modules.
 *
 * @param void* p - generic parameters (none used)
 *
 * @return n/a
 */
void UpdateManagerTask_Init(void* p)
{
    UpdateState.watchDogTimer = createTimer( TIMER_MSEC_TO_TICKS(UPDATE_WATCHDOG_PERIOD_MS), \
                                NULL, \
                                UPDATE_MESSAGE_ID_WatchdogExpired, \
                                FALSE, \
                                tBlockIdUpdateWatchdog, \
                                "UpdateWatchdogTimer", \
                                NULL);
    debug_assert(UpdateState.watchDogTimer != NULL);

    for (uint8_t i = 0; i < UPDATE_NUM_STATES; i++)
    {
        if (UpdateStateHandlers[i].Init)
        {
            UpdateStateHandlers[i].Init();
        }
    }

    UpdateManagerTask_ResetStats();

    ManagedUpdateTask = GetManagedTaskPointer(UPDATE_MANAGER_TASK_NAME);
    debug_assert(ManagedUpdateTask != NULL);

    UpdatePostMsg(UPDATE_MESSAGE_ID_DoEnterState, NOP_CALLBACK, NULL);
}

/*
 * @func UpdateManagerTask
 *
 * @brief Waits for new update messages on the it's queue.
 *
 * @param void* pvParameters - generic parameters (none used)
 *
 * @return n/a
 */
void UpdateManagerTask(void* pvParameters)
{
    /* NV needs to be initialized after the scheduler starts due to
    our SPI Flash driver (uses event bits). Since lots of places
    rely on system params, we want to get them set up first. */
#ifdef HAS_SYSPARAMS_NVRAM
    NV_InitSystemParameters();
#endif
    vTaskPrioritySet(ManagedUpdateTask->TaskHandle, configNORMAL_TASK_PRIORITY);
    for (;;)
    {
        TaskManagerPollQueue(ManagedUpdateTask);
    }
}

/*
 * @func UpdateManagerTask_HandleMessage
 *
 * @brief Passes the message to the appropriate handler for current state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return n/a
 */
void UpdateManagerTask_HandleMessage(GENERIC_MSG_t* msg)
{
    /* Log update related IPC messages for test. */
    if (msg->msgID == UPDATE_MESSAGE_ID_IPCPacketAvailable)
    {
        IpcPacket_t* p = (IpcPacket_t*) msg->params[0];
        LOG(ipc_update, ROTTEN_LOGLEVEL_VERBOSE, "IPC: %08X %08X %08X %08X %08X", REV_WORD_BYTES(p->w[0]), REV_WORD_BYTES(p->w[1]),
            REV_WORD_BYTES(p->w[2]), REV_WORD_BYTES(p->w[3]), REV_WORD_BYTES(p->w[4]));
    }

    if ((UpdateStateHandlers[UpdateState.State].MessageHandler == NULL) ||
            !UpdateStateHandlers[UpdateState.State].MessageHandler(msg))
    {
        UpdateModuleBase_HandleMessage(msg);
    }

    /* restart the timer from here for now as we know there's a spot available in the message queue */
    if ((msg->msgID == UPDATE_MESSAGE_ID_WatchdogExpired) && (UpdateState.State != UPDATE_INACTIVE))
    {
        UpdateManagerTask_SM2ReportUpdateProgress();
        timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
    }
}

/*
 * @func UpdateManagerTask_CompleteState(void)
 *
 * @brief Allows a module to signal the manager that it has completed a state.
 *
 * @return n/a
 */
void UpdateManagerTask_CompleteState(UpdateResult_t result)
{
    UpdateState.results[UpdateState.State].result = result;
    UpdateManagerTask_doExitState();

    if (UpdateState.autoUpdateAll)
    {
        UpdateState.State++;
    }
    else
    {
        UpdateState.State = UPDATE_REBOOT;
    }

    UpdateManagerTask_doEnterState();
}

/*
 * @func UpdateManagerTask_GetState
 *
 * @brief Returns the current update state.
 *
 * @param n/a
 *
 * @return update state
 */
UpdateBaseState_t UpdateManagerTask_GetState(void)
{
    return UpdateState.State;
}

TimerHandle_t UpdateManagerTask_GetWDT(void)
{
    return UpdateState.watchDogTimer;
}

/*
 * @func UpdateManagerTask_SetState
 *
 * @brief Set the current update state.
 *
 * @param state - the state to be set
 *
 * @return n/a
 */
void UpdateManagerTask_SetState(uint32_t state)
{
    UpdateState.State = (UpdateBaseState_t)state;
}

void UpdateManagerTask_doEnterState(void)
{
    UpdateState.results[UpdateState.State].startTime = GET_SYSTEM_UPTIME_MS();
    UI_ViewNotify(UPDATE_STATUS_CHANGE);
    UpdateManagerTask_SetProgress(0);
    if (UpdateStateHandlers[UpdateState.State].EnterState)
    {
        UpdateStateHandlers[UpdateState.State].EnterState();
    }
}

void UpdateManagerTask_doExitState(void)
{
    UpdateManagerTask_SetProgress(100);
    if (UpdateStateHandlers[UpdateState.State].ExitState)
    {
        UpdateStateHandlers[UpdateState.State].ExitState();
    }
    UpdateState.results[UpdateState.State].endTime = GET_SYSTEM_UPTIME_MS();
}

void UpdateManagerTask_SetAutoUpdateMode(BOOL enabled)
{
    UpdateState.autoUpdateAll = enabled;
}

void UpdateManagerTask_SetWaitForExtPeriphMode(BOOL enabled)
{
    UpdateState.waitForExtPeriph = enabled;
}

/*
 * @func UpdateManagerTask_AbortUpdate
 *
 * @brief Abort an ongoing update.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateManagerTask_AbortUpdate(void)
{
    // clean up
    UpdateManagerTask_doExitState();

    // reset update stats
    UpdateManagerTask_ResetStats();

    // set state to inactive
    UpdateManagerTask_SetState(UPDATE_INACTIVE);
}

void UpdateManagerTask_ResetStats(void)
{
    UpdateManagerTask_ResetProgress();

    for (uint8_t i = 0; i < UPDATE_NUM_STATES; i++)
    {
        UpdateState.results[i].startTime = 0;
        UpdateState.results[i].endTime = 0;
        UpdateState.results[i].result = UPDATE_SKIPPED;
    }
}

/*
 * @func UpdateManagerTask_SetProgress
 *
 * @brief Set the progress for the given state
 *
 * @param p Progress (0-100)
 *
 * @return n/a
 */
void UpdateManagerTask_SetProgress(int p)
{
    UpdateState.progress[UpdateState.State] = p;
}

/*
 * @func UpdateManagerTask_ResetProgress
 *
 * @brief Reset all progress
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateManagerTask_ResetProgress(void)
{
    for (unsigned int i = 0; i < UPDATE_NUM_STATES; i++)
    {
        UpdateState.progress[i] = 0;
    }
}

/*
 * @func UpdateManagerTask_GetTotalProgress
 *
 * @brief Get overall progress
 *
 * @param n/a
 *
 * @return overall progress (0-100)
 */
int UpdateManagerTask_GetTotalProgress(void)
{
    int i, j, total;

    for (i = 0, j = 0, total = 0; i < UPDATE_NUM_STATES; i++)
    {
        if (!UpdateStateHandlers[i].ignoreProgress)
        {
            total += UpdateState.progress[i];
            ++j;
        }
    }

    return (total / j);
}

/*
 * @func UpdateManagerTask_SM2ReportUpdateProgress
 *
 * @brief Report update progress to the SM2
 *
 * @param n/a
 *
 * @return overall progress (0-100)
 */

static void UpdateManagerTask_SM2ReportUpdateProgress(void)
{
    // If the ST model queue is full, don't do anything; this message is unimportant
    // and at first boot into user code the ST model task gets starved and can't process
    // these anyway and we end up with a queue full assert
    if (uxQueueSpacesAvailable(ManagedIpcTxTasks[IPC_DEVICE_SOUNDTOUCH]->Queue.qHandle) == 0)
    {
        return;
    }

    // TODO: should this function reside elsewhere?
    UpdateIncProg_t progress =
    {
        .segment = REV_WORD_BYTES(UpdateManagerTask_GetState()),
        .totalSegments = REV_WORD_BYTES(UPDATE_NUM_STATES),
        .percentComplete = REV_WORD_BYTES(UpdateManagerTask_GetTotalProgress()),
    };
    IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_UPDATE_INC_PROG, NULL, &progress, sizeof(progress));
}


void UpdateManagerTask_SetForceUpdate(BOOL forced)
{
    UpdateState.forced = forced;
}

void UpdateManagerTask_DoSingleUpdate(UpdateBaseState_t state, BOOL forced)
{
    if (state >= UPDATE_NUM_STATES)
    {
        return;
    }

    //UI_SetUnifySource(UI_GetUnifySourceIndex(SOURCE_UPDATE));
    UpdateManagerTask_SetForceUpdate(forced);
    UpdateManagerTask_SetAutoUpdateMode(FALSE);
    UpdateManagerTask_SetState(state);
    UpdateManagerTask_doEnterState();
}

void UpdateManagerTask_PublishVersionSkip(char* version)
{
    UpdateVersionSkip_t skip = 
    {
        .segmentSkipped = REV_WORD_BYTES(UpdateManagerTask_GetState()),
    };
    strncpy(version, skip.version, 24);
    IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_UPDATE_SKIP, NULL, &skip, sizeof(skip));
}