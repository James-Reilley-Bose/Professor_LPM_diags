//
// SystemStateTask.h
//
#ifndef _SYSTEM_STATE_TASK_H_
#define _SYSTEM_STATE_TASK_H_

#include "project.h"
#include "TaskDefs.h"
#include "genTimer.h"
#include "PowerTask.h"
#include "RivieraLPM_IpcProtocol.h"

extern ManagedTask* systemStateTaskHandle;

typedef struct
{
    BOOL (*HandleMessage)(GENERIC_MSG_t*);
    void (*Init)(void);
    void (*EnterState)(void);
    void (*ExitState)(void);
}SystemStateHandler_t;

#define DEFAULT_SYSTEM_TIMER_PERIOD_MSEC 1

#define SysStatePostMsg(_theMsg, cb, ...) QueueManagerPostCallbackMsg(&systemStateTaskHandle->Queue, _theMsg, (uint32_t []){__VA_ARGS__}, cb, NO_BLOCK_TIME);

const char* SystemStateTask_GetStateString(IpcLpmSystemState_t state);
void SystemStateTask_Init(void* p);
void SystemStateTask(void* p);
void SystemStateTask_HandleMessage(GENERIC_MSG_t* msg);
void SystemStateTask_GetState(uint32_t sequence, uint8_t connectionID);
void SystemStateTask_SetStateInternal(IpcLpmSystemState_t state);
void SystemStateTask_SetState(IpcLpmSystemState_t state, uint32_t sequence, uint8_t connectionID);
TimerHandle_t SystemStateTask_GetTimer(void);

#endif /* _SYSTEM_STATE_TASK_H_ */
