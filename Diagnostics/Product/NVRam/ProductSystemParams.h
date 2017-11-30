//
//ProductSystemParams.h
//
#ifndef PRODUCT_SYSTEM_PARAMS_H
#define PRODUCT_SYSTEM_PARAMS_H

#include "UpdateManagerBlob.h"
#include "nv_system.h"
#define PRODUCT_SYSTEM_STRUCT_VERSION 1

typedef struct
{
    uint8_t lastVolume;
    uint8_t lastSource;
    uint8_t systemMode;
    uint8_t screensaverEnabled;
    uint8_t unused2;
    uint8_t dceFullBootFromPowerOn;
    uint8_t language;
    uint8_t enableAIQ;
    uint8_t enableAutoOff;
    uint8_t audioMode;
    uint8_t unused3[4];
    uint8_t centerLevel;
    uint8_t surroundLevel;
    uint8_t bassLevel;          //This might not be accessable from the console.
    uint8_t trebleLevel;
    uint8_t audioDelay;         // range is 0-30 (for 0 to 300 MS)
    uint8_t hpMode;
    uint8_t ignoreAmpFault;     //Open space, shrink padding when adding params
    uint8_t cecEnabled;
    uint32_t cecFeatures;
    uint8_t inputAssignment[4]; //Assignment of Optical, Coax, and Rear Analog inputs
    uint32_t chipconRemoteAddress;
    uint8_t lastChipconPrimaryChannel;
    uint8_t lastChipconSecondaryChannel;
    uint8_t chipconChanTableIndex;
    uint8_t chipconRemoteType;
    uint8_t isAutoWakeEnabled;
    uint8_t remoteKeySelectsSingleHDMIInput;
    uint8_t skipSpeakerUpdate;
    uint8_t isDspWDEnabled;
    uint8_t expectedSpeakerConfig; // used to derive DeviceModel_DSP_SpeakerPackage and DeviceModel_DSP_AccessoryFlags sent to DSP
    uint8_t speakerAccessories;
    char blobVersion[MASTER_VERSION_STRING_LENGTH];
    uint8_t ignoreKeysDuringSourceChange;
    uint8_t speakerPackageVariant;
} NV_ProductSysParams_t;

void NV_CopyDefaultSystemParametersToRAM(void);

uint8_t NV_GetRemoteKeyThatSelectsHDMISingleInput(void);
void NV_SetRemoteKeyThatSelectsHDMISingleInput(uint8_t key);

BOOL NV_GetIgnoreAmpFault (void);
void NV_SetIgnoreAmpFault (BOOL ignore);

BOOL NV_GetScreensaverEnabled(void);
void NV_SetScreensaverEnabled(BOOL enabled);

// Chipcon user parameter APIs
uint32_t NV_GetChipconRemoteAddress(void);
void NV_SetChipconRemoteAddress(uint32_t newAddr);
uint8_t NV_GetChipconRemoteType(void);
void NV_SetChipconRemoteType(uint8_t rmType);
uint8_t NV_GetLastChipconPrimaryChannel (void);
void NV_SetLastChipconPrimaryChannel (uint8_t channel);
uint8_t NV_GetLastChipconSecondaryChannel (void);
void NV_SetLastChipconSecondaryChannel (uint8_t channel);
uint8_t NV_GetChipconChanTableIndex (void);
void NV_SetChipconChanTableIndex (uint8_t index);

SYSTEM_MODE NV_GetSystemMode(void);
void NV_SetSystemMode(SYSTEM_MODE mode);

void NV_SetLanguage(LANGUAGE_SELECTION lang);
LANGUAGE_SELECTION NV_GetLanguage(void);

uint8_t NV_GetAutoOff(void);
void NV_SetAutoOff(uint8_t val);

TV_INPUT_ALT_ASSIGNMENT NV_GetTVInputAssignment(TV_INPUT_OPTION_NAME opt);
void NV_SetTVInputAssignment(TV_INPUT_OPTION_NAME opt, TV_INPUT_ALT_ASSIGNMENT assign);

uint8_t NV_GetCenterLevel(void);
void NV_SetCenterLevel(uint8_t val);

uint8_t NV_GetSurroundLevel(void);
void NV_SetSurroundLevel(uint8_t val);

uint8_t NV_GetBassLevel(void);
void NV_SetBassLevel(uint8_t val);

uint8_t NV_GetTrebleLevel(void);
void NV_SetTrebleLevel(uint8_t val);

uint8_t NV_GetAudioMode(void);
void NV_SetAudioMode(uint8_t val);

uint8_t NV_GetAVSync(void);
void NV_SetAVSync(uint8_t val);

uint8_t NV_GetHPMode(void);
void NV_SetHPMode(uint8_t val);

uint8_t NV_GetCECMode(void);
void NV_SetCECMode(uint8_t val);

uint32_t NV_GetCECFeatures(void);

uint8_t NV_GetDceFullBootFromPowerOn (void);
BOOL NV_SetDceFullBootFromPowerOn (uint8_t on);

BOOL NV_GetAutoWakEnabled(void);
void NV_SetAutoWakeEnabled(BOOL enabled);

#ifdef SUPPORT_DSPWATCHDOG
BOOL NV_GetDspWDEnabled(void);
void NV_SetDspWDEnabled(BOOL enabled);
#endif

uint8_t NV_GetLastSource(void);
void NV_SetLastSource(uint8_t source);
uint8_t NV_GetLastVolume(void);
void NV_SetLastVolume(uint8_t volume);

uint8_t NV_GetSkipSpeakerUpdate(void);
void NV_SetSkipSpeakerUpdate(BOOL skip);

uint8_t NV_GetExpectedSpeakerConfig(void);
void NV_SetExpectedSpeakerConfig(uint8_t config);

uint8_t NV_GetSpeakerAccessories(void);
void NV_SetSpeakerAccessories(uint8_t accessory);

IpcSpeakerPackage_t NV_GetSpeakerPackageVariant(void);
void NV_SetSpeakerPackageVariant(IpcSpeakerPackage_t variant);

void NV_GetBlobVersion(char* version);
void NV_SetBlobVersion(char* version);
void NV_SetBlobVersionFromStoredBlob(void);

BOOL NV_GetIgnoreKeysDuringSourceChange(void);
void NV_SetIgnoreKeysDuringSourceChange(BOOL val);

BOOL NV_GetAIQEnabled(void);
void NV_SetAIQEnabled(BOOL val);

#endif
