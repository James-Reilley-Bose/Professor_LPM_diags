/*
  File  : IpcTxTask.c
  Title :
  Author  : Dillon Johnson
  Created : 04/10/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Generic task to handle transmitting IPC.
===============================================================================
*/

#include "TaskDefs.h"
#include "IpcTxTask.h"
#include "BoseUSART_API.h"
#include "i2cMaster_API.h"
#include "IpcInterface.h"
#include "IPCRouterTask.h"
#include "etap.h"
#include "IpcRx.h"
#include "BufferManager.h"
//#include "DeviceInterface.h"
#include "DeviceModelTask.h"

/* Logs */
SCRIBE_DECL(ipc_bulk);
SCRIBE_DECL(ipc_pt);
SCRIBE_DECL(ipc_raw);
SCRIBE_DECL(ipc_router);
SCRIBE_DECL(ipc_rx);
SCRIBE_DECL(ipc_tx);
SCRIBE_DECL(ipc_update);
SCRIBE_DECL(sys_events);

/* Globals */
static BOOL KeepHammering[IPC_NUM_DEVICES] = {0};

/* Private Functions */
static void IpcTxTask_HandleSend(GENERIC_MSG_t* msg);
static void IpcTxTask_HandleCancel(GENERIC_MSG_t* msg);
static void IpcTxTask_HandleHammer(GENERIC_MSG_t* msg);

/*
 * @func IpcTxTask_Init
 *
 * @brief Initializes an IPC TX task (this stuff will likely move to the router!).
 *
 * @param void* p - Pointer to the configuration for a specific IPC device.
 *
 * @return n/a
 */
void IpcTxTask_Init(void* p)
{
    IpcTxConfig_t* config = (IpcTxConfig_t*) p;

    /* Create synchronization */
    if (config->TxDmaEventGroupHandle == NULL)
    {
        // Create an Event Group handle
        config->TxDmaEventGroupHandle = xEventGroupCreate();
        RTOS_assert(config->TxDmaEventGroupHandle, "Failed to create IpcTxDMA event group.");
    }

    /* Configure the DMA channel */
    config->DmaTxHandle = dmaManager_ConfigureChannel(&config->DmaTxRequest, &config->DmaTxError);
    debug_assert(config->DmaTxError == DMA_MANAGER_NO_ERROR);

    LOG(ipc_tx, ROTTEN_LOGLEVEL_VERBOSE, "IPC Tx service started: %s", IpcTaskNames[config->DeviceID]);
}




/*
 * @func IpcTxTask
 *
 * @brief The task. Waits for IPC messages to send.
 *
 * @param void *pvParameters - Pointer to the configuration for a specific IPC device.
 *
 * @return n/a
 */
void IpcTxTask(void* pvParameters)
{
    IpcTxConfig_t* config = (IpcTxConfig_t*) pvParameters;
    debug_assert(ManagedIpcTxTasks[config->DeviceID] != NULL);

    for (;;)
    {
        TaskManagerPollQueue(ManagedIpcTxTasks[config->DeviceID]);
    }
}

/*
 * @func IpcTxTask_HandleMessage
 *
 * @brief Calls the appropriate message handler.
 *
 * @param GENERIC_MSG_t* msg - the message to process
 *
 * @return n/a
 */
void IpcTxTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case IPC_TX_MESSAGE_ID_Send:
            IpcTxTask_HandleSend(msg);
            break;
        case IPC_TX_MESSAGE_ID_Cancel:
            IpcTxTask_HandleCancel(msg);
            break;
        case IPC_TX_MESSAGE_ID_Hammer:
            IpcTxTask_HandleHammer(msg);
            break;
        default:
            //LOG(sys_events,ROTTEN_LOGLEVEL_NORMAL,"\tIpcTxTask\t%s was invalid message",eventStrings[msg->msgID]);
            break;
    }
}

/*
 * @func IpcTxTask_HandleSend
 *
 * @brief Sends IPC data down the appropriate pipe. Currently just headers...
 *
 * @param GENERIC_MSG_t* msg - the IPC_TX_MESSAGE_ID_Send message to process
 *                           - params[0] is a pointer to the IpcPacket_t
 *                           - params[1] is the destintation (high nibble) | size (low nibble)
 *
 * @return n/a
 */
static void IpcTxTask_HandleSend(GENERIC_MSG_t* msg)
{
    IpcPacket_t* packet = (IpcPacket_t*) msg->params[0];
    uint16_t destination = msg->params[1];

    if (DeviceModelTask_DeviceAvailable((Ipc_Device_t)destination) || (packet->s.opcode == IPC_STAT_REQ) ||
            (packet->s.opcode == IPC_LPM_H_S))
    {
        uint16_t size = msg->params[2];
        SendIpc(destination, packet->b, size);
    }
    else
    {
        LOG(ipc_tx, ROTTEN_LOGLEVEL_NORMAL, "Device unavailible: Message not sent! (from: %d, to: %d, opcode: 0x%X)", packet->s.sender, packet->s.destination, packet->s.opcode);
    }
}

/*
 * @func IpcTxTask_HandleCancel
 *
 * @brief Cancels DMA Transfers when timeouts occur...
 *
 * @param GENERIC_MSG_t* msg - the IPC_TX_MESSAGE_ID_Send message to process
 *                           - params[0] is the destintation (high nibble) | size (low nibble)
 *
 * @return n/a
 */
static void IpcTxTask_HandleCancel(GENERIC_MSG_t* msg)
{
    Ipc_Device_t destination = (Ipc_Device_t) msg->params[1];
    CancelIpc(destination);
}


/*
 * @func IpcTxTask_HandleHammer
 *
 * @brief Blast packets down the pipe until someone says stop
 *
 * @param GENERIC_MSG_t* msg - params[0] is the destination
 *
 * @return n/a
 */
static void IpcTxTask_HandleHammer(GENERIC_MSG_t* msg)
{
    uint16_t dest = msg->params[1]; // the first param is unused
    uint32_t count = 0;

    if ((dest == IPC_DEVICE_INVALID) || (dest >= IPC_NUM_DEVICES))
    {
        return;
    }

    KeepHammering[dest] = TRUE;

    IpcPacket_t* packet = BufferManagerGetBuffer(sizeof(IpcPacket_t));
    debug_assert(packet);
    IpcSendPacket_t pData =
    {
        .dest = (Ipc_Device_t) dest,
        .op = IPC_HAMMER,
        .params = NULL,
        .data = NULL,
        .length = 0,
        .sequence = 0,
        .connectionID = 0,
    };
    IpcBuildPacket(packet, &pData);

    do
    {
        SendIpc(dest, packet->b, sizeof(IpcPacket_t));
        count++;
        packet->s.data.w[0] = REV_WORD_BYTES(count);
    }
    while (KeepHammering[dest]);

    BufferManagerFreeBuffer(packet);
}

/*
 * @func IpcTxTask_StopHammer
 *
 * @brief Stop the hammer test.
 *
 * @param n/a
 *
 * @return n/a
 */
void IpcTxTask_StopHammer(uint32_t dest)
{
    KeepHammering[dest] = FALSE;
}
