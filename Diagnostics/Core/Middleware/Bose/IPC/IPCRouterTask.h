/*
  File  : IPCRouterTask.h
  Title :
  Author  : jd71069
  Created : 03/27/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Header for IPC Router task.

===============================================================================
*/
#ifndef IPC_ROUTER_H
#define IPC_ROUTER_H

#include "TaskDefs.h"
#include "IpcProtocolLpm.h"
#include "IpcRx.h"
#include "IpcInterface.h"

extern ManagedTask* ipcRouterTaskHandle;
extern IpcInterface_t* IpcInterfaceTable[];
extern ManagedTask* ManagedIpcTxTasks[];

#define TX_GET_PACKET_WAIT 100
#define IPCRouterPostMsg(_theMsg,...) QueueManagerPostCallbackMsg(&ipcRouterTaskHandle->Queue, _theMsg, (uint32_t []){__VA_ARGS__}, NOP_CALLBACK, NO_BLOCK_TIME);

void IPCRouter_TaskInit (void* p);
IpcPacket_t* IPCRouter_GetNextTxBuffer(void);
IpcPacket_t* IPCRouter_GetNextRxBuffer(void);
void IPCRouter_ReturnRxBuffer(IpcPacket_t* packet);
void IPCRouter_ReturnRxBufferCallback(uint32_t* packet, uint8_t unused);
void IPCRouter_ReturnTxBuffer(IpcPacket_t* packet);
void IPCRouter_ReturnTxBufferCallback(uint32_t* packet, uint8_t unused);
uint8_t IPCRouter_BuffersRxRemaining(void);
uint8_t IPCRouter_BuffersTxRemaining(void);

void IPCRouter_HandleMessage(GENERIC_MSG_t* msg);
void IPCRouterTask (void* pvParamaters);
void IPCRouter_RXDoneCallback(void* pThis);
BOOL IPCRouter_Send(uint8_t dest, uint8_t op, uint8_t* params, void* data, uint8_t length);
BOOL IPCRouter_SendResponse(const IpcSendPacket_t* response);

void IPCRouter_CancelTransfer(Ipc_Device_t dest);
void IPCRouter_RegisterDevice(IpcInterface_t* device, ManagedTask* task);
const IpcInterface_t* IpcRouter_GetInterface(Ipc_Device_t device);

#endif //IPC_ROUTER_H 
