//
// KeyTask.c - Example task that logs received IR keys.
//

#include "project.h"
#include "TaskDefs.h"
#include "KeyTask.h"
#include "ir.h"

SCRIBE_DECL(keys);

static void KeyTask_HandleKeyMessage(GENERIC_MSG_t* msg);

ManagedTask* KeyTaskHandle = NULL;

void KeyTask_Init(void* p)
{
    KeyTaskHandle = GetManagedTaskPointer(KEY_TASK_NAME);
    debug_assert(KeyTaskHandle);

    IR_Init();
}

void KeyTask(void* pvParameters)
{
    for (;;)
    {
        TaskManagerPollQueue(KeyTaskHandle);
    }
}

void KeyTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case Example_Message_ID_KeyReceived:
            KeyTask_HandleKeyMessage(msg);
            break;
        default:
            LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "KeyTask unhandled message %d", msg->msgID);
            break;
    }
}

void Product_HandleKey(KEY_DATA_t key)
{
    KeyTaskPostMsg(Example_Message_ID_KeyReceived, key.value, key.state);
}

static void KeyTask_HandleKeyMessage(GENERIC_MSG_t* msg)
{
    LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "%s %d", Key_Info[msg->params[0]].name, msg->params[1]);
}

