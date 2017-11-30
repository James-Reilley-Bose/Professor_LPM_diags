/*
  File  : IpcPassThrough.h
  Title :
  Author  : Derek Richardson
  Created : 07/14/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions to send IPC tap cmds to a DMA handle. Sends block until completion.
=======================================================================================
*/
#include "IpcRouterTask.h"
#include "etap.h"

//Because this timeout will and should never happen it is extremely large
//Will always be unblocked by pass through timer exhaustion
#define PASSTHROUGH_TAP_TIMEOUT_MS                  100000

//all timeouts subject for debate
#define PASSTHROUGH_CALLBACK_TIMER_PERIOD_MS        500
#define PASSTHROUGH_FIRST_RESPONSE_TIMEOUT_MS       2500
#define PASSTHROUGH_RESPONSE_TIMEOUT_MS             500
#define PASSTHROUGH_RCV_COMMAND_TIMEOUT_MS          1000

void IpcPassThrough_Init(void);
void IpcPassThrough_StartTimer(void);
void IpcPassThrough_Send(IpcDeviceId_t dest, uint8_t* data, uint32_t len);
void IpcPassThrough_HandleResponse(IpcPacket_t* packet);
void IpcPassThrough_ReceiveCommand(IpcPacket_t* p);
void IpcPassThrough_SendResponse(const char* resp);
void IpcRouter_PassThroughTimerHandler(void);

typedef enum
{
    PASSTHROUGH_IDLE,
    PASSTHROUGH_WAITING,
    PASSTHROUGH_RECIEVING_RESPONSE,
    PASSTHROUGH_RECEIVING_COMMAND,
    PASSTHROUGH_PROCESSING_COMMAND,
} PASSTHROUGH_STATE;

typedef struct
{
    PASSTHROUGH_STATE state;
    TimerHandle_t timer;
    IpcDeviceId_t destination;
    IpcDeviceId_t sender;
    uint32_t TimeLastActivity;
    uint16_t rxIndex;
    uint8_t nextExpectedPacket;
    char rxCommandBuffer[TAP_COMMAND_BUFFER_SIZE];
    char rxResponseBuffer[IPC_NUM_DATA_BYTES];
} PassThroughSocket_t;
