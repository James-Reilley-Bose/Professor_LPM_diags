/*
  File  : IPCBulk.h
  Title :
  Author  : Dillon Johnson
  Created : 04/22/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions/types to handle bulk IPC data transfers.
===============================================================================
*/

#ifndef _IPC_BULK_H_
#define _IPC_BULK_H_

#include "IpcRouterTask.h"
#include "IpcProtocolLpm.h"
#include "QueueManager.h"

/* Macros */
#define BULK_TRANSFER_MUTEX_TIMEOUT_MS      100     //
#define BULK_TRANSFER_RESERVE_TIMEOUT_MS    0       //
#define BULK_TRANSFER_BUFFER_SIZE           1200    // most space allocated at a time (for now)
#define BULK_NUM_SOCKETS                    8       //
#define BULK_SOCKET_NOT_OPEN                -1
#define BULK_SOCKET_ALREADY_OPEN            -2
#define LPM_BULK_TRANSFER_ID_START          0x0200
#define LPM_BULK_TRANSFER_ID_END            0x0300
#define BULK_CALLBACK_TIMER_PERIOD_MS       100
#define BULK_TRANSFER_TIMEOUT_MS            1000
#define BULK_TRANSFER_MAX_QUERIES           10

#define NotifyEndpoint(Q, message, p0, p1) QueueManagerPostCallbackMsg (Q, message, (uint32_t []){p0, p1}, NOP_CALLBACK, NO_BLOCK_TIME);
#define NotifyEndpointCallback(Q, message, p0, p1, c) QueueManagerPostCallbackMsg (Q, message, (uint32_t []){p0, p1}, c, NO_BLOCK_TIME);

#define IS_SPI_FLASH_ADDRESS(x) ((int)x < SPIFLASH_SIZE)

/* Types */
typedef enum // Bulk directions
{
    BULK_TRANSFER_TX = 0,
    BULK_TRANSFER_RX = 1,
} BULK_DIRECTION_t;

typedef struct
{
    uint8_t* Data;
    uint16_t Size;
    uint32_t BytesFilled;
    BULK_DIRECTION_t  Direction;
} BulkTransferBuffer_t;

typedef struct
{
    uint16_t ID;
    uint32_t Length;
    uint32_t Type;
    uint32_t FileID;
    uint8_t FileName[BULK_FILE_NAME_MAX_SIZE];
} BulkTransfer_t;

typedef struct
{
    BulkTransfer_t Transfer;
    IpcDeviceId_t Sender;
    IpcDeviceId_t Destination;
    uint32_t NoBufferCounter;
    uint8_t State;
    uint32_t BytesRxd;
    uint32_t BytesTxd;
    BulkTransferBuffer_t Buffer;
    uint32_t PacketsRxd;
    uint32_t PacketsTxd;
    uint32_t TimeLastActivity;
    xSemaphoreHandle Mutex;
    uint32_t Xor;
    ManagedQ* Q;
    BOOL Abort;
    uint8_t buffersUsed;
} BulkSocket_t;

typedef struct
{
    uint16_t tID;
    BULK_DIRECTION_t Direction;
    BOOL Active;
    uint32_t TotalLength;
    uint32_t BytesRxd;
} BulkEndpoint_t;

enum // Bulk States
{
    BULK_STATE_IDLE = 0,
    BULK_STATE_RESERVED,
    BULK_STATE_WAIT_QUERY,
    BULK_STATE_OPEN,
    BULK_STATE_WAIT_RESPONSE,
    BULK_STATE_WAIT_STATUS,
    BULK_STATE_WAIT_NO_BUFFER,
    BULK_STATE_SEND,
    BULK_STATE_WAIT_CLOSED,
    BULK_STATE_CLOSED
};

/* Functions */
void IPCBulk_Initialize(QueueHandle_t ipcTxBufferQHandle);
void IPCBulk_PacketHandler(IpcPacket_t* ipcPacket);
void IPCBulk_HandleAcknowledgement(uint16_t tID, uint8_t accept);
void IPCBulk_HandleAbort(uint16_t tID);
uint16_t IPCBulk_BulkTransfer(uint8_t dest, \
                              uint8_t* data, \
                              uint32_t length, \
                              uint32_t type, \
                              uint32_t FileID, \
                              const char* FileName,
                              ManagedQ* Q);
uint32_t IPCBulk_GetFileType(uint16_t tID);
uint32_t IPCBulk_GetAvailableSocketCount(void);
void IPCBulk_StartTimer(void);
void IPCBulk_WatchdogTimerCallback(void);
uint8_t* IPCBulk_GetFilename(uint16_t tID);
BOOL IPCBulk_isBulkTransferMessage(MESSAGE_ID_t msg);
void IPCBulk_ReturnBuffer(GENERIC_MSG_t* msg);
void IPCBulk_ActivateRxEndpoint(BulkEndpoint_t* endpoint, uint16_t tID, uint32_t TotalLength);
void IPCBulk_DeactivateEndpoint(BulkEndpoint_t* endpoint);
void IPCBulk_DeactivateSocket(uint16_t tID);

#endif // _IPC_BULK_H_
