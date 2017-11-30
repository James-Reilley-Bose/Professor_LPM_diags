#include "project.h"
#include "nv.h"
#include "nv_system.h"
#include "nv_system_struct.h"
#include "nvram.h"
//#include "volumeModel.h"
#include "globalParameterList.h"
#include "buffermanager.h"
#include "InternalFlashAPI.h"
#include "event_groups.h"
#include "UpdateManagerBlob.h"
#include "ProductSystemParams.h"
#ifdef BARDEEN
#include "UEITask.h"
#endif

#define NV_INITIALIZED_BIT 1
#define NV_WAIT_INITIALIZED_TICKS TIMER_MSEC_TO_TICKS(20000)

static void NV_WriteDefaultSystemParameters(void);
static void NV_WriteNextSystemParamsBlock(void);
static void NV_WriteCurrentSystemParamsBlock(void);
static void NV_FindCurrentSystemParameters(void);
static BOOL NV_DoesFlashMatchRAM(void);

extern const NV_SystemParams_t s_NVSystemParamDefaults;

SCRIBE_DECL(system);

// Generate a compile time error if the param size is wrong
extern const uint8_t NvSystemParamErrorCheck[(sizeof(NV_SystemParams_t) == NV_SYSTEM_PARAMS_SIZE) ? 1 : -1];

static EventGroupHandle_t sysParamsInitializedHandle = NULL;
extern NV_SystemParams_t systemParams = {0};
static uint32_t systemParamsAddress = SYS_PARAM_LAST_VALID_ADDR;
static uint8_t flashCompareBuf[128];

// Compile error - sizeof(flashCompareBuf) must be a factor of sizeof(NV_SystemParams_t)
extern const uint8_t NvBufSizeErrorCheck[(sizeof(NV_SystemParams_t) % sizeof(flashCompareBuf) == 0) ? 1 : -1];

void NV_CreateSystemParameterBlock(void)
{
    sysParamsInitializedHandle = xEventGroupCreate();
    debug_assert(sysParamsInitializedHandle);
}

void NV_InitSystemParameters(void)
{
    NV_FindCurrentSystemParameters();
    NV_ValidateSystemParams();
    xEventGroupSetBits(sysParamsInitializedHandle, NV_INITIALIZED_BIT);
}

void NV_WaitForInitialization(void)
{
    debug_assert(sysParamsInitializedHandle);

    EventBits_t bits = xEventGroupWaitBits(
                           sysParamsInitializedHandle,
                           NV_INITIALIZED_BIT,
                           pdFALSE, // do not clear on exit
                           pdFALSE, // wait on one bit (we only use one, so don't care)
                           NV_WAIT_INITIALIZED_TICKS );

    debug_assert(bits & NV_INITIALIZED_BIT);
}

static void NV_FindCurrentSystemParameters(void)
{
    systemParamsAddress = SYS_PARAM_LAST_VALID_ADDR;
    for (; systemParamsAddress >= EXT_FLASH_ADDR_SYS_PARAMS_START; systemParamsAddress -= sizeof(NV_SystemParams_t))
    {
        nvram_read(systemParamsAddress, sizeof(NV_SystemSettings_t), (uint8_t*)&systemParams);
        if (systemParams.sysSettings.systemStructVersion == 0xFF)
        {
            continue;
        }

        nvram_read(systemParamsAddress, sizeof(NV_SystemParams_t), (uint8_t*)&systemParams);
        if (systemParams.systemCtrlWord == NV_VALID_CTRL_WORD)
        {
            break;
        }
    }

    if (systemParamsAddress >= EXT_FLASH_ADDR_SYS_PARAMS_START)
    {
        return;
    }

    nvram_erase(EXT_FLASH_ADDR_SYS_PARAMS_START, EXT_FLASH_ADDR_SYS_PARAMS_END - 1);
    systemParamsAddress = EXT_FLASH_ADDR_SYS_PARAMS_START;
    NV_CopyDefaultSystemParametersToRAM();
    NV_WriteCurrentSystemParamsBlock();
}

static void NV_WriteDefaultSystemParameters(void)
{
    NV_CopyDefaultSystemParametersToRAM();
    NV_WriteNextSystemParamsBlock();
}

static void NV_WriteNextSystemParamsBlock(void)
{
    do
    {
        systemParamsAddress += sizeof(NV_SystemParams_t);
        if (systemParamsAddress > SYS_PARAM_LAST_VALID_ADDR)
        {
            LOG(system, ROTTEN_LOGLEVEL_VERBOSE, "System params are rolling over, erasing flash.");
            nvram_erase(EXT_FLASH_ADDR_SYS_PARAMS_START, EXT_FLASH_ADDR_SYS_PARAMS_END - 1);
            systemParamsAddress = EXT_FLASH_ADDR_SYS_PARAMS_START;
        }
        nvram_write(systemParamsAddress, sizeof(NV_SystemParams_t), (uint8_t*)&systemParams);
    }
    while (!NV_DoesFlashMatchRAM());
}

static void NV_WriteCurrentSystemParamsBlock(void)
{
    nvram_write(systemParamsAddress, sizeof(NV_SystemParams_t), (uint8_t*)&systemParams);
    if (!NV_DoesFlashMatchRAM())
    {
        NV_WriteNextSystemParamsBlock();
    }
}

static BOOL NV_DoesFlashMatchRAM(void)
{
    uint8_t* ramStruct = (uint8_t*) &systemParams;
    for (uint16_t i = 0; i < sizeof(NV_SystemParams_t); i += sizeof(flashCompareBuf))
    {
        nvram_read(systemParamsAddress + i, sizeof(flashCompareBuf), flashCompareBuf);
        if (memcmp(flashCompareBuf, &ramStruct[i], sizeof(flashCompareBuf)))
        {
            return FALSE;
        }
    }
    return TRUE;
}

static void NV_ValidateSystemParams(void)
{
    /////////////
    /// Begin band-aids
    ///
    /// Put your new band-aid at the top of this list.
    /////////////
    if (systemParams.sysSettings.systemStructVersion != NV_SYSTEM_VERSION)
    {
        systemParams.sysSettings.systemStructVersion = NV_SYSTEM_VERSION;
        NV_WriteNextSystemParamsBlock();
    }
}

void NV_CommitSystemParams(void)
{
    NV_WaitForInitialization();
    if (!NV_DoesFlashMatchRAM())
    {
        NV_WriteNextSystemParamsBlock();
    }
}

void NV_SetFirstTimeInit(uint8_t setflag)
{
    NV_WaitForInitialization();
    systemParams.sysSettings.firstTimeInit = setflag;
    NV_CommitSystemParams();
}

uint8_t NV_GetFirstTimeInit(void)
{
    NV_WaitForInitialization();
    return systemParams.sysSettings.firstTimeInit;
}

/*
*   s_NVSystemParams.sourceList pointer
*/
UnifySourceStruct* NV_GetUnifySourceListPtr (void)
{
    NV_WaitForInitialization();
    //TODO - re-add uniform source list?
    //return systemParams.sourceList;
    return NULL;
}

/*
*   sets the unify source list to the default values
*/
void NV_SetDefaultSourceList(void)
{
    NV_CopyDefaultSourceListToRAM();
    NV_CommitSystemParams();
}

void NV_CopyDefaultSourceListToRAM(void)
{
    //const UnifySourceStruct* defaultList = UI_GetDefaultSourceList();

    //TODO - re-add uniform source list?
    //memset(systemParams.sourceList, 0, (sizeof(UnifySourceStruct)*MAX_NUM_SOURCE_FOR_NV_STORAGE));
    //memcpy(systemParams.sourceList, &defaultList[0], (sizeof(UnifySourceStruct)*UITask_GetNumOfDefaultUnifySrcs()));
    //systemParams.sourceListLength = (sizeof(UnifySourceStruct) * MAX_NUM_SOURCE_FOR_NV_STORAGE);
}

BOOL NV_GetRebootOnAssert(void)
{
    NV_WaitForInitialization();
    return systemParams.sysSettings.rebootOnAssert;
}

void NV_SetRebootOnAssert(BOOL enabled)
{
    NV_WaitForInitialization();
    systemParams.sysSettings.rebootOnAssert = enabled;
}

void NV_FactoryResetSystemParams(void)
{
    NV_WriteDefaultSystemParameters();
}

/******************************************************************************
*
*   System Read, Write, and Search functions
*
******************************************************************************/


//Functions that transform internal types into DSP/BoseLink types

static const SPOKEN_LANGUAGE transformToSpokenLanguage[LANGUAGE_NUM] =
{
    SPOKEN_LANGUAGE_ENGLISH,             //  LANGUAGE_ENGLISH,
    SPOKEN_LANUGAGE_SPANISH,             //  LANUGAGE_SPANISH,
    SPOKEN_LANGUAGE_FRENCH,              //  LANGUAGE_FRENCH,
    SPOKEN_LANGUAGE_DANISH,              //  LANGUAGE_DANISH,
    SPOKEN_LANGUAGE_GERMAN,              //  LANGUAGE_GERMAN,
    SPOKEN_LANGUAGE_ITALIAN,             //  LANGUAGE_ITALIAN,
    SPOKEN_LANGUAGE_SWEDISH,             //  LANGUAGE_SWEDISH,
    SPOKEN_LANGUAGE_POLISH,              //  LANGUAGE_POLISH,
    SPOKEN_LANGUAGE_HUNGARIAN,           //  LANGUAGE_HUNGARIAN,
    SPOKEN_LANGUAGE_FINNISH,             //  LANGUAGE_FINNISH,
    SPOKEN_LANGUAGE_CZECH,               //  LANGUAGE_CZECH,
    SPOKEN_LANGUAGE_GREEK,               //  LANGUAGE_GREEK,
    SPOKEN_LANGUAGE_NORWEGIAN,           //  LANGUAGE_NORWEGIAN,
    SPOKEN_LANGUAGE_PORTUGUESE,          //  LANGUAGE_PORTUGUESE,
    SPOKEN_LANGUAGE_ROMANIAN,            //  LANGUAGE_ROMANIAN,
    SPOKEN_LANGUAGE_RUSSIAN,             //  LANGUAGE_RUSSIAN,
    SPOKEN_LANGUAGE_SLOVENIAN,           //  LANGUAGE_SLOVENIAN,
    SPOKEN_LANGUAGE_TURKISH,             //  LANGUAGE_TURKISH,
    SPOKEN_LANGUAGE_MANDARIN_CHINESE,    //  LANGUAGE_SIMPLE_MANDARIN,
    SPOKEN_LANGUAGE_THAI,                //  LANGUAGE_THAI,
    SPOKEN_LANGUAGE_KOREAN,              //  LANGUAGE_KOREAN,
    SPOKEN_LANGUAGE_MANDARIN_CHINESE,    //  LANGUAGE_TRADITIONAL_MANDARIN,
    SPOKEN_LANGUAGE_HEBREW,              //  LANGUAGE_HEBREW,
    SPOKEN_LANGUAGE_ARABIC,              //  LANGUAGE_ARABIC,
    SPOKEN_LANGUAGE_JAPANESE,            //  LANGUAGE_JAPANESE,
    SPOKEN_LANGUAGE_DUTCH,               //  LANGUAGE_DUTCH,
    SPOKEN_LANGUAGE_CANTONESE_CHINESE,   //  LANGUAGE_TRADITIONAL_CANTONESE,
};

SPOKEN_LANGUAGE NV_TransformLanguageSelectionToSpokenLanguage(LANGUAGE_SELECTION languageSelection)
{
    if (LANGUAGE_NUM <= languageSelection)
    {
        return SPOKEN_LANGUAGE_UNDEFINED;
    }

    return transformToSpokenLanguage[languageSelection];
}

uint8_t NV_GetSystemStructVer(void)
{
    NV_WaitForInitialization();
    return systemParams.sysSettings.systemStructVersion;
}
