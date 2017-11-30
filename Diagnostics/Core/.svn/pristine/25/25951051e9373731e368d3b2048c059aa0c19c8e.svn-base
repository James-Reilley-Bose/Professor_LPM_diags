/*
  File  : IpcKeys.h
  Title :
  Author  : dr1005920
  Created : 10/7/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions/types to handle and send IPC key messages
===============================================================================
*/

#ifndef _IPC_KEYS_H_
#define _IPC_KEYS_H_

#include "IpcProtocolLpm.h"
#include "KeyData.h"
#include "boselinkmessages.h"

typedef struct
{
    uint32_t time            : 32;
    uint32_t producer        : 32; // KEY_PRODUCER
    uint32_t room            : 32; // ROOM_NUMBER
    uint32_t keyValue        : 32; // KEY_VALUE
    uint32_t state           : 32; // LPM_KEY_STATE
} IpcKeyPayload;

void IpcKeys_Initialize(void);
void IpcKeys_SendKey(Ipc_Device_t dest, KEY_DATA_t key);
void IpcKeys_SendKeyInfo(Ipc_Device_t dest, KEY_DATA_t key);

void IpcKeys_KeyHookHandleRequest(IpcPacket_t* packet);
void IpcKeys_HandleKey(IpcPacket_t* packet);

#endif /* _IPC_KEYS_H_ */
