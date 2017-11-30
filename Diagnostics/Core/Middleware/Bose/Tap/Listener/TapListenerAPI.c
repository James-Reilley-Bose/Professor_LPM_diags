#include "project.h"
#include "serialTapListener.h"
#include "TapListenerAPI.h"
#include "ascii.h"
#include "rottenlog.h"
#include "stringbuffer.h"
#include "TaskManager.h"
#include "TaskDefs.h"
#include <string.h>

SCRIBE_DECL(system);
ManagedTask* ManagedTapListenerTask = NULL;

#define TapListenerPostMsg(_theMsg, _p1) QueueManagerPostCallbackMsg(&ManagedTapListenerTask->Queue, _theMsg, (uint32_t []){_p1},NOP_CALLBACK,NO_BLOCK_TIME);
#define TapListenerPostMsg2(_theMsg, _p1, _p2) QueueManagerPostCallbackMsg(&ManagedTapListenerTask->Queue, _theMsg, (uint32_t []){_p1, _p2},NOP_CALLBACK,NO_BLOCK_TIME);
////////////////////////
//Basic Task functions//
////////////////////////
void TapListenerModule_Init(void* p)
{
    ManagedTapListenerTask = GetManagedTaskPointer("TapListenerTask");
}

void TapListenerModule (void* pvParamaters)
{
    debug_assert(ManagedTapListenerTask != NULL);
    Listener_PrintString(TAP_NEW_LINE_AND_PROMPT);
    for (;;)
    {
        TaskManagerPollQueue(ManagedTapListenerTask);
    }
}

signed portBASE_TYPE TapListenerCheckAndPostTextForOutput (MESSAGE_ID_t msg, const char* text)
{
    if (ManagedTapListenerTask)
    {
        return TapListenerPostMsg(msg, (uint32_t)text);
    }
    else
    {
        return QUEUEMANAGER_ERROR;
    }
}

//nb: This is called from the USART ISR - so no delays
signed portBASE_TYPE TapListenerPostOverrunError ()
{
    return TapListenerPostMsg(TAPLISTENER_MESSAGE_ID_OverrunError, 0);
}

void TapListener_PrintPrompt(uint32_t* params, uint8_t numParams)
{
    TapListenerCheckAndPostTextForOutput(TAPLISTENER_MESSAGE_ID_Print, TAP_NEW_LINE_AND_PROMPT);
}

////////////////////////////////////
//Functions to handle each message//
////////////////////////////////////
void TapListener_PrintString(const char* stringToPrint)
{
    TapListenerCheckAndPostTextForOutput(TAPLISTENER_MESSAGE_ID_Print, stringToPrint);
}

void TapListener_PrintBuffer(StringBufferTable_t* table)
{
    TapListenerCheckAndPostTextForOutput(TAPLISTENER_MESSAGE_ID_PrintBuffer, (char*)table);
}

//
// @brief Handler for printing a string + new line (SERIALTAPLISTENER_MESSAGE_ID_Print)
///
void TapListener_HandleSerialTapListenerMessageIDPrint(GENERIC_MSG_t* msg)
{
    /*
     * params[0] = string to print
     */
    Listener_PrintString((const char*)msg->params[0]);
}

//
// @brief Handler for printing a stringbuffer (SERIALTAPLISTENER_MESSAGE_ID_PrintBuffer)
//      Note: This function just prints whatever is on the tail (FIFO)
///
void TapListener_HandleSerialTapListenerMessageIDPrintBuffer(GENERIC_MSG_t* msg)
{
    /*
     * params[0] = pointer to Buffer Table
     */
    if (msg->params[0] == NULL)
    {
        return;
    }
    StringBufferTable_t* BufferTable = (StringBufferTable_t*) msg->params[0];
    Listener_PrintString(BufferTable->buffers[BufferTable->tail].line);
    StringBuffer_IncrementTail(BufferTable);
}

/////////////////////////////////
//Function to dispatch messages//
/////////////////////////////////
void TapListenerModule_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case TAPLISTENER_MESSAGE_ID_Print:
            {
                TapListener_HandleSerialTapListenerMessageIDPrint(msg);
            }
            break;

        case TAPLISTENER_MESSAGE_ID_PrintBuffer:
            {
                TapListener_HandleSerialTapListenerMessageIDPrintBuffer(msg);
            }
            break;

        case TAPLISTENER_MESSAGE_ID_OverrunError:
            {
                /* Do we care? What do we do? */
                // RTOS_assert (0, "Etap UART Overflow");
            }
            break;

        default:
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "TapListener 0x%0X invalid message", msg->msgID);
            break;
    }
}
