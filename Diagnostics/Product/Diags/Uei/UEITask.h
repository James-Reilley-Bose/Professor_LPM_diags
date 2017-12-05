/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UEITASK_H
#define __UEITASK_H

#include "project.h"
#include "TaskManager.h"
#include "QueueManager.h"
#include "unifySourceList.h"
#include "UEIServices.h"
#include "unifySourceList.h"

extern ManagedTask* ManagedUEIBlasterTask;

typedef struct {
  uint8_t deviceStr[UEI_KEY_RECORD_SIZE];
  uint16_t codeID;
  uint8_t *dataPtr;
  uint16_t dataSize;
  BOOL quick;
}uei_ueiMsg_t;

//#define UEIBlasterPostMsg(_theMsg, _p1) QueueManagerPostCallbackMsg(&ManagedUEIBlasterTask->Queue, _theMsg, (uint32_t []){_p1},NOP_CALLBACK,NO_BLOCK_TIME);
#define UEIBlasterPostMsg(_theMsg, cb, ...) QueueManagerPostCallbackMsg(&ManagedUEIBlasterTask->Queue, _theMsg, (uint32_t[]){__VA_ARGS__}, cb, NO_BLOCK_TIME);
#define UEIBlasterPostMsg2(_theMsg, _p1, _p2, _cb) QueueManagerPostCallbackMsg(&ManagedUEIBlasterTask->Queue, _theMsg, (uint32_t []){_p1, _p2},_cb,NO_BLOCK_TIME);

#define UEI_INVALID_MORE_BUTTONS (0xFFFFFFFF)

extern void UEIBlasterModule_Init(void* p);
extern void UEIBlasterModule_PowerInit(void);
extern void Init_UEI_UART(void);
extern void UEIBlasterModule_DeInit(void);
extern void UEIBlasterModule_ShutDown(void);
extern void UEIBlasterModule (void *pvParamaters);
extern void UEIBlasterModule_HandleMessage(GENERIC_MSG_t* m);
extern void UEIStatusCallback(uint32_t* params, uint8_t numParams);
BOOL UEI_ProgrammedForSource(KEY_DATA_t key, SOURCE_ID src);
BOOL UEI_DoesCodsetHaveKey(KEY_VALUE key, SOURCE_ID src);
void UEI_ConsumeKey(KEY_DATA_t key, SOURCE_ID src);
void UEI_HandleDelCodeset(IpcPacket_t* packet);

void UEI_setUpdateMoreButtons(BOOL update);
BOOL UEI_getUpdateMoreButtons(void);

extern BOOL TakeUEISemaphore (uint32_t timeOutTicks);
extern BOOL GiveUEISemaphore (void);
void UEIBlasterModule_DeInit(void);
void UEIBlasterModule_PowerInit(void);

#endif /* __UEITASK_H */
