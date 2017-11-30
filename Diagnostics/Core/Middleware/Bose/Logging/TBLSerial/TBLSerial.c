#include "TBLSerial.h"
#include "BoseUSART_API.h"
#include "gpio_mapping.h"
#include "stringbuffer.h"
#include "LoggerTask.h"
#include "configuresystem.h"


static StringBuffer_t sbuffer[TBL_NUM_STRING_BUFFERS];
static StringBufferTable_t TBLBufferTable = {.size = TBL_NUM_STRING_BUFFERS, .head = 0, .tail = 0};
static xSemaphoreHandle s_PrintBufferMutex;
#define PRINT_MUTEX_TIMEOUT 10
//
//@brief TBLSerial_Init
//
//
void TBLSerial_Init()
{
    /* Initialize buffer table */
    TBLBufferTable.buffers = sbuffer;

    s_PrintBufferMutex = xSemaphoreCreateMutex();
}

//
//@brief TBLSerial_PrintString
//
//
void TBLSerial_PrintString(const char* s)
{
#ifdef USE_USB_LOGGER
    USB_LoggerPrintString(s);
#endif
}

//
//@brief TBLSerial_Printf
//
//
void TBLSerial_Printf(const char* formatstring, ...)
{
    va_list args;
    if ((TBLBufferTable.head + 1) % TBLBufferTable.size == TBLBufferTable.tail)
    {
        xSemaphoreTake(s_PrintBufferMutex, TIMER_MSEC_TO_TICKS(PRINT_MUTEX_TIMEOUT));
    }

    int16_t bufNum = ReserveNextDebugBufferAtomic(&TBLBufferTable);

    if (bufNum >= 0)
    {
        va_start(args, formatstring);
        if (VsnprintfHelper (&TBLBufferTable, bufNum, 0, formatstring, args ) < 0)
        {
            UnreserveLastDebugBufferAtomic(&TBLBufferTable);
            if ((TBLBufferTable.head + 1) % TBLBufferTable.size != TBLBufferTable.tail)
            {
                xSemaphoreGive(s_PrintBufferMutex);
            }
        }
        else
        {
            TBLBufferTable.buffers[bufNum].filled = TRUE;
        }
        va_end(args);
    }
    else
    {
        TBLSerial_PrintString("Log buffers full - dropped a message.\r\n");
    }
}

//
//@brief TBLSerial_GetBuffer
//
//
StringBufferTable_t* TBLSerial_GetBuffer(void)
{
    return &TBLBufferTable;
}
