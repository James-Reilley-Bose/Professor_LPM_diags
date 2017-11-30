/*
  File  : IpcPassthrough.c
  Title :
  Author  : Derek Richardson
  Created : 07/14/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions to send IPC tap cmds to a DMA handle. Sends block until completion.
=======================================================================================
*/

#include "project.h"
#include "IpcPassThrough.h"
#include "etap.h"
#include "TapParserAPI.h"
#include "rottenlog.h"
#include "genTimer.h"
#include "EventDefinitions.h"
#include <string.h>

SCRIBE_DECL(ipc_pt);

static PassThroughSocket_t passThroughSocket =
{
    PASSTHROUGH_IDLE,
    NULL,
    0,
    0,
    0
};

BOOL IsValidIpcDestination(uint8_t destination);
static void IpcPassThrough_DoneProcessingCommand(uint32_t* params, uint8_t numParams);
static void IpcPassThrough_DoneWithSend(void);
static void IpcPassThrough_ResetState(void);
static void IpcPassThrough_Timeout(void);

/*
 * @func IpcPassThrough_Init
 *
 * @brief Called once to initialize timer used by pass through
 *
 * @param void
 *
 * @return void
 */
void IpcPassThrough_Init(void)
{
    passThroughSocket.timer = createTimer( TIMER_MSEC_TO_TICKS(PASSTHROUGH_CALLBACK_TIMER_PERIOD_MS), \
                                           NULL, \
                                           IPCROUTER_MESSAGE_ID_PassThroughTimerExpired, \
                                           FALSE, \
                                           tBlockIdIPCPassThrough, \
                                           "IpcPassThroughTimer", \
                                           NULL);

    debug_assert(passThroughSocket.timer != NULL);
}

/*
 * @func IpcPassThrough_StartTimer
 *
 * @brief Called to start the non repeating pass through timer
 *
 * @param void
 *
 * @return void
 */
void IpcPassThrough_StartTimer(void)
{
    timerStart( passThroughSocket.timer, 0, &ipcRouterTaskHandle->Queue);
}

/*
 * @func IpcPassThrough_Send
 *
 * @brief Function to begin a pass through tap command transaction between lpm and *
 *
 * @param uint8_t dest - destination address
 *        uint8_t* data - data containing null terminated string of command to pass
 *        uint32_t len - length of string being passed
 *
 * @return void
 */
void IpcPassThrough_Send(IpcDeviceId_t dest, uint8_t* data, uint32_t len)
{
    if (passThroughSocket.state == PASSTHROUGH_IDLE)
    {
        passThroughSocket.destination = dest;
        passThroughSocket.state = PASSTHROUGH_WAITING;

        int i;
        uint8_t params[] = {0, 0};
        for (i = 0; len > i * IPC_NUM_DATA_BYTES; i++)
        {
            params[0] = i;

            //is this the last packet?
            if (len > ((i + 1) * IPC_NUM_DATA_BYTES))
            {
                IPCRouter_Send(dest, IPC_PASSTHROUGH_SEND, params, (data + IPC_NUM_DATA_BYTES * i), IPC_NUM_DATA_BYTES);
            }
            else
            {
                params[1] = TRUE;
                uint32_t bytesToSend = ( len % IPC_NUM_DATA_BYTES ) ? len % IPC_NUM_DATA_BYTES : IPC_NUM_DATA_BYTES;
                IPCRouter_Send(dest, IPC_PASSTHROUGH_SEND, params, (data + IPC_NUM_DATA_BYTES * i), bytesToSend);
            }
        }
        LOG(ipc_pt, ROTTEN_LOGLEVEL_VERBOSE, "%d pass through packets sent to %d", i, dest);

        passThroughSocket.TimeLastActivity = GET_SYSTEM_UPTIME_MS();
        IpcPassThrough_StartTimer();
        BlockTapOnEvent(TAPPARSER_MESSAGE_ID_PassThroughDone, PASSTHROUGH_TAP_TIMEOUT_MS);
    }
    else
    {
        LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Only one pass through command is allowed at a time!");
    }
}

/*
 * @func IpcPassThrough_HandleResponse
 *
 * @brief Function to handle a pass through response ipc packet and print it out tap
 *
 * @param IpcPacket_t* packet - pointer to packet recieved from *
 *
 * @return void
 */
void IpcPassThrough_HandleResponse(IpcPacket_t* packet)
{
    if (passThroughSocket.state == PASSTHROUGH_RECIEVING_RESPONSE || passThroughSocket.state == PASSTHROUGH_WAITING)
    {
        passThroughSocket.state = PASSTHROUGH_RECIEVING_RESPONSE;
        if (packet->s.sender == passThroughSocket.destination)
        {
            if (packet->s.params[0] == passThroughSocket.nextExpectedPacket)
            {
                packet->s.checkXOR = 0; // a little hack to make sure there's a '\0'
                TAP_Printf((const char*)packet->s.data.b); // printf because it actually copies & buffers

                passThroughSocket.nextExpectedPacket++;
                passThroughSocket.TimeLastActivity = GET_SYSTEM_UPTIME_MS();

                if (packet->s.params[1])
                {
                    IpcPassThrough_DoneWithSend();
                }
            }
            else
            {
                LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Packets out of order from %d", packet->s.sender);
                IpcPassThrough_DoneWithSend();
            }
        }
        else
        {
            LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Packets from wrong source recieved from %d instead of %d", packet->s.sender, passThroughSocket.destination);
            //Should we stop the pt if another chip is misbehaving?
            //IpcPassThrough_Done();
        }
    }
    else
    {
        LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Unexpected response from %d", packet->s.sender);
    }
}

/*
 * @func IpcPassThrough_ReceiveCommand
 *
 * @brief Receive command packets. Process the command once the entire string is received.
 *
 * @param IpcPacket_t* packet - pointer to received packet
 *
 * @return void
 */
void IpcPassThrough_ReceiveCommand(IpcPacket_t* p)
{
    if ((passThroughSocket.state != PASSTHROUGH_IDLE) &&
            (passThroughSocket.state != PASSTHROUGH_RECEIVING_COMMAND))
    {
        LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Passthrough command received while busy.");
        return;
    }
    else if (p->s.params[0] != passThroughSocket.nextExpectedPacket)
    {
        LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Passthrough command packet out of order.");
        IpcPassThrough_ResetState();
        return;
    }

    if (passThroughSocket.state == PASSTHROUGH_IDLE)
    {
        passThroughSocket.sender = p->s.sender;
        TapListener_SetForwardToIpc(TRUE);
        IpcPassThrough_StartTimer();
    }
    else if (p->s.sender != passThroughSocket.sender)
    {
        LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Can only handle passthrough from one device.");
        return;
    }

    passThroughSocket.state = PASSTHROUGH_RECEIVING_COMMAND;

    p->s.checkXOR = '\0'; // insert a null so we can use string operations
    if (passThroughSocket.rxIndex < sizeof(passThroughSocket.rxCommandBuffer))
    {
        uint8_t strLength = strlen((const char*)p->s.data.b);
        uint8_t lengthToCopy = (strLength + passThroughSocket.rxIndex > sizeof(passThroughSocket.rxCommandBuffer)) ?
                               (sizeof(passThroughSocket.rxCommandBuffer) - passThroughSocket.rxIndex) : strLength;
        strncpy(&passThroughSocket.rxCommandBuffer[passThroughSocket.rxIndex], (const char*)p->s.data.b, lengthToCopy);
        passThroughSocket.rxIndex += lengthToCopy;
    }

    if (p->s.params[1])
    {
        passThroughSocket.state = PASSTHROUGH_PROCESSING_COMMAND;
        passThroughSocket.nextExpectedPacket = 0;
        passThroughSocket.rxIndex = 0;
        passThroughSocket.rxCommandBuffer[sizeof(passThroughSocket.rxCommandBuffer) - 1] = '\0';
        TapParserPostTapCommand((uint32_t)passThroughSocket.rxCommandBuffer, IpcPassThrough_DoneProcessingCommand);
    }
}

/*
 * @func IpcPassThrough_CopyResponse
 *
 * @brief Packetize and send passthrough command responses
 *
 * @param const char * resp - the response string
 *
 * @return void
 */
void IpcPassThrough_SendResponse(const char* resp)
{
    uint16_t strLength = strlen(resp);

    while (strLength > 0)
    {
        if (strLength + passThroughSocket.rxIndex > sizeof(passThroughSocket.rxResponseBuffer))
        {
            uint8_t params[] = {passThroughSocket.nextExpectedPacket++, 0};
            uint16_t lengthToCopy = sizeof(passThroughSocket.rxResponseBuffer) - passThroughSocket.rxIndex;
            strncpy(&passThroughSocket.rxResponseBuffer[passThroughSocket.rxIndex], resp, lengthToCopy);

            IPCRouter_Send(passThroughSocket.sender, IPC_PASSTHROUGH_RESPONSE, params, passThroughSocket.rxResponseBuffer, IPC_NUM_DATA_BYTES);

            passThroughSocket.rxIndex = 0;
            resp += lengthToCopy;
            strLength -= lengthToCopy;
        }
        else
        {
            strncpy(&passThroughSocket.rxResponseBuffer[passThroughSocket.rxIndex], resp, strLength);
            passThroughSocket.rxIndex += strLength;
            break;
        }
    }
}

/*
 * @func IpcPassThrough_DoneProcessingCommand
 *
 * @brief Used as a callback when a received passthrough command is done being processed.
 *        This sends the last piece of the response with the done flag set.
 *
 * @param uint32_t* params - not used
 *        uint8_t numParams - not used
 *
 * @return void
 */
static void IpcPassThrough_DoneProcessingCommand(uint32_t* params, uint8_t numParams)
{
    uint8_t packetParams[] = {passThroughSocket.nextExpectedPacket, 1};
    IPCRouter_Send(passThroughSocket.sender, IPC_PASSTHROUGH_RESPONSE, packetParams, passThroughSocket.rxResponseBuffer, passThroughSocket.rxIndex);
    IpcPassThrough_ResetState();
}

/*
 * @func IpcPassThrough_DoneWithSend
 *
 * @brief Function to close session
 *
 * @param  void
 *
 * @return void
 */
static void IpcPassThrough_DoneWithSend(void)
{
    IpcPassThrough_ResetState();
    ETAPPostMsg(TAPPARSER_MESSAGE_ID_PassThroughDone, NULL);
}

/*
 * @func IpcPassThrough_ResetState
 *
 * @brief Clear out all state information so we're ready for a new command.
 *
 * @param  void
 *
 * @return void
 */
static void IpcPassThrough_ResetState(void)
{
    passThroughSocket.state = PASSTHROUGH_IDLE;
    passThroughSocket.nextExpectedPacket = 0;
    passThroughSocket.destination = IPC_DEVICE_INVALID;
    passThroughSocket.sender = IPC_DEVICE_INVALID;
    passThroughSocket.rxIndex = 0;
    memset(passThroughSocket.rxCommandBuffer, 0, sizeof(passThroughSocket.rxCommandBuffer));
    TapListener_SetForwardToIpc(FALSE);
}

/*
 * @func IpcPassThrough_Timeout
 *
 * @brief Function to note timeout errors and close session
 *
 * @param  void
 *
 * @return void
 */
void IpcPassThrough_Timeout(void)
{
    LOG(ipc_pt, ROTTEN_LOGLEVEL_NORMAL, "Timed out when sending pass through command!");

    if (IsValidIpcDestination(passThroughSocket.destination))
    {
        IPCRouter_CancelTransfer((Ipc_Device_t)passThroughSocket.destination);
    }
    TAP_PrintString("Pass through command timed out!");
    IpcPassThrough_DoneWithSend();
}

/*
 * @func IpcRouter_PassThroughTimerHandler
 *
 * @brief Function to check if pass through response has timed out
 *
 * @param  void
 *
 * @return void
 */
void IpcRouter_PassThroughTimerHandler(void)
{
    if (passThroughSocket.state == PASSTHROUGH_WAITING)
    {
        if (GET_MILLI_SINCE(passThroughSocket.TimeLastActivity) >= PASSTHROUGH_FIRST_RESPONSE_TIMEOUT_MS)
        {
            IpcPassThrough_Timeout();
        }
        else
        {
            IpcPassThrough_StartTimer();
        }
    }
    else if (passThroughSocket.state == PASSTHROUGH_RECIEVING_RESPONSE)
    {
        if (GET_MILLI_SINCE(passThroughSocket.TimeLastActivity) >= PASSTHROUGH_RESPONSE_TIMEOUT_MS)
        {
            IpcPassThrough_Timeout();
        }
        else
        {
            IpcPassThrough_StartTimer();
        }
    }
    else if (passThroughSocket.state == PASSTHROUGH_RECEIVING_COMMAND)
    {
        if (GET_MILLI_SINCE(passThroughSocket.TimeLastActivity) >= PASSTHROUGH_RCV_COMMAND_TIMEOUT_MS)
        {
            IpcPassThrough_ResetState();
        }
        else
        {
            IpcPassThrough_StartTimer();
        }
    }
}

BOOL IsValidIpcDestination(uint8_t destination)
{
    return ((destination > IPC_DEVICE_INVALID) &&
            (destination  < IPC_NUM_DEVICES));
}
