/*
    File    :   WirelessAudioMUUpdRx.c
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

#define LEFT_CHANNEL_DELAY 0 // ms
#define RIGHT_CHANNEL_DELAY 50 // ms
#define BASS_CHANNEL_DELAY 100 // ms

static void waTransfer_MUTransmitDelay(void);

SCRIBE_DECL(wa_update);

waTransferState myTransferState;
static BOOL waUpdating = FALSE;

#define NUM_SPEAKER_UPDATE_FILES 1
static UpdateFile_t WAMUpdateFile =  {0, 0, WA_VARIANT_DARR_FILE_NAME, FALSE};

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
        NULL,
        waTransfer_HandleMessageFunc_WA_UPDATE_IDLE,
        NULL,
        waTransfer_ExitFunc_WA_UPDATE_IDLE,
    },
    {
        waTransfer_EnterFunc_WA_RECIEVING_DATA,
        waTransfer_HandleMessageFunc_WA_RECIEVING_DATA,
        waTransfer_HandeTimerFunc_WA_RECIEVING_DATA,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_WAIT_INSTALL,
        waTransfer_HandleMessageFunc_WA_UPDATE_WAIT_INSTALL,
        waTransfer_HandleTimerFunc_WA_UPDATE_WAIT_INSTALL,
        NULL,
    },
    {
        waTransfer_EnterFunc_WA_UPDATE_INSTALL,
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

    // DR_TODO lets do something better here
    UI_SetErrorState(TRUE, SYSTEM_ERROR_FIRMWARE_CRITICAL);
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
    if( myWaTransferStates[myTransferState.state].handleTimerFunc != NULL)
    {
        myWaTransferStates[myTransferState.state].handleTimerFunc();
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
 * @func waTransfer_MUSendCommand
 *
 * @brief Sends a darr msg to the CU. Built in delay based on channel
 *
 * @param uint32_t msgID - msg opcode to send
 *
 * @return void
 */
void waTransfer_MUSendCommand(WA_BoseCmd_t command)
{
    waTransfer_MUTransmitDelay();
    WirelessAudio_I2C_SendDataMessage(command, NULL, 0);
    LOG(wa_update,ROTTEN_LOGLEVEL_VERBOSE,"Darr data message 0x%02X sent!", command);
}

/*
 * @func waTransfer_SetMapFieldFilled
 *
 * @brief Called to fill the data payload of a WA_UPD_XCHANGE data packet
 *
 * @param uint8_t index - the chunk index of the given packet to check if recieved
 * @param uint8_t* map - map to check
 *
 * @return void
 */
void waTransfer_SetMapFieldFilled(uint8_t index, uint8_t* map)
{
    debug_assert(index < WA_CHUNK_NUM_OF_MAP_ENTRIES);

    uint8_t mapByte = (index / BITS_PER_BYTE);
    uint8_t byteBit = (index % BITS_PER_BYTE);

    /*
        note: because these are written in order it is flipped
              -ie index 0 is bit7 of map[0]
    */
    map[mapByte] =  (map[mapByte] | (1 << (7 - byteBit)));
}

/*
 * @func waTransfer_HandleMapRequest
 *
 * @brief Sends current map to mu
 *
 * @param void
 *
 * @return void
 */
void waTransfer_HandleMapRequest(void)
{
    uint8_t data[WA_DATA_PKT_PAYLOAD_SIZE] = {0};
    memcpy(data, myTransferState.myCurrentMap, WA_TRANSFER_MAP_LENGTH);

    waTransfer_MUTransmitDelay();
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_MAP_RESPONSE, data, WA_TRANSFER_MAP_LENGTH);

    WA_LOG_MAP();
}

/*
 * @func waTransfer_RecieveTransferData
 *
 * @brief Writes given data packet to it's offset in current chunk
 *
 * @param waDataMessageBuffer* buffer - buffer of msg to write
 *
 * @return void
 */
void waTransfer_RecieveTransferData(WA_DataMessage_t* msg)
{
    uint8_t sectorIndex = msg->data[0];

    // if we haven't got this part of the chunk yet
    if(!waTransfer_GetMapFieldFilled(sectorIndex, myTransferState.myCurrentMap))
    {
        uint32_t address = (INTERNAL_FLASH_UPDATE_STORAGE_START + (myTransferState.currentChunk * WA_CHUNK_LEN) + (sectorIndex * WA_UPDATE_PAYLOAD_SIZE));
        if(InternalFlashAPI_Write(address, &msg->data[1], WA_UPDATE_PAYLOAD_SIZE) == WA_UPDATE_PAYLOAD_SIZE)
        {
          waTransfer_SetMapFieldFilled(sectorIndex, myTransferState.myCurrentMap);
          LOG(wa_update,ROTTEN_LOGLEVEL_INSANE,"Got transfer packet index %d from cu and wrote to %x!", sectorIndex, address);
        }
        else
        {
          LOG(wa_update,ROTTEN_LOGLEVEL_NORMAL,"Error writing packet index %d from cu to flash!", sectorIndex);
          debug_assert(0);
        }
    }// else do nothing might have been another mu requesting map
    else
    {
      LOG(wa_update,ROTTEN_LOGLEVEL_INSANE,"Got repeated packet index %d from cu!", sectorIndex);
    }
}

void waTransfer_MUTransmitDelay(void)
{
    TickType_t delay = 0;

    switch(WirelessAudioUtilities_GetChannel())
    {
        case WA_CHANNEL_SURROUND_LEFT:
            delay = LEFT_CHANNEL_DELAY;
            break;
        case WA_CHANNEL_SURROUND_RIGHT:
            delay = RIGHT_CHANNEL_DELAY;
            break;
        case WA_CHANNEL_BASS_SKIPPER:
        case WA_CHANNEL_BASS_PBB:    
            delay = BASS_CHANNEL_DELAY;
            break;
    }

    vTaskDelay(TIMER_MSEC_TO_TICKS(delay));
}

void waTransfer_MUReportVersion(void)
{
    uint8_t data[WA_DATA_PKT_PAYLOAD_SIZE] = {0};
    UpdateBlobHeader upd;

    if(ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        strncpy((char*)data, upd.MasterVersion, sizeof(upd.MasterVersion));
    }
    else
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "No blob, report empty version.");
        strncpy((char*)data, NO_VERSION, NO_VERSION_LENGTH);
    }

    LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "Reporting blob version %s to CU", data);

    waTransfer_MUTransmitDelay();
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_UPD_VERSION, data, sizeof(upd.MasterVersion));
}

void waTransfer_MUSendAuthentication(void)
{
    UpdateBlobHeader upd;
    uint8_t data = ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS);

    waTransfer_MUTransmitDelay();
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_UPD_AUTHENTICATE, &data, sizeof(data));
}

void waTransfer_DoWAMUpdate(BOOL forced)
{
    UpdateBlobHeader upd;
    if(!ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        // Uh oh....
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file!");
        debug_assert(FALSE);
        system_reboot();
    }

    ManifestEntry WAMManifest;
    if (!GetUpdateFileInfoFromManifest(&upd, BLOB_ADDRESS, &WAMUpdateFile, NUM_SPEAKER_UPDATE_FILES) ||
        !GetManifestEntryByName(&upd, BLOB_ADDRESS, &WAMManifest, WAMUpdateFile.filename))
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Unable to extract DARR update from blob!");
        debug_assert(FALSE);
        system_reboot();
    }

    char versionBuffer[4] = {0};
    snprintf(versionBuffer, sizeof(versionBuffer), "%d", WirelessAudioUtilities_GetVersion());

    if (forced ||
        strncmp(versionBuffer, WAMManifest.ImageVersion, strlen(versionBuffer)))
    {
        WirelessAudioUtilities_UpdateAndReprogram();
    }
}
