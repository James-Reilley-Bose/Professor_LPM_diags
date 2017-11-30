//
// RemoteTask.h
//

#ifndef REMOTE_TASK_H
#define REMOTE_TASK_H

#include "TaskDefs.h"


#define REMOTE_MESSAGE_NUM_PARAMS 2

// Provide NULL or 0 after message if no arguments
#define RemoteTaskPostMsg(msg, ...) QueueManagerPostCallbackMsg(&RemoteTaskHandle->Queue, msg, (uint32_t []){__VA_ARGS__}, NOP_CALLBACK, NO_BLOCK_TIME);
#define RemoteTaskPostCallbackMsg(msg, cb, ...) QueueManagerPostCallbackMsg(&RemoteTaskHandle->Queue, msg, (uint32_t []){__VA_ARGS__}, cb, NO_BLOCK_TIME);

#define REMOTE_TASK_NAME "RemoteTask"

extern ManagedTask* RemoteTaskHandle;

void Remote_Task_Init (void* p);
void Remote_Task_Run (void* pvParamaters);
void Remote_Task_HandleMessage(GENERIC_MSG_t* msg);

#endif //  REMOTE_TASK_H
