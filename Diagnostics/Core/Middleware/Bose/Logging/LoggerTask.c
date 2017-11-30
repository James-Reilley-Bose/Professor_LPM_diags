/*
 * LoggerTask.c - task that prints logging info to Boselink UART
 *
 * Author: Dillon Johnson
 */

#include "project.h"
#include "LoggerTask.h"
#include "TBLSerial.h"
#include "stringbuffer.h"
#include "TaskManager.h"

SCRIBE(annotate, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(flash, ROTTEN_LOGLEVEL_DISABLED);
SCRIBE(i2c, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc_pt, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc_raw, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc_router, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc_rx, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc_tx, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(ipc_update, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(power, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(sourcechange, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(system, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(sys_events, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(tap_events, ROTTEN_LOGLEVEL_DISABLED);
SCRIBE(tap_test_results, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(timer, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(update, ROTTEN_LOGLEVEL_NORMAL);
#ifdef LPM_HAS_RF_REMOTE
SCRIBE(remote, ROTTEN_LOGLEVEL_NORMAL);
#endif

ManagedTask ManagedLoggerTask;
ManagedModule ManagedLoggerModule;

/* File scope variables */
static StringBufferTable_t* BufferTable;

void LoggerTaskInit(void* p)
{
    BufferTable = TBLSerial_GetBuffer();

#ifdef USE_USB_LOGGER
    USB_LoggerTaskInit();
#endif
}

/*
 * @func LoggerTask
 *
 * @brief Continously loops, emptying log buffers.
 *
 * @param void *pvParameters -
 *
 * @return n/a
 */
void LoggerTask (void* pvParamaters)
{
    /* Print if there's something in the buffer, else wait */
    for (;;)
    {
        while ((BufferTable->head != BufferTable->tail) &&
                (BufferTable->buffers[BufferTable->tail].filled))
        {
            TBLSerial_PrintString(BufferTable->buffers[BufferTable->tail].line);
            StringBuffer_IncrementTail(BufferTable);
        }
        vTaskDelay(LOGGER_TASK_DELAY_TIME);
    }
}

/*
 * @func InitializeLogs
 *
 * @brief Initialize logs by writing to them.
 *
 * @param n/a
 *
 * @return n/a
 */
void InitializeScribes (void)
{
    uint8_t totalScribes = 0;

    // Init system scribes, common to all LPM boards
    INIT_LOG_SCRIBE(annotate, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(flash, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(i2c, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc_bulk, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc_pt, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc_raw, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc_router, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc_rx, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc_tx, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(ipc_update, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(power, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(sourcechange, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(system, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(sys_events, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(tap_events, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(tap_test_results, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(timer, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    INIT_LOG_SCRIBE(update, ROTTEN_LOGLEVEL_INSANE, totalScribes);
    
    // Init board specific scribes
    totalScribes += InitVariantScribes();

    debug_assert(totalScribes <= ROTTEN_SCRIBE_COUNT);
}
