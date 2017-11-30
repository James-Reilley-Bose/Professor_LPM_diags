#ifndef PROJECT_H
#define PROJECT_H

#define NATIVE_32_BIT_PROCESSOR
//#define INCLUDE_UART_BOARD_TEST

#include "defs.h"
#include "stm32f2xx.h"
#include "stm32f2xx_conf.h"
#include "bitband.h"
#include "rottenlog.h"
#include "instrument.h"
#include "HwMacros.h"
#include "configuresystem.h"
#include "interrupt.h"
#include "FreeRTOS.h"
#include "TimerIDs.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "QueueManager.h"
#include "Assert.h"
#include <string.h>

void system_core_reboot(void); //
void system_reboot(void); // Can be overwritten with variant specific routine
void TAP_AssertPrint(const char* file, const uint32_t line);

#define TIMER_INVALID_HANDLE    NULL
#define TIMER_AutoLoadYes       pdTRUE
#define TIMER_AutoLoadNo        pdFALSE

#define BLOCK_FOREVER portMAX_DELAY

/* Macros for converting time to RTOS ticks upon which the timers are based. */
#define TIMER_USEC_TO_TICKS(x)  ((x)/configRTOS_TICK_INTERVAL_US)
#define TIMER_TICKS_TO_USEC(x)  ((x)*configRTOS_TICK_INTERVAL_US)
#define TIMER_MSEC_TO_TICKS(x)  (((x) * 1000)/configRTOS_TICK_INTERVAL_US)
#define TIMER_SEC_TO_TICKS(x)   (((x) * 1000 * 1000)/configRTOS_TICK_INTERVAL_US)

#define ABL_INTERRUPT_PRIORITY (configLIBRARY_KERNEL_INTERRUPT_PRIORITY - 1)
#define I2C_INTERRUPT_PRIORITY (configLIBRARY_KERNEL_INTERRUPT_PRIORITY-2)

#define member_size(type, member) sizeof(((type *)0)->member)

#endif
