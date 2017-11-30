//
// WirelessAudioAPI.h
//

#ifndef WIRELESS_AUDIO_API_H
#define WIRELESS_AUDIO_API_H

#include "WirelessAudioTask.h"

#ifdef IS_CONSOLE
#define WA_DEFAULT_COCO {0xFF, 0xFF, 0xFF}
#define WA_VARIANT_DARR_FILE_NAME "darr.bin"
#else
#define WA_DEFAULT_COCO {0xFE, 0xFE, 0xFE}
#define WA_VARIANT_DARR_FILE_NAME "darr.bin"
#endif


void WirelessAudio_SetStandby(BOOL enterStandby, BOOL synchronous);
void WirelessAudio_PostAndWaitFor(MESSAGE_ID_t msg);
void WirelessAudio_FactoryDefault(void);
BOOL WirelessAudio_IsPreviouslyPaired(void);
uint8_t WirelessAudio_GetVersion(void);
WAState_t WirelessAudio_GetState(void);
BOOL WirelessAudio_WasDarrQuickBooted(void);
void WirelessAudio_QuickToFullBoot(void);
BOOL WirelessAudio_IsDarrUpdating(void);

#endif // WIRELESS_AUDIO_API_H
