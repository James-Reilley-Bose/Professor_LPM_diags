/*
  File  : IpcStatus.c
  Title :
  Author  : dr1005920
  Created : 10/7/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions/types to handle and send IPC health and status messages
===============================================================================
*/
#include "project.h"
#include "IpcStatus.h"
#include "IpcRouterTask.h"
#include "versionlib.h"
#include "IpcVariant.h"

void IpcStatus_SendHealthAndStatusRequest(Ipc_Device_t dest, StatusRequest_t dev)
{
    uint32_t payload = REV_WORD_BYTES(dev);
    IPCRouter_Send(dest, IPC_STAT_REQ, NULL, &payload, sizeof(payload));
}

void IpcStatus_SendLPMHealthAndStatus(Ipc_Device_t dest)
{
    IpcPacket_t payload;
    IpcVariant_GenerateLPMHSPayload(&payload);

    IPCRouter_Send(dest, IPC_LPM_H_S, NULL, &payload, sizeof(payload));
}
