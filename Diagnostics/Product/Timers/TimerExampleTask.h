//
// TimerExampleTask.h
//

#ifndef TIMER_EXAMPLE_TASK_H
#define TIMER_EXAMPLE_TASK_H

#define TIMER_EXAMPLE_TASK_NAME "MyTimerTask"

extern ManagedTask* TimerExampleTaskHandle;

#define TimerExampleTaskPostMsg(_theMsg, _p1, _p2) \
            QueueManagerPostCallbackMsg(&KeyTaskHandle->Queue, \
                                        _theMsg, \
                                        (uint32_t []){_p1, _p2}, \
                                        NOP_CALLBACK, \
                                        NO_BLOCK_TIME);

void TimerExampleTask_Init(void* p);
void TimerExampleTask(void* pvParameters);
void TimerExampleTask_HandleMessage(GENERIC_MSG_t* msg);

#endif // TIMER_EXAMPLE_TASK_H
