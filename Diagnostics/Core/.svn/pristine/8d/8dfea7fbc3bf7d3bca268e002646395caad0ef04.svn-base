// UpdateTask.h
//

#ifndef _UPDATE_TASK_H_
#define _UPDATE_TASK_H_

#include "project.h"
#include "IPCBulk.h"
#include "TaskManager.h"
#include "UpdateManagerBlob.h"
#include "UpdateManagerStateDefinitions.h"
#include "genTimer.h"

extern ManagedTask* ManagedUpdateTask;
#define UpdatePostMsg(_theMsg, cb, ...) QueueManagerPostCallbackMsg(&ManagedUpdateTask->Queue, _theMsg, (uint32_t []){__VA_ARGS__}, cb, NO_BLOCK_TIME);

#define UPDATE_MANAGER_TASK_NAME "UpdateTask"

#define UPDATE_WATCHDOG_PERIOD_MS 1000 // 1s is good enough

typedef struct
{
    uint32_t startTime;
    uint32_t endTime;
    UpdateResult_t result;
} UpdateStateResults_t;

typedef struct
{
    UpdateBaseState_t State;
    UpdateStateResults_t results[UPDATE_NUM_STATES];
    int progress[UPDATE_NUM_STATES];
    TimerHandle_t watchDogTimer;
    BOOL autoUpdateAll;
    BOOL waitForExtPeriph;
    BOOL forced;
    BulkEndpoint_t bulkUpdateEndpoint;
    BOOL ReceivedRebootPlease;
} UpdateState_t;

extern UpdateState_t UpdateState;

void UpdateManagerTask_Init(void* p);
void UpdateManagerTask(void* pvParameters);
void UpdateManagerTask_HandleMessage(GENERIC_MSG_t* msg);
void UpdateManagerTask_CompleteState(UpdateResult_t result);
void UpdateManagerTask_SetState(uint32_t state);
UpdateBaseState_t UpdateManagerTask_GetState(void);
TimerHandle_t UpdateManagerTask_GetWDT(void);
void UpdateManagerTask_doExitState(void);
void UpdateManagerTask_doEnterState(void);
void UpdateManagerTask_SetAutoUpdateMode(BOOL enabled);
void UpdateManagerTask_SetWaitForExtPeriphMode(BOOL enabled);
void UpdateManagerTask_AbortUpdate(void);
void UpdateManagerTask_ResetStats(void);
void UpdateManagerTask_SetProgress(int p);
void UpdateManagerTask_ResetProgress(void);
int UpdateManagerTask_GetTotalProgress(void);
void UpdateManagerTask_SetForceUpdate(BOOL forced);
void UpdateManagerTask_DoSingleUpdate(UpdateBaseState_t state, BOOL forced);
void UpdateManagerTask_PublishVersionSkip(char* version);

#endif // _UPDATE_TASK_H_
