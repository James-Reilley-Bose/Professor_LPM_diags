#include "project.h"
#include "TaskDefs.h"

void CreateAllTasks(void)
{
    for (int i = 0; i < NUM_MANAGED_TASKS; i++)
    {
        TaskManagerCreateTask(&ProductTasks[i]);
    }
}

ManagedTask* GetManagedTaskPointer(char* taskName)
{
    for (int i = 0; i < NUM_MANAGED_TASKS; i++)
    {
        if (strncasecmp(ProductTasks[i].Name, taskName, strlen(ProductTasks[i].Name)) == 0)
        {
            return &ProductTasks[i];
        }
    }
    return NULL;
}
