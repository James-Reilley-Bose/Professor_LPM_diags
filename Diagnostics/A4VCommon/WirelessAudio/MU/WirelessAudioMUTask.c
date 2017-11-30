//
// WirelessAudioMUTask.c
//

#include "project.h"
#include "WirelessAudioVariant.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioUtilities.h"
#include "AmpMute.h"
#include "viewNotify.h"

#include "WirelessAudioMUBase.h"
#include "WirelessAudioMUOff.h"
#include "WirelessAudioMUPairing.h"
#include "WirelessAudioMUOn.h"
#include "WirelessAudioMUUpdate.h"

SCRIBE_DECL(wa_task);

static const struct WA_STATE_HANDLERS WAStateHandlers[WA_NUM_STATES] =
{
    // WA_STATE_OFF
    {
        WirelessAudioMUOff_HandleMessage,
        NULL,
        WirelessAudioMUOff_EnterState,
        WirelessAudioMUOff_ExitState,
    },

    // WA_STATE_ON
    {
        WirelessAudioMUOn_HandleMessage,
        NULL,
        WirelessAudioMUOn_EnterState,
        WirelessAudioMUOn_ExitState,
    },

    // WA_STATE_PAIRING
    {
        WirelessAudioMUPairing_HandleMessage,
        NULL,
        WirelessAudioMUPairing_EnterState,
        WirelessAudioMUPairing_ExitState,
    },

    // WA_STATE_ERROR
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    
    // WA_STATE_UPDATE_SPEAKERS
    {
        WirelessAudioMUUpdate_HandleMessage,
        NULL,
        NULL,
        NULL,
    },
    
    // WA_STATE_QUICK_BOOTED
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

static WAState_t WAState = WA_STATE_OFF;
static WAState_t PreviousWAState = WA_STATE_OFF;

void WirelessAudioVariant_Init(void)
{
    WirelessAudioUtilities_ConfigureIO();
    WirelessAudio_I2C_Init();
    AmpMute_SetMute(AMP_MUTE_DARR, TRUE);

    for(WAState_t state = WA_STATE_OFF; state < WA_NUM_STATES; state++)
    {
        if(WAStateHandlers[state].Init) WAStateHandlers[state].Init();
    }
}

void WirelessAudioVariant_HandleMessage(GENERIC_MSG_t* msg)
{
    if(!WAStateHandlers[WAState].HandleMessage ||
       !WAStateHandlers[WAState].HandleMessage(msg))
    {
        WirelessAudioMUBase_HandleMessage(msg);
    }
}

void WirelessAudioVariant_GoToState(WAState_t state)
{
    if((state >= WA_NUM_STATES) || (state == WAState)) return;

    if(WAStateHandlers[WAState].ExitState) WAStateHandlers[WAState].ExitState();

    PreviousWAState = WAState;
    WAState = state;
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "WA MU state transition %d -> %d", PreviousWAState, WAState);

    if(WAStateHandlers[WAState].EnterState) WAStateHandlers[WAState].EnterState();
    UI_ViewNotify(ACCESSORY_STATE_CHANGED);
}

WAState_t WirelessAudioVariant_GetState(void)
{
    return WAState;
}

BOOL WirelessAudioVariant_IsConnected(void)
{
    BOOL connected = FALSE;
    if(WAState == WA_STATE_ON)
    {
        connected = WirelessAudioMUOn_GetCachedConnectionStatus();
    }
    return connected;
}
