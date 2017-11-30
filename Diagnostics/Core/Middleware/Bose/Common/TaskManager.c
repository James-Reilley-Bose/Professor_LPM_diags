#include "project.h"
#include "TaskManager.h"

/*
 * @func  TaskManagerCreateTask
 * @brief Creates a task, maybe q and inits all modules running in said task
 *
 * @param ManagedTask *task - task to create
 *
 * @return - void
 */
void TaskManagerCreateTask(ManagedTask* task)
{
    xTaskCreate(task->RunFunction, task->Name, task->StackSize, task->Params[0], task->Priority, &(task->TaskHandle));
    debug_assert(task->TaskHandle);

    if (task->Queue.qLength > 0)
    {
        ManagedQ* queue = QueueManagerInitQueue(task->Name, task->Queue.qLength, task->Queue.qMsgLength);
        debug_assert(queue);
        task->Queue = *queue;
    }
    else
    {
        task->Queue.qHandle = NULL;
    }

    for (uint8_t i = 0; i < task->NumberOfModules; i++)
    {
        if (task->TaskModule[i].InitFunction != NULL)
        {
            (task->TaskModule[i].InitFunction)(task->Params[i]);
        }
    }
    task->BlockingHandle = RegisterForEventBlocking();

    task->RunTime = 0;
}

/*
 * @func  TaskManagerPollQueue
 * @brief Checks q to see if it has pending messages to reduce common code
 *
 * @param task - Pointer to managed task
 *
 * @return - void
 */
void TaskManagerPollQueue(ManagedTask* task)
{
    xQueueHandle queueToPoll = task->Queue.qHandle;
    GENERIC_MSG_t messageToPopulate;
    if (xQueueReceive (queueToPoll, &messageToPopulate, portMAX_DELAY))
    {
        // +1 because we just popped a message from the queue
        unsigned portBASE_TYPE waitingCount = uxQueueMessagesWaiting (queueToPoll) + 1;

        if (waitingCount > task->Queue.qHighWatermark)
        {
            task->Queue.qHighWatermark = waitingCount;
        }

        for (int i = 0; i  < task->NumberOfModules; i++)
        {
            (void)(task->TaskModule[i].HandleMessageFunction)(&messageToPopulate);
        }
        if (messageToPopulate.CallbackFunction != NOP_CALLBACK)
        {
            (void)(messageToPopulate.CallbackFunction)(messageToPopulate.params, task->Queue.numParams);
        }
    }
}
