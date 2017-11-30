//
// UpdateModuleRadio.c
//
#include "WirelessAudioUpdateTransfer.h"
#include "UpdateManagerTask.h"
#include "UpdateManagerBlob.h"
#include "UpdateModuleSpeakers.h"
#include "genTimer.h"
#include "UITask.h"
#include "nv_system.h"
#include "AccessoryManager.h"

#include "IPCRouterTask.h"

static BOOL GetSpeakerUpdateInfo(UpdateBlobHeader* blob);

SCRIBE_DECL(update)

/* Globals */
static ManifestEntry SpeakerManifest = {0};
static SpeakerUpdateState_t* SpeakerUpdate = &UpdateState.Speaker;
UpdateFile_t SpeakerUpdateFile =  {0, 0, "speaker.bos", FALSE};

static BOOL GetSpeakerUpdateInfo(UpdateBlobHeader* blob)
{
    if(!GetManifestEntryByName(blob, BLOB_ADDRESS, &SpeakerManifest, SpeakerUpdateFile.filename))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to retrieve speaker manifest");
        return FALSE;
    }

    SpeakerUpdateFile.size = SpeakerManifest.ImageSize;
    SpeakerUpdateFile.fileAddress = SpeakerManifest.ImageOffset + sizeof(UpdateBlobHeader) + (blob->NumberOfImages * sizeof(ManifestEntry));
    SpeakerUpdateFile.found = TRUE;

    return TRUE;
}

/*
 * @func UpdateModuleSpeakers_Init
 *
 * @brief Initializes the Speakers update module.
 *
 * @param SpeakersUpdateState_t* updSpeaker - pointer to speaker update state
 *
 * @return n/a
 */
void UpdateModuleSpeakers_Init(void)
{
    SpeakerUpdate->State = UPDATE_SPEAKER_NOT_ACTIVE;
}

/*
 * @func UpdateModuleSpeakers_HandleMessage
 *
 * @brief Handles messages while in UPDATE_DO_SPEAKERS state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
BOOL UpdateModuleSpeakers_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = FALSE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            handled = FALSE;
            break;
        case UPDATE_MESSAGE_ID_CompleteState:
            UpdateManagerTask_CompleteState((UpdateResult_t) msg->params[0]);
            handled = TRUE;
            break;
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            handled = UpdateModuleSpeakers_HandleWatchdog();
            break;
        case UPDATE_MESSAGE_ID_DoEnterState:
            handled = FALSE;
            break;
        default:
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled message ID - %s (%s)", GetEventString(msg->msgID), __func__);
            break;
    }

    return handled;
}

/*
 * @func UpdateModuleSpeakers_Start(void)
 *
 * @brief Kicks off update to the mobile unit speakers (skipper and maxwells) but as it
 *        is asycronous it update complete isn't done till later
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleSpeakers_Start(UpdateBlobHeader* blob)
{
    if(NV_GetSkipSpeakerUpdate())
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Skip speaker update flag set.");
        UpdateManagerTask_CompleteState(UPDATE_SKIPPED);
    }
    else if(!GetSpeakerUpdateInfo(blob))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "No speaker update files found!");
        UpdateManagerTask_CompleteState(UPDATE_FILE_NOT_FOUND);
    }
    else if((SpeakerManifest.Flags & ACC_UPDATE_REQUIRED) ||
            UpdateState.forced)
    {
        SpeakerUpdate->State = UPDATE_SPEAKER_TRANSFERING_FILE;
        waTransfer_Start(SpeakerUpdateFile.fileAddress, SpeakerUpdateFile.size);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Speaker update not required.");
        UpdateManagerTask_CompleteState(UPDATE_SKIPPED);
    }
}

/*
 * @func UpdateModuleSpeaker_EnterDoSpeakers(void)
 *
 * @brief Called when the UPDATE_DO_SPEAKERS state is entered. This
 *        initializes the speaker update state.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleSpeakers_EnterDoSpeakers(void)
{
    UpdateBlobHeader upd;
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Starting Speaker Update");
        timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
        UpdateModuleSpeakers_Start(&upd);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file (Speakers)!");
        UpdateManagerTask_CompleteState(UPDATE_BLOB_ERROR);
    }
}

/*
 * @func UpdateModuleSpeakers_HandleWatchdog
 *
 * @brief Handles the watchdog while in UPDATE_DO_SPEAKERS state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
static BOOL UpdateModuleSpeakers_HandleWatchdog(void)
{
    static int cachedProgress = 0;

#if defined(LPM_HAS_ASOC)
    UpdateIncProg_t asoc_progress = {0,0,0};
#endif

    int progress = waTransfer_GetProgress();
    UpdateManagerTask_SetProgress(progress);


    if (progress != cachedProgress)
    {
        cachedProgress = progress;
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Wireless speaker update %d%% complete", progress);

#if defined(LPM_HAS_ASOC)
        asoc_progress.percentComplete = REV_WORD_BYTES(cachedProgress);

        IPCRouter_Send(IPC_DEVICE_AP, IPC_UPDATE_INC_PROG, NULL, &asoc_progress, sizeof(asoc_progress));
#endif
    }

    return TRUE;
}

void UpdateModuleSpeakers_ExitDoSpeakers(void)
{
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Speaker Update Complete");
    AccessoryManagerSetUpdate(FALSE);
    timerStop(UpdateManagerTask_GetWDT(), 0);
}

#if defined(BARDEEN)

/*
 * @func UpdateModuleSpeaker_EnterDoSpeakers(void)
 *
 * @brief Called when the UPDATE_DO_SPEAKERS state is entered. This
 *        initializes the speaker update state.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleSpeakerPrep_EnterDoSpeakers(void)
{
    UpdateBlobHeader upd;
    UpdateResult_t result;
    if (ReadBlobHeaderFromFlash_NoCRCCheck(&upd, BLOB_ADDRESS))
    {
        if (!GetSpeakerUpdateInfo(&upd))
        {
           result = UPDATE_FILE_NOT_FOUND;
        }
        else
        {
           result = UPDATE_COMPLETED;
        }

    }
    else
    {
        result = UPDATE_BLOB_ERROR;
    }
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Update Prep Speaker complete, status %d", result);
    UpdateManagerTask_CompleteState(result);
}


void UpdateModuleSpeakerPrep_ExitDoSpeakers(void)
{
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Speaker Prep Update Complete");
}

BOOL UpdateModuleSpeakerPrep_IsManifestAvail(void)
{
    return SpeakerUpdateFile.found;
}

const char * UpdateModuleSpeakerPrep_GetCUSpeakerBlobVer(void)
{
    if (SpeakerUpdateFile.found)
    {
        return SpeakerManifest.ImageVersion;
    }

    return NULL;
}



#endif