/*!
* @file        EventBlocking.c
* @version     $Id:
* @author      ja70076
* @brief       Facility for tasks to block on message events
*
* Copyright 2014 Bose Corporation.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "project.h"
#include "EventDefinitions.h"
#include "event_groups.h"
#include "EventBlocking.h"
#include "rottenlog.h"

SCRIBE_DECL(sys_events);


/*
   Provide a facility for Tasks to block on message events. This implementation uses Event Groups.
   Tasks must first register to get a Blocking Event Handle, used in subsequent BlockOnEvent calls.
   The BlockOnEvent call also takes a time out argument.

*/

/* Per EventGroups.h: If configUSE_16_BIT_TICKS is 1 then each event group contains 8
 * usable bits (bit0 to bit 7).  If configUSE_16_BIT_TICKS is set to 0 then each event
 * group has 24 usable bits (bit 0 to bit 23).
 */
#define CONFIGURED_NUM_EVENT_GROUP_BITS (configUSE_16_BIT_TICKS ? 8 : 24)
#define NUM_BLOCKING_EVENT_GROUPS ((configQUEUE_REGISTRY_SIZE / CONFIGURED_NUM_EVENT_GROUP_BITS + 1))

// allocate resources for 1 per queue
#define MAX_EVENT_BLOCKERS configQUEUE_REGISTRY_SIZE
#define UNUSED_MAP_SLOT 255

// Each FreeRTOS Event Group can be used for multiple blocking calls
EventGroupHandle_t BlockingEventGroupTable[NUM_BLOCKING_EVENT_GROUPS];

// Registration provides the event group/bit mapping for future requests
static uint8_t numberOfEventBlockingRegistrations = 0;

// Registration info record per client
typedef struct
{
    MESSAGE_ID_t theWakingEvent;
    uint8_t theEventGroupIndex;
    EventBits_t theWakeUpBit;
} EventBlockRegInfo;

// The Registration Table
EventBlockRegInfo EventBlockRegistrationTable[MAX_EVENT_BLOCKERS];


// Keep track of active blocking requests separately
static uint8_t numberOfActiveBlockingRequests = 0;

// The Active Blocker Map - contains the Reg Table index
uint8_t ActiveEventBlockers[MAX_EVENT_BLOCKERS];


/**
  * @brief API call to register for blocking.  Called 1 time per blocking client.
  *        Finds the next free EventBlockTable slot and creates/assigns Blocking Event Group and Bit.
  * @param  None.
  * @retval The table slot index - used in subsequent blocking request calls.
  */
EVENT_BLOCKING_HANDLE RegisterForEventBlocking ()
{
    uint8_t theEGIndex;
    EventBits_t theEGBits;
    debug_assert(numberOfEventBlockingRegistrations < configQUEUE_REGISTRY_SIZE);

    // grab the next slot
    EVENT_BLOCKING_HANDLE theHandle = numberOfEventBlockingRegistrations++;

    // determine which event group and bits to use - could be 16 or 32 bits per group
    theEGIndex = (theHandle / CONFIGURED_NUM_EVENT_GROUP_BITS);
    debug_assert(theEGIndex < NUM_BLOCKING_EVENT_GROUPS);
    theEGBits = (theHandle % CONFIGURED_NUM_EVENT_GROUP_BITS);

    // Do we need a new event group ?
    if (theEGBits == 0)
    {
        BlockingEventGroupTable[theEGIndex] = xEventGroupCreate();
    }

    // Remember the details for this requestor
    EventBlockRegistrationTable[theHandle].theEventGroupIndex = theEGIndex;
    EventBlockRegistrationTable[theHandle].theWakeUpBit = (EventBits_t) (0x1 << theEGBits);

    return (theHandle);
}


/**
  * @brief API call to request event blocking.  This call validates the calling parameters,
  *        stuffs the Wake Up Event, sets up the next active blocker slot, and finally blocks the calling task.
  * @param  TheBlockingHandle: EVENT_BLOCKING_HANDLE returned from registerForEventBlocking()
  * @param  TheEvent: The event to wait for
  * @param  TimeoutInTicks: The abort time associated with the blocking call
  * @retval A bool: True indicates the event occurred, False indicates the blocking call timed out.
  */
boolean BlockOnEvent (EVENT_BLOCKING_HANDLE TheBlockingHandle, MESSAGE_ID_t TheEvent, uint32_t TimeoutInTicks )
{

    if (TheBlockingHandle >= numberOfEventBlockingRegistrations)
    {
        LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "BlockOnEvent: Invalid Handle.");
        return (FALSE);
    }

    // not that this would ever happen...
    if (InInterrupt())
    {
        LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "BlockOnEvent: From Interrupt.");
        return (FALSE);
    }

    EventGroupHandle_t theEventGroup = BlockingEventGroupTable[EventBlockRegistrationTable[TheBlockingHandle].theEventGroupIndex];
    const EventBits_t theWakeUpBit = EventBlockRegistrationTable[TheBlockingHandle].theWakeUpBit;

    // clear the wake up bit
    xEventGroupClearBits(theEventGroup, theWakeUpBit);

    // stuff the wake up event
    EventBlockRegistrationTable[TheBlockingHandle].theWakingEvent = TheEvent;

    // bind this request to the next free active slot
    taskENTER_CRITICAL();
    uint8_t theActiveSlot = numberOfActiveBlockingRequests++;
    ActiveEventBlockers[theActiveSlot] = TheBlockingHandle;
    taskEXIT_CRITICAL();

    // and block - this will clear the bits before returning - the return value indicates if the call timed out
    EventBits_t theBits = xEventGroupWaitBits(theEventGroup, theWakeUpBit, pdTRUE, pdTRUE, TimeoutInTicks);

    // clear/coalesce the active slot and return
    taskENTER_CRITICAL();
    ActiveEventBlockers[theActiveSlot] = ActiveEventBlockers[--numberOfActiveBlockingRequests];
    taskEXIT_CRITICAL();

    // if theWakeUpBit is not set, the call timed out
    return (theBits == theWakeUpBit);

}


/**
  * @brief  API to wake tasks blocking on a particular event.  Called by the Queueing rouitines
  * @param  TheEvent: The event
  * @retval None.
  */
void CheckAndMaybeWakeBlockers (MESSAGE_ID_t TheEvent)
{
    // search the Active Blocking map - there may be more than 1 task waiting for a given event
    for (uint8_t i = 0; i < numberOfActiveBlockingRequests; i++)
    {
        uint8_t theRegTableSlot = ActiveEventBlockers[i];

        // look for a match
        if (TheEvent == EventBlockRegistrationTable[theRegTableSlot].theWakingEvent)
        {
            EventGroupHandle_t theEventGroup = BlockingEventGroupTable[EventBlockRegistrationTable[theRegTableSlot].theEventGroupIndex];
            const EventBits_t theWakeUpBit = EventBlockRegistrationTable[theRegTableSlot].theWakeUpBit;

            // Wake up the client that may or may not be blocking
            if (InInterrupt())
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                BaseType_t xResult;

                xResult = xEventGroupSetBitsFromISR(theEventGroup, theWakeUpBit, &xHigherPriorityTaskWoken);
                if (xResult == pdTRUE)
                {
                    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
                }
            }
            else
            {
                xEventGroupSetBits(theEventGroup, theWakeUpBit);
            }
        }
    }
}


/* Test code to exercise blocking events
 * Create NUM_BETEST_TASKS tasks that all execute the stress routine
 * which blocks on the ETAP_MESSAGE_ID_ProcessCommand event and then
 * prints blocking status after unblocking.  Each task will then delay a
 * variable amount of time and repeat the block
 *
 * NOTE: The #define to enable this facility is found in EventBlocking.h
 */

#ifdef ENABLE_BLOCKING_EVENT_STRESS_TEST

#define LOWER_NINE_BITS 0x1ff
#define SOME_FUDGE_FACTOR 23

#include "etap.h"
#define DEBUG_OUT TAP_Printf

void beStressRoutine (void* pvParamaters)
{
    EVENT_BLOCKING_HANDLE myBlockingHandle;
    boolean eventOccurred;
    uint32_t myRandomWaitTime;

    myRandomWaitTime = TIMER_MSEC_TO_TICKS((GET_SYSTEM_UPTIME_MS() & LOWER_NINE_BITS) * SOME_FUDGE_FACTOR);
    myBlockingHandle = RegisterForEventBlocking();

    DEBUG_OUT("\r\nHandle %d EvtGrp %d  Bit %x wait %x", myBlockingHandle,
              EventBlockRegistrationTable[myBlockingHandle].theEventGroupIndex,
              EventBlockRegistrationTable[myBlockingHandle].theWakeUpBit,
              myRandomWaitTime);
    while (1)
    {
        eventOccurred = BlockOnEvent(myBlockingHandle, TAPPARSER_MESSAGE_ID_ProcessCommand, myRandomWaitTime * 2);
        DEBUG_OUT("\r\nH%d - %s", myBlockingHandle, (eventOccurred ? "EVT" : "Timeout"));
        vTaskDelay(myRandomWaitTime);
        DEBUG_OUT("\r\nH%d - Block", myBlockingHandle);
    }
}

void beInitStressTest (void)
{
    char be_TaskName[16];
    uint8_t beTaskNumberIndex = 10;
    xTaskHandle theTask;

    strcpy(be_TaskName, "BE_STRESS_XX");
    be_TaskName[beTaskNumberIndex + 2] = '\0';

    for (uint8_t i = 0; i < NUM_BE_STRESS_TEST_TASKS; i++)
    {
        sprintf(&be_TaskName[beTaskNumberIndex], "%02d", i);

        if (xTaskCreate (beStressRoutine, be_TaskName, 100, NULL, configSERVICE_TASK_PRIORITY, &theTask) != pdPASS)
        {
            RTOS_assert (0, "BE Stress Test Failed Creating TASK");
        }
        vTaskDelay(GET_SYSTEM_UPTIME_MS() & LOWER_NINE_BITS);


    }
}
#endif  //ENABLE_BLOCKING_EVENT_STRESS_TEST

