#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "project.h"
#include "EventBlocking.h"

// Takes up 8 bytes * MAX_MODULES_PER_TASK * NUM_MANAGED_TASKS
#define MAX_MODULES_PER_TASK 2

typedef struct
{
    void (*InitFunction)(void* p);
    void (*HandleMessageFunction)(GENERIC_MSG_t*);
} ManagedModule;

typedef struct
{
    char* Name;
    xTaskHandle TaskHandle;
    ManagedQ Queue;
    EVENT_BLOCKING_HANDLE BlockingHandle;
    uint8_t NumberOfModules;
    ManagedModule TaskModule[MAX_MODULES_PER_TASK]; //Make this an array if a task-module model is desired
    void (*RunFunction)();
    UBaseType_t Priority;
    unsigned short StackSize;
    void* Params[MAX_MODULES_PER_TASK];
    unsigned portBASE_TYPE RunTime;
} ManagedTask;

void TaskManagerCreateTask(ManagedTask* task);
void TaskManagerPollQueue(ManagedTask* task);

#endif /* TASKMANAGER_H */
