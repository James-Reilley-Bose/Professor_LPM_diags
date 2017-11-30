/*
  File  : IPCRouterTask.c
  Title :
  Author  : jd71069
  Created : 03/27/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:

===============================================================================
*/
#include "project.h"
#include "IPCRouterTask.h"
#include "IpcProtocolLpm.h"
#include "IpcRx.h"
#include "IpcTxTask.h"
#include "TaskManager.h"
#include "TaskDefs.h"
#include "buffermanager.h"
#include "IpcInterface.h"
#include "IPCBulk.h"
#include "IpcPassThrough.h"
#include "IpcStatus.h"
#include "versionlib.h"
#include "IpcKeys.h"
#include "IpcTest.h"
#include "IpcVariant.h"
#include "UpdateManagerTask.h"
#include "LightSensorTask.h"
#include "SystemStateTask.h"

#include "LightBarCapSenseTask.h"
/* Logs
===============================================================================
*/
SCRIBE_DECL(demo);
SCRIBE_DECL(ipc_router);
SCRIBE_DECL(sys_events);
SCRIBE_DECL(ipc_rx);

/* Local Defines
===============================================================================
*/
#define NUM_RX_BUFFERS 20
#define NUM_TX_BUFFERS 20

/* Global Variable
===============================================================================
*/
ManagedTask* ipcRouterTaskHandle = NULL;
IpcInterface_t* IpcInterfaceTable[IPC_NUM_DEVICES] = {0};
ManagedTask* ManagedIpcTxTasks[IPC_NUM_DEVICES] = {0};

/* Local Variable
===============================================================================
*/
static IpcPacket_t IPCRouter_RxBuffers[NUM_RX_BUFFERS] = {0}; // buffers for rx'd packets
static IpcPacket_t IPCRouter_TxBuffers[NUM_TX_BUFFERS] = {0}; // buffers for tx'd packets
static QueueHandle_t IPC_rx_buffer;
static QueueHandle_t IPC_tx_buffer;
static IpcInterface_t DefaultInterface = {.type = (IpcInterfaceType_t) IPC_INTERFACE_INVALID};
static IpcInterface_t LoopbackInterface =
{
    .DeviceID = IPC_DEVICE_LPM,
    .type = IPC_INTERFACE_LOOPBACK,
    .uart = {
        .rx = {
            .packet = NULL,
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
        }
    },
    .stat = {0},
    .test = {0},
};

/* Local Prototypes
===============================================================================
*/
static void IPCRouter_BulkTimerHandler(void);
static void IPCRouter_RxMessage(IpcInterface_t* device, IpcPacket_t* packet);
static void IPCRouter_HandlePacket(IpcInterface_t* device, IpcPacket_t* packet);
static BOOL IpcRouter_RoutePacket(IpcInterface_t* device, IpcPacket_t* packet);
static BOOL IpcRouter_HandlePacketInternal(IpcInterface_t* device, IpcPacket_t* packet);
static void IPCRouter_ErrorHandler(IpcPacket_t* packet);
static void IPCRouter_BulkPacketHandler(IpcPacket_t* packet);
static void IPCRouter_DispatchStatusRequests(IpcPacket_t* packet);
static void IPCRouter_LogMessageHandler(IpcPacket_t* packet);
static void IPCRouter_SendPacketInternal(const IpcSendPacket_t* p);
static BOOL IPCRouter_ValidateIpcParams(const IpcSendPacket_t* p);

/*
===============================================================================
@func IPCRouterTask
@desc Generic task init
===============================================================================
*/
void IPCRouterTask (void* pvParamaters)
{
    IPCBulk_StartTimer();
    for (;;)
    {
        TaskManagerPollQueue(ipcRouterTaskHandle);
    }
}

/*
===============================================================================
@func IPCRouter_TaskInit
@desc Initialize IPC services.
===============================================================================
*/
void IPCRouter_TaskInit(void* p)
{
    /* Initialize interface table */
    for (uint_fast8_t i = 0; i < IPC_NUM_DEVICES; i++)
    {
        IpcInterfaceTable[i] = &DefaultInterface;
    }
    IPCRouter_RegisterDevice(&LoopbackInterface, ipcRouterTaskHandle);

    /* Initialize the rx packet buffer pool */
    IPC_rx_buffer = xQueueCreate(NUM_RX_BUFFERS, sizeof(IpcPacket_t*));
    debug_assert(IPC_rx_buffer);
    for (uint_fast8_t i = 0; i < NUM_RX_BUFFERS; i++)
    {
        IpcPacket_t* ipc = &IPCRouter_RxBuffers[i];
        debug_assert(xQueueSend(IPC_rx_buffer, &ipc, 0) != pdFALSE)
    }

    /* Initialize the tx packet buffer pool */
    IPC_tx_buffer = xQueueCreate(NUM_TX_BUFFERS, sizeof(IpcPacket_t*));
    debug_assert(IPC_tx_buffer);
    for (uint_fast8_t i = 0; i < NUM_TX_BUFFERS; i++)
    {
        IpcPacket_t* ipc = &IPCRouter_TxBuffers[i];
        debug_assert(xQueueSend(IPC_tx_buffer, &ipc, 0) != pdFALSE)
    }

    IpcKeys_Initialize();
    IPCBulk_Initialize(IPC_tx_buffer);
    IpcPassThrough_Init();

    ipcRouterTaskHandle = GetManagedTaskPointer("IPCRouterTask");
    debug_assert(ipcRouterTaskHandle);
}

void IPCRouter_RegisterDevice(IpcInterface_t* device, ManagedTask* task)
{
    debug_assert(device);
    debug_assert(device->DeviceID < IPC_NUM_DEVICES);
    debug_assert(IpcInterfaceTable[device->DeviceID] == &DefaultInterface);
    debug_assert(ManagedIpcTxTasks[device->DeviceID] == NULL);
    IpcInterfaceTable[device->DeviceID] = device;
    ManagedIpcTxTasks[device->DeviceID] = task;
}

/*
===============================================================================
@func IPCRouter_GetNextRxBuffer
@desc Finds the next available buffer.
===============================================================================
*/
IpcPacket_t* IPCRouter_GetNextRxBuffer(void)
{
    IpcPacket_t* packet = NULL;

    if (InInterrupt())
    {
        xQueueReceiveFromISR(IPC_rx_buffer, &packet, NULL);
    }
    else
    {
        xQueueReceive(IPC_rx_buffer, &packet, 0);
    }

    debug_assert(packet);
    return packet;
}


/*
===============================================================================
@func IPCRouter_ReturnRxBuffer
@desc Returns a packet buffer to the pool.
===============================================================================
*/
void IPCRouter_ReturnRxBuffer(IpcPacket_t* packet)
{
    LOG(ipc_router, ROTTEN_LOGLEVEL_VERBOSE, "Rx packet returned: %p", packet);

    // Ensure the packet being returned is from IPCRouter_RxBuffers
    debug_assert(((uint32_t) IPCRouter_RxBuffers % sizeof(IpcPacket_t)) == ((uint32_t) packet % sizeof(IpcPacket_t)));
    debug_assert((uint32_t) &IPCRouter_RxBuffers[0] <= (uint32_t) packet);
    debug_assert((uint32_t) packet <= (uint32_t) &IPCRouter_RxBuffers[NUM_RX_BUFFERS - 1]);

    if (InInterrupt())
    {
        if (xQueueSendFromISR(IPC_rx_buffer, &packet, NULL) == pdFALSE)
        {
            debug_assert(0);
        }
    }
    else
    {
        if (xQueueSend(IPC_rx_buffer, &packet, 0) == pdFALSE)
        {
            debug_assert(0);
        }
    }
}

uint8_t IPCRouter_BuffersRxRemaining(void)
{
    return uxQueueMessagesWaiting(IPC_rx_buffer);
}

void IPCRouter_ReturnRxBufferCallback(uint32_t* params, uint8_t numParams)
{
    IpcPacket_t* packet = (IpcPacket_t*) params[0];
    IPCRouter_ReturnRxBuffer(packet);
}

/*
===============================================================================
@func IPCRouter_GetNextTxBuffer
@desc Finds the next available buffer.
===============================================================================
*/
IpcPacket_t* IPCRouter_GetNextTxBuffer(void)
{
    IpcPacket_t* packet = NULL;
    if (InInterrupt())
    {
        xQueueReceiveFromISR(IPC_tx_buffer, &packet, NULL);
    }
    else
    {
        xQueueReceive(IPC_tx_buffer, &packet, 10);
    }
    debug_assert(packet);
    return packet;
}

/*
===============================================================================
@func IPCRouter_ReturnTxPacket
@desc Returns a packet buffer to the pool.
===============================================================================
*/
void IPCRouter_ReturnTxBuffer(IpcPacket_t* packet)
{
    LOG(ipc_router, ROTTEN_LOGLEVEL_VERBOSE, "Rx packet return: %p", packet);

    // Ensure the packet being returned is from IPCRouter_RxBuffers
    debug_assert(((uint32_t) IPCRouter_TxBuffers % sizeof(IpcPacket_t)) == ((uint32_t) packet % sizeof(IpcPacket_t)));
    debug_assert((uint32_t) &IPCRouter_TxBuffers[0] <= (uint32_t) packet);
    debug_assert((uint32_t) packet <= (uint32_t) &IPCRouter_TxBuffers[NUM_TX_BUFFERS - 1]);

    if (InInterrupt())
    {
        if (xQueueSendFromISR(IPC_tx_buffer, &packet, NULL) == pdFALSE)
        {
            debug_assert(0);
        }
    }
    else
    {
        if (xQueueSend(IPC_tx_buffer, &packet, 0) == pdFALSE)
        {
            debug_assert(0);
        }
    }
}

void IPCRouter_ReturnTxBufferCallback(uint32_t* params, uint8_t numParams)
{
    IpcPacket_t* packet = (IpcPacket_t*) params[0];

    IPCRouter_ReturnTxBuffer(packet);
}

uint8_t IPCRouter_BuffersTxRemaining(void)
{
    return uxQueueMessagesWaiting(IPC_tx_buffer);
}

/*
===============================================================================
@func IPCRouter_HandleMessage
@desc The task message handler.
===============================================================================
*/
void IPCRouter_HandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
        case IPCROUTER_MESSAGE_ID_RxMessage:
            {
                IPCRouter_RxMessage((IpcInterface_t*)message->params[0], (IpcPacket_t*)message->params[1]);
            }
            break;
        case IPCROUTER_MESSAGE_ID_BulkTimerExpired:
            {
                /* restart the Bulk Expiration Timer */
                IPCBulk_StartTimer();
                IPCRouter_BulkTimerHandler();
            }
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge:
            {
                IPCBulk_HandleAcknowledgement(message->params[0], message->params[1]);
            }
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
        case IPCROUTER_MESSAGE_ID_BulkTransferRequest:
        case IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer:
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
        case IPCROUTER_MESSAGE_ID_BulkTransferAbort:
        case IPCROUTER_MESSAGE_ID_BeginRTWTest:
        case IPCROUTER_MESSAGE_ID_EndRTWTest:
            {
                IpcTest_HandleMessage(message);
            }
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer:
            {
                IPCBulk_ReturnBuffer(message);
            }
            break;
        case IPCROUTER_MESSAGE_ID_PassThroughTimerExpired:
            {
                IpcRouter_PassThroughTimerHandler();
            }
            break;
        default:
            {
                LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "\tIPCRouter\t%s was invalid message", GetEventString(message->msgID));
            }
            break;
    }
}

/*
===============================================================================
@func IPCRouter_Send
@desc Send an IPC message to another device.
===============================================================================
*/
BOOL IPCRouter_Send(uint8_t dest, uint8_t op, uint8_t* params, void* data, uint8_t length)
{
    IpcSendPacket_t pData =
    {
        .dest = (Ipc_Device_t) dest,
        .op = (IpcOpcodes_t) op,
        .params = params,
        .data = data,
        .length = length,
        .sequence = 0,
        .connectionID = 0,
    };
    BOOL valid = IPCRouter_ValidateIpcParams(&pData);

    if(valid)
    {
        IPCRouter_SendPacketInternal(&pData);
    }
    else
    {
        LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Packet with invalid params not sent!");
    }

    return valid;
}

BOOL IPCRouter_SendResponse(const IpcSendPacket_t* pData)
{
    BOOL valid = IPCRouter_ValidateIpcParams(pData);
    if(valid)
    {
        IPCRouter_SendPacketInternal(pData);
    }
    return valid;
}

static void IPCRouter_SendPacketInternal(const IpcSendPacket_t* pData)
{
    if (pData->dest == IPC_DEVICE_LPM)
    {
        IpcRxConfig_t* loopbackConfig = &LoopbackInterface.uart.rx;

        // stuff this into the IPC receive queue
        loopbackConfig->packet = IPCRouter_GetNextRxBuffer();
        IpcBuildPacket(loopbackConfig->packet, pData);
        IPCRouter_RxMessage(&LoopbackInterface, loopbackConfig->packet);
    }
    else
    {
        IpcPacket_t* packet = (IpcPacket_t*) IPCRouter_GetNextTxBuffer();
        IpcBuildPacket(packet, pData);
        IpcTxPostMsg(IPC_TX_MESSAGE_ID_Send, pData->dest, packet, sizeof(IpcPacket_t), IPCRouter_ReturnTxBufferCallback);
    }
}

static BOOL IPCRouter_ValidateIpcParams(const IpcSendPacket_t* p)
{
    BOOL valid = TRUE;
    
    if (p->length > IPC_NUM_DATA_BYTES)
    {
        LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Length too long. Length: %d, Dest: %d", p->length, p->dest);
        valid = FALSE;
    }
    else if ((p->data == NULL) && (p->length > 0))
    {
        LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Invalid data pointer. op: %p, Dest: %d", p->op, p->dest);
        valid = FALSE;
    }
    else if (((p->dest < IPC_NUM_DEVICES) && (IpcInterfaceTable[p->dest]->type == IPC_INTERFACE_INVALID)) ||
             (p->dest >= IPC_NUM_DEVICES))
    {
        LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Invalid destination. Dest: %d", p->dest);
        valid = FALSE;
    }
    return valid;
}

/*
===============================================================================
@func IPCRouter_CancelTransfer
@desc Cancel DMA Submissions for timed out IPC Transfers.
===============================================================================
*/
void IPCRouter_CancelTransfer(Ipc_Device_t dest)
{
    IpcTxPostMsg(IPC_TX_MESSAGE_ID_Cancel, dest, NULL, 0, NULL);
}

/*
===============================================================================
@func IPCRouter_RxMessage
@desc Receive/process an IPC message.
===============================================================================
*/
static void IPCRouter_RxMessage(IpcInterface_t* device, IpcPacket_t* packet)
{
    debug_assert(device);
    LOG(ipc_rx, ROTTEN_LOGLEVEL_VERBOSE, "IPC received: %s", IpcTaskNames[device->DeviceID]);
    IpcLogPacket(packet);

    if (!IpcIsValidDestination((Ipc_Device_t) packet->s.destination) || !IpcIsValidPacket(packet))
    {
        LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "RxMessage invalid");
        device->stat.errors++;
        IPCRouter_ReturnRxBuffer(packet);
        return;
    }

    if (packet->s.destination == IPC_DEVICE_LPM)
    {
        IPCRouter_HandlePacket(device, packet);
    }
    else // forward to the destination device
    {
        device->stat.messagesForwarded++;
        IpcTxPostMsg(IPC_TX_MESSAGE_ID_Send,
                     packet->s.destination,
                     packet,
                     sizeof(IpcPacket_t),
                     IPCRouter_ReturnRxBufferCallback);
    }
}

static void IPCRouter_HandlePacket(IpcInterface_t* device, IpcPacket_t* packet)
{
    device->stat.messagesRxd++;
    if (!IpcRouter_RoutePacket(device, packet))
    {
        if (!IpcRouter_HandlePacketInternal(device, packet))
        {
            LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "IPCRouter received a message with an invalid opcode 0x%02x: %s", packet->s.opcode, IpcTaskNames[device->DeviceID]);
        }
    }
}

// This handles IPC packets that may need to be forwarded to another context.
// Packets may be returned to the buffer pool with callbacks or in this
// function directly.
static BOOL IpcRouter_RoutePacket(IpcInterface_t* device, IpcPacket_t* packet)
{
    BOOL routed = TRUE;

    switch (packet->s.opcode)
    {
        case IPC_STAT_REQ:
            IPCRouter_DispatchStatusRequests(packet);
            break;
        case IPC_UPDATE_AVAILABLE:
        case IPC_GET_VERSION:
        case IPC_UPDATE_ENTER:
        case IPC_UPDATE_REBOOT:
        case IPC_UPDATE_COMPLETE:
        case IPC_UPDATE_DOWNLOAD:
        case IPC_UPDATE_START_FLASH:
        case IPC_UPDATE_AUTHENTICATE:
        case IPC_UPDATE_INC_PROG:
        case IPC_UPDATE_UNIFIED_PROG:
        case IPC_UPDATE_STATUS:
        case IPC_UPDATE_GET_STATUS:
        case IPC_RIVIERA_CONTROL_UPDATE:
             UpdatePostMsg(UPDATE_MESSAGE_ID_IPCPacketAvailable, IPCRouter_ReturnRxBufferCallback, (uint32_t)packet);
             break;
        case IPC_LBCS_ANIM_DB_VERSION_REQ:
        case IPC_LBCS_START_ANIM:
        case IPC_LBCS_STOP_ANIM:
        case IPC_LBCS_ABORT_ANIM:
             LBCSPostMsg(LBCS_MSG_ID_HandleIPC, (uint32_t)packet);
             break;
        case IPC_DEVICE_BOOTED:
            // TODO - APQ Device Model
            break;
        case IPC_SET_POWER_STATE:
            SysStatePostMsg(IPCROUTER_MESSAGE_ID_RxMessage, IPCRouter_ReturnRxBufferCallback, (uint32_t)packet);
            break;
        case IPC_GET_POWER_STATE:
            SystemStateTask_GetState(packet->s.sequence, packet->s.connectionID);
            break;
        case IPC_SET_SYSTEM_STATE:
            SysStatePostMsg(SYSTEM_MESSAGE_ID_SystemStateSet, IPCRouter_ReturnRxBufferCallback, (uint32_t)packet);
            break;
        case IPC_GET_SYSTEM_STATE:
            SystemStateTask_GetState(packet->s.sequence, packet->s.connectionID);
            break;
        default:
            routed = IpcVariant_RoutePacket(device, packet);
            break;
    }

    return routed;
}

// This handles IPC packets in the IpcRouter context. All packets are returned
// in one spot at the end of the switch.
static BOOL IpcRouter_HandlePacketInternal(IpcInterface_t* device, IpcPacket_t* packet)
{
    BOOL retVal = TRUE;
    switch (packet->s.opcode)
    {
        case IPC_ECHO:
        case IPC_ECHO_RESPONSE:
            IpcTest_HandlePacket(device, packet);
            break;
        case IPC_LOG_EVT:
            IPCRouter_LogMessageHandler(packet);
            break;
        case IPC_ERROR:
            IPCRouter_ErrorHandler(packet);
            break;
        case IPC_BULK_NOTIFY:
        case IPC_BULK_RESPONSE:
        case IPC_BULK_DATA:
        case IPC_BULK_STATUS:
        case IPC_BULK_QUERY:
        case IPC_BULK_CLOSE:
        case IPC_BULK_CLOSED:
            IPCRouter_BulkPacketHandler(packet);
            break;
        case IPC_PASSTHROUGH_RESPONSE:
            IpcPassThrough_HandleResponse(packet);
            break;
        case IPC_PASSTHROUGH_SEND:
            IpcPassThrough_ReceiveCommand(packet);
            break;
        default:
            retVal = FALSE;
            break;
    }
    IPCRouter_ReturnRxBuffer(packet);
    return retVal;
}

/*
===============================================================================
@func IPCRouter_RXDoneCallback
@desc Callback for when Rx DMA completes (Interrupt context!)
===============================================================================
*/
void IPCRouter_RXDoneCallback(void* pThis)
{
    IpcInterface_t* device = (IpcInterface_t*) pThis;
    IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_RxMessage, (uint32_t) device, (uint32_t) device->uart.rx.packet);
}

/*
===============================================================================
@func IPCRouter_BulkTimerHandler
@desc Poll the state of bulk transfer.
===============================================================================
*/
static void IPCRouter_BulkTimerHandler(void)
{
    IPCBulk_WatchdogTimerCallback();
}

/*
===============================================================================
@func IPCRouter_LogMessageHandler
@desc
===============================================================================
*/
void IPCRouter_LogMessageHandler(IpcPacket_t* packet)
{
    // Add null char to end of packet
    packet->s.checkXOR = '\0';
    LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "%s", packet->s.data.b);
    LOG(demo, ROTTEN_LOGLEVEL_NORMAL, "%s", packet->s.data.b);
}

/*
===============================================================================
@func IPCRouter_ErrorHandler
@desc Handles Error IPC messages.
===============================================================================
*/
static void IPCRouter_ErrorHandler(IpcPacket_t* packet)
{
    LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Received error %d (%s)", REV_WORD_BYTES(packet->s.data.w[0]), __func__);
}

/*
===============================================================================
@func IPCRouter_BulkNotifyHandler
@desc Handles Bulk Notify IPC messages.
===============================================================================
*/
static void IPCRouter_BulkPacketHandler(IpcPacket_t* packet)
{
    IPCBulk_PacketHandler(packet);
}

/*
===============================================================================
@func IPCRouter_DispatchStatusRequests
@desc Dispatch message to different destinations based on request type.
===============================================================================
*/
static void IPCRouter_DispatchStatusRequests(IpcPacket_t* packet)
{
    uint32_t statusRequestType = REV_WORD_BYTES(packet->s.data.w[0]);

    switch (statusRequestType)
    {
        case GENERAL_STATUS:
            switch ((Ipc_Device_t)packet->s.sender)
            {
                case IPC_DEVICE_DSP:
                    IpcStatus_SendHealthAndStatusRequest((Ipc_Device_t)packet->s.sender, DSP_STATUS);
                    break;
                case IPC_DEVICE_LPM:
                    IpcStatus_SendHealthAndStatusRequest((Ipc_Device_t)packet->s.sender, LPM_STATUS);
                    break;
                case IPC_DEVICE_AP:
                    IpcStatus_SendHealthAndStatusRequest((Ipc_Device_t)packet->s.sender, ASOC_STATUS);
                    break;
                case IPC_DEVICE_REMOTE:
                    IpcStatus_SendHealthAndStatusRequest((Ipc_Device_t)packet->s.sender, RF_REMOTE_STATUS);
                    break;
                case IPC_DEVICE_F0:
                    IpcStatus_SendHealthAndStatusRequest((Ipc_Device_t)packet->s.sender, F0_STATUS);
                    break;
                case IPC_DEVICE_SOUNDTOUCH:
                    IpcStatus_SendHealthAndStatusRequest((Ipc_Device_t)packet->s.sender, ST_STATUS);
                    break;
                case IPC_DEVICE_INVALID:
                default:
                    break;
            }
        /* INTENTIONAL FALLTHROUGH */
        case LPM_STATUS:
            {
                IpcStatus_SendLPMHealthAndStatus((Ipc_Device_t)packet->s.sender);
                IPCRouter_ReturnRxBuffer(packet);
            }
            break;
#ifdef BARDEEN
        case RF_REMOTE_STATUS:
            {
                RFPostMsgWithCallback(RFTASK_MESSAGE_ID_IPCPacketAvailable, (uint32_t)packet, 0, IPCRouter_ReturnRxBufferCallback);
            }
            break;
#endif
        default:
            LOG(ipc_router, ROTTEN_LOGLEVEL_NORMAL, "Invalid status request for %d from %d", statusRequestType, packet->s.sender);
            IPCRouter_ReturnRxBuffer(packet);
            break;
    }
}

const IpcInterface_t* IpcRouter_GetInterface(Ipc_Device_t device)
{
    if(device >= IPC_NUM_DEVICES)
    {
        return NULL;
    }
    else
    {
        return IpcInterfaceTable[device];
    }
}

