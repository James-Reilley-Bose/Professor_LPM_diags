//
// UpdateModuleRadio.c
//
#include <stdio.h>
#include <string.h>
#include "WirelessAudioAPI.h"
#include "UpdateManagerTask.h"
#include "UpdateManagerBlob.h"
#include "UpdateModuleSpeakers.h"
#include "UITask.h"

SCRIBE_DECL(update);

static void UpdateModuleWAM_Start(UpdateBlobHeader* blob);

#define NUM_SPEAKER_UPDATE_FILES 1
static UpdateFile_t WAMUpdateFile =  {0, 0, WA_VARIANT_DARR_FILE_NAME, FALSE};

/*
 * @func UpdateModuleWAM_Init
 *
 * @brief Initializes the Speakers update module.
 *
 * @param SpeakersUpdateState_t* updSpeaker - pointer to speaker update state
 *
 * @return n/a
 */
void UpdateModuleWAM_Init(void)
{
}

/*
 * @func UpdateModuleWAM_HandleMessage
 *
 * @brief Handles messages while in UPDATE_DO_SPEAKERS state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
BOOL UpdateModuleWAM_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = FALSE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_CompleteState:
            UpdateManagerTask_CompleteState((UpdateResult_t)msg->params[0]);
            handled = TRUE;
            break;
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            handled = TRUE;
            break;
        case UPDATE_MESSAGE_ID_DoEnterState:
            handled = FALSE;
            break;
        default:
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled message ID - %d (%s)", msg->msgID, __func__);
            break;
    }

    return handled;
}

/*
 * @func UpdateModuleWAM_Start(void)
 *
 * @brief Kicks off update to the mobile unit speakers (skipper and maxwells) but as it
 *        is asycronous it update complete isn't done till later
 *
 * @param n/a
 *
 * @return n/a
 */
static void UpdateModuleWAM_Start(UpdateBlobHeader* blob)
{
    ManifestEntry WAMManifest;
    if (!GetUpdateFileInfoFromManifest(blob, BLOB_ADDRESS, &WAMUpdateFile, NUM_SPEAKER_UPDATE_FILES) ||
        !GetManifestEntryByName(blob, BLOB_ADDRESS, &WAMManifest, WAMUpdateFile.filename))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Error reading WAM update files.");
        UpdateManagerTask_CompleteState(UPDATE_FILE_NOT_FOUND);
        return;
    }

    char versionBuffer[4] = {0};
    sprintf(versionBuffer, "%d", WirelessAudioUtilities_GetVersion());

    // The DARR has a different version format than everything else in the system,
    // we'll just compare the major field
    if (!UpdateState.forced &&
        !strncmp(versionBuffer, WAMManifest.ImageVersion, strlen(versionBuffer)))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "WAM already at version %s", WAMManifest.ImageVersion);
        UpdateManagerTask_PublishVersionSkip(WAMManifest.ImageVersion);
        UpdateManagerTask_CompleteState(UPDATE_SKIPPED_VERSION);
        return;
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Upd WAM from v%s. to v%s.", versionBuffer, WAMManifest.ImageVersion);
    }

    timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
    WirelessAudioPostMsg(WA_MSG_ID_Update_Darr, NOP_CALLBACK, NULL);
}

/*
 * @func UpdateModuleWAM_EnterDoWAM(void)
 *
 * @brief Called when the UPDATE_DO_SPEAKERS state is entered. This
 *        initializes the speaker update state.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleWAM_EnterDoWAM(void)
{
    UpdateBlobHeader upd;
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        UpdateModuleWAM_Start(&upd);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file (WAM)!");
        UpdateManagerTask_CompleteState(UPDATE_BLOB_ERROR);
    }
}

void UpdateModuleWAM_ExitDoWAM(void)
{
    timerStop(UpdateManagerTask_GetWDT(), 0);
}
