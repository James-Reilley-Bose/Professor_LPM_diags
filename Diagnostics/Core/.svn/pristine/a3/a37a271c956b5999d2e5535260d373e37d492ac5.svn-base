#ifndef UI_TASK_H
#define UI_TASK_H

#include "project.h"
#include "TaskDefs.h"
//#include "HDMITask.h"
#include "KeyData.h"
#include "IpcProtocolLpm.h"
#include "sourceInterface.h"
//#include "unifySourceList.h"
//#include "WirelessAudioAPI.h"

extern ManagedTask* uiTaskHandle;

#define UIPostMsg(_theMsg, cb, ...) QueueManagerPostCallbackMsg(&uiTaskHandle->Queue, _theMsg, (uint32_t []){__VA_ARGS__}, cb, NO_BLOCK_TIME);

#define UI_SPK_TAP_UPDATE_OFF      0xff

#define SYSTEM_ERROR_NONE               0
#define SYSTEM_ERROR_AMP_FAULT          (1<<0)
#define SYSTEM_ERROR_AMP_DC_OFFSET      (1<<1)
#define SYSTEM_ERROR_AMP_CLIPPING       (1<<2)
#define SYSTEM_ERROR_FIRMWARE_CRITICAL  (1<<3)
#define SYSTEM_ERROR_I2C_FAILURE        (1<<4)
#define SYSTEM_ERROR_RFD_MISCONFIG      (1<<5)
#define SYSTEM_ERROR_SM2_FAILURE        (1<<6)
#define SYSTEM_ERROR_POWER_FAIL         (1<<7)
#define SYSTEM_ERROR_ASOC_FAILURE       (1<<8)

typedef enum ui_timout_params_t
{
    TO_NO_AUDIO,
    TO_NO_NETWORK,
    TO_NO_USER_INTERACTION,
} UITimeOutParameter;

typedef enum ui_audio_params_t
{
    UI_AV_SYNC_DELAY,
    UI_BASS_ADJUSTMENT,
    UI_TREBLE_ADJUSTMENT,
} UIAudioParameters;

void UITask(void* pvParamaters);
void UI_TaskInit(void* p);
void UI_HandleMessage(GENERIC_MSG_t* message);

DSPSource_t UI_GetDspSourceFromSourceID(SOURCE_ID sourceID);

SOURCE_ID UI_GetLastSource(void);

#if 0 // TODO - bring in, delete, or move
void UI_HandleSourceChangeRequest(UnifySourceIndex index);
BOOL UI_SetUnifySource(UnifySourceIndex index);
UnifySourceIndex UI_GetCurrentUnifySourceIndex();
#endif

#if 0 // TODO - bring this in or move it.
// Special tap stuff from Triode.  Ignore keys, echo them to tap, etc.
#define KH_MASK_WORDS (256/32)
typedef enum
{
    KH_HOOK_MAP_ECHO,            //!< KH_HOOK_MAP_ECHO
    KH_HOOK_MAP_IGNORE,          //!< KH_HOOK_MAP_IGNORE
    KH_NUM_HOOKMAPS              //!< KH_NUM_HOOKMAPS
} KH_HOOK_MAP_ID;
void UI_HookMapClearAll( KH_HOOK_MAP_ID mapid );
void UI_HookMapSetAll(KH_HOOK_MAP_ID mapid);
void UI_SetKeyHook(KH_HOOK_MAP_ID mapid, KEY_VALUE key);
void UI_ClearKeyHook(KH_HOOK_MAP_ID mapid, KEY_VALUE key);
void UI_GetKeyMask(KH_HOOK_MAP_ID mapid, uint32_t* keyMask);
#endif

#endif //UI_TASK_H
