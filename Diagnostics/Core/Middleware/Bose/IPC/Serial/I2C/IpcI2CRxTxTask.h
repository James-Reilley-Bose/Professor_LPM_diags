#ifndef IPCI2CRXTXTASK_H
#define IPCI2CRXTXTASK_H

#include "IpcInterface.h"

void IpcI2CRxTxTask_Init(void* p);
void IpcI2CRxTxTask(void* pvParameters);
void IpcI2CRxTxTask_HandleMessage(GENERIC_MSG_t* msg);
IpcInterface_t* IpcI2CRxTxTask_GetIpcInterface(void);

extern void IpcI2CTask_ConfigureEXTI();
extern ManagedTask* ManagedIpcTxTasks[];

#define IpcI2CRxTxPostMsg(_theMsg, destination, callback) QueueManagerPostCallbackMsg( \
  &ManagedIpcTxTasks[destination]->Queue, \
  _theMsg, \
  NULL, \
  callback, \
  NO_BLOCK_TIME);

#endif
