#ifndef NV_SYSTEM_H
#define NV_SYSTEM_H

#include "nv.h"
#include "unifySourceList.h"

#define SYS_PARAM_INVALID_ADDRESS 0xFFFFFFFF
#define SYS_PARAM_NUM_BLOCKS (EXT_FLASH_SYS_PARAMS_LENGTH / NV_SYSTEM_PARAMS_SIZE)
#define SYS_PARAM_LAST_VALID_ADDR (EXT_FLASH_ADDR_SYS_PARAMS_START + (SYS_PARAM_NUM_BLOCKS - 1) * sizeof(NV_SystemParams_t))

#define UNPAIRED_ADDRESS 0xffffffff
typedef enum
{
    REMOTE_TYPE_UNKNOWN = 0,
    REMOTE_TYPE_RESOLUTE = 1
} REMOTE_TYPE;

typedef enum
{
    TV_INPUT_OPTICAL,
    TV_INPUT_COAX,
    TV_INPUT_ANALOG,
} TV_INPUT_OPTION_NAME;

typedef enum
{
    TV_INPUT_ALT_ASSIGNMENT_TV_AUTO,
    TV_INPUT_ALT_ASSIGNMENT_CBL_SAT,
    TV_INPUT_ALT_ASSIGNMENT_BD_DVD,
    TV_INPUT_ALT_ASSIGNMENT_GAME,
    TV_INPUT_ALT_ASSIGNMENT_AUX,
    TV_INPUT_ALT_ASSIGNMENT_NUM
} TV_INPUT_ALT_ASSIGNMENT;

typedef enum
{
    CEC_MODE_DISABLED          = 0,
    CEC_MODE_COMPLIANCE        = 1, //0x0F - Minimal functionality + System Audio + Send Routing + Receive Routing (Full CEC compliance)
    CEC_MODE_USABILITY_1        = 2, //0x0D - Minimal functionality + System Audio + Send Routing
    CEC_MODE_USABILITY_2        = 3, //0x09 - Minimal functionality + System Audio
    CEC_MODE_USABILITY_3        = 4  //0x01 - Minimal functionality
} CEC_MODE;

typedef enum
{
    SYSTEM_MODE_NORMAL,
    SYSTEM_MODE_HOSPITALITY,
    SYSTEM_MODE_NUM,
    SYSTEM_MODE_INVALID,
} SYSTEM_MODE;

void NV_CreateSystemParameterBlock(void);
void NV_WaitForInitialization(void);
void NV_CopyDefaultSystemParametersToRAM(void);
void NV_CopyDefaultSourceListToRAM(void);

void NV_InitSystemParameters(void);
void NV_CommitSystemParams(void);
void NV_FactoryResetSystemParams(void);

void NV_SetFirstTimeInit(uint8_t setflag);
uint8_t NV_GetFirstTimeInit(void);

uint8_t NV_GetRebootOnAssert(void);
void NV_SetRebootOnAssert(BOOL enabled);

void NV_LanguageChange(uint8_t languageIndex);
SPOKEN_LANGUAGE NV_TransformLanguageSelectionToSpokenLanguage(LANGUAGE_SELECTION languageSelection);

void NV_ValidateSystemParams(void);

//Defined in the 'variant'SourceList.c file
UnifySourceStruct* NV_GetUnifySourceListPtr (void);

//Used for factory default and TAP testing
void NV_SetDefaultSourceList(void);

uint8_t NV_GetWiredGain(void);
void NV_SetWiredGain(uint8_t gain);

uint8_t NV_GetSystemStructVer(void);

#endif
