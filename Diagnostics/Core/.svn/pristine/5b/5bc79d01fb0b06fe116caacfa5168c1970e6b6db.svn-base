//
// IpcTest.c
//

#include "IPCRouterTask.h"
#include "IpcBulk.h"

SCRIBE_DECL(ipc_router);
SCRIBE_DECL(ipc_bulk);

#define NUM_BULK_TESTS 8
#define RTW_PRINT_RATE 500

static void IpcTest_BeginRoundTheWorldTest(IpcDeviceId_t destination);
static void IpcTest_EndRoundTheWorldTest(IpcDeviceId_t destination);
static void IpcTest_BulkCompleteHandler(GENERIC_MSG_t* message);
static void IpcTest_BulkErrorHandler(GENERIC_MSG_t* message);
static void IpcTest_BulkRequestHandler(GENERIC_MSG_t* message);
static void IpcTest_BulkFullBufferHandler(GENERIC_MSG_t* message);
static BulkEndpoint_t* IpcTest_GetNewTestEndpoint(void);
static BulkEndpoint_t* IpcTest_GetOpenTestEndpoint(uint16_t tID);
static void IpcTest_HandleEcho(IpcPacket_t* packet);
static void IpcTest_HandleEchoResponse(IpcInterface_t* device, IpcPacket_t* packet);
static uint32_t IpcTest_IncrementEchoCount(uint32_t nboCount);

static BulkEndpoint_t BulkTests[NUM_BULK_TESTS] = {0};

void IpcTest_HandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
            IpcTest_BulkCompleteHandler(message);
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferRequest:
            IpcTest_BulkRequestHandler(message);
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer:
            IpcTest_BulkFullBufferHandler(message);
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
            IpcTest_BulkErrorHandler(message);
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferAbort:
            IPCBulk_HandleAbort(message->params[0]);
            break;
        case IPCROUTER_MESSAGE_ID_BeginRTWTest:
            IpcTest_BeginRoundTheWorldTest((IpcDeviceId_t)message->params[0]);
            break;
        case IPCROUTER_MESSAGE_ID_EndRTWTest:
            IpcTest_EndRoundTheWorldTest((IpcDeviceId_t)message->params[0]);
            break;
    }
}

void IpcTest_HandlePacket(IpcInterface_t* device, IpcPacket_t* p)
{
    switch (p->s.opcode)
    {
        case IPC_ECHO:
            IpcTest_HandleEcho(p);
            break;
        case IPC_ECHO_RESPONSE:
            IpcTest_HandleEchoResponse(device, p);
            break;
    }
}

/*
===============================================================================
@func IPCRouter_BeginRoundTheWorldTest
@desc Start the Round-the-world test. This sends a packet off to travel
  the wire indefinitely.
===============================================================================
*/
static void IpcTest_BeginRoundTheWorldTest(IpcDeviceId_t destination)
{
    IpcInterface_t* device = IpcInterfaceTable[destination];
    if (!device)
    {
        return;
    }

    device->test.echoActive = TRUE;
    device->test.echoCount = 0;
    device->stat.errors = 0;
    device->stat.dmaErrors = 0;

    uint8_t data[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    IPCRouter_Send(destination, IPC_ECHO, NULL, data, sizeof(data));
}

/*
===============================================================================
@func IPCRouter_EndRoundTheWorldTest
@desc End RTW for a specific interface. Print stats.
===============================================================================
*/
static void IpcTest_EndRoundTheWorldTest(IpcDeviceId_t destination)
{
    IpcInterface_t* device = IpcInterfaceTable[destination];
    if (!device)
    {
        return;
    }

    device->test.echoActive = FALSE;

    // report stats from test.
    LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "IPC RTW STATS:");
    LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Messages Rx'd: %d", device->test.echoCount);

    // These are included so old tests don't break... but they're not that relevant.
    LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Message ERRORS: %d", device->stat.errors);
    LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "DMA Rx ERRORS: %d", device->stat.dmaErrors);
}

/*
===============================================================================
@func IpcTest_BulkCompleteHandler
@desc Handles bulk completed messages for test files.
===============================================================================
*/
static void IpcTest_BulkCompleteHandler(GENERIC_MSG_t* message)
{
    BulkEndpoint_t* test = IpcTest_GetOpenTestEndpoint(message->params[0]);

    if (test)
    {
        if (((test->Direction == BULK_TRANSFER_RX) && (test->TotalLength == test->BytesRxd)) ||
                (test->Direction == BULK_TRANSFER_TX))
        {
            LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk transfer test complete (%04x)", test->tID);
        }
        else
        {
            LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk transfer test error (%04x): not all bytes received", test->tID);
        }

        IPCBulk_DeactivateEndpoint(test);
    }
}

/*
===============================================================================
@func IpcTest_BulkErrorHandler
@desc Handles errors for test files.
===============================================================================
*/
static void IpcTest_BulkErrorHandler(GENERIC_MSG_t* message)
{
    BulkEndpoint_t* test = IpcTest_GetOpenTestEndpoint(message->params[0]);

    /* Just log and clean up */
    if (test)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk transfer test closed on error (%04x)", test->tID);
        IPCBulk_DeactivateEndpoint(test);
    }
}

/*
===============================================================================
@func IpcTest_BulkRequestHandler
@desc Handles bulk requests for test files.
===============================================================================
*/
static void IpcTest_BulkRequestHandler(GENERIC_MSG_t* message)
{
    BulkEndpoint_t* test = IpcTest_GetOpenTestEndpoint(message->params[0]);

    if (test) // if already open, decline
    {
        IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge, test->tID, BULK_RESPONSE_DECLINE);
        return;
    }

    test = IpcTest_GetNewTestEndpoint();

    if (test) // if there was an open endpoint, initialize and accept
    {
        IPCBulk_ActivateRxEndpoint(test, message->params[0], message->params[1]);
    }
    else
    {
        IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge, message->params[0], BULK_RESPONSE_DECLINE);
    }
}

/*
===============================================================================
@func IpcTest_BulkFullBufferHandler
@desc Handles full buffers for test files.
===============================================================================
*/
static void IpcTest_BulkFullBufferHandler(GENERIC_MSG_t* message)
{
    uint16_t tID = (message->params[0] & 0xffff0000) >> 16; // this is packed so we can queue all the params
    uint16_t length = message->params[0] & 0xffff;
    uint8_t* data = (uint8_t*) message->params[1];

    BulkEndpoint_t* test = IpcTest_GetOpenTestEndpoint(tID);

    if (test)
    {
        test->BytesRxd += length;
    }

    /* Not doing anything with the data right now */
    IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer, tID, (uint32_t) data);
}

/*
===============================================================================
@func IpcTest_StartBulkTest
@desc Starts a bulk transfer and records some metadata for the test.
===============================================================================
*/
uint16_t IpcTest_StartBulkTest(uint8_t dest, uint8_t* data, uint32_t length, uint32_t type, uint32_t FileID, const char* FileName, ManagedQ* Q)
{
    uint16_t tID = 0;
    BulkEndpoint_t* test = IpcTest_GetNewTestEndpoint();

    if (test == NULL) // if an endpoint isn't available, just exit
    {
        return tID;
    }

    tID = IPCBulk_BulkTransfer(dest, data, length, type, FileID, FileName, Q);

    if (tID) // if it opened, let's initialize the test state
    {
        test->tID = tID;
        test->Direction = BULK_TRANSFER_TX;
        test->Active = TRUE;
        test->TotalLength = length;
    }

    return tID;
}

/*
===============================================================================
@func IpcTest_GetNewTestEndpoint
@desc Looks for an available test endpoint.
===============================================================================
*/
static BulkEndpoint_t* IpcTest_GetNewTestEndpoint(void)
{
    BulkEndpoint_t* test = NULL;

    /* Look for an open test endpoint */
    for (uint8_t i = 0; i < NUM_BULK_TESTS; i++)
    {
        if (!BulkTests[i].Active)
        {
            test = &BulkTests[i];
            break;
        }
    }

    return test;
}

/*
===============================================================================
@func IpcTest_GetOpenTestEndpoint
@desc Looks for an open endpoint with the specified transfer ID.
===============================================================================
*/
static BulkEndpoint_t* IpcTest_GetOpenTestEndpoint(uint16_t tID)
{
    BulkEndpoint_t* test = NULL;

    for (uint8_t i = 0; i < NUM_BULK_TESTS; i++)
    {
        if (BulkTests[i].Active && (BulkTests[i].tID == tID))
        {
            test = &BulkTests[i];
            break;
        }
    }

    return test;
}

/*
===============================================================================
@func IpcTest_HandleEcho
@desc
===============================================================================
*/
static void IpcTest_HandleEcho(IpcPacket_t* packet)
{
    IpcSendPacket_t pData =
    {
        .dest = (Ipc_Device_t) packet->s.sender,
        .op = IPC_ECHO_RESPONSE,
        .params = NULL,
        .data = packet->s.data.b,
        .length = 8,
#ifndef USE_OLD_128_BYTE_STRUCTURE
        .sequence = packet->s.sequence,
        .connectionID = packet->s.connectionID,
#endif
    };
    packet->s.data.w[1] = IpcTest_IncrementEchoCount(packet->s.data.w[1]);
    IPCRouter_SendResponse(&pData);
}

/*
===============================================================================
@func IpcTest_HandleEchoResponse
@desc
===============================================================================
*/
static void IpcTest_HandleEchoResponse(IpcInterface_t* device, IpcPacket_t* packet)
{
    if (device->test.echoActive)
    {
        packet->s.data.w[1] = IpcTest_IncrementEchoCount(packet->s.data.w[1]);
        IPCRouter_Send(packet->s.sender, IPC_ECHO, NULL, packet->s.data.b, 8);
        if (++device->test.echoCount % RTW_PRINT_RATE == 0)
        {
            LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "%d RTW Echoes Received from: %s.", RTW_PRINT_RATE, IpcTaskNames[device->DeviceID]);
        }
    }
}

static uint32_t IpcTest_IncrementEchoCount(uint32_t nboCount)
{
    uint32_t count = REV_WORD_BYTES(nboCount) + 1;
    nboCount = REV_WORD_BYTES(count);
    return nboCount;
}
