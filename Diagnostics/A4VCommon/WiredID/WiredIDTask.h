#ifndef WIRED_BASS_TASK_H
#define WIRED_BASS_TASK_H

#include "project.h"
#include "TaskManager.h"
#include "WiredIDVariant.h"
#include "WiredIDUtilities.h"

#define  ID_TIM                 TIM7
#define  ID_TIM_IRQn            TIM7_IRQn

extern ManagedTask *wiredIDTaskHandle;
#define WiredIDPostMsg(_theMsg, cb, ...) \
    QueueManagerPostCallbackMsg(&wiredIDTaskHandle->Queue, _theMsg, (uint32_t[]){__VA_ARGS__}, cb, NO_BLOCK_TIME);

void WiredID_TaskInit(void* param);
void WiredID_Task(void *param);
void WiredID_StartTimer(void);
void WiredID_StopTimer(void);
void WiredID_ChangeTimerPeriod(TickType_t newPeriod);
void WiredID_HandleMessage(GENERIC_MSG_t *msg);
void WiredID_StartStateMachine(void);

//PULSE high and low duration in ms
//TODO: Pulse time for future products
//TODO: These can be reduced if desired
#define PBB_HIGH_TIME       250
#define PBB_LOW_TIME        250
#define CONSOLE_HIGH_TIME   700
#define CONSOLE_LOW_TIME    700

//unrealistic imaginary numbers, decide later
#define BASS2_HIGH_TIME     500
#define BASS2_LOW_TIME      500
#define CONSOLE2_HIGH_TIME  1000
#define CONSOLE2_LOW_TIME   1000

//Console accepts bass if received pulse time within PBB_HIGH/LOW_TIME +/- BASS_VARIANCE
#define BASS_VARIANCE       50 
//BASS accepts Console if received pulse time within CONSOLE_HIGH/LOW_TIME +/- CONSOLE_VARIANCE
#define CONSOLE_VARIANCE   150

//If TEST 1, write the received dc value to a spare GPIO for testing
//If TEST 0, disable the test
#define TEST                0   

#endif
