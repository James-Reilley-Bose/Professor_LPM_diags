//
// UpdateModuleSpeakers.h
//

#ifndef _UPDATE_MODULE_SPEAKERS_H
#define _UPDATE_MODULE_SPEAKERS_H

#include "UpdateManagerBlob.h"

typedef enum // Speaker Update Substates
{
    UPDATE_SPEAKER_NOT_ACTIVE,
    UPDATE_SPEAKER_WAIT_READY,
    UPDATE_SPEAKER_TRANSFERING_FILE,
    UPDATE_SPEAKER_TRANSFER_COMPLETE,
    UPDATE_SPEAKER_UPDATING_RADIO,
    UPDATE_SPEAKER_UPDATING_LPM,
    UPDATE_SPEAKER_DONE,

    UPDATE_SPEAKER_NUM_STATES,
} SpeakerUpdateSubState_t;

typedef struct
{
    SpeakerUpdateSubState_t State;
    //TBD
} SpeakerUpdateState_t;

void UpdateModuleSpeakers_Init(void);
BOOL UpdateModuleSpeakers_HandleMessage(GENERIC_MSG_t* msg);
void UpdateModuleSpeakers_Start(UpdateBlobHeader* blob);
void UpdateModuleSpeakers_EnterDoSpeakers(void);
BOOL UpdateModuleSpeakers_HandleWatchdog(void);
void UpdateModuleSpeakers_ExitDoSpeakers(void);

#if defined(BARDEEN)
void UpdateModuleSpeakerPrep_EnterDoSpeakers(void);
void UpdateModuleSpeakerPrep_ExitDoSpeakers(void);

BOOL UpdateModuleSpeakerPrep_IsManifestAvail(void);
const char * UpdateModuleSpeakerPrep_GetCUSpeakerBlobVer(void);

#endif

#endif // _UPDATE_MODULE_SPEAKERS_H
