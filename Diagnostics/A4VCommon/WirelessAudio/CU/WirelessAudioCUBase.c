//
// WirelessAudioCUBase.c
//

#include "project.h"
#include "WirelessAudioTask.h"
#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioData.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioNV.h"
#include "WirelessAudioUpdate.h"

#include "UpdateManagerTask.h"
#include "DeviceModelTask.h"
#include "BMutex.h"

SCRIBE_DECL(wa_task);

static void WirelessAudioCUBase_HandleEventOccurred(GENERIC_MSG_t* msg);
static void WirelessAudioCUBase_ProcessDataMessage(WA_DataMessage_t* message);
static void WirelessAudioCUBase_CheckForUnlock(SemaphoreHandle_t sem);
static void WirelessAudioCUBase_HandleQuiesce(GENERIC_MSG_t* msg);
static void WirelessAudioCUBase_HandleFD(GENERIC_MSG_t* msg);
static void WirelessAudioCUBase_HandleEraseCoco(GENERIC_MSG_t* msg);
static void WirelessAudioCUBase_HandleSTRadioStatus(GENERIC_MSG_t* msg);
static void WirelessAudioCUBase_HandleReconfigureRFBand(void);
static void WirelessAudioCUBase_HandleUpdate(GENERIC_MSG_t* msg);
static void WirelessAudioCUBase_HandleEnterUpdateTransfer(GENERIC_MSG_t* msg);
static void WirelessAudioCUBase_SetRFBand(GENERIC_MSG_t* msg);

void WirelessAudioCUBase_HandleMessage(GENERIC_MSG_t* msg)
{
    switch(msg->msgID)
    {
        case WA_MSG_ID_HandleEXTI:
            WirelessAudioCUBase_HandleEventOccurred(msg);
            break;
        case WA_MSG_ID_Quiesce:
            WirelessAudioCUBase_HandleQuiesce(msg);
            break;
        case WA_MSG_ID_SendTestMessage:
            WirelessAudioUtilities_SendTestMessage();
            break;
        case WA_MSG_ID_FactoryDefault:
            WirelessAudioCUBase_HandleFD(msg);
            break;
        case WA_MSG_ID_EraseCoco:
            WirelessAudioCUBase_HandleEraseCoco(msg);
            break;
        case WA_MSG_ID_ST_Radio_Status:
            WirelessAudioCUBase_HandleSTRadioStatus(msg);
            break;
        case WA_MSG_ID_ReconfigureRFBand:
            WirelessAudioCUBase_HandleReconfigureRFBand();
            break;
        case WA_MSG_ID_Update_Darr:
            WirelessAudioCUBase_HandleUpdate(msg);
            break;
        case WA_MSG_ID_Enter_Update_Transfer:
            WirelessAudioCUBase_HandleEnterUpdateTransfer(msg);
            break;
        case WA_MSG_ID_ToggleDataMessageDump:
            WirelessAudioUtilities_ToggleDataMessageDump();
            break;
        case WA_MSG_ID_SetRFBand:
            WirelessAudioCUBase_SetRFBand(msg);
            break;
        case WA_MSG_ID_OpenPairingWindow:
            WirelessAudioUtilities_OpenPairingWindow(msg->params[0]);
            break;
        default:
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Unhandled event %s", GetEventString(msg->msgID));
            break;
    }
}

static void WirelessAudioCUBase_HandleEventOccurred(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_ReadAllDataMessages(WirelessAudioCUBase_ProcessDataMessage);
}

static void WirelessAudioCUBase_ProcessDataMessage(WA_DataMessage_t* message)
{
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Base recv message %d", message->opcode);
}

static void WirelessAudioCUBase_CheckForUnlock(SemaphoreHandle_t sem)
{
    if(sem)
    {
        BMutex_Give(sem);
    }
}

static void WirelessAudioCUBase_HandleQuiesce(GENERIC_MSG_t* msg)
{
    WirelessAudioVariant_GoToState(WA_STATE_OFF);
    WirelessAudioCUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioCUBase_HandleFD(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_FactoryDefault();
    WirelessAudioCUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioCUBase_HandleEraseCoco(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_ClearCoco();
    WirelessAudioCUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioCUBase_HandleSTRadioStatus(GENERIC_MSG_t* msg)
{
    uint32_t wlan0 = 0, wlan1 = 0;
    //TODO - function and first parameters undefined
//    DeviceModelTask_GetField(DeviceModel_ST_wlan0, &wlan0);
//    DeviceModelTask_GetField(DeviceModel_ST_wlan1, &wlan1);

    // Convert SM2's frequency to a DARR band ID
    uint32_t sm2Band = wlan0 ? wlan0 : wlan1;
    sm2Band /= 100000; // We receive these in Khz (e.g. 2412000), convert to 24, 52, 58
    uint8_t bandMask = WA_BAND_NONE_BIT_MASK;
    if((sm2Band >= WA_BAND_24_LOWER_LIMIT) &&
       (sm2Band <= WA_BAND_24_UPPER_LIMIT))
    {
        bandMask = WA_BAND_24_BIT_MASK;
    }
    else if((sm2Band >= WA_BAND_52_LOWER_LIMIT) &&
            (sm2Band <= WA_BAND_52_UPPER_LIMIT))
    {
        bandMask = WA_BAND_52_BIT_MASK;
    }
    else if((sm2Band >= WA_BAND_58_LOWER_LIMIT) &&
            (sm2Band <= WA_BAND_58_UPPER_LIMIT))
    {
        bandMask = WA_BAND_58_BIT_MASK;
    }

    uint8_t activeBandsMask = WirelessAudioUtilities_GetActiveBandsMask();
    if(bandMask & activeBandsMask)
    {
        activeBandsMask &= ~bandMask;
        WirelessAudioUtilities_SetActiveBandsMask(activeBandsMask);
        WirelessAudioPostMsg(WA_MSG_ID_ReconfigureRFBand, NOP_CALLBACK, NULL);
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Applying band mask: 0x%X", activeBandsMask);
    }
    else
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "No band changed required: 0x%X", activeBandsMask);
    }
}

static void WirelessAudioCUBase_HandleReconfigureRFBand(void)
{
    uint8_t bands = (uint8_t) WirelessAudioUtilities_GetActiveBands();
    WirelessAudio_I2C_WriteCommand(&bands, WA_CMD_FIX_RF_BAND, sizeof(bands));
}

static void WirelessAudioCUBase_HandleUpdate(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_UpdateAndReprogram();
    WirelessAudioCUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
}

static void WirelessAudioCUBase_HandleEnterUpdateTransfer(GENERIC_MSG_t* msg)
{
    WirelessAudioVariant_GoToState(WA_STATE_UPDATE_SPEAKERS);
    WirelessAudioCUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioCUBase_SetRFBand(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_SetActiveBandsMask(msg->params[0]);
    WirelessAudioPostMsg(WA_MSG_ID_ReconfigureRFBand, NOP_CALLBACK, NULL);
}
