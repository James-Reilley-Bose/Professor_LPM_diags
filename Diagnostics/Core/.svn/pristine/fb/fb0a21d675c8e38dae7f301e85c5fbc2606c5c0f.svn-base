////////////////////////////////////////////////////////////////////////////////
/// @file            etapTasks.c
/// @version         $Id: $
/// @brief           Task data via ETAP
/// @author          Stolen from Triode by Dillon Johnson
/// @date            Creation Date: 2015
///
/// Copyright 2015 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "project.h"
#include "etapTasks.h"
#include "etap.h"
#include "SystemAnalysis.h"
#include "buffermanager.h"
#include "TaskDefs.h"

typedef struct
{
    signed char* pcHead;                /*< Points to the beginning of the queue storage area. */
    signed char* pcTail;                /*< Points to the byte at the end of the queue storage area. */
    signed char* pcWriteTo;             /*< Points to the free next place in the storage area. */
    signed char* pcReadFrom;            /*< Points to the last place that a queued item was read from. */
} FREE_RTOS_Q_ACCESSOR_t;

#ifdef ANALYZE_TASK_RUNTIMES
static void PrintTaskSummaryData (FREE_RTOS_TASK_DATA_t* data)
{
    TAP_Printf("%-*s%15d%15d%15d%15d%15d\r\n", configMAX_TASK_NAME_LEN, data->name, data->runTimePercentage, data->QSize, data->QHigh, 4 * data->stackSize, 4 * (data->stackSize - data->stackHighWater));
}

static void GatherSummaryData (FREE_RTOS_TASK_DATA_t* data)
{
    uint32_t monitorTimeUS = SystemAnalysis_GetRealTimeMsec() * 10;

    for (uint32_t i = 0; i < NUM_MANAGED_TASKS; i++)
    {
        ManagedTask* t = &ProductTasks[i];
        FREE_RTOS_TASK_DATA_t* d = &data[i];

        d->name = t->Name;
        d->runTimePercentage = SystemAnalysis_GetRunTimeUS(ProductTasks[i].TaskHandle) / monitorTimeUS;
        d->Q = t->Queue.qHandle;
        d->QSize = t->Queue.qLength;
        d->QHigh = t->Queue.qHighWatermark;
        d->stackSize = t->StackSize;
        d->stackHighWater = (uint32_t) uxTaskGetStackHighWaterMark(t->TaskHandle);
        d->numParams = t->Queue.numParams;
    }
}

#else // ANALYZE_TASK_RUNTIMES
static void GatherSummaryData (FREE_RTOS_TASK_DATA_t* data)
{
}
#endif // ANALYZE_TASK_RUNTIMES

#ifdef ANALYZE_INTERRUPTS
void GatherInterruptData(ISRMonitor_t* data, uint8_t numISRData)
{
    memcpy((uint8_t*)data, (uint8_t*)isrMonitor, (NUM_MONITOR_ISRS * sizeof(ISRMonitor_t)));
}

void PrintInterruptData(ISRMonitor_t* data, uint8_t numISRData)
{
    const char* names[NUM_MONITOR_ISRS] =
    {
        "CC_E",
        "CC_T",
        "DMA",
        "DWAM_U",
        "DWAM_E",
        "HDMI_E",
        "I2C_EVT",
        "I2C_ERR",
        "IPC_I2C_F0_E",
        "IPC",
        "IR_TIMER_ISR",
        "SPI_DMA",
        "SPI_RXNE",
        "TAP_UART_ISR",
        "UEI_UART_ISR",
        "USB",
        "PWR_VSYS_OK",
        "PSOC_ISR",
    };
    TAP_Printf("%-15s%15s%15s%15s\n\r", "ISR Name", "Worst Case", "Exceed Count", "Total Count");
    for (uint8_t i = 0; i < NUM_MONITOR_ISRS; i++)
    {
        TAP_Printf("%-15s%15d%15d%15d\n\r", names[i], data[i].g_WorstCaseIntLatency, data[i].g_InterruptExceedCount, data[i].g_InterruptTotalCount);
    }
}

#else // ANALYZE_INTERRUPTS
void GatherInterruptData(ISRMonitor_t* data, uint8_t numISRData)
{
}
#endif // ANALYZE_INTERRUPTS


TAPCommand(TAP_HandleFREtapMsg)
{
    char* taskName = "!";
    if (CommandLine->numArgs >= 1)
    {
        taskName = CommandLine->args[0];
    }

    ISRMonitor_t* isr = BufferManagerGetBuffer(sizeof(ISRMonitor_t) * NUM_MONITOR_ISRS);
    debug_assert(isr);
    FREE_RTOS_TASK_DATA_t* data = BufferManagerGetBuffer(sizeof(FREE_RTOS_TASK_DATA_t) * NUM_MANAGED_TASKS);
    debug_assert(data);
    unsigned portBASE_TYPE* rt = BufferManagerGetBuffer(sizeof(portBASE_TYPE) * NUM_MANAGED_TASKS);
    debug_assert(rt);

    memset(isr, 0x00, sizeof(ISRMonitor_t) * NUM_MONITOR_ISRS);
    memset(data, 0x00, sizeof (FREE_RTOS_TASK_DATA_t) * NUM_MANAGED_TASKS);
    memset(rt, 0x00, sizeof (portBASE_TYPE) * NUM_MANAGED_TASKS);

    BOOL good = TRUE;

    /* Wrap the data gathering in a critical section so their data does not change */
    portENTER_CRITICAL();
    unsigned portBASE_TYPE runTime = SystemAnalysis_GetRealTimeMsec();
    GatherSummaryData(data);
    GatherInterruptData(isr, NUM_MONITOR_ISRS);

    switch (taskName[0])
    {
        case '0':
            /* Restart the run time gathering */
            SystemAnalysis_ClearRunTimes();
            good = FALSE;
            break;
        case '2':
            /* Clear the interrupt information */
            SystemAnalysis_ClearInterruptData();
            good = FALSE;
            break;
        default:
            break;
    }
    portEXIT_CRITICAL();

    if (good)
    {
        uint8_t index = NUM_MANAGED_TASKS;
        ManagedTask* task;

        TAP_Printf("Run time = %dms\r\n\n", runTime);

        /* Was a specific task specified? */
        if ((task = GetManagedTaskPointer(taskName)) != NULL)
        {
            for (index = 0; index < NUM_MANAGED_TASKS; index++)
            {
                if (data[index].Q == task->Queue.qHandle)
                {
                    break;
                }
            }
        }

#ifdef ANALYZE_INTERRUPTS
        TAP_PrintString("###################### Interrupt Data ######################\n");
        PrintInterruptData(isr, NUM_MONITOR_ISRS);
#else // ANALYZE_INTERRUPTS
        TAP_PrintString("Interrupt analysis not enabled.");
#endif // ANALYZE_INTERRUPTS

        /* this sends a summary of all the tasks in one message. It is of the following format:
        *
        *  NAME     \% Util  Q    High       Stack High
        *  main     5       12   20         100   400
        *  IR       1       1    6          200   800
        *
        *  Name is returned as a NULL terminated string then there are 4 uint16 values per task
        */
#ifdef ANALYZE_TASK_RUNTIMES
        TAP_PrintString("\r\n######################### Task Data ########################\r\n");
        TAP_Printf("%-*s%15s%15s%15s%15s%15s\n\r", configMAX_TASK_NAME_LEN, "Name", "Runtime %", "Queue Size", "Queue High", "Stack Size", "Stack High");
        if (index == NUM_MANAGED_TASKS) // print all
        {
            for (uint8_t current = 0; current < NUM_MANAGED_TASKS; current++)
            {
                PrintTaskSummaryData(&data[current]);
            }

            TAP_Printf("Idle                    %3d\r\n", SystemAnalysis_GetIdleTaskRunTime());
            TAP_Printf("Timer Daemon            %3d\r\n", SystemAnalysis_GetTimerDaemonRunTime());
        }
        else // print specific task, dump queue
        {
            uint8_t* qInBytes;
            GENERIC_MSG_t* msg;
            FREE_RTOS_Q_ACCESSOR_t* qPtr = (FREE_RTOS_Q_ACCESSOR_t*)(data[index].Q);

            PrintTaskSummaryData(&data[index]);

            /* Get the last message the task processed.*/
            /* Next do the message Q itself. The Q Handle points to the address of the Q. */
            msg = (GENERIC_MSG_t*)(qPtr->pcReadFrom);

            /* Go backward through the Q and dump the prior 10 (max) messages */
            TAP_PrintString("\r   ID        param1\r");
            TAP_PrintString("-------------------\r");
            int i;
            for (i = 0 ; (i < MAX_SUPPORTED_Q_ITEMS) && (i < data[index].QSize) ; i++)
            {
                TAP_Printf("%d %0.8x %0.8x\r\n", i, (uint32_t)(msg->msgID), (uint32_t)(msg->params[0]));

                if (msg == (GENERIC_MSG_t*)(qPtr->pcHead))
                {
                    msg = (GENERIC_MSG_t*)(qPtr->pcTail);
                }
                qInBytes = (uint8_t*)(msg);

                /* The tail of the list is actually just a NULL so still back up one
                *  message position when the Head is reached.
                */
                msg = (GENERIC_MSG_t*)(qInBytes - task->Queue.qMsgLength);
            }
        }
#else // ANALYZE_TASK_RUNTIMES
        TAP_PrintString("\n\rTask runtime analysis not enabled.");
#endif // ANAYLYZE_TASK_RUNTIMES
    }

    BufferManagerFreeBuffer(isr);
    BufferManagerFreeBuffer(data);
    BufferManagerFreeBuffer(rt);
}
