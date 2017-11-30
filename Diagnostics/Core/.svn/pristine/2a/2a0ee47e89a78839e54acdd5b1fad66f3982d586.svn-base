#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include "project.h"
#include "FreeRTOSConfig.h"
#include "EventDefinitions.h"

#define QUEUEMANAGER_ERROR 0xFF
#define QUEUEMANAGER_POST_SUCCESS 0x00

//This should probabaly live in RTOS config
#define configQUEUE_MESSAGE_MAX_PARAMS 5

#define DEFAULT_NUM_PARAMS      2
#define NO_PARAM_MESSAGE_SIZE   (sizeof(GENERIC_MSG_t) - sizeof(((GENERIC_MSG_t*)0)->params))
#define MESSAGE_SIZE(numParams) (NO_PARAM_MESSAGE_SIZE + sizeof(uint32_t) * (numParams))
#define NOP_CALLBACK            NULL
#define NO_BLOCK_TIME           0

// Message definition
typedef struct
{
    MESSAGE_ID_t msgID;
    void(*CallbackFunction)(uint32_t*, uint8_t);
    uint32_t params[configQUEUE_MESSAGE_MAX_PARAMS];
} GENERIC_MSG_t;

//Struct to store all queue info
typedef struct
{
    xQueueHandle qHandle;
    char* qName;
    unsigned portBASE_TYPE qLength;
    unsigned portBASE_TYPE qMsgLength;
    unsigned portBASE_TYPE qHighWatermark;
    uint8_t numParams;
} ManagedQ;

extern ManagedQ qListOfManagedQueues[configQUEUE_REGISTRY_SIZE];
extern uint8_t numberOfRegisteredQueues;

ManagedQ* QueueManagerInitQueue(char* pcQueueName, unsigned portBASE_TYPE uxQueueLength, unsigned portBASE_TYPE uxItemSize);
signed portBASE_TYPE QueueManagerPostCallbackMsg (ManagedQ* Q, MESSAGE_ID_t msgID, uint32_t params[], void (*callbackFunc)(uint32_t*, uint8_t), uint32_t timeoutMs);
ManagedQ* GetQueueByName(char* pcQueueName);
int8_t GetQueueIndexFromHandle(xQueueHandle handle);
char* GetQueueNameFromHandle(xQueueHandle handle);
uint32_t GetQueueSpacesAvailable(ManagedQ* Q);
uint32_t GetQueueMessagesWaiting(ManagedQ* Q);

#endif /* QUEUEMANAGER_H */
