#ifndef TAPLISTENER_H
#define TAPLISTENER_H
#include "TaskManager.h"
#include "QueueManager.h"
#include "stringbuffer.h"

extern ManagedTask* ManagedTapListenerTask;

// Task Module Management Interface
void TapListenerModuleCreate();
void TapListenerModule_Init(void* p);
void TapListener_PrintPrompt(uint32_t* params, uint8_t numParams);
void TapListenerModule_HandleMessage(GENERIC_MSG_t* msg);

signed portBASE_TYPE TapListenerPostOverrunError ();
void TapListener_PrintString(const char* stringToPrint);
void TapListener_PrintBuffer(StringBufferTable_t* table);
void TapListenerModule (void* pvParamaters);

#endif //TAPLISTENER_H
