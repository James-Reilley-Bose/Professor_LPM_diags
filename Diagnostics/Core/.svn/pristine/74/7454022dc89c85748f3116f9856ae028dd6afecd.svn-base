/*
  File  : IpcStatus.h
  Title :
  Author  : dr1005920
  Created : 10/7/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions/types to handle and send IPC health and status messages
===============================================================================
*/

#ifndef _IPC_HEALTH_AND_STATUS_H_
#define _IPC_HEALTH_AND_STATUS_H_

#include "IpcProtocolLpm.h"
#define LPM_IMAGE_TYPE LPM_APP

void IpcStatus_HandleRequestHealthAndStatus(IpcPacket_t* packet);
void IpcStatus_SendHealthAndStatusRequest(Ipc_Device_t dest, StatusRequest_t dev);
void IpcStatus_HandleDSPHealthAndStatus(IpcPacket_t* packet);
void IpcStatus_HandleSTHealthAndStatus(IpcPacket_t* packet);
void IpcStatus_SendLPMHealthAndStatus(Ipc_Device_t dest);
void IpcStatus_HandleRemoteHealthAndStatus(IpcPacket_t* packet);
void IpcStatus_HandleAsocHealthAndStatus(IpcPacket_t* packet);
void IpcStatus_HandleF0HealthAndStatus(IpcPacket_t* packet);

#endif /* _IPC_HEALTH_AND_STATUS_H_ */
