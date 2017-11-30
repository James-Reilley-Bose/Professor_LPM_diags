/*
  File  : SystemAnalysis.c
  Title :
  Author  : Stolen fron Triode by Dillon Johnson
  Created : 9/9/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Macros and functions to profile interrupts and tasks.
===============================================================================
*/

#include <stdlib.h>
#include <string.h>

#include "project.h"
#include "SystemAnalysis.h"
#include "TaskDefs.h"

#define MAX_DATA_COLLECT_TIME_USEC 3600000000UL // 1 hour in usec

/* Keep track of the longest interval between between successive calls to the Idle task.
*  This gives us some insight into the peak system loading. If it exceeds the following
*  predefined number then we should investigate why.
*
*  the s_MonitorIdleTaskTicks is incremented in the system tick callback and reset when
*  the idle task runs. This ensures we can place a worst case check on the idle task even
*  if the idle task gets completley blocked (meaning g_WorstCaseIdleTaskDiff won't be updated).
*/
portBASE_TYPE g_WorstCaseIdleTaskDiff = 0;

#ifdef ANALYZE_INTERRUPTS
ISRMonitor_t isrMonitor[NUM_MONITOR_ISRS];

/*
 * @func SystemAnalysis_ClearInterruptData
 *
 * @brief Resets the interrupt data.
 *
 * @param none
 *
 * @return none
 */
void SystemAnalysis_ClearInterruptData (void)
{
    memset(isrMonitor, 0, (sizeof(ISRMonitor_t) * NUM_MONITOR_ISRS));
}
#else
void SystemAnalysis_ClearInterruptData (void) {}
#endif

#ifdef ANALYZE_ISR_RUNTIME_DURATIONS
/*
 * @func SystemAnalysis_MeasureIsrDurations
 *
 * @brief Start or Stop measuring duration of ISR.
 *
 * @param myIsrIndex - ISR_MONITOR_INDEX to define a particular ISR
 * @param enable - TRUE = start measuring, FALSE = stop measuring
 *
 * @return none
 */
void SystemAnalysis_MeasureIsrDurations (ISR_MONITOR_INDEX myIsrIndex, BOOL enable)
{
    if (enable == isrMonitor[myIsrIndex].g_MeasureDuration)
    {
        return;    // no change in status
    }

    // Clear previous accumulations before every start
    if (enable == TRUE)
    {
        isrMonitor[myIsrIndex].g_InterruptDuration = 0;
        isrMonitor[myIsrIndex].g_DurationRolloverCount = 0;
    }
    isrMonitor[myIsrIndex].g_MeasureDuration = enable;
}
#else
void SystemAnalysis_MeasureIsrDurations (ISR_MONITOR_INDEX myIsrIndex, BOOL enable) {}
#endif

#ifdef ANALYZE_TASK_RUNTIMES
/* These are all static in order to reduce time for auto-allocation cycles. */
static portBASE_TYPE    s_TaskStartRunTime = 0;
static portBASE_TYPE    s_Diff = 0;
static system_time_t    s_StartTime = 0;
static portBASE_TYPE    s_ExecutingIndex;
static unsigned portBASE_TYPE s_IdleTaskRunTime = 0;
static unsigned portBASE_TYPE s_TimerDaemonRunTime = 0;

/*
 * @func SystemAnalysis_ClearRunTimes
 *
 * @brief Clear runtime data and get a new start time.
 *
 * @param none
 *
 * @return none
 */
void SystemAnalysis_ClearRunTimes (void)
{
    /* Clear all run times. */
    for (uint32_t i = 0; i < NUM_MANAGED_TASKS; i++)
    {
        ProductTasks[i].RunTime = 0;
    }

    s_IdleTaskRunTime = 0;
    s_TimerDaemonRunTime = 0;

    /* We need a total time over which the measurements are made. */
    s_StartTime = GET_SYSTEM_UPTIME_MS();

    s_ExecutingIndex = NUM_MANAGED_TASKS;
}

/*
 * @func SystemAnalysis_Init
 *
 * @brief Initialize system analysis. Call SystemAnalysis_ClearRuntimes so we can get
 *        system time at startup.
 *
 * @param none
 *
 * @return none
 */
void SystemAnalysis_Init (void)
{
    SystemAnalysis_ClearRunTimes();
    SystemAnalysis_ClearInterruptData();
}

/*
 * @func SystemAnalysis_TaskSwitchedOut
 *
 * @brief FreeRTOS will call this routines each time a task is switched out.
 *        The functions keep track of what task is executing and when it gets
 *        switched out, determines the run time and adds it to the accumulated run
 *        time for that task.
 *
 * @param xTaskHandle* tcb - points to the relevant task about to be switched out
 *
 * @return none
 */
void SystemAnalysis_TaskSwitchedOut (xTaskHandle* tcb)
{
    s_Diff = GET_MEASURE_TIME_NOW() - s_TaskStartRunTime;
    if (s_Diff < 0)
    {
        s_Diff = 0xFFFF - s_TaskStartRunTime + GET_MEASURE_TIME_NOW();
    }

    /* tcb points to the task about to be switched out but we already have the index from
    *  the time it was originally switched in.
    *
    *  The very first call from the RTOS is a task switched out call which we use to indicate
    *  that the idle and timer tasks have been created so we can add those to the list.
    */
    if (s_ExecutingIndex < NUM_MANAGED_TASKS)
    {

        ProductTasks[s_ExecutingIndex].RunTime += s_Diff;

        /* Measuring usec in 32-bits gives just over 1 hour of run time before the counts
        *  start overflowing. Reset them all if we get close otherwise the numbers will
        *  be meaningless.
        */
        if (ProductTasks[s_ExecutingIndex].RunTime > MAX_DATA_COLLECT_TIME_USEC)
        {
            /* Don't reset the spread logic */
            SystemAnalysis_Init();
        }
    }
    else if (s_ExecutingIndex == NUM_MANAGED_TASKS)
    {
        s_IdleTaskRunTime += s_Diff;

        if (s_IdleTaskRunTime > MAX_DATA_COLLECT_TIME_USEC)
        {
            /* Don't reset the spread logic */
            SystemAnalysis_Init();
        }
    }
    else if (s_ExecutingIndex == NUM_MANAGED_TASKS + 1)
    {
        s_TimerDaemonRunTime += s_Diff;

        if (s_TimerDaemonRunTime > MAX_DATA_COLLECT_TIME_USEC)
        {
            /* Don't reset the spread logic */
            SystemAnalysis_Init();
        }
    }
}

/*
 * @func SystemAnalysis_TaskSwitchedIn
 *
 * @brief FreeRTOS will call this routines each time a task is switched in.
 *        The functions keep track of what task is executing and when it gets
 *        switched out, determines the run time and adds it to the accumulated run
 *        time for that task.
 *
 * @param xTaskHandle* tcb - points to the relevant task about to be switched in.
 *
 * @return none
 */
void SystemAnalysis_TaskSwitchedIn (xTaskHandle* tcb)
{
    s_TaskStartRunTime = GET_MEASURE_TIME_NOW();

    for (uint32_t i = 0; i < NUM_MANAGED_TASKS; i++)
    {
        if (tcb == ProductTasks[i].TaskHandle)
        {
            s_ExecutingIndex = i;
            return;
        }
    }

    if (tcb == xTaskGetIdleTaskHandle())
    {
        s_ExecutingIndex = NUM_MANAGED_TASKS;
    }
    else if (tcb == xTimerGetTimerDaemonTaskHandle())
    {
        s_ExecutingIndex = NUM_MANAGED_TASKS + 1;
    }
    else
    {
        s_ExecutingIndex = NUM_MANAGED_TASKS + 2;
    }

    return;
}

/*
 * @func SystemAnalysis_GetRealTimeMsec
 *
 * @brief Gets the amount of time that has passed in milliseconds since s_StartTime
 *        was set.
 *
 * @param none
 *
 * @return none
 */
unsigned portBASE_TYPE SystemAnalysis_GetRealTimeMsec (void)
{
    return (GET_SYSTEM_UPTIME_MS() - s_StartTime);
}

/*
 * @func SystemAnalysis_GetRunTimeUS
 *
 * @brief Gets the run time for the specified task.
 *
 * @param void* handle - the task handle
 *
 * @return unsigned portBASE_TYPE - run time
 */
unsigned portBASE_TYPE SystemAnalysis_GetRunTimeUS (void* handle)
{
    /* Find the handle and return the run time. We can do it in a loop because this
    *  doesn't happen unless an ETAP command is processed.
    */
    for (int i = 0 ; i < NUM_MANAGED_TASKS ; i++)
    {
        if (handle == ProductTasks[i].TaskHandle)
        {
            return ProductTasks[i].RunTime;
        }
    }
    return 0;
}

/*
 * @func SystemAnalysis_GetIdleTaskRunTime
 *
 * @brief Gets the run time of the idle task.
 *
 * @param none
 *
 * @return unsigned portBASE_TYPE - run time
 */
unsigned portBASE_TYPE SystemAnalysis_GetIdleTaskRunTime (void)
{
    uint32_t monitorTimeUS = SystemAnalysis_GetRealTimeMsec() * 10;

    return s_IdleTaskRunTime / monitorTimeUS;
}

/*
 * @func SystemAnalysis_GetTimerDaemonRunTime
 *
 * @brief Gets the run time of the timer daemon.
 *
 * @param none
 *
 * @return unsigned portBASE_TYPE - run time
 */
unsigned portBASE_TYPE SystemAnalysis_GetTimerDaemonRunTime (void)
{
    uint32_t monitorTimeUS = SystemAnalysis_GetRealTimeMsec() * 10;

    return s_TimerDaemonRunTime / monitorTimeUS;
}

#else   // ANALYZE_TASK_RUNTIMES
void SystemAnalysis_ClearRunTimes (void) {}
void SystemAnalysis_Init (void) {}

unsigned portBASE_TYPE SystemAnalysis_GetRealTimeMsec (void)
{
    return 0;
}
unsigned portBASE_TYPE SystemAnalysis_GetRunTime (void* handle)
{
    return 0;
}
unsigned portBASE_TYPE SystemAnalysis_GetSizeOfBucketData(void)
{
    return 0;
}
#endif  // ANALYZE_TASK_RUNTIMES
