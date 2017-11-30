#ifndef SYSTEM_ANALYSIS_DEFINES_H
#define SYSTEM_ANALYSIS_DEFINES_H

#include "product_macros.h"
// Defines for FreeRTOS
#ifdef ANALYZE_TASK_RUNTIMES
#define INCLUDE_xTaskGetIdleTaskHandle 1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1

#define traceTASK_SWITCHED_IN() \
{\
    extern void SystemAnalysis_TaskSwitchedIn (xTaskHandle *); \
    SystemAnalysis_TaskSwitchedIn ((xTaskHandle *)pxCurrentTCB); \
}

#define traceTASK_SWITCHED_OUT() \
{\
    extern void SystemAnalysis_TaskSwitchedOut (xTaskHandle *); \
    SystemAnalysis_TaskSwitchedOut ((xTaskHandle *)pxCurrentTCB); \
}
#endif // ANALYZE_TASK_RUNTIMES

#endif // SYSTEM_ANALYSIS_DEFINES_H
