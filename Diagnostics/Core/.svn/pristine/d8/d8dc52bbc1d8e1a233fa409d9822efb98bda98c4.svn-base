#include "project.h"
#include "etap.h"
#include "TapListenerAPI.h"
#include "TapParserAPI.h"
#include "ascii.h"
#include "EventDefinitions.h"
#include "rottenlog.h"
#include "EventBlocking.h"
#include "TaskManager.h"
#include "TaskDefs.h"
#include "etapKeys.h"

SCRIBE_DECL(system);

////////////////////////
//Basic Task functions//
////////////////////////
#ifdef LPM_HAS_KEY_HANDLER
static void TapParserTask_HandleTapKeyPressAndHold(GENERIC_MSG_t* msg);
#endif

// API to block the TAP Parser task/thread
boolean BlockTapOnEvent(MESSAGE_ID_t event, uint32_t timeoutTicks)
{
    return (BlockOnEvent(ManagedTapParserTask->BlockingHandle, event, timeoutTicks));
}

void ETAPTaskModule_Init(void* p)
{
#ifdef LPM_HAS_KEY_HANDLER
    InitializeTAPKeys();
#endif
    ManagedTapParserTask = GetManagedTaskPointer("TapParserTask");
}

void ETAPTaskModule_DeInit()
{
    //NOP
}

void TapParserTask (void* pvParamaters)
{
    debug_assert(ManagedTapParserTask != NULL);
    for (;;)
    {
        TaskManagerPollQueue(ManagedTapParserTask);
    }
}

////////////////////////////////////
//Functions to handle each message//
////////////////////////////////////


/////////////////////////////////
//Function to dispatch messages//
/////////////////////////////////
void ETAPTaskModule_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case TAPPARSER_MESSAGE_ID_WaitTimerExpired:
            {
                //NOP
            }
            break;
        case TAPPARSER_MESSAGE_ID_ProcessCommand:
            {
                //Called by the listener after a return is hit

                /* params[0] = string to parse and such
                  params[1] = num of chars
                */
                TAP_ProcessCommand((char*)msg->params[0]);
            }
            break;
#ifdef LPM_HAS_KEY_HANDLER
        case TAPPARSER_MESSAGE_ID_TapKeyPressAndHold:
            {
                TapParserTask_HandleTapKeyPressAndHold(msg);
            }
            break;
#endif
        case TAPPARSER_MESSAGE_ID_PassThroughDone:
            {
                //NOP
                //Just used to block
            }
            break;
        default:
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "TapParser 0x%0X invalid message", msg->msgID);
            break;
    }
}

#ifdef LPM_HAS_KEY_HANDLER
static void TapParserTask_HandleTapKeyPressAndHold(GENERIC_MSG_t* msg)
{
    TapKeyHandlePressAndHold();
}
#endif
