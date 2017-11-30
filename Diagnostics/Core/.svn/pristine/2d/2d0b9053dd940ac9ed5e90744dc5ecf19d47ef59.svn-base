/*
  File  : IPCBulk.c
  Title :
  Author  : Dillon Johnson
  Created : 04/22/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions to handle bulk IPC data Transfers.
===============================================================================
*/

#include <string.h>
#include "project.h"
#include "IPCBulk.h"
#include "IpcInterface.h"
#include "IpcTxTask.h"
#include "BufferManager.h"
#include "genTimer.h"
#include "IpcVariant.h"
#include "nvram.h"

SCRIBE_DECL(ipc_bulk);

#define IPC_BULK_MAX_TX_BUFFER_AVAILABLE 1200 // So we don't try to queue up to many packets
#define BULK_TRANSFER_MAX_BUFFERS 6 // Maximum buffer blocks a socket can use at once

/* Private functions */
static void IPCBulk_ReceiveNotify(IpcPacket_t* ipcPacket, BulkSocket_t* socket);
static void IPCBulk_ReceiveResponse(IpcPacket_t* ipcPacket, BulkSocket_t* socket);
static void IPCBulk_ReceiveQuery(IpcPacket_t* ipcPacket, BulkSocket_t* socket);
static void IPCBulk_ReceiveStatus(IpcPacket_t* ipcPacket, BulkSocket_t* socket);
static void IPCBulk_ReceiveData(IpcPacket_t* ipcPacket, BulkSocket_t* socket);
static void IPCBulk_ReceiveClose(IpcPacket_t* ipcPacket, BulkSocket_t* socket);
static void IPCBulk_ReceiveClosed(IpcPacket_t* ipcPacket, BulkSocket_t* socket);

static void IPCBulk_SendNotify(BulkSocket_t* socket);
static void IPCBulk_SendQuery(BulkSocket_t* socket);
static void IPCBulk_SendData(BulkSocket_t* socket, uint32_t bufferAvailable);
static void IPCBulk_SendClose(BulkSocket_t* socket, BOOL done);

static BulkSocket_t* IPCBulk_ReserveSocket(uint16_t tID);
static BulkSocket_t* IPCBulk_GetSocket(uint16_t tID);
static void IPCBulk_CloseSocket(BulkSocket_t* socket);

/* Test functions */
void IPCBulk_SetInvalidTID(uint16_t tID);

/* Globals */
static BulkSocket_t Sockets[BULK_NUM_SOCKETS] = {0};
static xSemaphoreHandle BulkMutex = NULL;
static uint16_t TransferCounter = LPM_BULK_TRANSFER_ID_START;
static uint16_t InvalidTestTID = 0;
TimerHandle_t BulkTransferTimerHandle = NULL;
static QueueHandle_t IPC_tx_buffer;

/*
 * @func IPCBulk_Initialize
 *
 * @brief Initialize bulk transfer state.
 *
 * @param void
 *
 * @return void
 */
void IPCBulk_Initialize(QueueHandle_t ipcTxBufferQHandle)
{
    BulkMutex = xSemaphoreCreateMutex();
    debug_assert(BulkMutex != NULL);

    for (uint8_t i = 0; i < BULK_NUM_SOCKETS; i++)
    {
        Sockets[i].Mutex = xSemaphoreCreateMutex();
        debug_assert(Sockets[i].Mutex != NULL);
    }

    BulkTransferTimerHandle = createTimer( TIMER_MSEC_TO_TICKS(BULK_CALLBACK_TIMER_PERIOD_MS), \
                                           NULL, \
                                           IPCROUTER_MESSAGE_ID_BulkTimerExpired, \
                                           FALSE, \
                                           tBlockIdIPCBulk, \
                                           "IpcBulkTimer", \
                                           NULL);

    debug_assert(BulkTransferTimerHandle != NULL);

    IPC_tx_buffer = ipcTxBufferQHandle;
}

/*
 * @func IPCBulk_PacketHandler
 *
 * @brief Handles received bulk packets.
 *
 * @param IpcPacket_t* ipcPacket - the received packet
 *
 * @return void
 */
void IPCBulk_PacketHandler(IpcPacket_t* ipcPacket)
{
    uint16_t tID = IpcGetUint16(ipcPacket->s.params);
    BulkSocket_t* socket = IPCBulk_GetSocket(tID);

    if (socket == NULL)
    {
        /* If it's a notification, try and open a socket */
        if (ipcPacket->s.opcode == IPC_BULK_NOTIFY)
        {
            if ((socket = IPCBulk_ReserveSocket(tID)) != NULL)
            {
                IPCBulk_ReceiveNotify(ipcPacket, socket);
            }
        }
        else
        {
            LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Unexpected message (%04x)", tID);
        }
    }
    else
    {
        switch (ipcPacket->s.opcode)
        {
            case IPC_BULK_RESPONSE:
                IPCBulk_ReceiveResponse(ipcPacket, socket);
                break;
            case IPC_BULK_DATA:
                IPCBulk_ReceiveData(ipcPacket, socket);
                break;
            case IPC_BULK_STATUS:
                IPCBulk_ReceiveStatus(ipcPacket, socket);
                break;
            case IPC_BULK_QUERY:
                IPCBulk_ReceiveQuery(ipcPacket, socket);
                break;
            case IPC_BULK_CLOSE:
                IPCBulk_ReceiveClose(ipcPacket, socket);
                break;
            case IPC_BULK_CLOSED:
                IPCBulk_ReceiveClosed(ipcPacket, socket);
                break;
            default:
                LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Received non-bulk opcode (%d).", ipcPacket->s.opcode);
                break;
        }
    }
}

/*
 * @func IPCBulk_HandleAcknowledgement
 *
 * @brief Handles the applications response to a BULK_NOTIFY
 *
 * @param uint16_t tID - the transfer ID
 * @param uint8_t accept - accept or decline the transfer (not a boolean!)
 *
 * @return void
 */
void IPCBulk_HandleAcknowledgement(uint16_t tID, uint8_t accept)
{
    uint32_t data = REV_WORD_BYTES(accept);

    BulkSocket_t* socket = IPCBulk_GetSocket(tID);

    if (socket == NULL)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "No socket open for tID 0x%04x (%s)", tID, __func__);
        return;
    }

    tID = IpcGetUint16((uint8_t*) &tID); // reverse the transfer ID

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed (%s)", IpcTaskNames[socket->Sender]);
        debug_assert(0);
        return;
    }

    if (!IPCRouter_Send(socket->Sender, \
                        IPC_BULK_RESPONSE, \
                        (uint8_t*) &tID, \
                        &data, \
                        sizeof(data)))
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk response failed (%04x)", socket->Transfer.ID);
    }

    if (accept == BULK_RESPONSE_DECLINE)
    {
        IPCBulk_CloseSocket(socket);
    }

    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_HandleAbort
 *
 * @brief Handles application requests to abort an ongoing bulk transfer.
 *
 * @param uint16_t tID - the transfer ID
 *
 * @return void
 */
void IPCBulk_HandleAbort(uint16_t tID)
{
    BulkSocket_t* socket = IPCBulk_GetSocket(tID);

    if (socket == NULL)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "No socket open for tID 0x%04x (%s)", tID, __func__);
        return;
    }

    tID = IpcGetUint16((uint8_t*) &tID); // reverse the transfer ID

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed (%s)", IpcTaskNames[socket->Sender]);
        debug_assert(0);
        return;
    }

    if (socket->Sender != IPC_DEVICE_LPM) // if Rx, we need to cancel after receiving a query
    {
        socket->Abort = TRUE;
    }
    else // if Tx, we can go ahead and cancel
    {
        IPCBulk_SendClose(socket, FALSE);
        socket->Abort = TRUE;
    }

    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_ReceiveNotify
 *
 * @brief Attempts to start a bulk transfer upon receiving a Bulk Notify
 *
 * @param IpcPacket_t* ipcPacket - pointer to the received Bulk Notify packet
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return TRUE if started, FALSE otherwise
 */
static void IPCBulk_ReceiveNotify(IpcPacket_t* ipcPacket, BulkSocket_t* socket)
{
    BOOL accept = FALSE;

    /* Copy the metadata */
    BulkNotify_t* notification = (BulkNotify_t*) ipcPacket->s.data.w;
    ReverseWords(notification, (sizeof(BulkNotify_t) - BULK_FILE_NAME_MAX_SIZE) / sizeof(uint32_t));
    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) != pdFALSE)
    {
        socket->Transfer.Length = notification->length;
        socket->Transfer.Type = notification->type;
        socket->Transfer.FileID = notification->fileId;
        strncpy((char*)socket->Transfer.FileName, (const char*)&notification->filename, BULK_FILE_NAME_MAX_SIZE);

        /* Initialize the receive state */
        socket->Sender = ipcPacket->s.sender;
        socket->Destination = IPC_DEVICE_LPM;
        socket->NoBufferCounter = 0;
        socket->State = BULK_STATE_WAIT_QUERY;
        socket->BytesRxd = 0;
        socket->BytesTxd = 0;
        socket->Buffer.Data = NULL;
        socket->Buffer.Size = 0;
        socket->Buffer.BytesFilled = 0;
        socket->Buffer.Direction = BULK_TRANSFER_RX;
        socket->Xor = 0;
        socket->Abort = FALSE;
        socket->buffersUsed = 0;

        /* Find the appropriate task endpoint */
        accept = TRUE;
        switch (socket->Transfer.Type)
        {
            case BULK_TYPE_TEST:
                socket->Q = &ipcRouterTaskHandle->Queue;
                break;
            case BULK_TYPE_SOFTWARE_UPDATE:
                socket->Q = GetQueueByName("UpdateTask");
                break;
            case BULK_TYPE_HOSPITALITY_FILE:
            case BULK_TYPE_SOURCE_LIST:
                socket->Q = GetQueueByName("UITask");
                break;
            case BULK_TYPE_BLASTER_RECORD:
                socket->Q = GetQueueByName("UEIBlasterTask");
                break;
            default:
                accept = IpcBulkVariant_GetQueueForBulkType(socket->Transfer.Type, &(socket->Q));
                LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Unhandled file type.");
                break;
        }

        /* This shouldn't ever happen, but might as well protect against it */
        if (socket->Transfer.Length == 0)
        {
            accept = FALSE;
            uint16_t tID = IpcGetUint16((uint8_t*) &socket->Transfer.ID); // reverse the transfer ID
            BulkResponse_t resp = {.accept = REV_WORD_BYTES(BULK_RESPONSE_DECLINE)};
            if (!IPCRouter_Send(socket->Sender, \
                                IPC_BULK_RESPONSE, \
                                (uint8_t*) &tID, \
                                &resp, \
                                sizeof(resp)))
            {
                LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk response failed (%04x)", socket->Transfer.ID);
            }
        }
        xSemaphoreGive(socket->Mutex);
    }
    else
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed (%s)", __func__);
        debug_assert(0);
        return;
    }

    /* If the bulk layer is ready to go, ask the application if it wants to accept the transfer */
    if (accept)
    {
        // Ensure Q is valid
        debug_assert(socket->Q);
        NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferRequest, socket->Transfer.ID, socket->Transfer.Length);
    }
    else // close the socket.
    {
        IPCBulk_CloseSocket(socket);
    }
}

/*
 * @func IPCBulk_ReceiveResponse
 *
 * @brief Reads a devices response after we've sent a bulk notify.
 *
 * @param IpcPacket_t* ipcPacket - the packet with the response
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
static void IPCBulk_ReceiveResponse(IpcPacket_t* ipcPacket, BulkSocket_t* socket)
{
    BulkResponse_t* response = (BulkResponse_t*) ipcPacket->s.data.w;
    response->accept = REV_WORD_BYTES(response->accept);

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed (%s)", IpcTaskNames[ipcPacket->s.sender]);
        debug_assert(0);
        return;
    }

    if (socket->State == BULK_STATE_WAIT_RESPONSE)
    {
        if (response->accept == BULK_RESPONSE_ACCEPT)
        {
            IPCBulk_SendQuery(socket);
            socket->State = BULK_STATE_WAIT_STATUS;
            socket->TimeLastActivity = GET_SYSTEM_UPTIME_MS();
        }
        else
        {
            NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, socket->Transfer.FileID);
            LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk transfer declined (%04x)", socket->Transfer.ID);
            IPCBulk_CloseSocket(socket);
        }
    }
    else
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Out of order response (%04x)", socket->Transfer.ID);
        NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, socket->Transfer.FileID);
        IPCBulk_CloseSocket(socket);
    }

    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_ReceiveQuery
 *
 * @brief Lets the sender know how much buffer space we have
 *
 * @param IpcPacket_t* ipcPacket - pointer to the received Bulk Query packet
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
void IPCBulk_ReceiveQuery(IpcPacket_t* ipcPacket, BulkSocket_t* socket)
{
    BulkStatus_t status = {.bufferAvailable = 0, .status = BULK_STATUS_CONTINUE};

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed in %s. Socket state = %d.)", __func__, socket->State);
        debug_assert(0);
        return;
    }

    /* Check our state */
    if ((socket->State != BULK_STATE_WAIT_QUERY) && (socket->State != BULK_STATE_OPEN))
    {
        NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, socket->Transfer.FileID);
        IPCBulk_CloseSocket(socket);
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Out of order query (%04x)", socket->Transfer.ID);
    }
    else
    {
        if (socket->Abort) // Close the transfer down?
        {
            socket->Buffer.Size = 0;
            socket->Buffer.BytesFilled = 0;
            status.status = BULK_STATUS_ERROR;
        }
        else if ((socket->Buffer.Data == NULL) && // Do we need a new buffer?
                 (socket->buffersUsed < BULK_TRANSFER_MAX_BUFFERS))
        {
            uint32_t bytesToMalloc = MIN(BULK_TRANSFER_BUFFER_SIZE, socket->Transfer.Length - socket->BytesRxd);
            /* 2000 is a "magic" number. Arbitrarily chosen so bulk transfer doesn't hog too much heap. */
            if (BufferManagerMemoryRemaining() >= 2000)
            {
                socket->Buffer.Data = BufferManagerGetBuffer(bytesToMalloc);
            }
            else
            {
                socket->Buffer.Data = NULL;
            }
            if (socket->Buffer.Data)
            {
                socket->Buffer.Size = bytesToMalloc;
                socket->buffersUsed += 1;
            }
            else
            {
                socket->Buffer.Size = 0;
                LOG(ipc_bulk, ROTTEN_LOGLEVEL_VERBOSE, "Not enough room to malloc for bulk (TID = 0x%04x)", socket->Transfer.ID);
            }
        }

        /* Reply */
        status.bufferAvailable = socket->Buffer.Size - socket->Buffer.BytesFilled;
        ReverseWords(&status, sizeof(BulkStatus_t) / sizeof(uint32_t));
        IPCRouter_Send(ipcPacket->s.sender, \
                       IPC_BULK_STATUS, \
                       ipcPacket->s.params, \
                       &status, \
                       sizeof(status));

        socket->TimeLastActivity = GET_SYSTEM_UPTIME_MS();
        socket->State = BULK_STATE_OPEN;
    }

    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_ReceiveStatus
 *
 * @brief Reads a devices status after we've sent them a query.
 *
 * @param IpcPacket_t* ipcPacket - the packet with the device's status
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
static void IPCBulk_ReceiveStatus(IpcPacket_t* ipcPacket, BulkSocket_t* socket)
{
    BulkStatus_t* status = (BulkStatus_t*) ipcPacket->s.data.w;
    ReverseWords( status, sizeof(BulkStatus_t) / sizeof(uint32_t));

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed in %s. Socket state = %d.)", __func__, socket->State);
        debug_assert(0);
        return;
    }

    if (socket->State == BULK_STATE_WAIT_STATUS)
    {
        if (status->status == BULK_STATUS_CONTINUE)
        {
            if (status->bufferAvailable > 0)
            {
                IPCBulk_SendData(socket, status->bufferAvailable);
            }
            else
            {
                socket->State = BULK_STATE_WAIT_NO_BUFFER;
                socket->NoBufferCounter++;
            }
            socket->TimeLastActivity = GET_SYSTEM_UPTIME_MS();
        }
        else // shut'r down
        {
            NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, NULL);
            IPCBulk_SendClose(socket, FALSE);
        }
    }
    /* If we're aborting, it's safe to just ignore this message. */
    else if (!socket->Abort)
    {
        NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, socket->Transfer.FileID);
        IPCBulk_CloseSocket(socket);
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Out of order status (%04x)", socket->Transfer.ID);
    }

    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_ReceiveData
 *
 * @brief Processes bulk data.
 *
 * @param IpcPacket_t* ipcPacket - pointer to the received Bulk Data packet
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
void IPCBulk_ReceiveData(IpcPacket_t* ipcPacket, BulkSocket_t* socket)
{
    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed in %s. Socket state = %d.)", __func__, socket->State);
        debug_assert(0);
        return;
    }

    if (socket->State == BULK_STATE_OPEN)
    {
        uint32_t numBytesToCopy = MIN(socket->Transfer.Length - socket->BytesRxd, IPC_NUM_DATA_BYTES);
        memcpy(&socket->Buffer.Data[socket->Buffer.BytesFilled], ipcPacket->s.data.b, numBytesToCopy);

        socket->Xor ^= IpcCalculateXor (&socket->Buffer.Data[socket->Buffer.BytesFilled], numBytesToCopy);
        socket->BytesRxd += numBytesToCopy;
        socket->Buffer.BytesFilled += numBytesToCopy;
        socket->TimeLastActivity = GET_SYSTEM_UPTIME_MS();

        /* If the buffer is full, or there isn't room for a full packet or the remaining data, pass the buffer up */
        if ((socket->Buffer.BytesFilled == socket->Buffer.Size) ||
                ((socket->Buffer.Size - socket->Buffer.BytesFilled < IPC_NUM_DATA_BYTES) &&
                 (socket->Transfer.Length - socket->BytesRxd > socket->Buffer.Size - socket->Buffer.BytesFilled)))
        {
            uint32_t packed = (socket->Transfer.ID << 16) + (socket->Buffer.BytesFilled);
            NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer, packed, (uint32_t) socket->Buffer.Data);
            socket->Buffer.Data = NULL;
            socket->Buffer.Size = 0;
            socket->Buffer.BytesFilled = 0;
        }
    }
    else
    {
        NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, socket->Transfer.FileID);
        IPCBulk_CloseSocket(socket);
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Received unexpected data (%04x)", socket->Transfer.ID);
    }

    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_ReceiveClose
 *
 * @brief Tells the sender if the transfer was succesfully completed.
 *
 * @param IpcPacket_t* ipcPacket - pointer to the received Bulk Verify packet
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
void IPCBulk_ReceiveClose(IpcPacket_t* ipcPacket, BulkSocket_t* socket)
{
    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed in %s. Socket state = %d.)", __func__, socket->State);
        debug_assert(0);
        return;
    }

    if (socket->State == BULK_STATE_OPEN)
    {
        BulkClose_t* close = (BulkClose_t*) ipcPacket->s.data.w;
        BulkClosed_t closedResponse = {.status = REV_WORD_BYTES(BULK_CLOSE_OK)};
        /* Respond */
        if ( (socket->BytesRxd == socket->Transfer.Length) && (socket->Xor == close->checksum))
        {
            NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferComplete, socket->Transfer.ID, NULL);
        }
        else
        {
            closedResponse.status = REV_WORD_BYTES(BULK_CLOSE_ERROR);
            NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, NULL);
        }

        IPCRouter_Send(ipcPacket->s.sender, \
                       IPC_BULK_CLOSED, \
                       ipcPacket->s.params, \
                       &closedResponse, \
                       sizeof(closedResponse));
    }
    else
    {
        NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, NULL);
    }

    IPCBulk_CloseSocket(socket);

    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_ReceiveClosed
 *
 * @brief Reads a devices status to see if they received the entire transfer.
 *
 * @param IpcPacket_t* ipcPacket - the packet with the device's status
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
static void IPCBulk_ReceiveClosed(IpcPacket_t* ipcPacket, BulkSocket_t* socket)
{
    BulkClosed_t* closed = (BulkClosed_t*) ipcPacket->s.data.w;
    ReverseWords(closed, sizeof(BulkClosed_t) / sizeof(uint32_t));

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed in %s. Socket state = %d.)", __func__, socket->State);
        debug_assert(0);
        return;
    }

    if (socket->State == BULK_STATE_WAIT_CLOSED)
    {
        if (closed->status == BULK_CLOSE_OK)
        {
            NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferComplete, socket->Transfer.ID, socket->Transfer.FileID);
        }
        else // error
        {
            NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, socket->Transfer.FileID);
        }
    }
    else
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Out of order closed (%04x)", socket->Transfer.ID);
        NotifyEndpoint(socket->Q, IPCROUTER_MESSAGE_ID_BulkTransferError, socket->Transfer.ID, socket->Transfer.FileID);
    }

    socket->State = BULK_STATE_CLOSED;
    xSemaphoreGive(socket->Mutex);
}

/*
 * @func IPCBulk_SendNotify
 *
 * @brief Notifies a device that we'd like to transfer something.
 *
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
static void IPCBulk_SendNotify(BulkSocket_t* socket)
{
    /* Pack in the data */
    uint16_t params = REV_HALF_WORD_BYTES(socket->Transfer.ID);
    BulkNotify_t notification =
    {
        .length = socket->Transfer.Length,
        .type = socket->Transfer.Type,
        .fileId = socket->Transfer.FileID,
    };
    ReverseWords(&notification, (sizeof(BulkNotify_t) - BULK_FILE_NAME_MAX_SIZE) / sizeof(uint32_t));
    memset(notification.filename, 0, BULK_FILE_NAME_MAX_SIZE);
    strncpy((char*) notification.filename, (const char*)socket->Transfer.FileName, BULK_FILE_NAME_MAX_SIZE);

    /* Send the notification */
    if (!IPCRouter_Send(socket->Destination, \
                        IPC_BULK_NOTIFY, \
                        (uint8_t*)&params, \
                        &notification, \
                        sizeof(BulkNotify_t)))
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk notify failed (%s)", socket->Transfer.ID);
    }

    socket->State = BULK_STATE_WAIT_RESPONSE;
    socket->TimeLastActivity = GET_SYSTEM_UPTIME_MS();
}

/*
 * @func IPCBulk_SendQuery
 *
 * @brief Queries a device's buffer status.
 *
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 *
 * @return void
 */
static void IPCBulk_SendQuery(BulkSocket_t* socket)
{
    uint16_t params = REV_HALF_WORD_BYTES(socket->Transfer.ID);
    if (!IPCRouter_Send(socket->Destination, \
                        IPC_BULK_QUERY, \
                        (uint8_t*)&params, \
                        NULL, \
                        0))
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk query failed (%04x)", socket->Transfer.ID);
    }
    else
    {
        socket->State = BULK_STATE_WAIT_STATUS;
    }
}

/*
 * @func IPCBulk_SendData
 *
 * @brief Sends bulk data to a device.
 *
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 * @param uint32_t bufferAvailable - max number of bytes to send
 *
 * @return void
 */
static void IPCBulk_SendData(BulkSocket_t* socket, uint32_t bufferAvailable)
{
    uint32_t bytesLeft = socket->Transfer.Length - socket->BytesTxd;
    uint16_t params = REV_HALF_WORD_BYTES(socket->Transfer.ID);
    BOOL bytesSent = FALSE;
    BOOL noEnoughBuffers = FALSE;

    bufferAvailable = MIN(bufferAvailable, IPC_BULK_MAX_TX_BUFFER_AVAILABLE);

    /* While there is enough buffer space for a full packet, or we are at the end of a transfer and
       the rest of the data can fit in the remaining buffer space. */
    while ((bytesLeft > 0) && (((bufferAvailable / IPC_NUM_DATA_BYTES) > 0) || (bufferAvailable >= bytesLeft)))
    {
        /* Don't consume all the tx buffers with bulk transfer. Leave some for the
           rest of the system */
        if (uxQueueMessagesWaiting(IPC_tx_buffer) < 8)
        {
            noEnoughBuffers = TRUE;
            break;
        }

        uint16_t bytesToSend = MIN(bytesLeft, IPC_NUM_DATA_BYTES);

        if (IS_SPI_FLASH_ADDRESS(socket->Buffer.Data))
        {
            unsigned char sendBuffer[IPC_NUM_DATA_BYTES];
            uint32_t readAddr = ((uint32)socket->Buffer.Data + socket->BytesTxd);
            nvram_read(readAddr, bytesToSend, (uint8_t*)&sendBuffer);

            socket->Xor ^= IpcCalculateXor ((unsigned char*)&sendBuffer, bytesToSend);
            if (!IPCRouter_Send(socket->Destination, IPC_BULK_DATA, (uint8_t*)&params, &sendBuffer, bytesToSend))
            {
                LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk data failed (%s)", IpcTaskNames[socket->Destination]);
                break;
            }
        }
        else
        {
            socket->Xor ^= IpcCalculateXor (&socket->Buffer.Data[socket->BytesTxd], bytesToSend);
            if (!IPCRouter_Send(socket->Destination, IPC_BULK_DATA, (uint8_t*)&params, &socket->Buffer.Data[socket->BytesTxd], bytesToSend))
            {
                LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk data failed (%s)", IpcTaskNames[socket->Destination]);
                break;
            }
        }

        bytesSent = TRUE;
        socket->BytesTxd += bytesToSend;
        bytesLeft = socket->Transfer.Length - socket->BytesTxd;
        bufferAvailable -= bytesToSend;
    }

    if (!bytesSent && !noEnoughBuffers)
    {
        socket->NoBufferCounter++;
    }

    if (socket->Transfer.Length == socket->BytesTxd)
    {
        IPCBulk_SendClose(socket, TRUE);
    }
    else
    {
        IPCBulk_SendQuery(socket);
    }
}

/*
 * @func IPCBulk_SendClose
 *
 * @brief Sends a Close to the destination to see if they received all the bytes.
 *
 * @param BulkSocket_t* socket - pointer to the socket for this transaction
 * @param BOOL done - true if done, false to cancel
 *
 * @return void
 */
static void IPCBulk_SendClose(BulkSocket_t* socket, BOOL done)
{
    uint16 params = REV_HALF_WORD_BYTES(socket->Transfer.ID);
    BulkClose_t close;

    if (done)
    {
        close.status = REV_WORD_BYTES(BULK_CLOSE_OK);
    }
    else
    {
        close.status = REV_WORD_BYTES(BULK_CLOSE_ERROR);
    }
    close.checksum = socket->Xor;
    if (socket->Transfer.ID == InvalidTestTID)
    {
        close.checksum = ~close.checksum;
        InvalidTestTID = 0;
    }

    if (!IPCRouter_Send(socket->Destination, \
                        IPC_BULK_CLOSE, \
                        (uint8_t*)&params, \
                        &close, \
                        sizeof(BulkClose_t)))
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Error send failed (%s)", IpcTaskNames[socket->Destination]);
    }
    else
    {
        socket->State = BULK_STATE_WAIT_CLOSED;
    }
}

/*
 * @func IPCBulk_ReserveSocket
 *
 * @brief Reserve a socket for a new transaction.
 *
 * @param uint16_t tID - the transaction ID.
 *
 * @return BulkSocket_t* - pointer to the socket.
 */
static BulkSocket_t* IPCBulk_ReserveSocket(uint16_t tID)
{
    BulkSocket_t* socket = NULL;

    /* Reserve the first open socket */
    BOOL reserved = FALSE;
    for (uint8_t i = 0; i < BULK_NUM_SOCKETS; i++)
    {
        /* Instantly timeout on the mutex; we assume the socket is already in use */
        if (xSemaphoreTake(Sockets[i].Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_RESERVE_TIMEOUT_MS)) != pdFALSE)
        {
            if (Sockets[i].State == BULK_STATE_IDLE)
            {
                socket = &Sockets[i];
                Sockets[i].State = BULK_STATE_RESERVED;
                Sockets[i].Transfer.ID = tID;
                Sockets[i].PacketsRxd = 0;
                Sockets[i].PacketsTxd = 0;
                Sockets[i].TimeLastActivity = GET_SYSTEM_UPTIME_MS();
                reserved = TRUE;
            }

            xSemaphoreGive(Sockets[i].Mutex);

            if (reserved)
            {
                break;
            }
        }
        // no else, we just assume the socket is busy and go to the next one
    }

    return socket;
}

/*
 * @func IPCBulk_GetSocket
 *
 * @brief Checks if there's a socket open with the provided transaction ID.
 *
 * @param uint16_t tID - the transaction ID.
 *
 * @return BulkSocket_t* - pointer to the socket.
 */
static BulkSocket_t* IPCBulk_GetSocket(uint16_t tID)
{
    BulkSocket_t* socket = NULL;

    for (uint8_t i = 0; i < BULK_NUM_SOCKETS; i++)
    {
        if ((Sockets[i].State != BULK_STATE_IDLE) && (tID == Sockets[i].Transfer.ID))
        {
            socket = &Sockets[i];
        }

        if (socket != NULL)
        {
            break;
        }
    }

    return socket;
}

uint32_t IPCBulk_GetFileType(uint16_t tID)
{
    BulkSocket_t* socket = IPCBulk_GetSocket(tID);
    if (socket)
    {
        return socket->Transfer.Type;
    }
    else
    {
        return BULK_TYPE_INVALID;
    }
}


/*
 * @func IPCBulk_CloseSocket
 *
 * @brief Closes a socket. Its assumed the caller has locked BulkMutex.
 *
 * @param uint16_t tID - the transaction ID.
 *
 * @return BulkSocket_t* - pointer to the socket.
 */
static void IPCBulk_CloseSocket(BulkSocket_t* socket)
{
    socket->State = BULK_STATE_CLOSED;
    if ((socket->Buffer.Data != NULL) && (socket->Buffer.Direction == BULK_TRANSFER_RX))
    {
        BufferManagerFreeBuffer(socket->Buffer.Data);
        socket->Buffer.Data = NULL;
    }
}

/*
 * @func IPCBulk_DestorySocket
 *
 * @brief Marks socket for reuse
 *
 * @param BulkSocket_t* socket - socket to destory
 *
 * @return void
 */
static void IPCBulk_DestroySocket(BulkSocket_t* socket)
{
    debug_assert(socket);
    if (socket->State != BULK_STATE_CLOSED)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Error DestorySocket: socket wasn't previously closed. Socket state = %d (%04x)", socket->State, socket->Transfer.ID);
        IPCBulk_CloseSocket(socket);
    }
    socket->State = BULK_STATE_IDLE;
}

uint32_t IPCBulk_GetAvailableSocketCount(void)
{
    uint32_t count = 0;
    for (uint8_t i = 0; i < BULK_NUM_SOCKETS; i++)
    {
        if (Sockets[i].State == BULK_STATE_IDLE)
        {
            count++;
        }
    }
    return count;
}

/*
 * @func IPCBulk_GetNewTransferID
 *
 * @brief Claims a new transfer ID.
 *
 * @param void
 *
 * @return uint16_t - the reserved transfer ID, or 0 if something bad happened
 */
static uint16_t IPCBulk_GetNewTransferID(void)
{
    uint16_t tID = 0;

    if (xSemaphoreTake(BulkMutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) != pdFALSE)
    {
        tID = TransferCounter++;
        if (TransferCounter >= LPM_BULK_TRANSFER_ID_END)
        {
            TransferCounter = LPM_BULK_TRANSFER_ID_START;
        }

        xSemaphoreGive(BulkMutex);
    }
    else
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed (%s)", __func__);
        debug_assert(0);
    }

    return tID;
}

/*
 * @func IPCBulk_BulkTransfer
 *
 * @brief Starts a bulk transfer.
 *
 * @param uint8_t dest - the address of the device to send to
 * @param uint8_t* data - pointer to the data to send
 * @param uint32_t length - number of bytes in the data
 *
 * @return uint16_t tID - the transfer ID if it was started, NULL otherwise
 */
uint16_t IPCBulk_BulkTransfer(uint8_t dest, uint8_t* data, uint32_t length, uint32_t type, uint32_t FileID, const char* FileName, ManagedQ* Q)
{
    uint16_t tID = 0;

    if (((data == NULL) || (length == 0)) ||
            (dest >= IPC_NUM_DEVICES) ||
            (dest == IPC_DEVICE_INVALID) ||
            (dest == IPC_DEVICE_LPM))
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Invalid request (%s).", __func__);
        return tID;
    }

    BulkSocket_t* socket = IPCBulk_ReserveSocket(tID);
    if (!socket) // if all sockets are in use
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "All sockets busy (%s)!", __func__);
        return tID;
    }

    tID = IPCBulk_GetNewTransferID();

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed (%s)", IpcTaskNames[dest]);
        debug_assert(0);
        return 0;
    }

    /* Fill in transfer metadata */
    socket->Transfer.ID = tID;
    socket->Transfer.Length = length;
    socket->Transfer.Type = type;
    socket->Transfer.FileID = FileID;
    strncpy((char*)socket->Transfer.FileName, FileName, BULK_FILE_NAME_MAX_SIZE);

    /* Initialize TX state */
    socket->Destination = dest;
    socket->Sender = IPC_DEVICE_LPM;
    socket->NoBufferCounter = 0;
    socket->BytesRxd = 0;
    socket->BytesTxd = 0;
    socket->Buffer.Data = data;
    socket->Buffer.Size = length;
    socket->Buffer.Direction = BULK_TRANSFER_TX;
    socket->Xor = 0;
    socket->Q = Q;
    socket->Abort = FALSE;

    IPCBulk_SendNotify(socket);

    xSemaphoreGive(socket->Mutex);

    return tID;
}

/*
 * @func IPCBulk_StartTimer
 *
 * @brief Starts a software timer to call the cleanup function.
 *
 * @param void
 *
 * @return void
 */
void IPCBulk_StartTimer(void)
{
    timerStart( BulkTransferTimerHandle, 0, &ipcRouterTaskHandle->Queue);
}

/*
 * @func IPCBulk_CleanupTimerCallback
 *
 * @brief Called periodically to check the status of ongoing bulk transfers.
 *        If needed, it will end transfers that are hung.
 *
 * @param void
 *
 * @return void
 */
void IPCBulk_WatchdogTimerCallback(void)
{
    for (uint8_t i = 0; i < BULK_NUM_SOCKETS; i++)
    {
        if (xSemaphoreTake(Sockets[i].Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) != pdFALSE)
        {
            if ((Sockets[i].State != BULK_STATE_IDLE) && (Sockets[i].State != BULK_STATE_CLOSED))
            {
                if (GET_SYSTEM_UPTIME_MS() - Sockets[i].TimeLastActivity >= BULK_TRANSFER_TIMEOUT_MS)
                {
                    LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Socket timed out in state %d (%04x)", Sockets[i].State, Sockets[i].Transfer.ID);
                    NotifyEndpoint(Sockets[i].Q, IPCROUTER_MESSAGE_ID_BulkTransferError, Sockets[i].Transfer.ID, Sockets[i].Transfer.FileID);
                    IPCBulk_CloseSocket(&Sockets[i]);
                }
                else if (Sockets[i].NoBufferCounter >= BULK_TRANSFER_MAX_QUERIES)
                {
                    LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Transfer ID %04x reach maxed queries.", Sockets[i].Transfer.ID);
                    NotifyEndpoint(Sockets[i].Q, IPCROUTER_MESSAGE_ID_BulkTransferError, Sockets[i].Transfer.ID, Sockets[i].Transfer.FileID);
                    IPCBulk_CloseSocket(&Sockets[i]);
                }
                else if (Sockets[i].State == BULK_STATE_WAIT_NO_BUFFER)
                {
                    IPCBulk_SendQuery(&Sockets[i]);
                }
            }

            xSemaphoreGive(Sockets[i].Mutex);
        }
        else
        {
            LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed (%s)", __func__);
            debug_assert(0);
        }
    }
}

void IPCBulk_SetInvalidTID(uint16_t tID)
{
    InvalidTestTID = tID;
}

uint8_t* IPCBulk_GetFilename(uint16_t tID)
{

    BulkSocket_t* socket = IPCBulk_GetSocket(tID);

    if (socket)
    {
        return socket->Transfer.FileName;
    }
    else
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Bulk socket get failed");
        return NULL;
    }
}

BOOL IPCBulk_isBulkTransferMessage(MESSAGE_ID_t msg)
{
    switch (msg)
    {
        case IPCROUTER_MESSAGE_ID_BulkTimerExpired:
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
        case IPCROUTER_MESSAGE_ID_BulkTransferRequest:
        case IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge:
        case IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer:
        case IPCROUTER_MESSAGE_ID_BulkTransferAbort:
            return TRUE;
        default:
            return FALSE;
    }
}

void IPCBulk_ReturnBuffer(GENERIC_MSG_t* msg)
{
    uint8_t* buffer = (uint8_t*) msg->params[1];

    if (!buffer)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Received null buffer return");
        return;
    }

    BufferManagerFreeBuffer(buffer);

    BulkSocket_t* socket = IPCBulk_GetSocket(msg->params[0]);

    if ((socket != NULL) &&
            (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) != pdFALSE))
    {
        socket->buffersUsed -= 1;
        xSemaphoreGive(socket->Mutex);
    }
}

void IPCBulk_ActivateRxEndpoint(BulkEndpoint_t* endpoint, uint16_t tID, uint32_t TotalLength)
{
    endpoint->Active = TRUE;
    endpoint->BytesRxd = 0;
    endpoint->Direction = BULK_TRANSFER_RX;
    endpoint->tID = tID;
    endpoint->TotalLength = TotalLength;

    IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge, endpoint->tID, BULK_RESPONSE_ACCEPT);
}

void IPCBulk_DeactivateEndpoint(BulkEndpoint_t* endpoint)
{
    debug_assert(endpoint);

    if (!endpoint->Active)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Error IPCBulk_DeactiveEndpoint: endpoint is already deactivated");
        return;
    }

    IPCBulk_DeactivateSocket(endpoint->tID);
    endpoint->Active = FALSE;
}

void IPCBulk_DeactivateSocket(uint16_t tID)
{
    BulkSocket_t* socket = IPCBulk_GetSocket(tID);
    if (!socket)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Error IPCBulk_DeactivateSocket: invalid socket (0x%04x)", tID);
        return;
    }

    if (xSemaphoreTake(socket->Mutex, TIMER_MSEC_TO_TICKS(BULK_TRANSFER_MUTEX_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL, "Mutex take failed in %s. Socket state = %d.", __func__, socket->State);
        debug_assert(0);
        return;
    }

    IPCBulk_DestroySocket(socket);

    xSemaphoreGive(socket->Mutex);
}
