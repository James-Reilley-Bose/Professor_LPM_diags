/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "SystemAnalysisDefines.h"
#include "Assert.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* NOTE : This file is also used by the assembler so it can only contain #defines - no typedefs */

#define configUSE_PREEMPTION        1
#define configUSE_IDLE_HOOK         1
#define configUSE_TICK_HOOK         1

/*
** To be transportable, we must use the hardware defines, make sure not to add
** any hardwired numbers to this file
*/

/* Here are valid values for desired sys clk,
 * 16*,24,96,120 MHz, 16 is not currently fully supported
 */
/* Here are valid values for desired peripheral clk values.  Not all pclks are compatible with all sys clk configurations
 * 6,15,24,30
 */
/*
 * For description of flash wait states at each sys clock frequency (DESIRED_FLASH_ARC_LATENCY)
 * see RM0033 section 2.3.4
 */
#ifndef DESIRED_SYS_CLK_MHZ
#define DESIRED_SYS_CLK_MHZ   (120)  // The default CPU Freq, this can be changed by setting a define in the IAR configuration
#endif

#define configCPU_CLOCK_HZ          ( ( unsigned long ) (DESIRED_SYS_CLK_MHZ * 1000000) )

#define configRTOS_TICK_INTERVAL_US 1000
#define configTICK_RATE_HZ          ( ( portTickType ) (1000000/configRTOS_TICK_INTERVAL_US) )

/* it is interesting to note that this value should be set to the minimum
*  number of different priorites used and not the available levels.
*
*  So a setting of 4 means there are 4 different priority levels in this
*  implementation - it does not mean only values 0 - 3 are used.
*/
#define configMAX_PRIORITIES        ( ( unsigned portBASE_TYPE ) 5)

/* Stack size is in unsigned ints (4 bytes)  - so 100 = 400 bytes */
#define configMINIMAL_STACK_SIZE    ( ( unsigned short ) 100)

#if defined(BARDEEN) || defined(GINGER)
#define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 54 * 1024 ) )
#else
#define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 50.0 * 1024 ) )
#endif
#define configMAX_TASK_NAME_LEN     ( 20 )
#define INCLUDE_pcTaskGetTaskName 1
#define configUSE_TRACE_FACILITY    0
#define configUSE_16_BIT_TICKS      0
#define configIDLE_SHOULD_YIELD     1
#define configUSE_MUTEXES           1
#define configUSE_RECURSIVE_MUTEXES 1
#define configUSE_COUNTING_SEMAPHORES 1
#define configCHECK_FOR_STACK_OVERFLOW 2
#define configASSERT_DEFINED 1
#define configUSE_QUEUE_SETS 1



/* Timers - need to have timers.c as part of the RTOS source code.
*
*  NOTE :  Timers execute in the context of the call back task so the timer
*  stack needs to account for this.
*/
#define configUSE_TIMERS            1

/* Co-routine definitions. These are used in co-operative multitasking only
*  and only work if the tasks that share the routines are at the same priority.
*  This is not used here.
*/
#define configUSE_CO_ROUTINES       0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           0
#define INCLUDE_vTaskDelete                 0
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_uxTaskGetStackHighWaterMark 1

/* Task suspension can be useful to save power as the suspended task will never
*  run until ResumeTask is called.
*/
#define INCLUDE_vTaskSuspend        1
#define INCLUDE_vTaskDelayUntil     1
#define INCLUDE_vTaskDelay          1

/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY         255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    191 /* equivalent to 0xb0, or priority 11. */

/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 15

/* Task priorities -  the automatically provided Idle task will run at the KERNEL_INT priority
*  of 15. */

/* The remaining tasks share timeslices and must remain at the same priority as they
*  frequently block on each other. If you block on a lower priority task that depends
*  on messages to run, you'll lock up.
*/

#define configLOW_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define configNORMAL_TASK_PRIORITY        (tskIDLE_PRIORITY + 2)
#define configHIGH_TASK_PRIORITY          (tskIDLE_PRIORITY + 3)
#define configCRITICAL_TASK_PRIORITY      (tskIDLE_PRIORITY + 4)

/* The timer task priority cannot to too high as it may starve other tasks or too low as
*  high tasks may prevent accurate timing events.
*/
#define configTIMER_TASK_PRIORITY       (tskIDLE_PRIORITY + 4)

/* The Timer Q size needs to handle all the timers that are started before the scheduler
*  is started hence it's slightly higher than normal Q size.
*/
#define configTIMER_QUEUE_LENGTH        10
#define configTIMER_TASK_STACK_DEPTH    (configMINIMAL_STACK_SIZE + 20)

/* By defining this value, a Q (including Semaphores and Mutexs) can be debugged using
*  the IAR FreeRTOS Kernel plugin which gives insight into the Q's and their behaviors
*  - useful for debugging tasks.
*
*  The number is the maximum number of Q's that can be registered.
*/
#define configQUEUE_REGISTRY_SIZE       30

/* add FreeRTOS support for event blocking */
#define INCLUDE_xTimerPendFunctionCall 1
#define INCLUDE_xEventGroupSetBitFromISR  1

#endif /* FREERTOS_CONFIG_H */
