/*
    File    :   WirelessAudioMUUpdRx.c
    Title   :   
    Author  :   dr1005920
    Created :   01/05/16
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:  Implementation of file transfer over DARR data channel for MU

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

#include "AmpTask.h"
#include "AmpMute.h"


SCRIBE_DECL(wa_update);

#define WA_UPDATE_WAIT_ENTER_UPDATE_TIME_MS 10000
#define WA_UPDATE_WAIT_RESPONSE_TIME_MS 3500
#define WA_UPDATE_POLL_TIME_MS 5000  

static BOOL firstChunk = TRUE;
static BOOL mapCleared = FALSE;
static BOOL updateForced = FALSE;
/***************************** WA_UPDATE_IDLE *************************************/
void waTransfer_HandleMessageFunc_WA_UPDATE_IDLE(WA_DataMessage_t* msg)
{
    switch(msg->opcode)
    {
        case WA_BCMD_ENTER_UPDATE:
            updateForced = msg->data[0];
            waTransfer_GoToNextState(WA_RECIEVING_DATA);
            break;
        case WA_BCMD_UPD_VERSION:
            waTransfer_MUReportVersion();
            break;
        default:
            LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled opcode 0x%02X at line %d", msg->opcode, __LINE__);
            break;
    }
}

void waTransfer_ExitFunc_WA_UPDATE_IDLE(void)
{
    Amp_DisableStatusMonitor();
    AmpMute_SetMute(AMP_MUTE_UPDATE, TRUE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(250));
    Amp_Enable(FALSE);

    // Erase update storage
    InternalFlashAPI_EraseSectorByAddress(INTERNAL_FLASH_UPDATE_STORAGE_START);
    InternalFlashAPI_EraseSectorByAddress(INTERNAL_FLASH_UPDATE_STORAGE_START + INTERNAL_FLASH_SECTOR_SIZE_128KB);

    // Reset all values
    myTransferState.currentChunk = 0;
    firstChunk = TRUE;
    mapCleared = FALSE;
    
    // Send Ready to update
    waTransfer_MUSendCommand(WA_BCMD_READY_TO_UPDATE);
    
    //set the updating flag so MU stops sending TM data or monitor network status
    waSetUpdating(TRUE);
}

/**************************** WA_RECIEVING_DATA ************************************/
void waTransfer_EnterFunc_WA_RECIEVING_DATA(void)
{
    changeTimerPeriod(myTransferState.timer, WA_UPDATE_MU_UPDATE_TIMEOUT_MS, 0);
    timerStart(myTransferState.timer, 0, &wirelessAudioTaskHandle->Queue);
}

void waTransfer_HandleMessageFunc_WA_RECIEVING_DATA(WA_DataMessage_t* msg)
{
    switch(msg->opcode)
    {
        case WA_BCMD_UPD_DATA:
            mapCleared = FALSE;
            waTransfer_RecieveTransferData(msg);
            break;
        case WA_BCMD_MAP_REQUEST:
            waTransfer_HandleMapRequest();
            break;
        case WA_BCMD_CLEAR_MAP:
            memset(myTransferState.myCurrentMap, 0x00, WA_TRANSFER_MAP_LENGTH);
            if(!firstChunk && !mapCleared)
            {
                myTransferState.currentChunk++;
            }
            firstChunk = FALSE;
            mapCleared = TRUE;
            waTransfer_MUSendCommand(WA_BCMD_MAP_CLEARED);
            break;
        case WA_BCMD_UPD_AUTHENTICATE:
            waTransfer_MUSendAuthentication();
            break;
        case WA_BCMD_ENTER_UPDATE:
            // CU missed our ready to update so send it again
            waTransfer_MUSendCommand(WA_BCMD_READY_TO_UPDATE);
            break;
        case WA_BCMD_UPD_INSTALL:
            waTransfer_GoToNextState(WA_UPDATE_WAIT_INSTALL);
            break;
        default:
            LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled opcode 0x%02X at line %d", msg->opcode, __LINE__);
            break;
    }
    timerReset(myTransferState.timer, 0, &wirelessAudioTaskHandle->Queue);
}

void waTransfer_HandeTimerFunc_WA_RECIEVING_DATA(void)
{
    static uint8_t resetCount = 0;

    LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "No msgs. Reset.");
    WirelessAudioUtilities_InitializeDarr(FALSE);

    if(++resetCount > WA_UPDATE_MAX_RESETS_MU)
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Timed out.");
        waSetUpdating(FALSE);
        vTaskDelay(TIMER_MSEC_TO_TICKS(2000));
        system_reboot();
    }
    else
    {
        timerReset(myTransferState.timer, 0, &wirelessAudioTaskHandle->Queue);
    }
}

/************************* WA_UPDATE_WAIT_INSTALL *********************************/
void waTransfer_EnterFunc_WA_UPDATE_WAIT_INSTALL(void)
{
    changeTimerPeriod(myTransferState.timer, WA_UPDATE_MU_WAIT_INSTALL_MS, 0);
    timerStart(myTransferState.timer, 0, &wirelessAudioTaskHandle->Queue);
}

void waTransfer_HandleMessageFunc_WA_UPDATE_WAIT_INSTALL(WA_DataMessage_t* msg)
{
    switch(msg->opcode)
    {
        case WA_BCMD_UPD_INSTALL:
            timerReset(myTransferState.timer, 0, &wirelessAudioTaskHandle->Queue);
            waTransfer_MUSendCommand(WA_BCMD_UPD_INSTALL);
            break;
        default:
            LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled opcode 0x%02X in wait install", msg->opcode);
            break;
    }
}

void waTransfer_HandleTimerFunc_WA_UPDATE_WAIT_INSTALL(void)
{
    waTransfer_GoToNextState(WA_UPDATE_INSTALLING);
}

/**************************** WA_UPDATE_INSTALL ************************************/
void waTransfer_EnterFunc_WA_UPDATE_INSTALL(void)
{
    UI_ViewNotify(UPDATE_STATUS_CHANGE);
    waTransfer_DoWAMUpdate(updateForced);

    UpdateModuleMicro_EnterDoMicro();
}
