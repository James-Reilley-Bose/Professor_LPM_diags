/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UEITASK_H
#define __UEITASK_H

#include "project.h"
#include "TaskManager.h"
#include "QueueManager.h"
#include "UEIServices.h"


extern ManagedTask* ManagedUEIBlasterTask;


#define UEIBlasterPostMsg(_theMsg, cb, ...) QueueManagerPostCallbackMsg(&ManagedUEIBlasterTask->Queue, _theMsg, (uint32_t[]){__VA_ARGS__}, cb, NO_BLOCK_TIME);


extern void UEIBlasterModule_Init(void* p);
extern void UEIBlasterModule_PowerInit(void);
extern void Init_UEI_UART(void);
extern void UEIBlasterModule_DeInit(void);
extern void UEIBlasterModule (void *pvParamaters);
extern void UEIBlasterModule_HandleMessage(GENERIC_MSG_t* m);



void UEIBlasterModule_DeInit(void);
void UEIBlasterModule_PowerInit(void);

#endif /* __UEITASK_H */
