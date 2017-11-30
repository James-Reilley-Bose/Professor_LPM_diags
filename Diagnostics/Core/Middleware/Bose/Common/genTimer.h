/*
    File    :   genTimer.h
    Title   :
    Author  :   lec
    Created :   01/20/15
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:  generic timer abstractions definitions

===============================================================================
*/
#ifndef GENTIMER_H
#define GENTIMER_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "FreeRTOS.h"
#include "timers.h"
#include "EnumGenerator.h"
#include "VariantTimers.h"

/*
** Timerblocks are administered internally, so CREATE A NEW ENUM FOR EACH TIMER
** YOU INTEND TO USE, then use that enum in all future timer transactions.
*/

typedef enum
{
    FOREACH_TIMER(GENERATE_ENUM)
    tBlockIdMax,
} eTimerBlockId_t;

/*
** Function prototypes
*/

TimerHandle_t createTimer(uint32_t counts, void* args, MESSAGE_ID_t mid, BaseType_t recurring, eTimerBlockId_t tID, const char* name, ManagedQ* q);
BaseType_t deleteTimer( TimerHandle_t xTimer, TickType_t xBlockTime  );
BaseType_t timerStart( TimerHandle_t xTimer, TickType_t xBlockTime, ManagedQ* q );
BaseType_t timerStartFromISR( TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTask, ManagedQ* q );
BaseType_t timerStop(TimerHandle_t xTimer, TickType_t xBlockTime);
BaseType_t timerStopFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken);
BaseType_t timerReset( TimerHandle_t xTimer, TickType_t xBlockTime, ManagedQ* q );
BaseType_t timerResetFromISR( TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTask, ManagedQ* q );
void changeTimerPeriod( TimerHandle_t xTimer, TickType_t xNewPeriod , TickType_t xBlockTime );
BOOL timerIsRunning(TimerHandle_t xTimer);

#ifdef __cplusplus
}
#endif
#endif /* GENTIMER_H */
