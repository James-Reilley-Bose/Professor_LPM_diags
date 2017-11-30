//
// WirelessAudioUtilities.h
//

#ifndef WIRELESS_AUDIO_UTILITIES_H
#define WIRELESS_AUDIO_UTILITIES_H

#include "project.h"
#include "WirelessAudioData.h"
#include "WirelessAudioI2C.h"

#define WA_DARR_INIT_RETRIES 3

void WirelessAudioUtilities_Init(void);
void WirelessAudioUtilities_ConfigureIO(void);
void WirelessAudioUtilities_ResetDarr(void);
void WirelessAudioUtilities_AssertReset(BOOL assert);
BOOL WirelessAudioUtilities_WaitForInitBit(void);
void WirelessAudioUtilities_TurnOff(void);
void WirelessAudioUtilities_TurnOn(void);
void WirelessAudioUtilities_ReadAllDataMessages(void (*processMessageCallback)(WA_DataMessage_t*));
BOOL WirelessAudioUtilities_InitializeDarr(BOOL updateCoco);
uint32_t WirelessAudioUtilities_GetBroadcastAddress(void);
void WirelessAudioUtilities_GetDefaultCoco(uint8_t* coco);
BOOL WirelessAudioUtilities_IsDefaultCoco(uint8_t* coco);
BOOL WirelessAudioUtilities_ReadCoco(uint8_t* coco);
void WirelessAudioUtilities_UpdateCoco(void);
void WirelessAudioUtilities_GetCurrentCoco(uint8_t* coco);
void WirelessAudioUtilities_SendIdAck(WA_ChannelID_t channel);
void WirelessAudioUtilities_SendSNAck(WA_ChannelID_t channel, uint8_t* sn);
uint16_t WirelessAudioUtilities_GetChannelBitMask(WA_ChannelID_t channel);
void WirelessAudioUtilities_OpenPairingWindow(BOOL open);
void WirelessAudioUtilities_ProgramCoco(uint8_t* coco);
void WirelessAudioUtilities_ProgramCocoFromMfg(void);
BOOL WirelessAudioUtilities_IsLinked(void);
void WirelessAudioUtilities_SendID(void);
uint8_t WirelessAudioUtilities_GetVersion(void);
void WirelessAudioUtilities_SendSN(void);
void WirelessAudioUtilities_SendTelemetry(void);
void WirelessAudioUtilities_PeriodicService(void);
uint8_t WirelessAudioUtilities_GetChannel(void);
void WirelessAudioUtilities_SendTestMessage(void);
uint32_t WirelessAudioUtilities_CalculateSerialCRC(char* serialNumber);
uint16_t WirelessAudioUtilities_GetCocoSum(void);
void WirelessAudioUtilities_FactoryDefault(void);
void WirelessAudioUtilities_ClearCoco(void);
BOOL WirelessAudioUtilities_IsPreviouslyPaired(void);
uint8_t WirelessAudioUtilities_ConvertBandToMask(WA_Band_t band);
WA_Band_t WirelessAudioUtilities_ConvertMaskToBand(uint8_t bandMask);
uint8_t WirelessAudioUtilities_GetActiveBandsMask(void);
void WirelessAudioUtilities_SetActiveBandsMask(uint8_t bandMask);
WA_Band_t WirelessAudioUtilities_GetActiveBands(void);
BOOL WirelessAudioUtilities_IsPairingNeeded(void);
BOOL WirelessAudioUtilities_IsPairingActive(void);
void WirelessAudioUtilities_UpdateAndReprogram(void);
BOOL WirelessAudioUtilities_DoesRegionAllowPair(void);
void WirelessAudioUtilities_ToggleDataMessageDump(void);
BOOL WirelessAudioUtilities_GetDataMessageDumpEnabled(void);
void WirelessAudioVariantUtils_Reboot(void);
BOOL WirelessAudioUtilities_WasDarrQuickBooted(void);

#endif // WIRELESS_AUDIO_UTILITIES_H
