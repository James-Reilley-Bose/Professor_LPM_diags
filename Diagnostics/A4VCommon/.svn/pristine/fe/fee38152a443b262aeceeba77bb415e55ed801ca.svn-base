/*
    File    :   WirelessAudioCUUpdTx.c
    Title   :   
    Author  :   dr1005920
    Created :   01/05/16
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:  Implementation of file transfer over DARR data channel for CU

===============================================================================
*/
#include "project.h"
#include "genTimer.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioTask.h"
#include "WirelessAudioUpdateTransfer.h"
#include "UpdateModuleMicro.h"
#include "UpdateManagerTask.h"
#include "internalFlashAPI.h"
#include "versionlib.h"
#include "UITask.h"
#include "viewNotify.h"

#include "nvram.h"

SCRIBE_DECL(wa_update);

#define WA_UPDATE_WAIT_ENTER_UPDATE_TIME_MS 10000
#define WA_UPDATE_WAIT_RESPONSE_TIME_MS 3500
#define WA_UPDATE_POLL_TIME_MS 5000  

/***************************** WA_UPDATE_IDLE *************************************/
void waTransfer_EnterFunc_WA_UPDATE_IDLE(void)
{
    waSetUpdating(FALSE);
}

/************************ WA_UPDATE_CHECK_VERSION *********************************/
static ManifestEntry WAManifest;
void waTransfer_EnterFunc_WA_CHECK_VERSION(void)
{
    UpdateBlobHeader upd;

    waSetUpdating(TRUE);

    if(!ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS) ||
       !GetManifestEntryByName(&upd, BLOB_ADDRESS, &WAManifest, "speaker.bos"))
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file!");
        waTransfer_Abort(UPDATE_BLOB_ERROR);
        return;
    }

    waTransfer_CUSendCommand(WA_BCMD_UPD_VERSION, 0, WA_UPDATE_WAIT_RESPONSE_TIME_MS);
}

void waTransfer_HandleMessageFunc_WA_CHECK_VERSION(WA_DataMessage_t* msg)
{
    if(msg->opcode == WA_BCMD_UPD_VERSION)
    {
        myTransferState.msgsReceived++;

        LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "MU reported blob version %s", msg->data);
        if(VERSION_COMPARE_SAME_VERSION != VersionCompareVersions((const char*)msg->data,WAManifest.ImageVersion))
        {
            myTransferState.updateRequired = TRUE;
        }

        if (myTransferState.msgsReceived == myTransferState.numConnectedMUs)
        {   
            if(myTransferState.updateRequired ||
               UpdateState.forced)
            {
                waTransfer_GoToNextState(WA_UPDATE_STARTING_UPDATE);
            }
            else
            {
                waTransfer_Abort(UPDATE_SKIPPED_VERSION);
            }
        }
    }
}

void waTransfer_HandleTimerFunc_WA_CHECK_VERSION(void)
{
    LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "Retrying version check, only received %d responses", myTransferState.msgsReceived);
    myTransferState.msgsReceived = 0;
    myTransferState.updateRequired = FALSE;
    waTransfer_CUSendCommand(WA_BCMD_UPD_VERSION, 0, WA_UPDATE_WAIT_ENTER_UPDATE_TIME_MS);
}

/*********************** WA_UPDATE_STARTING_UPDATE *******************************/
void waTransfer_EnterFunc_WA_UPDATE_STARTING_UPDATE(void)
{
    UI_ViewNotify(UPDATE_STATUS_CHANGE);
    waTransfer_CUSendCommand(WA_BCMD_ENTER_UPDATE, UpdateState.forced, WA_UPDATE_WAIT_ENTER_UPDATE_TIME_MS);
}

void waTransfer_HandleMessageFunc_WA_UPDATE_STARTING_UPDATE(WA_DataMessage_t* msg)
{
    if(msg->opcode == WA_BCMD_READY_TO_UPDATE)
    {
        myTransferState.msgsReceived++;

        // If we got one from all our friends
        if (myTransferState.msgsReceived == myTransferState.numConnectedMUs)
        {   
            //Go to WA_UPDATE_SENDING_CHUNK to send first one
            waTransfer_GoToNextState(WA_UPDATE_CLEARING_MAP);
        }
    }
}

void waTransfer_HandleTimerFunc_WA_UPDATE_STARTING_UPDATE(void)
{
    LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "Retrying WA_ENTER_UPDATE, only received %d responses", myTransferState.msgsReceived);
    myTransferState.msgsReceived = 0;
    waTransfer_CUSendCommand(WA_BCMD_ENTER_UPDATE, 0, WA_UPDATE_WAIT_ENTER_UPDATE_TIME_MS);
}

/************************* WA_UPDATE_CLEARING_MAP ********************************/
void waTransfer_EnterFunc_WA_UPDATE_CLEARING_MAP(void)
{
    waTransfer_CUSendCommand(WA_BCMD_CLEAR_MAP, 0, WA_UPDATE_WAIT_RESPONSE_TIME_MS);
}

void waTransfer_HandleMessageFunc_WA_UPDATE_CLEARING_MAP(WA_DataMessage_t* msg)
{
    if(msg->opcode == WA_BCMD_MAP_CLEARED)
    {
        myTransferState.msgsReceived++;
        if(myTransferState.msgsReceived == myTransferState.numConnectedMUs)
        {
            waTransfer_GoToNextState(WA_UPDATE_SENDING_CHUNK);
        }
    }
}

void waTransfer_HandleTimerFunc_WA_UPDATE_CLEARING_MAP(void)
{
    LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "Retrying WA_CLEAR_MAP, only received %d responses", myTransferState.msgsReceived);
    myTransferState.msgsReceived = 0;
    waTransfer_CUSendCommand(WA_BCMD_CLEAR_MAP, 0, WA_UPDATE_WAIT_RESPONSE_TIME_MS);
}

/************************* WA_UPDATE_SENDING_CHUNK *******************************/
void waTransfer_EnterFunc_WA_UPDATE_SENDING_CHUNK(void)
{
     myTransferState.rectifyRetries = 0;
     waTransfer_SendNextChunk();
     waTransfer_GoToNextState(WA_UPDATE_REQUESTING_MAP);
}

/************************ WA_UPDATE_REQUESTING_MAP *******************************/
void waTransfer_EnterFunc_WA_UPDATE_REQUESTING_MAP(void)
{
    memset(myTransferState.myCurrentMap, 0xFF, WA_TRANSFER_MAP_LENGTH);
    waTransfer_CUSendCommand(WA_BCMD_MAP_REQUEST, 0, WA_UPDATE_WAIT_RESPONSE_TIME_MS);
}

void waTransfer_HandleMessageFunc_WA_UPDATE_REQUESTING_MAP(WA_DataMessage_t* msg)
{
    if(msg->opcode == WA_BCMD_MAP_RESPONSE)
    {
        waTransfer_RecieveMap(msg);
        myTransferState.msgsReceived++;
        if(myTransferState.msgsReceived == myTransferState.numConnectedMUs)
        {
            waTransfer_GoToNextState(WA_UPDATE_RECTIFYING_CHUNK);
        }
    }
}

void waTransfer_HandleTimerFunc_WA_UPDATE_REQUESTING_MAP(void)
{
    LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "Retrying WA_MAP_REQUEST, only received %d responses", myTransferState.msgsReceived);
    memset(myTransferState.myCurrentMap, 0xFF, WA_TRANSFER_MAP_LENGTH);
    myTransferState.msgsReceived = 0;
    waTransfer_CUSendCommand(WA_BCMD_MAP_REQUEST, 0, WA_UPDATE_WAIT_RESPONSE_TIME_MS);
}

/*********************** WA_UPDATE_RECTIFYING_CHUNK ******************************/
void waTransfer_EnterFunc_WA_UPDATE_RECTIFYING_CHUNK(void)
{
    if(waTransfer_IsRectifyNeeded())
    {
        waTransfer_RectifyChunk();
        waTransfer_GoToNextState(WA_UPDATE_REQUESTING_MAP);
        return;
    }

    myTransferState.currentChunk++;

    if(myTransferState.currentChunk < myTransferState.numberOfChunks)
    {
        waTransfer_GoToNextState(WA_UPDATE_CLEARING_MAP);
    }
    else
    {
        waTransfer_GoToNextState(WA_UPDATE_AUTHENTICATE);
    }
}

/************************* WA_UPDATE_AUTHENTICATE *********************************/
void waTransfer_EnterFunc_WA_UPDATE_AUTHENTICATE(void)
{
    // Should probably increase watchdog time during this step
    waTransfer_CUSendCommand(WA_BCMD_UPD_AUTHENTICATE, 0, WA_UPDATE_POLL_TIME_MS);
}

void waTransfer_HandleMessageFunc_WA_UPDATE_AUTHENTICATE(WA_DataMessage_t* msg)
{
    if(msg->opcode == WA_BCMD_UPD_AUTHENTICATE)
    {
        if(!msg->data[0]) // data[0] indicates authentication success/failure
        {
            // TODO: retry!
            waTransfer_Abort(UPDATE_FAILED);
            return;
        }

        myTransferState.msgsReceived++;
        if(myTransferState.msgsReceived == myTransferState.numConnectedMUs)
        {
            waTransfer_GoToNextState(WA_UPDATE_INSTALL);
        }
    }
}

void waTransfer_HandleTimerFunc_WA_UPDATE_AUTHENTICATE(void)
{
    waTransfer_CUSendCommand(WA_BCMD_UPD_AUTHENTICATE, 0, WA_UPDATE_POLL_TIME_MS);
    myTransferState.msgsReceived = 0;
}

/************************** WA_UPDATE_INSTALL **********************************/
void waTransfer_EnterFunc_WA_UPDATE_INSTALL(void)
{
    // Should probably increase watchdog time during this step
    waTransfer_CUSendCommand(WA_BCMD_UPD_INSTALL, 0, WA_UPDATE_POLL_TIME_MS);
}

void waTransfer_HandleMessageFunc_WA_UPDATE_INSTALL(WA_DataMessage_t* msg)
{
    if(msg->opcode == WA_BCMD_UPD_INSTALL)
    {
        myTransferState.msgsReceived++;
        if(myTransferState.msgsReceived == myTransferState.numConnectedMUs)
        {
            waTransfer_GoToNextState(WA_UPDATE_EXIT);       
        }
    }
}

void waTransfer_HandleTimerFunc_WA_UPDATE_INSTALL(void)
{
    waTransfer_CUSendCommand(WA_BCMD_UPD_INSTALL, 0, WA_UPDATE_POLL_TIME_MS);
    myTransferState.msgsReceived = 0;
}

/***************************** WA_UPDATE_EXIT ************************************/
void waTransfer_EnterFunc_WA_UPDATE_EXIT(void)
{
    // Notify update manager
    UpdatePostMsg(UPDATE_MESSAGE_ID_CompleteState, NOP_CALLBACK, UPDATE_COMPLETED);

    // Go to idle
    waTransfer_GoToNextState(WA_UPDATE_IDLE);

    // no longer in update state
    waSetUpdating(FALSE);
}
