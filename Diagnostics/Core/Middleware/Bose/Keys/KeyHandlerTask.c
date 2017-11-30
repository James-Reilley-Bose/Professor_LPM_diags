/*
 * KeyHandlerTask.c - Serializes/offloads key events from various key producers.
 *
 * Author: Dillon Johnson
 */

#include "project.h"
#include "KeyHandler.h"
#include "KeyHandlerTask.h"
#include "TaskDefs.h"
#include "ir.h"
#include "ConsoleKeys.h"

SCRIBE_DECL(keys);

/* Function prototypes */
void InternalHandleKeyPress(KEY_VALUE held_key, KEY_PRODUCER_ATTRIBUTES* producer); // defined in KeyHandler.c
void InternalHandleKeyRelease(KEY_PRODUCER_ATTRIBUTES* producer); // defined in KeyHandler.c
static void KeyHandlerTask_HandleKeyPress(GENERIC_MSG_t* msg);
static void KeyHandlerTask_HandleKeyRelease(GENERIC_MSG_t* msg);
static void KeyHandlerTask_HandlePrintKeyList(GENERIC_MSG_t* msg);
static void KeyHandlerTask_HandleResetKeyList(GENERIC_MSG_t* msg);

/* Globals */
ManagedTask* ManagedKeyHandlerTask = NULL;

/*
 * @func KeyHandlerTask_Init
 *
 * @brief Initializes Key Handler task (currently nothing to do!).
 *
 * @param n/a
 *
 * @return n/a
 */
void KeyHandlerTask_Init(void* p)
{
    ManagedKeyHandlerTask = GetManagedTaskPointer("KeyHandlerTask");
    debug_assert(ManagedKeyHandlerTask != NULL);
    IR_Init();
#ifdef LPM_HAS_CONSOLE_KEYS
    CK_Init();
#endif    
}

/*
 * @func KeyHandlerTask
 *
 * @brief Waits for new key messages on the it's queue.
 *
 * @param n/a
 *
 * @return n/a
 */
void KeyHandlerTask(void* pvParameters)
{
    for (;;)
    {
        TaskManagerPollQueue(ManagedKeyHandlerTask);
    }
}

/*
 * @func KeyHandlerTask_HandleMessage
 *
 * @brief Calls the appropriate message handler.
 *
 * @param n/a
 *
 * @return n/a
 */
void KeyHandlerTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case KEYHANDLER_MESSAGE_ID_KeyPress:
            {
                KeyHandlerTask_HandleKeyPress(msg);
            }
            break;
        case KEYHANDLER_MESSAGE_ID_KeyRelease:
            {
                KeyHandlerTask_HandleKeyRelease(msg);
            }
            break;
        case KEYHANDLER_MESSAGE_ID_PrintKeyList:
            {
                KeyHandlerTask_HandlePrintKeyList(msg);
            }
            break;
        case KEYHANDLER_MESSAGE_ID_ResetKeyList:
            {
                KeyHandlerTask_HandleResetKeyList(msg);
            }
            break;
#ifdef LPM_HAS_CONSOLE_KEYS
        case KEYHANDLER_MESSAGE_ID_CK_Timer_KeyScan:
            {
                CK_KeyScanTimerExpired();
            }
            break;
        case KEYHANDLER_MESSAGE_ID_CK_Timer_KeyDebounce:
            {
                CK_DebounceTimerFired();
            }
            break;
        case KEYHANDLER_MESSAGE_ID_CK_Timer_KeyVulcan:
            {
                CK_VulcanTimer();
            }
            break;
#endif
        default:
            LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "KeyHandler 0x%0X was invalid message", msg->msgID);
            break;
    }
}

static void KeyHandlerTask_HandleKeyPress(GENERIC_MSG_t* msg)
{
    InternalHandleKeyPress((KEY_VALUE) msg->params[0], (KEY_PRODUCER_ATTRIBUTES*) msg->params[1]);
}

static void KeyHandlerTask_HandleKeyRelease(GENERIC_MSG_t* msg)
{
    InternalHandleKeyRelease((KEY_PRODUCER_ATTRIBUTES*) msg->params[1]);
}

static void KeyHandlerTask_HandlePrintKeyList(GENERIC_MSG_t* msg)
{
    PrintKeyList();
}

static void KeyHandlerTask_HandleResetKeyList(GENERIC_MSG_t* msg)
{
    ResetKeyList();
}
