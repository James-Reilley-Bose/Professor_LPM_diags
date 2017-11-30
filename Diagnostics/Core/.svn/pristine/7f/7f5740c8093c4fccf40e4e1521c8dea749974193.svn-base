/*!
* @file        NVTask.h
* @version     $Id:
* @author      dr1005920
* @brief
*
* Copyright 2010 Bose Corporation.
*/

#ifndef INCLUDE_NV_TASK_H
#define INCLUDE_NV_TASK_H

#include "platform_settings.h"
#include "TaskManager.h"

extern ManagedTask* ManagedNVTask;

// Task Module Management Interface
void NVTask(void* pvParamaters);
void NVTask_HandleMessage(GENERIC_MSG_t* msg);
void NVTask_Init(void* p);
void NVTask_DeInit();
void NVTask_HandleMessage(GENERIC_MSG_t* msg);


#define NVTaskPostMsg(_theMsg, _p1) QueueManagerPostCallbackMsg(&ManagedNVTask->Queue, _theMsg, (uint32_t []){_p1},NOP_CALLBACK,NO_BLOCK_TIME)
#define NVTaskPostMsg2(_theMsg, _p1, _p2) QueueManagerPostCallbackMsg(&ManagedNVTask->Queue, _theMsg, (uint32_t []){_p1, _p2},NOP_CALLBACK,NO_BLOCK_TIME)

boolean BlockNVTaskOnEvent(MESSAGE_ID_t event, uint32_t timeoutTicks);

#endif //INCLUDE_NV_TASK_H
