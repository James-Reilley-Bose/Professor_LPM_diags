#include "etap.h"
#include "EventDefinitions.h"
#include "IpcTxTask.h"
#include "IpcProtocolLpm.h"
#include "IPCRouterTask.h"
#include "IPCBulk.h"
#include "etapIpc.h"
#include "buffermanager.h"
#include <stdlib.h>
#include <string.h>
#include "IpcInterface.h"
#include "IpcStatus.h"
#include "IpcTest.h"

//helper functions
void eTapIpcSend(CommandLine_t* CommandLine);
void eTapIpcSendRaw(CommandLine_t* CommandLine);
void eTapIpcRtw(CommandLine_t* CommandLine);
void eTapIpcBulk(CommandLine_t* CommandLine);
void eTapIpcBulkAbort(CommandLine_t* CommandLine);
void eTapIpcHammer(CommandLine_t* CommandLine);
void eTapIpcBuffer(CommandLine_t* CommandLine);
void IPCBulk_SetInvalidTID(uint16_t tID);
void eTapIpcSetLogOpcodes(CommandLine_t* CommandLine);
static void eTapIpcStats(void);
static void printSpiStats(const IpcInterface_t* interface);

static const char* deviceNames[] =
{
    "invalid",
    "dsp",
    "lpm",
    "ap",
    "remote",
    "f0",
    "st",
};

/*
 * @func TAP_IpcCommand
 *
 * @brief Allows you to send IPC to a specific device
 */
TAPCommand(TAP_IpcCommand)
{
    if (CommandLine->numArgs == 0)
    {
        TAP_PrintString(IPC_HELP_TEXT);
        return;
    }
    if (strcmp("send", CommandLine->args[0]) == 0)
    {
        eTapIpcSend(CommandLine);
    }
    else if (strcmp("sendraw", CommandLine->args[0]) == 0)
    {
        eTapIpcSendRaw(CommandLine);
    }
    else if (strcmp("rtw", CommandLine->args[0]) == 0)
    {
        eTapIpcRtw(CommandLine);
    }
    else if (strcmp("bulk", CommandLine->args[0]) == 0)
    {
        eTapIpcBulk(CommandLine);
    }
    else if (strcmp("bulkabort", CommandLine->args[0]) == 0)
    {
        eTapIpcBulkAbort(CommandLine);
    }
    else if (strcmp("hammer", CommandLine->args[0]) == 0)
    {
        eTapIpcHammer(CommandLine);
    }
    else if (strcmp("buffer", CommandLine->args[0]) == 0)
    {
        eTapIpcBuffer(CommandLine);
    }
    else if (strcmp("log", CommandLine->args[0]) == 0)
    {
        eTapIpcSetLogOpcodes(CommandLine);
    }
    else if (strcmp("stats", CommandLine->args[0]) == 0)
    {
        eTapIpcStats();
    }
    else
    {
        TAP_PrintString(IPC_HELP_TEXT);
    }
}

//SEND: ipc send, destination, opcode, data...
void eTapIpcSend(CommandLine_t* CommandLine)
{
    IpcDeviceId_t dest;
    uint8_t data[IPC_NUM_DATA_BYTES];
    uint8_t opcode;
    BOOL valid;
    if (CommandLine->numArgs != 4)
    {
        TAP_PrintString(IPC_SEND_HELP_TEXT);
        return;
    }
    valid = stringToDestination(CommandLine->args[1], &dest);
    if (valid == FALSE)
    {
        TAP_PrintString("Invalid Destination");
        return;
    }
    opcode = atoi(CommandLine->args[2]);
    if (opcode == 0)
    {
        TAP_PrintString("Invalid opcode");
        return;
    }
    valid = TAP_HexStringToUInt8Array(CommandLine->args[3], data, IPC_NUM_DATA_BYTES);
    if (valid == FALSE)
    {
        TAP_PrintString("Data is invalid");
        return;
    }
    IPCRouter_Send(dest, opcode, NULL, data, sizeof(data));
}

//SENDRAW: ipc sendraw, destination, data<1,128>...
void eTapIpcSendRaw(CommandLine_t* CommandLine)
{
    uint8_t dest;
    uint16_t length;
    BOOL valid;
    if (CommandLine->numArgs != 3)
    {
        TAP_PrintString(IPC_SENDRAW_HELP_TEXT);
        return;
    }
    valid = stringToDestination(CommandLine->args[1], &dest);
    if (valid == FALSE)
    {
        TAP_PrintString("Invalid Destination");
        return;
    }
    length = strlen(CommandLine->args[2]) / 2;
    uint8_t* packet = BufferManagerGetBuffer(length);
    debug_assert(packet);
    valid = TAP_HexStringToUInt8Array(CommandLine->args[2], (uint8_t*) packet, length);
    if (valid == FALSE)
    {
        TAP_PrintString("Data is invalid");
        BufferManagerFreeBuffer(packet);
        return;
    }

    IpcLogPacket((IpcPacket_t*) packet);
    IpcTxPostMsg(IPC_TX_MESSAGE_ID_Send, dest, packet, length, BufferManagerFreeBufferCallback);
}

//RTW: ipc rtw
void eTapIpcRtw(CommandLine_t* CommandLine)
{
    BOOL isValid = FALSE;
    uint8_t dest;
    isValid = stringToDestination(CommandLine->args[1], &dest);
    if (isValid == FALSE)
    {
        TAP_PrintString("Invalid Destination");
        return;
    }
    uint32_t loopCount = TAP_DecimalArgToInt(CommandLine, 2, &isValid);
    if (isValid)
    {
        if (loopCount)
        {
            IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BeginRTWTest, dest, loopCount);
            TAP_PrintString("RTW test started.");
        }
        else
        {
            IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_EndRTWTest, dest);
            TAP_PrintString("RTW test ended.");
        }
    }
    else
    {
        TAP_PrintString(IPC_RTW_HELP_TEXT);
    }
}

//BULK: ipc bulk
#define TEST_BULK_IMAGE_LENGTH 1000000
#define NUM_BULK_PARAMETERS 4
void eTapIpcBulk(CommandLine_t* CommandLine)
{
    BOOL valid[NUM_BULK_PARAMETERS] = {0};
    uint8_t destination = 0;
    uint32_t length = 0, fileID = 0;
    char* filename = NULL;
    BOOL validXor = FALSE;
    uint32_t i = 0;

    if (CommandLine->numArgs < 6)
    {
        TAP_PrintString(IPC_BULK_HELP_TEXT);
        return;
    }

    /* Get the parameters */
    valid[0] = stringToDestination(CommandLine->args[1], &destination);
    length = TAP_DecimalArgToInt(CommandLine, 2, &valid[1]);
    fileID = TAP_HexArgToInt(CommandLine, 3, &valid[2]);
    filename = CommandLine->args[4];
    validXor = TAP_DecimalArgToInt(CommandLine, 5, &valid[3]);
    for (i = 0; i < NUM_BULK_PARAMETERS; i++)
    {
        if (!valid[i])
        {
            TAP_PrintString("Invalid param.");
            return;
        }
    }

    /* Limit to our max buffer size */
    if (length > TEST_BULK_IMAGE_LENGTH)
    {
        length = TEST_BULK_IMAGE_LENGTH;
    }
    else
    {
        length -= (length % sizeof(uint32_t)); // chop off non-word-aligned stuff
    }

    /* Send it */
    uint16_t tID = IpcTest_StartBulkTest(destination, (uint8_t*) FLASH_BASE, length, BULK_TYPE_TEST, fileID, filename, &ipcRouterTaskHandle->Queue);
    if (tID)
    {
        TAP_Printf("Transfer started. ID = 0x%04x.\r\n", tID);
        if (!validXor)
        {
            IPCBulk_SetInvalidTID(tID);
        }
    }
    else
    {
        TAP_PrintString("Transfer unable to start.\r\n");
    }
}

// BULK_ABORT: ipc bulkabort,<tID>
void eTapIpcBulkAbort(CommandLine_t* CommandLine)
{
    if (CommandLine->numArgs < 2)
    {
        TAP_PrintString(IPC_BULK_ABORT_HELP_TEXT);
        return;
    }

    BOOL valid = FALSE;
    uint32_t tID = TAP_HexArgToInt(CommandLine, 1, &valid);

    if (valid)
    {
        IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferAbort, tID);
    }
    else
    {
        TAP_PrintString("Invalid transfer ID.");
    }
}

void eTapIpcHammer(CommandLine_t* CommandLine)
{
    if (CommandLine->numArgs < 3)
    {
        TAP_PrintString(IPC_HAMMER_HELP_TEXT);
        return;
    }

    BOOL valid[2] = {FALSE, FALSE};
    uint8_t dest = IPC_DEVICE_INVALID;

    valid[0] = stringToDestination(CommandLine->args[1], &dest);
    uint32_t start = TAP_HexArgToInt(CommandLine, 2, &valid[1]);

    if (valid[0] && valid[1])
    {
        if (start)
        {
            IpcTxPostMsg(IPC_TX_MESSAGE_ID_Hammer, dest, NULL, 0, NULL);
        }
        else
        {
            IpcTxTask_StopHammer(dest);
        }
    }
    else
    {
        TAP_PrintString(IPC_HAMMER_HELP_TEXT);
    }
}

void eTapIpcBuffer(CommandLine_t* CommandLine)
{
    TAP_Printf("IPC Rx Buffers Remaining: %d\n\r", IPCRouter_BuffersRxRemaining());
    TAP_Printf("IPC Tx Buffers Remaining: %d\n\r", IPCRouter_BuffersTxRemaining());
    TAP_Printf("IPC Bulk Sockets Available: %d\n\r", IPCBulk_GetAvailableSocketCount());
}

BOOL stringToDestination(char* str, uint8_t* destination)
{
    for(Ipc_Device_t i = IPC_DEVICE_DSP; i < IPC_NUM_DEVICES; i++)
    {
        if(strcmp(deviceNames[i], str) == 0)
        {
            *destination = i;
            return TRUE;
        }
    }
    return FALSE;
}

void eTapIpcSetLogOpcodes(CommandLine_t* CommandLine)
{
    uint8_t opcodes[MAX_RAWOPCODES];
    BOOL valid;
    if (CommandLine->numArgs != 2)
    {
        TAP_PrintString(IPC_LOG_HELP_TEXT);
        return;
    }
    valid = TAP_HexStringToUInt8Array(CommandLine->args[1], opcodes, MAX_RAWOPCODES);
    if (valid == FALSE)
    {
        TAP_PrintString("Opcode list is invalid");
        return;
    }
    IpcSetRawOpcodesToLog(opcodes);
}

static void eTapIpcStats(void)
{
    TAP_PrintString("\nIPC Stats\n\n");
    for(Ipc_Device_t i = IPC_DEVICE_DSP; i < IPC_NUM_DEVICES; i++)
    {
        const IpcInterface_t* interface = IpcRouter_GetInterface(i);
        if((interface->type == IPC_INTERFACE_INVALID) || (interface->type == IPC_INTERFACE_LOOPBACK))
        {
            continue;
        }

        TAP_Printf("Device: %s\n", deviceNames[i]);
        TAP_Printf("\tMessages received: %d\n", interface->stat.messagesRxd);
        TAP_Printf("\tErrors: %d\n", interface->stat.messagesForwarded);
        TAP_Printf("\tForwarded: %d\n", interface->stat.messagesForwarded);

        // print interface specific data
        switch(interface->type)
        {
            case IPC_INTERFACE_SPI:
                printSpiStats(interface);
                break;
            default:
                break;
        }

        TAP_PrintString("\n");
    }
}

static void printSpiStats(const IpcInterface_t* interface)
{
    TAP_Printf("\tAvg. latency: %dms\n", interface->spi.stat.totalLatency / interface->stat.messagesRxd);
    TAP_Printf("\tWorst latency: %dms\n", interface->spi.stat.worstLatency);
}
