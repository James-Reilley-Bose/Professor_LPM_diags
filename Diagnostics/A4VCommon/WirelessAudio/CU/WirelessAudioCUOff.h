//
// WirelessAudioCUOff.h
//

#ifndef WIRELESS_AUDIO_CU_OFF_H
#define WIRELESS_AUDIO_CU_OFF_H

#include "project.h"

void WirelessAudioCUOff_EnterState(void);
BOOL WirelessAudioCUOff_HandleMessage(GENERIC_MSG_t* msg);
void WirelessAudioCUOff_ExitState(void);

#endif // WIRELESS_AUDIO_CU_OFF_H
