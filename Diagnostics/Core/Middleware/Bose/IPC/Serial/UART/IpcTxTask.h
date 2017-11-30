/*
  File  : IpcTxTask.h
  Title :
  Author  : Dillon Johnson
  Created : 04/10/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Generic task to handle transmitting IPC.
===============================================================================
*/

#ifndef _IPC_TX_TASK_H_
#define _IPC_TX_TASK_H_

#include "IpcProtocolLpm.h"
#include "IpcTx.h"
#include "QueueManager.h"
#include "TaskManager.h"

/* Macros */
#define IPC_TX_NUM_PARAMS 3
#define IPC_TX_POST_BLOCK_MS    100
#define IpcTxPostMsg(_theMsg, destination, packet, length, callback) QueueManagerPostCallbackMsg( \
  &ManagedIpcTxTasks[destination]->Queue, \
  _theMsg, \
  (uint32_t[]){(uint32_t)packet, destination, length}, \
  callback, \
  IPC_TX_POST_BLOCK_MS);

/* Functions */
void IpcTxTask_Init (void* p);
void IpcTxTask(void* pvParameters);
void IpcTxTask_HandleMessage(GENERIC_MSG_t* msg);
void IpcTxTask_StopHammer(uint32_t dest);

#endif // _IPC_TX_TASK_H_
