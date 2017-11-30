//
//ProductSystemParams.c
//
#include "project.h"
#include "nv.h"
#include "nv_system.h"
#include "nv_system_struct.h"
#include "nvram.h"
//#include "volumeModel.h"
#include "CEC_ControlInterface.h"
#include "globalParameterList.h"
#include "buffermanager.h"
#include "InternalFlashAPI.h"
#include "event_groups.h"
#include "UpdateManagerBlob.h"
#include "systemBehaviors.h"
#include "ProductSystemParams.h"
#include "CEC_Utilities.h"
#ifdef BARDEEN
#include "UEITask.h"
#endif

extern NV_SystemParams_t systemParams;

const NV_SystemParams_t s_NVSystemParamDefaults =
{
    .sysSettings = {
        .firstTimeInit           =  0x01,
        .rebootOnAssert          = TRUE,
        .systemStructVersion     = NV_SYSTEM_VERSION,
    },
    .product = {
        .lastVolume              = 0,
        .lastSource              = 0,
        .systemMode              = 0, //Normal vs. Hospitality (uint8_t)SYSTEM_MODE_NORMAL,
        .screensaverEnabled      = 0,
        .unused2                 = 0,
        .dceFullBootFromPowerOn  = FALSE,
        .language                = LANGUAGE_ENGLISH,
        .enableAIQ               = FALSE,
        .enableAutoOff           = TRUE,
        .audioMode               = 0x02, //0x02 == IPC_AUDIO_MODE_NORMAL in A4V_IpcProtocol.h
        .unused3                 = { 0, 0, 0, 0 },
        .centerLevel             = 0,
        .surroundLevel           = 0,
        .bassLevel               = 0,
        .trebleLevel             = 0,
        .audioDelay              = 0,
        .hpMode                  = 0,
        .ignoreAmpFault          = FALSE,
        .cecEnabled              = CEC_MODE_COMPLIANCE,
        //TODO - BRING IN CEC?
        //.cecFeatures             = CEC_FEATURE_ALL,
        .inputAssignment         = {TV_INPUT_ALT_ASSIGNMENT_TV_AUTO, TV_INPUT_ALT_ASSIGNMENT_TV_AUTO, TV_INPUT_ALT_ASSIGNMENT_TV_AUTO, TV_INPUT_ALT_ASSIGNMENT_TV_AUTO},
        .chipconRemoteAddress    = UNPAIRED_ADDRESS,
        .lastChipconPrimaryChannel   = 0xed,
        .lastChipconSecondaryChannel = 0xe1,
        .chipconChanTableIndex       = 0,
        .chipconRemoteType           = (uint8_t)REMOTE_TYPE_UNKNOWN,
        .isAutoWakeEnabled       = FALSE,
        .remoteKeySelectsSingleHDMIInput = BOSE_INVALID_KEY,
        .skipSpeakerUpdate           = 0,
        .isDspWDEnabled          = TRUE,
        .expectedSpeakerConfig   = 0,
        .speakerAccessories      = 0,
        .blobVersion             = {0},
    },
    //TODO - re-add uniform source list?
    //.sourceListLength = (sizeof(UnifySourceStruct) * MAX_NUM_SOURCE_FOR_NV_STORAGE),
    .systemCtrlWord = NV_VALID_CTRL_WORD,
};

void NV_CopyDefaultSystemParametersToRAM(void)
{
    memcpy(&systemParams, &s_NVSystemParamDefaults, sizeof(NV_SystemParams_t));

    systemParams.product.language = NV_GetDefaultLanguage();
    systemParams.product.cecFeatures = GP_defaultCECFeatures();
#if 0 // TODO - add SystemBehavior_GetDefaultVolume and SystemBehavior_GetDefaultSource
    systemParams.product.lastVolume = SystemBehavior_GetDefaultVolume();
    systemParams.product.lastSource = SystemBehavior_GetDefaultSource();
#endif
    NV_CopyDefaultSourceListToRAM();
}

uint8_t NV_GetRemoteKeyThatSelectsHDMISingleInput(void)
{
    NV_WaitForInitialization();
    return systemParams.product.remoteKeySelectsSingleHDMIInput;
}

void NV_SetRemoteKeyThatSelectsHDMISingleInput(uint8_t key)
{
    NV_WaitForInitialization();
    systemParams.product.remoteKeySelectsSingleHDMIInput = key;
    NV_CommitSystemParams();
}

BOOL NV_GetIgnoreAmpFault (void)
{
    NV_WaitForInitialization();
    return systemParams.product.ignoreAmpFault;
}

void NV_SetIgnoreAmpFault (BOOL ignore)
{
    NV_WaitForInitialization();
    systemParams.product.ignoreAmpFault = ignore;
}

void NV_SetScreensaverEnabled(BOOL enabled)
{
    NV_WaitForInitialization();
    systemParams.product.screensaverEnabled = (uint8_t)enabled;
}

BOOL NV_GetScreensaverEnabled(void)
{
    NV_WaitForInitialization();
    return (BOOL)systemParams.product.screensaverEnabled;
}

uint32_t NV_GetChipconRemoteAddress(void)
{
    NV_WaitForInitialization();
    return systemParams.product.chipconRemoteAddress;
}

void NV_SetChipconRemoteAddress(uint32_t newAddr)
{
    NV_WaitForInitialization();
    systemParams.product.chipconRemoteAddress = newAddr;
}

uint8_t NV_GetChipconRemoteType(void)
{
    NV_WaitForInitialization();
    return systemParams.product.chipconRemoteType;
}

void NV_SetChipconRemoteType(uint8_t rmType)
{
    NV_WaitForInitialization();
    systemParams.product.chipconRemoteType = rmType;
}

uint8_t NV_GetLastChipconPrimaryChannel (void)
{
    NV_WaitForInitialization();
    return systemParams.product.lastChipconPrimaryChannel;
}

void NV_SetLastChipconPrimaryChannel (uint8_t channel)
{
    NV_WaitForInitialization();
    systemParams.product.lastChipconPrimaryChannel = channel;
}

uint8_t NV_GetLastChipconSecondaryChannel (void)
{
    NV_WaitForInitialization();
    return systemParams.product.lastChipconSecondaryChannel;
}

void NV_SetLastChipconSecondaryChannel (uint8_t channel)
{
    NV_WaitForInitialization();
    systemParams.product.lastChipconSecondaryChannel = channel;
}

uint8_t NV_GetChipconChanTableIndex (void)
{
    NV_WaitForInitialization();
    return systemParams.product.chipconChanTableIndex;
}

void NV_SetChipconChanTableIndex (uint8_t index)
{
    NV_WaitForInitialization();
    systemParams.product.chipconChanTableIndex = index;
}

/*
*   s_NVSystemParams.systemMode
*/
SYSTEM_MODE NV_GetSystemMode(void)
{
    NV_WaitForInitialization();
    return (SYSTEM_MODE) systemParams.product.systemMode;
}

void NV_SetSystemMode(SYSTEM_MODE mode)
{
    NV_WaitForInitialization();
    systemParams.product.systemMode = mode;
}

void NV_SetLanguage(LANGUAGE_SELECTION lang)
{
    NV_WaitForInitialization();
    systemParams.product.language = (uint8_t)lang;
}

LANGUAGE_SELECTION NV_GetLanguage(void)
{
    NV_WaitForInitialization();
    return (LANGUAGE_SELECTION)systemParams.product.language;
}

uint8_t NV_GetAutoOff(void)
{
    NV_WaitForInitialization();
    return systemParams.product.enableAutoOff;
}

void NV_SetAutoOff(uint8_t val)
{
    if (val == 1 || val == 0)
    {
        NV_WaitForInitialization();
        systemParams.product.enableAutoOff = val;
    }
}

TV_INPUT_ALT_ASSIGNMENT NV_GetTVInputAssignment(TV_INPUT_OPTION_NAME opt)
{
    NV_WaitForInitialization();
    return ((TV_INPUT_ALT_ASSIGNMENT)systemParams.product.inputAssignment[opt]);
}

void NV_SetTVInputAssignment(TV_INPUT_OPTION_NAME opt, TV_INPUT_ALT_ASSIGNMENT assign)
{
    NV_WaitForInitialization();
    systemParams.product.inputAssignment[opt] = assign;
}

uint8_t NV_GetCenterLevel(void)
{
    NV_WaitForInitialization();
    return systemParams.product.centerLevel;
}

void NV_SetCenterLevel(uint8_t val)
{
    NV_WaitForInitialization();
    systemParams.product.centerLevel = val;
}

uint8_t NV_GetSurroundLevel()
{
    NV_WaitForInitialization();
    return systemParams.product.surroundLevel;
}

void NV_SetSurroundLevel(uint8_t val)
{
    NV_WaitForInitialization();
    systemParams.product.surroundLevel = val;
}

uint8_t NV_GetBassLevel(void)
{
    NV_WaitForInitialization();
    return systemParams.product.bassLevel;
}

void NV_SetBassLevel(uint8_t val)
{
    NV_WaitForInitialization();
    systemParams.product.bassLevel = val;
}

uint8_t NV_GetTrebleLevel(void)
{
    NV_WaitForInitialization();
    return systemParams.product.trebleLevel;
}

void NV_SetTrebleLevel(uint8_t val)
{
    NV_WaitForInitialization();
    systemParams.product.trebleLevel = val;
}

uint8_t NV_GetAudioMode(void)
{
    NV_WaitForInitialization();
    return systemParams.product.audioMode;
}

void NV_SetAudioMode(uint8_t val)
{
    NV_WaitForInitialization();
    systemParams.product.audioMode = val;
}

uint8_t NV_GetAVSync(void)
{
    NV_WaitForInitialization();
    return systemParams.product.audioDelay;
}

void NV_SetAVSync(uint8_t val)
{
    NV_WaitForInitialization();
    systemParams.product.audioDelay = val;
}

uint8_t NV_GetHPMode(void)
{
    NV_WaitForInitialization();
    return systemParams.product.hpMode;
}
void NV_SetHPMode(uint8_t val)
{
    NV_WaitForInitialization();
    systemParams.product.hpMode = val;
}
uint8_t NV_GetCECMode(void)
{
    NV_WaitForInitialization();
    return systemParams.product.cecEnabled;
}

void NV_SetCECMode(uint8_t val)
{
    if (isValidCECMode(val))
    {
        NV_WaitForInitialization();
        systemParams.product.cecEnabled = val;
    }
}

uint32_t NV_GetCECFeatures(void)
{
    NV_WaitForInitialization();
    return systemParams.product.cecFeatures;
}

/*
*   s_NVSystemParams.dceFullBootFromPowerOn
*/
uint8_t NV_GetDceFullBootFromPowerOn (void)
{
    NV_WaitForInitialization();
    return systemParams.product.dceFullBootFromPowerOn;
}

/*
*  Returns TRUE if the value in NV does not match the desired value and thus
*  requires a commit.
*/
BOOL NV_SetDceFullBootFromPowerOn (uint8_t on)
{
    BOOL matched = TRUE;
    if (systemParams.product.dceFullBootFromPowerOn != on)
    {
        NV_WaitForInitialization();
        matched = FALSE;
        systemParams.product.dceFullBootFromPowerOn = on;
    }
    return matched;
}

BOOL NV_GetAutoWakEnabled(void)
{
    NV_WaitForInitialization();
    return systemParams.product.isAutoWakeEnabled;
}

void NV_SetAutoWakeEnabled(BOOL enabled)
{
    NV_WaitForInitialization();
    systemParams.product.isAutoWakeEnabled = enabled;
}

#ifdef SUPPORT_DSPWATCHDOG
BOOL NV_GetDspWDEnabled(void)
{
    NV_WaitForInitialization();
    return systemParams.product.isDspWDEnabled;
}

void NV_SetDspWDEnabled(BOOL enabled)
{
    NV_WaitForInitialization();
    systemParams.product.isDspWDEnabled = enabled;
}
#endif

uint8_t NV_GetLastSource(void)
{
    NV_WaitForInitialization();
    return systemParams.product.lastSource;
}

void NV_SetLastSource(uint8_t source)
{
    debug_assert ((uint8_t)UI_GetUnifySourceIndex((SOURCE_ID)source) <= UITask_GetNumOfDefaultUnifySrcs());
    NV_WaitForInitialization();
    systemParams.product.lastSource = source;
}

/*
*   s_NVSystemParams.lastVolume
*/
uint8_t NV_GetLastVolume(void)
{
    NV_WaitForInitialization();
    return systemParams.product.lastVolume;
}

void NV_SetLastVolume(uint8_t volume)
{
    NV_WaitForInitialization();
    systemParams.product.lastVolume = volume;
}

uint8_t NV_GetSkipSpeakerUpdate(void)
{
    NV_WaitForInitialization();
    return systemParams.product.skipSpeakerUpdate;
}

void NV_SetSkipSpeakerUpdate(BOOL skip)
{
    NV_WaitForInitialization();
    systemParams.product.skipSpeakerUpdate = skip;
}

uint8_t NV_GetExpectedSpeakerConfig(void)
{
    //BRW TODO: Fix names before commit!!!!
    NV_WaitForInitialization();
    return systemParams.product.expectedSpeakerConfig;
}

void NV_SetExpectedSpeakerConfig(uint8_t config)
{
    NV_WaitForInitialization();
    systemParams.product.expectedSpeakerConfig = config;
}

uint8_t NV_GetSpeakerAccessories(void)
{
    NV_WaitForInitialization();
    return systemParams.product.speakerAccessories;
}

void NV_SetSpeakerAccessories(uint8_t accessories)
{
    NV_WaitForInitialization();
    systemParams.product.speakerAccessories = accessories;
}

void NV_SetSpeakerPackageVariant(IpcSpeakerPackage_t variant)
{
    NV_WaitForInitialization();
    systemParams.product.speakerPackageVariant = (uint8_t)variant;
}

IpcSpeakerPackage_t NV_GetSpeakerPackageVariant(void)
{
    NV_WaitForInitialization();
    return (IpcSpeakerPackage_t)systemParams.product.speakerPackageVariant;
}
void NV_GetBlobVersion(char* version)
{
    NV_WaitForInitialization();
    strncpy(version, systemParams.product.blobVersion, MASTER_VERSION_STRING_LENGTH);
    version[MASTER_VERSION_STRING_LENGTH - 1] = '\0';
}

void NV_SetBlobVersion(char* version)
{
    NV_WaitForInitialization();
    strncpy(systemParams.product.blobVersion, version, MASTER_VERSION_STRING_LENGTH);
    systemParams.product.blobVersion[MASTER_VERSION_STRING_LENGTH - 1] = '\0';
}

void NV_SetBlobVersionFromStoredBlob(void)
{
    NV_WaitForInitialization();
    GetStoredBlobVersion(systemParams.product.blobVersion);
    NV_CommitSystemParams(); // always commit this
}

BOOL NV_GetIgnoreKeysDuringSourceChange(void)
{
    NV_WaitForInitialization();
    return (BOOL) systemParams.product.ignoreKeysDuringSourceChange;
}

void NV_SetIgnoreKeysDuringSourceChange(BOOL val)
{
    NV_WaitForInitialization();
    systemParams.product.ignoreKeysDuringSourceChange = (uint8_t) val;
}

BOOL NV_GetAIQEnabled(void)
{
    NV_WaitForInitialization();
    return (BOOL) systemParams.product.enableAIQ;
}

void NV_SetAIQEnabled(BOOL val)
{
    NV_WaitForInitialization();
    systemParams.product.enableAIQ = (uint8_t) val;
}
