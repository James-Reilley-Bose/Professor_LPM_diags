/*
  File  : ipc_rx.h
  Title :
  Author  : jd71069
  Created : 03/27/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Header for IPC Receive over DMA

===============================================================================
*/
#ifndef IPC_RX_H
#define IPC_RX_H

#include "project.h"
#include "DmaManager.h"
#include "IpcProtocolLpm.h"
#include "IpcInterface.h"

void IPC_RX_StartService(IpcInterface_t* pThis);
BOOL IPC_RX_RestartService(IpcInterface_t* pThis);
void IPC_RX_StopService(IpcInterface_t* pThis);
void IPC_RX_MessageCallBack(uint32_t* params, uint8_t numParams);
void IPC_RX_MonitorTask(void* p);
void IPC_RX_Monitor_TaskInit(void* p);

#endif //IPC_RX_H
