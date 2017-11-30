//
// WirelessAudioCUOff.c
//

#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "UITask.h"
#include "BMutex.h"

SCRIBE_DECL(wa_task);

static void WirelessAudioCUOff_HandleInit(GENERIC_MSG_t* msg);

void WirelessAudioCUOff_EnterState(void)
{
    WirelessAudioUtilities_TurnOff();
}

BOOL WirelessAudioCUOff_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;
    switch(msg->msgID)
    {
        case WA_MSG_ID_Init:
            WirelessAudioCUOff_HandleInit(msg);
            break;
        case WA_MSG_ID_ST_Radio_Status:
            // Let this one trickle down to the base
            handled = FALSE;
            break;
    }
    return handled;
}

static void WirelessAudioCUOff_HandleInit(GENERIC_MSG_t* msg)
{
    if(WirelessAudioUtilities_InitializeDarr(TRUE))
    {
        WirelessAudioVariant_GoToState(WA_STATE_ON);
    }
    else
    {
        WirelessAudioVariant_GoToState(WA_STATE_ERROR);
    }

    SemaphoreHandle_t sem = (SemaphoreHandle_t) msg->params[0];
    if(sem)
    {
        BMutex_Give(sem);
    }
}

void WirelessAudioCUOff_ExitState(void)
{
    UIPostMsg(UI_MSG_ID_LLW_RadioInitialized, NOP_CALLBACK, 0);
}
