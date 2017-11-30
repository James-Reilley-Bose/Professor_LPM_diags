// SourceList.c
#include "UITask.h"
//#include "volumeModel.h"
#include "nv_system.h"
#include "nv_system_struct.h"

#define MAX_NUM_OF_UNIFY_SOURCES 8
#define NUM_DEFAULT_UNIFY_SOURCES 8

// Compile time error check to make sure the constant definitions stay in sync.  That is,
// make sure there is enough NV storage for the number of sources.
uint8_t errorCheck[(MAX_NUM_SOURCE_FOR_NV_STORAGE>=MAX_NUM_OF_UNIFY_SOURCES) ? 1 : 0];
/* Logs
===============================================================================*/
SCRIBE_DECL(uitask);


const UnifySourceStruct unifySourceList[MAX_NUM_OF_UNIFY_SOURCES] = {
    {
        .sourceType = LPM_IPC_SRCTYPE_BOSE_INTERNAL,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_UNSPECIFIED,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .displayInList = FALSE,
        .sourceID = SOURCE_STANDBY,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_NORMAL,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
    {
        .sourceType = LPM_IPC_SRCTYPE_UNDEFINED,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_UNSPECIFIED,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .displayInList = TRUE,
        .sourceID = SOURCE_HDMI_1,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_NORMAL,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
    {
        .sourceType = LPM_IPC_SRCTYPE_BOSE_INTERNAL,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_UNSPECIFIED,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .displayInList = FALSE,
        .sourceID = SOURCE_ADAPTIQ,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_NORMAL,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
    {
        .sourceType = LPM_IPC_SRCTYPE_UNDEFINED,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_VIDEO,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .displayInList = TRUE,
        .sourceID = SOURCE_TV,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_NORMAL,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
    {
        .sourceType = LPM_IPC_SRCTYPE_UNDEFINED,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_AUDIO,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .displayInList = TRUE,
        .sourceID = SOURCE_SHELBY,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_DIRECT,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
    {
        .sourceType = LPM_IPC_SRCTYPE_UNDEFINED,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_UNSPECIFIED,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .displayInList = TRUE,
        .sourceID = SOURCE_BLUETOOTH,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_DIRECT,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
    {
        .sourceType = LPM_IPC_SRCTYPE_BOSE_INTERNAL,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_UNSPECIFIED,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .sourceID = SOURCE_UPDATE,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_NORMAL,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
    {
        .sourceType = LPM_IPC_SRCTYPE_BOSE_INTERNAL,
        .sourceTypeSeqNum = 0,
        .contentType = LPM_IPC_CONTENT_UNSPECIFIED,
        .ueiKeyRecordID = "",
        .moreButtonsBitMap = NULL,
        .sourceID = SOURCE_FACTORY_DEFAULT,
        .AVSyncDelay = 0,
        .bassLevel = 0,
        .trebleLevel = 0,
        .audioTrack = IPC_DUAL_MONO_LEFT,
        .audioMode = IPC_AUDIO_MODE_NORMAL,
        .audioInput = LPM_IPC_AUDIO_INPUT_DEFAULT,
    },
};

/*
===============================================================================
@func UITask_UnifySourceList
@brief returns the start of the Unify Source List
===============================================================================
*/
UnifySourceStruct*  UITask_GetUnifySourceStruct(UnifySourceIndex index)
{
    UnifySourceStruct* sourceList;
    debug_assert(index < UI_GetTotalNumSources());

    sourceList = NV_GetUnifySourceListPtr();
    return &sourceList[index];
}

/*
===============================================================================
@func UI_GetTotalNumSources
@brief returns number of unify sources
===============================================================================
*/
uint8_t UI_GetTotalNumSources(void)
{
    return MAX_NUM_OF_UNIFY_SOURCES;
}

/*
===============================================================================
@func UITask_GetDefaultSourceList
@brief returns pointer to default source list
===============================================================================
*/
const UnifySourceStruct* UI_GetDefaultSourceList(void)
{
    return &unifySourceList[0];
}

/*
===============================================================================
@func UITask_GetNumOfDefaultUnifySrcs
@brief returns number of unify sources
===============================================================================
*/
uint8_t UITask_GetNumOfDefaultUnifySrcs(void)
{
    return NUM_DEFAULT_UNIFY_SOURCES;
}

/*
===============================================================================
@func UITask_GetMaxNumOfUnifySrcs
@brief returns maximum number of unify sources
===============================================================================
*/
uint8_t UITask_GetMaxNumOfUnifySrcs(void)
{
    return MAX_NUM_OF_UNIFY_SOURCES;
}
/*
===============================================================================
@func  UITask_SetSourceList
@brief Replaces current unifySourceList
===============================================================================
*/
void UITask_SetSourceList(UnifySourceStruct* newlist, uint8_t numSources)
{
    LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Invalid Attempt to change Unify source list");
}

/*
===============================================================================
@func UI_BulkSrcListHandler
@brief Handles a received bulk source list message from ASOC
===============================================================================
*/
void UI_BulkSrcListHandler(GENERIC_MSG_t* msg)
{
    LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Invalid Attempt to tx/rx ASOC source list");
}


void UnifySetCurrentAudioProfile(SOURCE_ID src)
{
    UnifySourceStruct* srcPtr;

    srcPtr = UITask_GetUnifySourceStruct(UI_GetUnifySourceIndex(src));

#if 0
    VolumeModel_SetAudioParam(VM_AUDIO_MODE, srcPtr->audioMode);
    VolumeModel_SetAVSync(srcPtr->AVSyncDelay);
    VolumeModel_SetAudioParam(VM_DUAL_MONO, srcPtr->audioTrack);
#endif
}

void UnifyChangeAudioProfile(UnifySourceIndex src, IpcAudioMode_t mode)
{
    UnifySourceStruct* srcPtr;
    srcPtr = UITask_GetUnifySourceStruct(src);
    srcPtr->audioMode = mode;
}

DSPSource_t UnifyGetAudioMuxSetting(SOURCE_ID source)
{
    return DSP_SOURCE_NONE;
}

void UI_SetShowInList(SOURCE_ID src, BOOL show)
{
    // NOP
}
