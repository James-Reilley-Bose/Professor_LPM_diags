
#include "project.h"
#include "TaskDefs.h"
#include "genTimer.h"
#include "WiredIDTask.h"
#include "WiredIDUtilities.h"

#define WIRED_ID_TIMER_DEFAULT_MS      200

SCRIBE(wired_id, ROTTEN_LOGLEVEL_NORMAL);

ManagedTask *wiredIDTaskHandle  = NULL;
static TimerHandle_t wired_id_timer = NULL;

/* @func
 * @brief TaskInit called before scheduler
 */
void WiredID_TaskInit(void *param)
{
    WiredIDVariant_Init();
    wiredIDTaskHandle = GetManagedTaskPointer("WiredIDTask");
    debug_assert(wiredIDTaskHandle);
}

/* @func   WiredID_Task
 * @brief  Task function
 */
void WiredID_Task(void *param)
{
    wired_id_timer = createTimer(TIMER_MSEC_TO_TICKS(WIRED_ID_TIMER_DEFAULT_MS),
                                 NULL,
                                 WIREDID_TimerExpired,
                                 FALSE,
                                 tBlockIdWiredID,
                                 "WBassTimer",
                                 NULL);
    debug_assert(wired_id_timer);
    
    WiredID_StartStateMachine();
    
    while(1)
    {
        TaskManagerPollQueue(wiredIDTaskHandle);
    }
}

/*@func  WiredID_HandleMessage 
 *@brief WiredID Messageg Handler
 */
void WiredID_HandleMessage(GENERIC_MSG_t *msg)
{
    WiredIDVariant_HandleMessage(msg);
}

/* 
 * @func   WiredID_StartStateMachine
 * @brief  Start Wired ID state machine
 */
void WiredID_StartStateMachine(void)
{
    WiredIDVariant_StartIDStateMachine();    
}

/*@func
 *brief functions to start, stop and change SW timer period. 
 *sw timer shared by states
 */
void WiredID_StartTimer(void)
{
    timerStart(wired_id_timer, 0, &wiredIDTaskHandle->Queue);
}

void WiredID_StopTimer(void)
{
    timerStop(wired_id_timer, portMAX_DELAY);
}

void WiredID_ChangeTimerPeriod(TickType_t newPeriod)
{
    changeTimerPeriod(wired_id_timer, newPeriod, portMAX_DELAY);
}

