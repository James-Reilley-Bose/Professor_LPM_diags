/*
    File    :   WirelessAudioCUUpdTxUtil.c
    Title   :   
    Author  :   dr1005920
    Created :   01/05/16
    Language:   C
    Copyright:  (C) 2016 Bose Corporation, Framingham, MA

    Description:  State machine of file transfer/update over DARR data channel
                    -Also holds all functions used across multiple states

===============================================================================
*/
#include <math.h>

#include "project.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioData.h"
#include "WirelessAudioUpdateTransfer.h"
#include "WirelessAudioAPI.h"
#include "WirelessAudioTask.h"

#include "AccessoryManager.h"
#include "genTimer.h"
#include "internalFlashAPI.h"
#include "UpdateManagerTask.h"
#include "uitask.h"

#include "nvram.h"
#include "nv_mfg.h"

SCRIBE_DECL(wa_task);
SCRIBE_DECL(wa_update);

waTransferState myTransferState;
static BOOL waUpdating = FALSE;

static const uint8_t filledMap[WA_TRANSFER_MAP_LENGTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void waSetUpdating(BOOL state)
{
  waUpdating = state;
}

BOOL waGetUpdating(void)
{
  return waUpdating;
}

waUpdateState waGetUpdateState(void)
{
    return myTransferState.state;
}

const waTransferStateStruct myWaTransferStates[] = {
    {
        waTransfer_EnterFunc_WA_UPDATE_IDLE,
        NULL,
        NULL,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_CHECK_VERSION,
        waTransfer_HandleMessageFunc_WA_CHECK_VERSION,
        waTransfer_HandleTimerFunc_WA_CHECK_VERSION,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_STARTING_UPDATE,
        waTransfer_HandleMessageFunc_WA_UPDATE_STARTING_UPDATE,
        waTransfer_HandleTimerFunc_WA_UPDATE_STARTING_UPDATE,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_CLEARING_MAP,
        waTransfer_HandleMessageFunc_WA_UPDATE_CLEARING_MAP,
        waTransfer_HandleTimerFunc_WA_UPDATE_CLEARING_MAP,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_SENDING_CHUNK,
        NULL,
        NULL,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_REQUESTING_MAP,
        waTransfer_HandleMessageFunc_WA_UPDATE_REQUESTING_MAP,
        waTransfer_HandleTimerFunc_WA_UPDATE_REQUESTING_MAP,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_RECTIFYING_CHUNK,
        NULL,
        NULL,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_AUTHENTICATE,
        waTransfer_HandleMessageFunc_WA_UPDATE_AUTHENTICATE,
        waTransfer_HandleTimerFunc_WA_UPDATE_AUTHENTICATE,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_INSTALL,
        waTransfer_HandleMessageFunc_WA_UPDATE_INSTALL,
        waTransfer_HandleTimerFunc_WA_UPDATE_INSTALL,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_EXIT,
        NULL,
        NULL,
        NULL,
    },
};

void waTransfer_Init(void)
{
    myTransferState.timer = createTimer(   WA_DATA_REQUEST_TIMER_INTERVAL, \
                                           NULL, \
                                           WA_MSG_ID_Data_Request_Timeout, \
                                           FALSE, \
                                           tBlockIdwaTransfer, \
                                           "waDataRequestTimer", \
                                           NULL);

    debug_assert(myTransferState.timer != NULL);
}

/*
 * @func waTransfer_Abort
 *
 * @brief This is called when a transfer fails for whatever reason
 * 
 * @param void
 *
 * @return void
 */
void waTransfer_Abort(UpdateResult_t result)
{
    LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Aborting WA update: %d", result);

    myTransferState.state = WA_UPDATE_IDLE;
    timerStop(myTransferState.timer, 0);
    UpdatePostMsg(UPDATE_MESSAGE_ID_CompleteState, NOP_CALLBACK, result);
}

/*
 * @func waTransfer_GoToNextState
 *
 * @brief Called to transition from one state to the next. Because this is not a
 *          sequencial state machine it is called from a number of places. Calls exit
 *          for current state and enter for the dest
 * 
 * @param waUpdateState state - state to switch to
 *
 * @return void
 */
void waTransfer_GoToNextState(waUpdateState state)
{
    LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "Going to state %d", state);

    timerStop(myTransferState.timer, 0);

    // Reset all retry and recieved vars
    myTransferState.sendRetries = 0;
    myTransferState.msgsReceived = 0;

    if(state < WA_UPDATE_NUM_OF_STATES)
    {
        if( myWaTransferStates[myTransferState.state].exitFunc != NULL)
        {
            myWaTransferStates[myTransferState.state].exitFunc();
        }

        vTaskDelay(TIMER_MSEC_TO_TICKS(25));

        myTransferState.state = state;

        if( myWaTransferStates[myTransferState.state].enterFunc != NULL)
        {
            myWaTransferStates[myTransferState.state].enterFunc();
        }
    }
    else
    {
        waTransfer_Abort(UPDATE_FAILED);
    }
}


/*
 * @func waTransfer_HandleTimer
 *
 * @brief Passes a darr update/tx message along to the current states handler
 * 
 * @param buffer - the buffer to be passed to current states handler
 *
 * @return void
 */
void waTransfer_HandleDarrMessage(WA_DataMessage_t* msg)
{
    if( myWaTransferStates[myTransferState.state].handleMessageFunc != NULL)
    {
        myWaTransferStates[myTransferState.state].handleMessageFunc(msg);
    }
}

/*
 * @func waTransfer_HandleTimer
 *
 * @brief Passes a timer expired notification to current state and increments the
 *          the retry counter
 * 
 * @param void
 *
 * @return void
 */
void waTransfer_HandleTimer(void)
{
    // Increment retry count
    myTransferState.sendRetries++;
    
    if((myTransferState.sendRetries != 0) &&
       (myTransferState.sendRetries % WA_TRANSFER_RETRIES_BEFORE_RESET == 0))
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Retry count caused reset.");
        WirelessAudioUtilities_InitializeDarr(FALSE);
    }

    if(myTransferState.sendRetries < WA_TRANSFER_MAX_RETRIES)
    {
        if( myWaTransferStates[myTransferState.state].handleTimerFunc != NULL)
        {
            myWaTransferStates[myTransferState.state].handleTimerFunc();
        }
    }
    else
    {
        waTransfer_Abort(UPDATE_TIMEOUT);
    }
}

/*
 * @func waTransfer_GetMapFieldFilled
 *
 * @brief Broadcasts a darr msg with no data (ie. just op code) and optionally
 *          starts the watchdog to ensure all parties got it
 * 
 * @param uint8_t index - packet index in map to check
 * @param uint8_t* map - pointer to map to check
 *
 * @return BOOL - whether or no that slot was filled
 */
BOOL waTransfer_GetMapFieldFilled(uint8_t index, uint8_t* map)
{
    debug_assert(index < WA_CHUNK_NUM_OF_MAP_ENTRIES);

    uint8_t mapByte = (index / BITS_PER_BYTE);
    uint8_t byteBit = (index % BITS_PER_BYTE);

    /* 
        note: because these are written in order it is flipped 
              -ie index 0 is bit7 of map[0]
    */
    return (map[mapByte] & (1 << (7 - byteBit)));
}

/*
 * @func waTransfer_CUSendCommand
 *
 * @brief Broadcasts a darr msg with no data (ie. just op code) and optionally
 *          starts the watchdog to ensure all parties got it
 * 
 * @param uint32_t msgID - msg opcode to send
 * @param BOOL startTimer - TRUE if you want to start watchdog timer
 *
 * @return void
 */
void waTransfer_CUSendCommand(WA_BoseCmd_t command, uint8_t param, TickType_t timerPeriod)
{
    WirelessAudio_I2C_SendDataMessage(command, &param, sizeof(param));

    if(timerPeriod)
    {
        changeTimerPeriod( myTransferState.timer, TIMER_MSEC_TO_TICKS(timerPeriod), 0);
        timerStart(myTransferState.timer, 0, &wirelessAudioTaskHandle->Queue);
    }

    LOG(wa_update,ROTTEN_LOGLEVEL_VERBOSE,"Darr data message 0x%02X sent!", command);
}

/*
 * @func waTransfer_Start
 *
 * @brief This is called in waTask Context when the WA_MSG_ID_Enter_Update_Transfer
 *          message is retrieved. Starts the update tx processes state machine.
 * 
 * @param uin32_t imgOffset - offset in flash of thing to tx
 * @param uin32_t size - size of blob
 *
 * @return void
 */
void waTransfer_Start(uint32_t imgOffset, uint32_t size)
{
    myTransferState.dataOffsetStart = imgOffset;
    myTransferState.transferLength = size;
    myTransferState.numberOfChunks = (uint32_t) ceil((float) size / WA_CHUNK_LEN);
    myTransferState.msgsReceived = 0;
    myTransferState.sendRetries = 0;
    myTransferState.currentChunk = 0;

    myTransferState.numConnectedMUs = AccessoryManager_NumberOfWirelessSpeakersConnected();
    
    if(myTransferState.numConnectedMUs > 0)
    {
        WirelessAudio_PostAndWaitFor(WA_MSG_ID_Enter_Update_Transfer);
        waTransfer_GoToNextState(WA_UPDATE_CHECK_VERSION);
    }
    else
    {
        UpdatePostMsg(UPDATE_MESSAGE_ID_CompleteState, NOP_CALLBACK, UPDATE_SKIPPED);
    }
}

/*
 * @func waTransfer_BuildBlobPayload
 *
 * @brief Called to fill the data payload of a WA_UPD_XCHANGE data packet
 * 
 * @param waDataMessage* payload - the data packet to get filled
 * @param uint8_t index - the chunk index of the given packets data
 * @param uint32_t dataOffset - offset of data in nvram
 * @param uint8_t length - length to read ( always WA_DATA_PKT_PAYLOAD_SIZE except the last packet of last chunk )
 *
 * @return void
 */
void waTransfer_BuildBlobPayload(uint8_t* data,  uint8_t index, uint32_t dataOffset, uint8_t length)
{
    data[0] = index;

    // Copy length from spiflash to payload buffer
    // Note below is only line to change if one day we have to do this from internal
    nvram_read(dataOffset, length, &data[1]);

    // Set the rest to 0x00
    if(length < WA_UPDATE_PAYLOAD_SIZE)
    {
        memset(&data[1] + length, 0x00, WA_UPDATE_PAYLOAD_SIZE - length);
    }
}

/*
 * @func waTransfer_IsMapFilled
 *
 * @brief Broadcasts a darr msg with no data (ie. just op code) and optionally
 *          starts the watchdog to ensure all parties got it
 * 
 * @param uint8_t* map - pointer to map to check
 * @param uint32_t chunkLength - length of chunk being checked (will be WA_CHUNK_LEN except for lst chunk)
 *
 * @return BOOL - whether or not the map was completely filled
 */
BOOL waTransfer_IsMapFilled(uint8_t* map, uint32_t chunkLength)
{
    BOOL retVal = FALSE;
    uint8_t numOfEntries = chunkLength/WA_UPDATE_PAYLOAD_SIZE;
    
    if(numOfEntries % BITS_PER_BYTE == 0)
    {
        return !(memcmp(&filledMap, map, numOfEntries/BITS_PER_BYTE));
    }
    else
    {
        retVal = !(memcmp(&filledMap, map, numOfEntries/BITS_PER_BYTE));
        if(retVal)
        {
            for(uint8_t i = 0; i < (numOfEntries % BITS_PER_BYTE); i++)
            {
                if(!waTransfer_GetMapFieldFilled((numOfEntries - (numOfEntries % BITS_PER_BYTE)  + i),map))
                {
                    retVal = FALSE;
                    break;
                }
            }
        }
    }

    return retVal;
}

/*
 * @func waTransfer_RecieveMap
 *
 * @brief Handler for recieving map from mu's
 * 
 * @param waDataMessage* buffer - the data packet holding the map
 *
 * @return void
 */
void waTransfer_RecieveMap(WA_DataMessage_t* msg)
{
    for(uint8_t i = 0; i < WA_TRANSFER_MAP_LENGTH; i++)
    {
        myTransferState.myCurrentMap[i] = myTransferState.myCurrentMap[i] & msg->data[i];
    }
}

/*
 * @func waTransfer_IsRectifyNeeded
 *
 * @brief Check received maps to see if anything needs to be resent
 * 
 * @param void
 *
 * @return void
 */
BOOL waTransfer_IsRectifyNeeded(void)
{
    uint32_t length = 0;
    if(myTransferState.currentChunk < (myTransferState.transferLength / WA_CHUNK_LEN))
    {
        length = WA_CHUNK_LEN;
    }
    else /* If last chunk lenght might be < chunk length */
    {
        length = (myTransferState.transferLength % WA_CHUNK_LEN);
    }

    timerStop(myTransferState.timer, 0);

    return !waTransfer_IsMapFilled((uint8_t*)&myTransferState.myCurrentMap, length);
}

/*
 * @func waTransfer_SendChunk
 *
 * @brief Handler for recieving map from mu's
 * 
 * @param uint32_t chunkNumber - chunk number
 * @param uint32_t dataChunkOffset - offset of chunk in flash
 * @param uint32_t length - length of the chunk
 *
 * @return void
 */
static void waTransfer_SendChunk(uint32_t chunkNumber, uint32_t dataChunkOffset, uint32_t length)
{
    uint32_t myDataOffset = dataChunkOffset;
    uint8_t numOfEntries = length / WA_UPDATE_PAYLOAD_SIZE;
    uint8_t numOfLeftOverBytes = length % WA_UPDATE_PAYLOAD_SIZE;

    uint8_t data[WA_DATA_PKT_PAYLOAD_SIZE] = {0};

    uint8_t i = 0;
    for(; i < numOfEntries; i++ )
    {
        waTransfer_BuildBlobPayload(data, i, myDataOffset, WA_UPDATE_PAYLOAD_SIZE);
        WirelessAudio_I2C_SendDataMessage(WA_BCMD_UPD_DATA, data, WA_DATA_PKT_PAYLOAD_SIZE);
        myDataOffset += WA_UPDATE_PAYLOAD_SIZE;
        vTaskDelay(TIMER_MSEC_TO_TICKS(WA_TRANSFER_PACKET_DELAY));
    }

    if(numOfLeftOverBytes > 0)
    {
        waTransfer_BuildBlobPayload(data, i, myDataOffset, numOfLeftOverBytes);
        vTaskDelay(TIMER_MSEC_TO_TICKS(WA_TRANSFER_PACKET_DELAY));
        WirelessAudio_I2C_SendDataMessage(WA_BCMD_UPD_DATA, data, WA_DATA_PKT_PAYLOAD_SIZE);
        myDataOffset += numOfLeftOverBytes;
    }
    
    vTaskDelay(TIMER_MSEC_TO_TICKS(WA_TRANSFER_PACKET_DELAY));
}

/*
 * @func waTransfer_SendNextChunk
 *
 * @brief Function that kicks off the transfer of a chunk
 * 
 * @param void
 *
 * @return void
 */
void waTransfer_SendNextChunk(void)
{
    LOG(wa_update,ROTTEN_LOGLEVEL_VERBOSE,"Sending chunk %d of %d to mu!", myTransferState.currentChunk, myTransferState.numberOfChunks); 

    if(myTransferState.currentChunk < myTransferState.numberOfChunks - 1)
    {
        waTransfer_SendChunk( myTransferState.currentChunk, 
                   (myTransferState.dataOffsetStart + myTransferState.currentChunk * WA_CHUNK_LEN), 
                   WA_CHUNK_LEN );
    }
    else if(myTransferState.currentChunk == myTransferState.numberOfChunks - 1)
    {
        // The last chunk might not be full
        uint32_t length = (myTransferState.transferLength % WA_CHUNK_LEN) ?
                           myTransferState.transferLength % WA_CHUNK_LEN :
                           WA_CHUNK_LEN;
        waTransfer_SendChunk(myTransferState.currentChunk, 
                   (myTransferState.dataOffsetStart + myTransferState.currentChunk * WA_CHUNK_LEN), 
                   length); /* Send remainder */        
    }
    else
    {
      debug_assert(0);
    }
}

/*
 * @func waTransfer_RectifyChunk
 *
 * @brief Resends data packets for all missing map entries
 * 
 * @param void
 *
 * @return void
 */
void waTransfer_RectifyChunk(void)
{
    uint32_t dataChunkOffset = (myTransferState.dataOffsetStart + (myTransferState.currentChunk * WA_CHUNK_LEN));
    uint32_t length = 0;

    if(myTransferState.currentChunk < (myTransferState.transferLength / WA_CHUNK_LEN))
    {
        length = WA_CHUNK_LEN;
    }
    else /* If last chunk lenght might be < chunk length */
    {
        length = (myTransferState.transferLength % WA_CHUNK_LEN);
    }

    if (myTransferState.rectifyRetries > WA_TRANSFER_MAX_RETRIES )
    {
        waTransfer_Abort(UPDATE_TIMEOUT);
    }
    else
    {
        //Rectify Chunk (AKA all maps recieved call back)
        uint8_t numOfEntries = length / WA_UPDATE_PAYLOAD_SIZE;
        uint8_t numOfLeftOverBytes = length % WA_UPDATE_PAYLOAD_SIZE;
        uint8_t i = 0;
        uint8_t data[WA_DATA_PKT_PAYLOAD_SIZE] = {0};
        
        for(; i < numOfEntries;  i++) //each_empy_bit_in_map
        {
            if(!waTransfer_GetMapFieldFilled(i, (uint8_t*)&myTransferState.myCurrentMap))
            {                
                waTransfer_BuildBlobPayload(data, i, dataChunkOffset + (i * WA_UPDATE_PAYLOAD_SIZE), WA_UPDATE_PAYLOAD_SIZE);
                WirelessAudio_I2C_SendDataMessage(WA_BCMD_UPD_DATA, data, WA_DATA_PKT_PAYLOAD_SIZE);
                vTaskDelay(TIMER_MSEC_TO_TICKS(WA_TRANSFER_PACKET_DELAY));
                LOG(wa_update,ROTTEN_LOGLEVEL_VERBOSE,"Sent rectify packet index %d to mu!",  data[0]);
            }
        }

        if(numOfLeftOverBytes > 0)
        {
            if(!waTransfer_GetMapFieldFilled(i, (uint8_t*)&myTransferState.myCurrentMap))
            {
                waTransfer_BuildBlobPayload(data, i, dataChunkOffset + (i * WA_UPDATE_PAYLOAD_SIZE), numOfLeftOverBytes);
                WirelessAudio_I2C_SendDataMessage(WA_BCMD_UPD_DATA, data, WA_DATA_PKT_PAYLOAD_SIZE);
                LOG(wa_update,ROTTEN_LOGLEVEL_VERBOSE,"Sent rectify packet index %d to mu!",  data[0]);
            }
        }
    }

    myTransferState.rectifyRetries++;
}

int waTransfer_GetProgress(void)
{
    return (myTransferState.currentChunk * 100 / myTransferState.numberOfChunks);
}
