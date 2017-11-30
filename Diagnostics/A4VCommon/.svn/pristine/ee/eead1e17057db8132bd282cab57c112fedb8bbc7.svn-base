//
// WirelessAudioVariantUtils.h - Variant specific utilities that are
//                               implemented across all platforms
//

#ifndef WIRELESS_AUDIO_VARIANT_UTILS_H
#define WIRELESS_AUDIO_VARIANT_UTILS_H

#include "project.h"

void WirelessAudioVariantUtils_Init(void);
BOOL WirelessAudioVariantUtils_InitializeDarr(void);
uint32_t WirelessAudioVariantUtils_GetBroadcastAddress(void);
void WirelessAudioVariantUtils_GetDefaultCoco(uint8_t* coco);
BOOL WirelessAudioVariantUtils_IsDefaultCoco(uint8_t* coco);
uint8_t WirelessAudioVariantUtils_GetChannel(void);
void WirelessAudioVariantUtils_SendTelemetry(uint32_t serialNumberCRC);
void WirelessAudioVariantUtils_PeriodicService(void);
uint8_t WirelessAudioVariantUtils_GetActiveBandsMask(void);
void WirelessAudioVariantUtils_SetActiveBandsMask(uint8_t bandMask);
BOOL WirelessAudioVariantUtils_IsPairingNeeded(void);
BOOL WirelessAudioVariantUtils_DoesRegionAllowPair(void);
BOOL WirelessAudioVariantUtils_WasDarrQuickBooted(void);
void WirelessAudioVariantUtils_EnableFullInitSequence(void);

#endif // WIRELESS_AUDIO_VARIANT_UTILS_H
