/*
  File: A4V_IpcProtocol.c
  Author: Dillon Johnson
  Created: 12/21/2015
  Copyright: (C) 2015 Bose Corporation, Framingham, MA

  Description:
    Data structures and enumerations for A4V IPC on the LPM.
*****************************************************************************
*/

#include "project.h"
#include "driverLibCommon.h"
#include "IpcProtocolLpm.h"
#include "IPCRouterTask.h"

/* Logs */
SCRIBE_DECL(ipc_raw);
SCRIBE_DECL(ipc_bulk);
SCRIBE_DECL(ipc_update);
SCRIBE_DECL(ipc_pt);

BOOL AlwaysLogOpcode(uint8_t opcode);

static uint8_t rawOpcodesToLog[MAX_RAWOPCODES] = {0};

char* const IpcTaskNames[IPC_NUM_DEVICES] =
{
    IPC_DEVICE_INVALID_NAME,
    IPC_DEVICE_DSP_NAME,
    IPC_DEVICE_LPM_NAME,
    IPC_DEVICE_AP_NAME,
    IPC_DEVICE_REMOTE_NAME,
    IPC_DEVICE_F0_NAME,
    IPC_DEVICE_SOUNDTOUCH_NAME,
};


/*
 * @func IpcBuildPacket
 *
 * @brief Uses the params to create a valid IPC header in the buffer space.
 *
 * @param IpcPacket_t* pkt - The packet to be filled
 * @param IpcSendPacket_t pData - The data used to create the packet
 *
 * @return void
 */
void IpcBuildPacket(IpcPacket_t* pkt, const IpcSendPacket_t* pData)
{
    pkt->s.sender = IPC_DEVICE_LPM;
    pkt->s.destination = pData->dest;
    pkt->s.opcode = pData->op;
    if (pData->params != NULL)
    {
        pkt->s.params[0] = pData->params[0];
        pkt->s.params[1] = pData->params[1];
    }
    else
    {
        pkt->s.params[0] = 0x00;
        pkt->s.params[1] = 0x00;
    }
#ifndef USE_OLD_128_BYTE_STRUCTURE
    pkt->s.sequence = pData->sequence;
    pkt->s.connectionID = pData->connectionID;
#endif

    memcpy(pkt->s.data.b, pData->data, pData->length);
    memset(&pkt->s.data.b[pData->length], 0, IPC_NUM_DATA_BYTES - pData->length);
    pkt->s.checkXOR = IpcCalculateXor(pkt->b, sizeof(IpcPacket_t) - sizeof(uint32_t));
}

/*
 * @func IpcCalculateXor
 *
 * @brief Calculates a word-wise xor.
 *
 * @param IpcPacket_t* hdr - The filled in header that needs a checksum.
 *
 * @return 32 bit xor value
 */
uint32_t IpcCalculateXor(uint8_t* data, uint32_t length)
{
    uint32_t cs = 0;
    uint16_t i = 0;

    /* Get the all the bytes that are a multiple of 4 */
    for (i = 0; i < length / sizeof(uint32_t); i++)
    {
        cs ^= ((uint32_t*)data)[i];
    }

    /* Get the last few bytes */
    uint32_t lastWord = 0;
    for (i = length - (length % sizeof(uint32_t)); i < length; i++)
    {
        lastWord |= data[i] << (i % sizeof(uint32_t) * 8);
    }
    cs ^= lastWord;

    return cs;
}

/*
===============================================================================
@func IpcLogPacket
@desc Logs data most often consumed by Automation - careful editing!!
===============================================================================
*/
void IpcLogPacket(IpcPacket_t* packet)
{
    uint32_t* data = packet->w;
    for (uint8_t i = 0; i < 4; i++)
    {
        // If this packet is one we always want to log, log it at NORMAL otherwise log it at INSANE
        // Well, this is ugly dup code, but I tried using a variable for the loglevel and iar barfed
        if (AlwaysLogOpcode(packet->s.opcode))
        {
            LOG(ipc_raw, ROTTEN_LOGLEVEL_NORMAL, "[%2d - %2d]: %08X %08X %08X %08X %08X %08X %08X %08X", (i * 8 + 0), (i * 8 + 7),
                REV_WORD_BYTES(data[i * 8 + 0]),  REV_WORD_BYTES(data[i * 8 + 1]),  REV_WORD_BYTES(data[i * 8 + 2]),  REV_WORD_BYTES(data[i * 8 + 3]),
                REV_WORD_BYTES(data[i * 8 + 4]),  REV_WORD_BYTES(data[i * 8 + 5]),  REV_WORD_BYTES(data[i * 8 + 6]),  REV_WORD_BYTES(data[i * 8 + 7]));
        }
        else
        {
            LOG(ipc_raw, ROTTEN_LOGLEVEL_INSANE, "[%2d - %2d]: %08X %08X %08X %08X %08X %08X %08X %08X", (i * 8 + 0), (i * 8 + 7),
                REV_WORD_BYTES(data[i * 8 + 0]),  REV_WORD_BYTES(data[i * 8 + 1]),  REV_WORD_BYTES(data[i * 8 + 2]),  REV_WORD_BYTES(data[i * 8 + 3]),
                REV_WORD_BYTES(data[i * 8 + 4]),  REV_WORD_BYTES(data[i * 8 + 5]),  REV_WORD_BYTES(data[i * 8 + 6]),  REV_WORD_BYTES(data[i * 8 + 7]));
        }
    }

    if (IsBulkControlOpcode(packet->s.opcode)) // the most convenient place to print this...
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_VERBOSE, "Tx: %08X %08X %08X %08X %08X", REV_WORD_BYTES(packet->w[0]), REV_WORD_BYTES(packet->w[1]),
            REV_WORD_BYTES(packet->w[2]), REV_WORD_BYTES(packet->w[3]), REV_WORD_BYTES(packet->w[4]));
    }
    else if (IsUpdateOpcode(packet->s.opcode))
    {
        LOG(ipc_update, ROTTEN_LOGLEVEL_VERBOSE, "UPDATE: %08X %08X %08X %08X %08X", REV_WORD_BYTES(packet->w[0]), REV_WORD_BYTES(packet->w[1]),
            REV_WORD_BYTES(packet->w[2]), REV_WORD_BYTES(packet->w[3]), REV_WORD_BYTES(packet->w[4]));
    }
    else if (IsPassthroughOpcode(packet->s.opcode))
    {
        LOG(ipc_pt, ROTTEN_LOGLEVEL_VERBOSE, ": %08X %08X %08X %08X %08X", REV_WORD_BYTES(packet->w[0]), REV_WORD_BYTES(packet->w[1]),
            REV_WORD_BYTES(packet->w[2]), REV_WORD_BYTES(packet->w[3]), REV_WORD_BYTES(packet->w[4]));
    }
    else if (IsEchoOpcode(packet->s.opcode))
    {
        /* logging needed for automation - please do not change */
        LOG(ipc_raw, ROTTEN_LOGLEVEL_VERBOSE, "ECHO: %08X %08X %08X %08X %08X", REV_WORD_BYTES(packet->w[0]), REV_WORD_BYTES(packet->w[1]),
            REV_WORD_BYTES(packet->w[2]), REV_WORD_BYTES(packet->w[3]), REV_WORD_BYTES(packet->w[4]));
    }
}

/*
===============================================================================
@func IPCIsValidPacket
@desc Validates header using checksum.
===============================================================================
*/
BOOL IpcIsValidPacket(IpcPacket_t* packet)
{
    return (packet->s.checkXOR == IpcCalculateXor(packet->b, sizeof(IpcPacket_t) - sizeof(uint32_t)));
}

/*
 * @func ReverseWords
 *
 * @brief Flip the bytes for numToFlip words
 *
 * @param IpcPacket_t* hdr - The filled in header that needs a checksum.
 *
 * @return n/a
 */
void ReverseWords(void* words, uint32_t numToFlip)
{
    uint32_t* w = (uint32_t*) words;
    for (uint32_t i = 0; i < numToFlip; i++)
    {
        w[i] = REV_WORD_BYTES(w[i]);
    }
}

/*
 * @func IpcGetUint16
 *
 * @brief Retrieve a 16bit value from an IPC message.
 *
 * @param uint8_t* data - pointer to the MSB
 *
 * @return the value
 */
uint16_t IpcGetUint16(uint8_t* data)
{
    uint16 value = (data[0] << 8) | data[1];

    return value;
}

/*
 * @func IsBulkControlOpcode
 *
 * @brief Determines if the opcode is related to Bulk Transfer.
 *
 * @param uint8_t opcode - the opcode to check
 *
 * @return BOOL - true if a Bulk Transfer opcode
 */
BOOL IsBulkControlOpcode(uint8_t opcode)
{
    return ((opcode >= IPC_BULK_NOTIFY) && (opcode <= IPC_BULK_CLOSED) && (opcode != IPC_BULK_DATA));
}

/*
 * @func IsUpdateOpcode
 *
 * @brief Determines if the opcode is related to update.
 *
 * @param uint8_t opcode - the opcode to check
 *
 * @return BOOL - true if an update opcode
 */
BOOL IsUpdateOpcode(uint8_t opcode)
{
    return ((opcode >= IPC_UPDATE_AVAILABLE) && (opcode <= IPC_UPDATE_GET_STATUS));
}


/*
 * @func IsEchoOpcode
 *
 * @brief Determines if the opcode is related to echo
 *
 * @param uint8_t opcode - the opcode to check
 *
 * @return BOOL - true if an echo opcode
 */
BOOL IsEchoOpcode(uint8_t opcode)
{
    return ((opcode == IPC_ECHO) || (opcode == IPC_ECHO_RESPONSE));
}

/*
 * @func IsPassthroughOpcode
 *
 * @brief Determines if the opcode is for passthrough.
 *
 * @param uint8_t opcode - the opcode to check
 *
 * @return BOOL - true if an update opcode
 */
BOOL IsPassthroughOpcode(uint8_t opcode)
{
    return ((opcode == IPC_PASSTHROUGH_SEND) || (opcode == IPC_PASSTHROUGH_RESPONSE));
}

/**
 * Sometimes you just want to always log a certain IPC opcode (or ten), this lets you do that
 * @param opcodes
 */
void IpcSetRawOpcodesToLog(uint8_t* opcodes)
{
    memset(rawOpcodesToLog, 0, MAX_RAWOPCODES);
    memcpy(rawOpcodesToLog, opcodes, MAX_RAWOPCODES);
}

static BOOL AlwaysLogOpcode(uint8_t opcode)
{
    for (int i = 0; i < MAX_RAWOPCODES; i++)
    {
        if (rawOpcodesToLog[i] == opcode)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL IpcIsValidDestination(Ipc_Device_t destination)
{
    return ((destination < IPC_NUM_DEVICES) &&
            (IpcInterfaceTable[destination] != NULL) &&
            (IpcInterfaceTable[destination]->type != IPC_INTERFACE_INVALID));
}
