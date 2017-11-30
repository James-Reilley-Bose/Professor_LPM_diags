//
// WirelessAudioCUTask.c
//

#include "project.h"
#include "WirelessAudioVariant.h"
#include "WirelessAudioCUTask.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioUtilities.h"

#include "WirelessAudioCUBase.h"
#include "WirelessAudioCUOff.h"
#include "WirelessAudioCUOn.h"
#include "WirelessAudioCUPairing.h"
#include "WirelessAudioCUUpdate.h"

SCRIBE_DECL(wa_task);

static const struct WA_STATE_HANDLERS WAStateHandlers[WA_NUM_STATES] =
{
    // WA_STATE_OFF
    {
        WirelessAudioCUOff_HandleMessage,
        NULL,
        WirelessAudioCUOff_EnterState,
        WirelessAudioCUOff_ExitState,
    },

    // WA_STATE_ON
    {
        WirelessAudioCUOn_HandleMessage,
        NULL,
        NULL,
        NULL,
    },

    // WA_STATE_PAIRING
    {
        WirelessAudioCUPairing_HandleMessage,
        NULL,
        WirelessAudioCUPairing_EnterState,
        WirelessAudioCUPairing_ExitState,
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
        WirelessAudioCUUpdate_HandleMessage,
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
        WirelessAudioCUBase_HandleMessage(msg);
    }
}

void WirelessAudioVariant_GoToState(WAState_t state)
{
    if((state >= WA_NUM_STATES) || (state == WAState)) return;

    if(WAStateHandlers[WAState].ExitState) WAStateHandlers[WAState].ExitState();

    PreviousWAState = WAState;
    WAState = state;
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "WA CU state transition %d -> %d", PreviousWAState, WAState);

    if(WAStateHandlers[WAState].EnterState) WAStateHandlers[WAState].EnterState();
}

WAState_t WirelessAudioVariant_GetState(void)
{
    return WAState;
}

BOOL WirelessAudioVariant_IsConnected(void)
{
    // TODO: fix this, this just makes it compile
    return FALSE;
}
