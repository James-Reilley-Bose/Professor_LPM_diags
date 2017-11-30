#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "project.h"
#include "etap.h"
#include "rottenlog.h"
#include "EventBlocking.h"
#include "QueueManager.h"
#include "TaskDefs.h"
#ifdef LAST_GASP
#include "lastGasp.h"
#endif

SCRIBE_DECL(system);

ManagedQ qListOfManagedQueues[configQUEUE_REGISTRY_SIZE];
char errorString[128];
uint8_t numberOfRegisteredQueues = 0;

/*
 * @func  QueueManagerInitQueue
 * @brief Q create and add to registry abstracted and combined pretty much + some Queue Manager stuffs.
 *
 * @param char *pcQueueName - Name string of a queue to be initialized;
 * @param unsigned portBASE_TYPE uxQueueLength - The maximum number of items that the queue can contain;
 * @param unsigned portBASE_TYPE uxItemSize - The number of bytes each item in the queue will require.
 *                                            Items are queued by copy, not by reference, so this is the number of bytes that will be copied for each posted item.
 *                                            Each item on the queue must be the same size.
 * @return - A pointer to the managed q that is stored in the list of managed queues
 */
ManagedQ* QueueManagerInitQueue(char* pcQueueName, const unsigned portBASE_TYPE uxQueueLength, const unsigned portBASE_TYPE uxItemSize)
{
    if (uxQueueLength > 0)
    {
        debug_assert(numberOfRegisteredQueues < configQUEUE_REGISTRY_SIZE);
        qListOfManagedQueues[numberOfRegisteredQueues].qHandle = xQueueCreate(uxQueueLength, uxItemSize);
        vQueueAddToRegistry(qListOfManagedQueues[numberOfRegisteredQueues].qHandle, pcQueueName);

        RTOS_assert((qListOfManagedQueues[numberOfRegisteredQueues].qHandle != NULL), "Q Init Failed");

        qListOfManagedQueues[numberOfRegisteredQueues].qName = pcQueueName;
        qListOfManagedQueues[numberOfRegisteredQueues].qLength = uxQueueLength;
        qListOfManagedQueues[numberOfRegisteredQueues].qMsgLength = uxItemSize;

        numberOfRegisteredQueues++;

        return &qListOfManagedQueues[numberOfRegisteredQueues - 1];
    }
    else
    {
        return NULL;
    }
}

/*
 * @func  QueueManagerPostCallbackMsg
 * @brief Post an event to a given queue and register a callback for when it is serviced.
 *
 * @param char *pcQueueName - Name string of a queue to post event message to;
 * @param MESSAGE_ID_t msgID - The message you want to send to the given queue;
 * @param uint32_t params[] - Array of argument parameters;
 * @param void * callbackFunc - function to call after processing message
 *
 * @return - 0 if the given message has been posted successfully, and -1 otherwise.
 */

signed portBASE_TYPE QueueManagerPostCallbackMsg(ManagedQ* Q, MESSAGE_ID_t msgID, uint32_t* params, void (*callbackFunc)(uint32_t*, uint8_t), uint32_t timeoutMs)
{
    debug_assert(Q);
    debug_assert(Q->qHandle);

    GENERIC_MSG_t msg;
    msg.msgID = msgID;
    msg.CallbackFunction = callbackFunc;

    uint8_t len = Q->qMsgLength - NO_PARAM_MESSAGE_SIZE;
    if (params)
    {
        memcpy(msg.params, params, len);
    }
    else
    {
        memset(msg.params, 0x00, len);
    }

    CheckAndMaybeWakeBlockers(msgID);

    if (!InInterrupt())
    {
        if (xQueueSend(Q->qHandle, &msg, TIMER_MSEC_TO_TICKS(timeoutMs)) != pdTRUE)
        {
            snprintf(errorString, sizeof(errorString), "Unable to post message %s to %s: Queue is full", GetEventString(msgID), Q->qName);
#ifdef LAST_GASP
            UnwindQueue(Q->qHandle, 100 );
#endif
            if (strcmp(Q->qName, "TapListenerTask") == 0)
            {
                LOG(system, ROTTEN_LOGLEVEL_VERBOSE, errorString);
            }
            else
            {
                RTOS_assert(0, errorString);
            }
            return QUEUEMANAGER_ERROR;
        }
    }
    else
    {
        if (xQueueSendToBackFromISR(Q->qHandle, &msg, NULL) != pdTRUE)
        {
            snprintf(errorString, sizeof(errorString), "Unable to post message %s to %s: Queue is full", GetEventString(msgID), Q->qName);
#ifdef LAST_GASP
            //   The following a few lines is for debugging only, i.e., unwinding the queue to see what messages were in the queue.
            //   Similar to UnwindQueue above in the same function, here is its ISR-safe and simplified version.
            GENERIC_MSG_t message;
            for (int i = 0; i < Q->qLength; i++ )
            {
                xQueueReceiveFromISR( Q->qHandle, &message, NULL);
            }
#endif
            RTOS_assert(0, errorString);
            return QUEUEMANAGER_ERROR;
        }
    }
    return 0;
}

/*
 * @func  GetQueueByName
 * @brief Get a queue by searching its name.
 *
 * @param char *pcQueueName - Name string of a queue from which to get the handle
 *
 * @return - The queue that match the given name, 0 if name does not match.
 */
ManagedQ* GetQueueByName(char* pcQueueName)
{
    for (uint8_t index = 0; index < numberOfRegisteredQueues; index++)
    {
        if (strncasecmp(qListOfManagedQueues[index].qName, pcQueueName, strlen(qListOfManagedQueues[index].qName)) == 0)
        {
            return &qListOfManagedQueues[index];
        }
    }
    return NULL;
}

/*
 * @func  GetQueueIndexFromHandle
 * @brief Get the index of a queue from q's handle.
 *
 * @param xQueueHandle handle - Handle of a queue from which to get the index
 *
 * @return - The index of the queue that match the given handle, -1 if name does not match.
 */
int8_t GetQueueIndexFromHandle(xQueueHandle handle)
{
    for (int i = 0; i < numberOfRegisteredQueues; i++)
    {
        if (qListOfManagedQueues[i].qHandle == handle)
        {
            return i;
        }
    }
    return QUEUEMANAGER_ERROR;
}

/*
 * @func  GetQueueNameFromHandle
 * @brief Get the name of a queuefrom q's handle.
 *
 * @param xQueueHandle handle - Handle of a queue from which to get the string name
 *
 * @return - The string name of the queue that match the given handle, NULL if name does not match.
 */
char* GetQueueNameFromHandle(xQueueHandle handle)
{
    for (int i = 0; i < numberOfRegisteredQueues; i++)
    {
        if (qListOfManagedQueues[i].qHandle == handle)
        {
            return qListOfManagedQueues[i].qName;
        }
    }
    return NULL;
}

/*
 * @func GetQueueSpacesAvailable
 * @brief Get the amount of space available in the queue.
 *
 * @param ManagedQ *Q - Memory location of managed queue that the message is to be posted to;
 * @return - Number of available queue spaces.
 */
uint32_t GetQueueSpacesAvailable(ManagedQ* Q)
{
    debug_assert(Q);
    debug_assert(Q->qHandle);

    return (uxQueueSpacesAvailable(Q->qHandle));
}

/*
 * @func GetQueueMessagesWaiting
 * @brief Get the number of messages in the queue.
 *
 * @param ManagedQ *Q - Memory location of managed queue that the message is to be posted to;
 * @return - Number of messages in the queue.
 */
uint32_t GetQueueMessagesWaiting(ManagedQ* Q)
{
    uint32_t messagesWaiting = 0;
    debug_assert(Q);
    debug_assert(Q->qHandle);

    if (!InInterrupt())
    {
        messagesWaiting = uxQueueMessagesWaiting(Q->qHandle);
    }
    else
    {
        messagesWaiting = uxQueueMessagesWaitingFromISR(Q->qHandle);
    }

    return messagesWaiting;
}
