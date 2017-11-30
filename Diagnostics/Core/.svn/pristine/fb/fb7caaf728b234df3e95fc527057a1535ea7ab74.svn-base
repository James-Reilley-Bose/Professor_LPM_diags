//
// SpiIpcTask.h
//

#ifndef SPI_IPC_TASK_H
#define SPI_IPC_TASK_H

#include "TaskManager.h"

#define SPI_IPC_TASK_NAME "SpiIpcTask"

#define SpiIpcPostMsg(msg, p1, p2) \
        QueueManagerPostCallbackMsg(&SpiIpcTaskHandle->Queue, \
                                    msg, \
                                    (uint32_t []){p1, p2}, \
                                    NOP_CALLBACK, \
                                    NO_BLOCK_TIME);

extern ManagedTask* SpiIpcTaskHandle;

void SpiIpcTask_Init(void* p);
void SpiIpcTask(void* pvParameters);
void SpiIpcTask_HandleMessage(GENERIC_MSG_t* msg);
SemaphoreHandle_t SpiIpcTask_GetCtsSemHandle(void);

#endif // SPI_IPC_TASK_H
