#ifndef SYSTEM_BEHAVIORS_H
#define SYSTEM_BEHAVIORS_H

//#include "A4V_IpcProtocol.h"
#include "KeyData.h"
//#include "A4V_IpcProtocol.h"
#include "sourceInterface.h"
//#include "nv_system.h"
#include "nv_mfg.h"
//#include "volumeModel.h"
#include "AccessoryManager.h"
//#include "SystemTimeout.h"

//void SystemBehavior_InitVolumeModel(AudioControl_t* audioSettings);
void SystemBehavior_UIInit(void);
void SystemBehavior_UIColdBoot(BOOL outOfBox);
void SystemBehavior_UITurnBoardOff(void);
BOOL SystemBehavior_HandleKey(KEY_DATA_t key);
void SystemBehavior_DisplayLayerTimerExpired(void);
uint8_t SystemBehavior_CheckDSPEnergyPresent(void);
void SystemBehavior_HandleBTData(IpcPacket_t* packet);
void SystemBehavior_NV_SetTrebleLevel(int8_t trebleLevel);
void SystemBehavior_NV_SetCenterLevel(int8_t centerLevel);
void SystemBehavior_NV_SetSurroundLevel(int8_t surroundLevel);
int8_t SystemBehavior_GetBassMin(void);
int8_t SystemBehavior_GetBassMax(void);
int8_t SystemBehavior_GetBassDefault(void);
int8_t SystemBehavior_GetBassStep(void);
int8_t SystemBehavior_GetTrebleMin(void);
int8_t SystemBehavior_GetTrebleMax(void);
int8_t SystemBehavior_GetTrebleDefault(void);
int8_t SystemBehavior_GetTrebleStep(void);
int8_t SystemBehavior_GetSurroundMin(void);
int8_t SystemBehavior_GetSurroundMax(void);
int8_t SystemBehavior_GetSurroundDefault(void);
int8_t SystemBehavior_GetSurroundStep(void);
int8_t SystemBehavior_GetCenterMin(void);
int8_t SystemBehavior_GetCenterMax(void);
int8_t SystemBehavior_GetCenterDefault(void);
int8_t SystemBehavior_GetCenterStep(void);
void SystemBehavior_AiqEnableImageMonitoring(BOOL state);
BOOL SystemBehavior_Aiq_Monitoring_GetState(void);
void SystemBehavior_ActivateStandbySource(void);
void SystemBehavior_DeactivateStandbySource(void);
void SystemBehavior_HandleAutowake(void);
 void SystemBehavior_AutowakeWakeUpDSPinTVSource(void);
BOOL SystemBehavior_AutowakeGetSilenceState(void);
void SystemBehavior_AutowakeSetSilenceState(BOOL silence);
void SystemBehavior_AutowakeSetSilenceTimerState(BOOL state);
BOOL SystemBehavior_AutowakeGetSilenceTimerState(void);
BOOL SystemBehaviors_GetEDIDReplicate();
SOURCE_ID SystemBehavior_GetCECFirstInput(void);
BOOL SystemBehavior_CECCanSwitchToSource(SOURCE_ID source);
SOURCE_ID SystemBehavior_GetSourceIDFromPort(hdmi_port port);
void SystemBehavior_HandleSystemError(int8_t reason);
int8_t SystemBehavior_GetMaxTurnOnVolume(void);
int8_t SystemBehavior_GetMinTurnOnVolume(void);
LANGUAGE_SELECTION SystemBehavior_GetLanguage(void);
SPOKEN_LANGUAGE SystemBehavior_TransformLangSelectToSpokenLang(LANGUAGE_SELECTION lang);
BOOL SystemBehavior_GetDspWDEnabled(void);
IpcPowerState_t SystemBehavior_PollDspPowerState(IpcPowerState_t desiredState, uint32_t timeoutMs);
void SystemBehavior_HandleDcOffset(BOOL inDcOffset);
KEY_VALUE GetKeyAssignedToHDMIInput();
BOOL SetAssignableHDMIInputID(KEY_VALUE keyValue);
void SystemBehavior_UIOn(void);
void SystemBehavior_UIOff(void);
uint8_t SystemBehavior_GetSpeakerPackage(void);
int8_t SystemBehavior_GetDefaultVolume(void);
int8_t SystemBehavior_GetMaxVolume(void);
int8_t SystemBehavior_GetMinVolume(void);
void SystemBehavior_SetSourceDualMonoSelect(SOURCE_ID src, uint8_t track);
void SystemBehavior_SetSourceAVSync(SOURCE_ID src, uint16_t delay);
void SystemBehavior_SetSourceAudioMode(SOURCE_ID src, uint8_t mode);
uint8_t SystemBehavior_GetSourceDualMonoSelect(SOURCE_ID src);
uint16_t SystemBehavior_GetSourceAVSync(SOURCE_ID src);
uint8_t SystemBehavior_GetSourceAudioMode(SOURCE_ID src);
void SystemBehavior_UnmuteAccessory(void);
void SystemBehavior_HandleHdmiPresence(hdmi_port port);
void SystemBehavior_HandleOOBSetupDone(void);
BOOL SystemBehavior_CheckVideoPresent(void);
SOURCE_ID SystemBehavior_GetDefaultSource(void);
void SystemBehavior_AmpFault(void);
void SystemBehavior_GatherSpeakerInfo(IpcAccessoryList_t* accList);
void SystemBehavior_HandleConsoleCableStatusChanged(BOOL status);
void SystemBehavior_HandleConsoleDcStatusChanged(BOOL status);
void SystemBehavior_HandleNoActiveSourceTimer(void);
void SystemBehavior_HandleLlwNetworkStatus(BOOL connected);
void SystemBehavior_HandleMuPairingClosed(BOOL success);
BOOL SystemBehavior_IsSourceKey(KEY_VALUE key);
uint16_t SystemBehavior_GetCurrentAVSync(void);
void SystemBehavior_ResetSourceAudioParameters(SOURCE_ID src);
void SystemBehavior_HandleLlwPairingClosed(void);
void SystemBehavior_PlayToneToConnected(PairingToneState_t toneState);

void SystemBehavior_AiQExit(void);
void SystemBehavior_SourceListComplete(IpcCompleteStatus_t status);
void SystemBehavior_CreateMoreButtons(void);
void SystemBehavior_ClearMoreButtons(void);

void SystemBehavior_ScreenSaverTriggered(void);
void SystemBehavior_ScreenSaverReset(void);
void SystemBehavior_HandleTimeoutCommand(IpcPacket_t* packet);
void SystemBehavior_HandleHospitalityResponse(BOOL success);
//eSystem_Timeout_t SystemBehavior_GetDefaultTimeouts(void);
BOOL SystemBehavior_IsSourceChangeAllowed(SOURCE_ID currentSource, SOURCE_ID desiredSource);

#endif //SYSTEM_BEHAVIORS_H
