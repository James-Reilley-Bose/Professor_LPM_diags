//
// SpiSlaveTestTask.h
//

#ifndef SPI_SLAVE_TEST_TASK_H
#define SPI_SLAVE_TEST_TASK_H

#include "TaskManager.h"

#define SpiSlaveTestPostMsg(msg, p1, p2) \
        QueueManagerPostCallbackMsg(&SpiSlaveTestTask->Queue, \
                                    msg, \
                                    (uint32_t []){p1, p2}, \
                                    NOP_CALLBACK, \
                                    NO_BLOCK_TIME);

#define SpiSlaveTestPostMsgCB(msg, p1, p2, cb) \
        QueueManagerPostCallbackMsg(&SpiSlaveTestTask->Queue, \
                                    msg, \
                                    (uint32_t []){p1, p2}, \
                                    cb, \
                                    NO_BLOCK_TIME);

extern ManagedTask* SpiSlaveTestTask;

void SpiSlaveTask_Init(void* p);
void SpiSlaveTask(void* pvParameters);
void SpiSlaveTask_HandleMessage(GENERIC_MSG_t* msg);
SemaphoreHandle_t SpiSlaveTask_GetCtsSemHandle(void);

#endif // SPI_SLAVE_TEST_TASK_H
