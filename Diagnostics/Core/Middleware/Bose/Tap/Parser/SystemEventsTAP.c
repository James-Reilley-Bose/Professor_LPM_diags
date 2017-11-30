#include "project.h"
#include "SystemEventsTAP.h"
#include "etap.h"
#include <string.h>
#include "QueueManager.h"
#include "TapParserAPI.h"
#include "EventDefinitions.h"
#include "EventBlocking.h"
#include "rottenlog.h"

#ifdef INCLUDE_EVENT_STRINGS
SCRIBE_DECL(sys_events)


// Some events should not be posted via TAP - they confiuse the system and lead to undefined behavior
// This routine identifies such events, and prevents the damage from occuring.
static BOOL EventShouldNotBePostedViaTAP (char* queueName, char* systemEvent)
{
    if ((strcasecmp(queueName, "TapListenerTask") == 0) ||
            (strcasecmp(queueName, "TapParserTask") == 0)   ||
            (strcasecmp(systemEvent, "KEYHANDLER_MESSAGE_ID_PrintKeyList") == 0) ||
            (strcasecmp(systemEvent, "IPCROUTER_MESSAGE_ID_RxMessage") == 0))
    {
        return TRUE;
    }


    return FALSE;
}

static void PostSysEventFromStrings(CommandLine_t* CommandLine)
{
    char* systemEvent = CommandLine->args[1];
    MESSAGE_ID_t event = GetEventFromString(systemEvent);

    if (event == INVALID_EVENT)
    {
        TAP_Printf("\"%s\" is not a valid system event.", systemEvent);
    }
    else if (EventShouldNotBePostedViaTAP("EveryQueue", systemEvent))
    {
        // Don't post it - Produce LOG output and return
        LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "\tSys Event\t%s NOT posted", GetEventString(event));
        TAP_Printf("");
    }
}


uint32_t* GetParams(CommandLine_t* CommandLine)
{
    static uint32_t params[configQUEUE_MESSAGE_MAX_PARAMS] = {0};

    for (int i = 0; i < (CommandLine->numArgs - 2); i++)
    {
        params[i] = (uint32_t)CommandLine->args[i + 2];
    }
    return params;
}

static void PostEventFromStrings(CommandLine_t* CommandLine)
{
    char* queueName = CommandLine->args[0];
    ManagedQ* Q;
    xQueueHandle qh;

    Q = GetQueueByName(queueName);

    if (Q)
    {
        qh = Q->qHandle;
        char* systemEvent = CommandLine->args[1];
        MESSAGE_ID_t event = GetEventFromString(systemEvent);

        if (event == INVALID_EVENT)
        {
            TAP_Printf("\"%s\" is not a valid system event.", systemEvent);
        }
        // posting bogus TAP events will confuse TAP and yield undefined behavior...
        else if (EventShouldNotBePostedViaTAP(queueName, systemEvent))
        {
            // Don't post it - Produce LOG output and return
            LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "\t%s\t%s NOT posted", GetQueueNameFromHandle(qh), GetEventString(event));
            TAP_Printf("");
        }
        else
        {
            QueueManagerPostCallbackMsg(Q, event, GetParams(CommandLine), NOP_CALLBACK, NO_BLOCK_TIME);
            TAP_Printf("");
        }
    }
    else
    {
        TAP_Printf("Q not found!");
    }
}

//
// @func TAP_PostEvent
// @brief TAP command to send a event to the system.
//
TAPCommand(TAP_PostEvent)
{
    switch (CommandLine->numArgs)
    {
        case 0:
            {
                TAP_PrintString("---------------------------\n\rValid Queue's:\n\r");
                for (uint16_t i = 0; i < numberOfRegisteredQueues; i++)
                {
                    TAP_PrintString(qListOfManagedQueues[i].qName);
                    TAP_PrintString("\n\r");
                }

                TAP_PrintString("---------------------------\n\rValid Events:\n\r");
                for (uint16_t i = 0; i != INVALID_EVENT; i++)
                {
                    TAP_PrintString(GetEventString((MESSAGE_ID_t)i));
                    TAP_PrintString("\n\r");
                }
                TAP_PrintString("---------------------------\n\r");
            }
            break;

        default:
            {
                if (strncasecmp("system", CommandLine->args[0], strlen(CommandLine->args[0])) == 0)
                {
                    PostSysEventFromStrings(CommandLine);
                }
                else
                {
                    PostEventFromStrings(CommandLine);
                }
            }
            break;
    }
}

//
// @func TAP_BlockOnEvent
// @brief TAP command to block the TAP Task waiting for a specified event.
//
TAPCommand(TAP_BlockOnEvent)
{
    if ((CommandLine->numArgs < 1) || (CommandLine->numArgs > 2))
    {
        TAP_Printf(BLOCK_ON_EVENT_HELP_TEXT);
    }
    else
    {
        uint8_t  argLen = 0;
        uint32_t timeout = TIMER_MSEC_TO_TICKS(500);  // default timeout time to 500 MSec
        boolean  found = FALSE;

        // was a timeout time provided
        if (CommandLine->numArgs > 1)
        {
            timeout = TAP_DecimalArgToInt(CommandLine, 1, &found);
            if (found)
            {
                timeout = TIMER_MSEC_TO_TICKS(timeout);
            }
        }

        // now find the event to block on
        MESSAGE_ID_t eventToBlockOn = INVALID_EVENT;
        argLen = strlen(CommandLine->args[0]);

        // if test, use the ETAP_MESSAGE_ID_ProcessCommand event and solicit keyboard input
        if ((argLen == 4) && (strncmp(CommandLine->args[0], "test", argLen) == 0))
        {
            TAP_Printf("Blocking on ProcessCommand event - timeout in 5 seconds.");
            TAP_Printf("Type any key and hit CR to produce that event...");
            timeout = TIMER_MSEC_TO_TICKS(5000); // make timeout time to 5 Seconds
            found = BlockTapOnEvent(TAPPARSER_MESSAGE_ID_ProcessCommand, timeout);
            TAP_Printf("Blocking Call Returned: %s", (found ? "Event Occurred" : "Time Out Expired"));
        }

#ifdef ENABLE_BLOCKING_EVENT_STRESS_TEST
        else if ((argLen == 6) && (strncmp(CommandLine->args[0], "stress", argLen) == 0))
        {
            TAP_Printf("Starting Stress Test.  Submit TAP commands to generate the event");
            beInitStressTest();
        }
#endif
        else
        {
            found = FALSE;

            // lookup the event
            for (uint16_t i = 0; i <= INVALID_EVENT; i++)
            {
                if ((strncasecmp(GetEventString((MESSAGE_ID_t)i), CommandLine->args[0], argLen)) == 0)
                {
                    // we have a winner
                    eventToBlockOn = (MESSAGE_ID_t) i;
                    found = TRUE;
                    break;
                }
            }

            // if we found a match, make the blocking call
            if (found)
            {
                found = BlockTapOnEvent(eventToBlockOn, timeout);
                TAP_Printf("Blocking Call Returned: %s", (found ? "Event Occurred" : "Time Out Expired"));
            }
            else
            {
                TAP_Printf("BE: No Event matches \"%s\"", CommandLine->args[0]);
            }
        }
    }
}
#endif


