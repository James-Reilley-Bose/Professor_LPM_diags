/*
  File  : SystemAnalysis.h
  Title :
  Author  : Stolen fron Triode by Dillon Johnson
  Created : 9/9/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Macros and functions to profile interrupts and tasks.
===============================================================================
*/

#ifndef INCLUDE_SYSTEM_ANALYSIS
#define INCLUDE_SYSTEM_ANALYSIS

#include "portmacro.h"
#include "configuresystem.h"
#include "SystemAnalysisDefines.h"
#include "instrument.h"

/* Interrupt analysis will measure the time between an interrupt firing and the time
*  the associated task processes the resulting message.
*/
#define ANALYZE_INTERRUPTS

/* Interrupt Runtime analysis will measure the accumulated time spent in each ISR. */
//#define ANALYZE_ISR_RUNTIME_DURATIONS

/*
*  Worst case idle task time.
*/
extern portBASE_TYPE g_WorstCaseIdleTaskDiff;
/*
*   ETAP output will block the idle task from running so the more output the higher
*   the expected delay should be set.
*/
#ifdef USE_DEBUG_ASSERT
#define WORST_CASE_EXPECTED_IDLE_DIFF_TIME_MSEC        1200
#else
#define WORST_CASE_EXPECTED_IDLE_DIFF_TIME_MSEC        1000
#endif

typedef struct isr_monitor_struct_t
{
    unsigned portBASE_TYPE g_WorstCaseIntLatency;
    unsigned portBASE_TYPE g_InterruptStartTime;
    unsigned portBASE_TYPE g_InterruptExceedCount;
    unsigned portBASE_TYPE g_InterruptTotalCount;
#ifdef ANALYZE_ISR_RUNTIME_DURATIONS
    unsigned portBASE_TYPE g_InterruptDuration;
    BOOL g_MeasureDuration;
    uint8_t g_DurationRolloverCount;
#endif
} ISRMonitor_t;

typedef enum
{
    CC_E,
    CC_T,
    DMA,
    DWAM_U,
    DWAM_E,
    HDMI_E,
    I2C_EVT,
    I2C_ERR,
    IPC_I2C_F0_E,
    IPC,
    IR_TIMER_ISR,
    SPI_DMA,
    SPI_RXNE,
    TAP_UART_ISR,
    UEI_UART_ISR,
    USB,
    PWR_VSYS_OK,
    PSOC_ISR,
    NUM_MONITOR_ISRS,
} ISR_MONITOR_INDEX;

#ifdef ANALYZE_INTERRUPTS
extern ISRMonitor_t isrMonitor[NUM_MONITOR_ISRS];

#define INTERRUPT_RUN_LATENCY_BEGIN(idx) isrMonitor[idx].g_InterruptStartTime = GET_MEASURE_TIME_NOW();
static inline void INTERRUPT_RUN_LATENCY_END(ISR_MONITOR_INDEX idx, uint32_t max)
{
    ++isrMonitor[idx].g_InterruptTotalCount;
    uint32_t diff = MEASURE_TIME_SINCE(isrMonitor[idx].g_InterruptStartTime);
    if ((max > 0) && (diff > max))
    {
        ++isrMonitor[idx].g_InterruptExceedCount;
    }
    if (diff > isrMonitor[idx].g_WorstCaseIntLatency)
    {
        isrMonitor[idx].g_WorstCaseIntLatency = diff;
    }
#ifdef ANALYZE_ISR_RUNTIME_DURATIONS
    if (isrMonitor[idx].g_MeasureDuration)
    {
        if ((isrMonitor[idx].g_InterruptDuration + diff) < isrMonitor[idx].g_InterruptDuration)
        {
            isrMonitor[idx].g_DurationRolloverCount++;
        }
        isrMonitor[idx].g_InterruptDuration += diff;
    }
#endif
}

#else // ANALYZE_INTERRUPTS

#define INTERRUPT_RUN_LATENCY_BEGIN(idx)
#define INTERRUPT_RUN_LATENCY_END(idx,max)

#endif // ANALYZE_INTERRUPTS

void SystemAnalysis_Init (void);
void SystemAnalysis_ClearRunTimes (void);
void SystemAnalysis_ClearInterruptData (void);
void SystemAnalysis_MeasureIsrDurations (ISR_MONITOR_INDEX myIsrIndex, BOOL enable);

unsigned portBASE_TYPE SystemAnalysis_GetRunTimeUS(void* handle);
unsigned portBASE_TYPE SystemAnalysis_GetBucketSize (void);
unsigned portBASE_TYPE SystemAnalysis_GetSizeOfBucketData(void);

unsigned portBASE_TYPE SystemAnalysis_GetRealTimeMsec (void);
unsigned portBASE_TYPE SystemAnalysis_GetIdleTaskRunTime (void);
unsigned portBASE_TYPE SystemAnalysis_GetTimerDaemonRunTime (void);

#endif // INCLUDE_SYSTEM_ANALYSIS
