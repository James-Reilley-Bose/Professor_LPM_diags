//
// WirelessAudioMUBase.c
//

#include "project.h"
#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioData.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioUpdate.h"
#include "WirelessAudioAPI.h"

SCRIBE_DECL(wa_task);

static void WirelessAudioMUBase_HandleEventOccurred(GENERIC_MSG_t* msg);
static void WirelessAudioMUBase_ProcessDataMessage(WA_DataMessage_t* message);
static void WirelessAudioMUBase_CheckForUnlock(SemaphoreHandle_t sem);
static void WirelessAudioMUBase_HandleQuiesce(GENERIC_MSG_t* msg);
static void WirelessAudioMUBase_HandleFD(GENERIC_MSG_t* msg);
static void WirelessAudioMUBase_HandleEraseCoco(GENERIC_MSG_t* msg);
static void WirelessAudioMUBase_HandleUpdate(GENERIC_MSG_t* msg);
static void WirelessAudioMUBase_HandleInit(GENERIC_MSG_t* msg);
static void WirelessAudioMUBase_HandleQuickToFullBoot(GENERIC_MSG_t* msg);

void WirelessAudioMUBase_HandleMessage(GENERIC_MSG_t* msg)
{
    switch(msg->msgID)
    {
        case WA_MSG_ID_HandleEXTI:
            WirelessAudioMUBase_HandleEventOccurred(msg);
            break;
        case WA_MSG_ID_Quiesce:
            WirelessAudioMUBase_HandleQuiesce(msg);
            break;
        case WA_MSG_ID_SendTestMessage:
            WirelessAudioUtilities_SendTestMessage();
            break;
        case WA_MSG_ID_FactoryDefault:
            WirelessAudioMUBase_HandleFD(msg);
            break;
        case WA_MSG_ID_EraseCoco:
            WirelessAudioMUBase_HandleEraseCoco(msg);
            break;
        case WA_MSG_ID_Update_Darr:
            WirelessAudioMUBase_HandleUpdate(msg);
            break;
        case WA_MSG_ID_ToggleDataMessageDump:
            WirelessAudioUtilities_ToggleDataMessageDump();
            break;
        case WA_MSG_ID_Init:
            WirelessAudioMUBase_HandleInit(msg);
            break;
        case WA_MSG_ID_OpenPairingWindow:
            WirelessAudioUtilities_OpenPairingWindow(msg->params[0]);
            break;
        case WA_MSG_ID_QuickToFullBoot:
            WirelessAudioMUBase_HandleQuickToFullBoot(msg);
            break;
        default:
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Unhandled event %d", msg->msgID);
            break;
    }
}

static void WirelessAudioMUBase_HandleEventOccurred(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_ReadAllDataMessages(WirelessAudioMUBase_ProcessDataMessage);
}

static void WirelessAudioMUBase_ProcessDataMessage(WA_DataMessage_t* message)
{
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Base recv message %d", message->opcode);
}

static void WirelessAudioMUBase_CheckForUnlock(SemaphoreHandle_t sem)
{
    if(sem)
    {
        BMutex_Give(sem);
    }
}

static void WirelessAudioMUBase_HandleQuiesce(GENERIC_MSG_t* msg)
{
    WirelessAudioVariant_GoToState(WA_STATE_OFF);
    WirelessAudioMUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioMUBase_HandleFD(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_FactoryDefault();
    WirelessAudioMUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioMUBase_HandleEraseCoco(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_ClearCoco();
    WirelessAudioMUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioMUBase_HandleUpdate(GENERIC_MSG_t* msg)
{
    WirelessAudioUtilities_UpdateAndReprogram();
    WirelessAudioMUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioMUBase_HandleInit(GENERIC_MSG_t* msg)
{
    if(!WirelessAudioUtilities_InitializeDarr(TRUE))
    {
        WirelessAudioVariant_GoToState(WA_STATE_ERROR);
    }
    WirelessAudioMUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}

static void WirelessAudioMUBase_HandleQuickToFullBoot(GENERIC_MSG_t* msg)
{
    WirelessAudioVariant_GoToState(WA_STATE_OFF);
    WirelessAudioPostMsg(WA_MSG_ID_Init, NOP_CALLBACK, NULL);
    WirelessAudioMUBase_CheckForUnlock((SemaphoreHandle_t) msg->params[0]);
}
