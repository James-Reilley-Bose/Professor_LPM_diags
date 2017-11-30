/*
 * KeyHandlerTask.h - Serializes/offloads key events from various key producers.
 *
 * Author: Dillon Johnson
 */

#ifndef _KEY_HANDLER_TASK_H_
#define _KEY_HANDLER_TASK_H_

#include "TaskManager.h"
#include "QueueManager.h"

extern ManagedTask* ManagedKeyHandlerTask;

#define KeyHandlerPostMsg(_theMsg, _p1, _p2) QueueManagerPostCallbackMsg(&ManagedKeyHandlerTask->Queue, _theMsg, (uint32_t []){_p1,_p2},NOP_CALLBACK, NO_BLOCK_TIME);
#define KeyHandlerPostMsgWithCallback(_theMsg, _p1, _p2, cb) QueueManagerPostCallbackMsg(&ManagedKeyHandlerTask->Queue, _theMsg, (uint32_t []){_p1,_p2},cb, NO_BLOCK_TIME);

void KeyHandlerTask_Init(void* p);
void KeyHandlerTask(void* pvParameters);
void KeyHandlerTask_HandleMessage(GENERIC_MSG_t* msg);

#endif // _KEY_HANDLER_TASK_H_

