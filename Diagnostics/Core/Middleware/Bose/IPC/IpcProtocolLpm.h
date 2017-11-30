/*
  File: IpcProtocolLpm.h
  Author: Dillon Johnson
  Created: 12/21/2015
  Copyright: (C) 2015 Bose Corporation, Framingham, MA

  Description:
    Data structures and enumerations for A4V IPC on the LPM.
*****************************************************************************
*/

#ifndef IPC_PROTOCOL_LPM_H
#define IPC_PROTOCOL_LPM_H

#include "RivieraLPM_IpcProtocol.h"

typedef struct
{
    Ipc_Device_t dest;
    IpcOpcodes_t op;
    uint8_t* params;
    void* data;
    uint32_t length;
    uint32_t sequence;
    uint8_t connectionID;
} IpcSendPacket_t;

extern char* const IpcTaskNames[];
#define IPC_DEVICE_INVALID_NAME ""
#define IPC_DEVICE_DSP_NAME "DSPModel"
#define IPC_DEVICE_LPM_NAME "ASOCModel"
#define IPC_DEVICE_AP_NAME "IPCAPTask"
#define IPC_DEVICE_REMOTE_NAME "IPCREMOTETask"
#define IPC_DEVICE_F0_NAME "IPCF0Task"
#define IPC_DEVICE_SOUNDTOUCH_NAME "STDeviceModel"

#define REV_WORD_BYTES(x) __REV(x)
#define REV_HALF_WORD_BYTES(x) __REV16(x)

void IpcBuildPacket(IpcPacket_t* pkt, const IpcSendPacket_t* pData);
uint32_t IpcCalculateXor (uint8_t* data, uint32_t length);
void IpcLogPacket(IpcPacket_t* packet);
BOOL IpcIsValidPacket(IpcPacket_t* header);
void ReverseWords(void* words, uint32_t numToFlip);
uint16_t IpcGetUint16(uint8_t* data);

BOOL IsBulkControlOpcode(uint8_t opcode);
BOOL IsUpdateOpcode(uint8_t opcode);
BOOL IsPassthroughOpcode(uint8_t opcode);
BOOL IsEchoOpcode(uint8_t opcode);
void IpcSetRawOpcodesToLog(uint8_t* opcodes);
BOOL IpcIsValidDestination(Ipc_Device_t destination);

#define MAX_RAWOPCODES 10

#endif // IPC_PROTOCOL_LPM_H
