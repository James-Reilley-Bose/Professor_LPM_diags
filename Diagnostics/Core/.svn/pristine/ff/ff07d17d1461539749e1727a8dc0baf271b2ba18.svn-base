//
// KeyTask.h
//

#ifndef KEY_TASK_H
#define KEY_TASK_H

#include "KeyData.h"

extern ManagedTask* KeyTaskHandle;

#define KeyTaskPostMsg(_theMsg, _p1, _p2) \
            QueueManagerPostCallbackMsg(&KeyTaskHandle->Queue, \
                                        _theMsg, \
                                        (uint32_t []){_p1, _p2}, \
                                        NOP_CALLBACK, \
                                        NO_BLOCK_TIME);

#define KEY_TASK_NAME "KeyTask"

void KeyTask_Init(void* p);
void KeyTask(void* pvParameters);
void KeyTask_HandleMessage(GENERIC_MSG_t* msg);
void Product_HandleKey(KEY_DATA_t key);

#endif // KEY_TASK_H
