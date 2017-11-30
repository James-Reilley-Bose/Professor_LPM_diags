//
// WirelessAudioMUOff.c
//

#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "AmpMute.h"
#include "UITask.h"

SCRIBE_DECL(wa_task);

static void WirelessAudioMUOff_CheckForUnlock(GENERIC_MSG_t* msg);

void WirelessAudioMUOff_EnterState(void)
{
    AmpMute_SetMute(AMP_MUTE_DARR, TRUE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(50));
    WirelessAudioUtilities_TurnOff();
}

BOOL WirelessAudioMUOff_HandleMessage(GENERIC_MSG_t* msg)
{
    if(msg->msgID == WA_MSG_ID_Init)
    {
        if(WirelessAudioUtilities_InitializeDarr(TRUE))
        {
            WirelessAudioVariant_GoToState(WA_STATE_ON);
        }
        else
        {
            WirelessAudioVariant_GoToState(WA_STATE_ERROR);
        }
    }

    WirelessAudioMUOff_CheckForUnlock(msg);

    return TRUE;
}

void WirelessAudioMUOff_ExitState(void)
{
    UIPostMsg(UI_MSG_ID_LLW_RadioInitialized, NOP_CALLBACK, 0);
}

static void WirelessAudioMUOff_CheckForUnlock(GENERIC_MSG_t* msg)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t) msg->params[0];
    switch(msg->msgID)
    {
        case WA_MSG_ID_Init:
        case WA_MSG_ID_FactoryDefault:
        case WA_MSG_ID_Enter_Update_Transfer:
        case WA_MSG_ID_Quiesce:
            if(sem) BMutex_Give(sem);
            break;
    }
}
