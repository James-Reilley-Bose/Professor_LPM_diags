/*!
* @file        NVTask.c
* @version     $Id:
* @author      dr1005920
* @brief
*
* Copyright 2010 Bose Corporation.
*/

#include "project.h"
#include "etap.h"
#include "EventDefinitions.h"
#include "rottenlog.h"
#include "EventBlocking.h"
#include "TaskManager.h"
#include "TaskDefs.h"
#include "project.h"
#include "EventDefinitions.h"
#include "rottenlog.h"
#include "EventBlocking.h"
#include "TaskManager.h"
#include "TaskDefs.h"
#include "nv_struct.h"
#include "NVTask.h"

SCRIBE_DECL(sys_events);
SCRIBE_DECL(event);

/* Global Variable
===============================================================================*/
ManagedTask* ManagedNVTask = NULL;


////////////////////////
//Basic Task functions//
////////////////////////

// API to block the TAP Parser task/thread
boolean BlockNVTaskOnEvent(MESSAGE_ID_t event, uint32_t timeoutTicks)
{
    return (BlockOnEvent(ManagedNVTask->BlockingHandle, event, timeoutTicks));
}

void NVTask_Init(void* p)
{
    NV_Init();
    ManagedNVTask = GetManagedTaskPointer("NVTask");
}

void NVTask_DeInit()
{
    //NOP
}

void NVTask (void* pvParamaters)
{
    debug_assert(ManagedNVTask != NULL);

    for (;;)
    {
        TaskManagerPollQueue(ManagedNVTask);
    }
}

////////////////////////////////////
//Functions to handle each message//
////////////////////////////////////


/////////////////////////////////
//Function to dispatch messages//
/////////////////////////////////
void NVTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        default:
            LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "%s: unhandled 0x%0X", __func__, msg->msgID);
    }
}

