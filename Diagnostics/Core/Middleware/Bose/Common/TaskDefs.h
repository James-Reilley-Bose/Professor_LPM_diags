#ifndef TASK_DEFS_H
#define TASK_DEFS_H

#include "TaskManager.h"
#include "QueueManager.h"

extern ManagedTask ProductTasks[];
extern const unsigned int NUM_MANAGED_TASKS;

//Below is just to neaten up the tasks array
#define NEW_MANAGED_QUEUE(_size, _msgLength) \
        {                                    \
            NULL,                            \
            NULL,                            \
            (_size),                         \
            MESSAGE_SIZE(_msgLength),        \
            NULL,                            \
            _msgLength,                      \
        }

void CreateAllTasks(void);
ManagedTask* GetManagedTaskPointer(char* taskName);
#endif
