//
// WirelessAudioVariant.h
//

#ifndef WIRELESS_AUDIO_VARIANT_H
#define WIRELESS_AUDIO_VARIANT_H

#include "QueueManager.h"
#include "WirelessAudioUtilities.h"

typedef enum
{
    WA_STATE_OFF,
    WA_STATE_ON,
    WA_STATE_PAIRING,
    WA_STATE_ERROR,
    WA_STATE_UPDATE_SPEAKERS,
    WA_STATE_QUICK_BOOTED,

    WA_NUM_STATES
} WAState_t;

struct WA_STATE_HANDLERS
{
    BOOL (*HandleMessage)(GENERIC_MSG_t*);
    void (*Init)(void);
    void (*EnterState)(void);
    void (*ExitState)(void);
};

void WirelessAudioVariant_Init(void);
void WirelessAudioVariant_HandleMessage(GENERIC_MSG_t* msg);
void WirelessAudioVariant_GoToState(WAState_t state);
WAState_t WirelessAudioVariant_GetState(void);
BOOL WirelessAudioVariant_IsConnected(void);

#endif // WIRELESS_AUDIO_VARIANT_H
