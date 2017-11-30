//
// WirelessAudioCUPairing.h
//

#ifndef WIRELESS_AUDIO_CU_PAIRING_H
#define WIRELESS_AUDIO_CU_PAIRING_H

#include "project.h"

void WirelessAudioCUPairing_EnterState(void);
void WirelessAudioCUPairing_ExitState(void);
BOOL WirelessAudioCUPairing_HandleMessage(GENERIC_MSG_t* msg);

#endif // WIRELESS_AUDIO_CU_PAIRING_H
