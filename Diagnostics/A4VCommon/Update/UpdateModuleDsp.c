//
// UpdateModuleDsp.c
//

#include "UpdateModuleDsp.h"
#include "UpdateManagerTask.h"
#include "UpdateManagerBlob.h"
#include "IpcProtocolLpm.h"
#include "IpcRouterTask.h"
#include "genTimer.h"
#include "versionlib.h"
#include "DeviceModel_DSP.h"
#include "UITask.h"

SCRIBE_DECL(update);

/* Private functions */
static BOOL UpdateModuleDsp_HandlePacket(GENERIC_MSG_t* msg);
static BOOL UpdateModuleDsp_HandleBulk(GENERIC_MSG_t* msg);
static BOOL UpdateModuleDsp_HandleWatchdog(void);

/* Globals */
DSPUpdateState_t DspUpdate;

// ERC should be sent first
// RESET_ERC should be sent second
#define DSP_NUM_UPDATE_FILES 8
UpdateFile_t DspUpdateFiles[DSP_NUM_UPDATE_FILES] =
{
    {0, 0, "ERC.UPD", FALSE},
    {0, 0, "RESET_ERC.UPD", FALSE},
    {0, 0, "SMRT00.UPD", FALSE},
    {0, 0, "SMRT01.UPD", FALSE},
    {0, 0, "SMRT02.UPD", FALSE},
    {0, 0, "INSTALLER00.UPD", FALSE},
    {0, 0, "INSTALLER01.UPD", FALSE},
    {0, 0, "INSTALLER02.UPD", FALSE},
};

/*
 * @func UpdateModuleDsp_Init
 *
 * @brief Initializes the DSP update module.
 *
 * @param DSPUpdateState_t* dsp - pointer to dsp update state
 *
 * @return n/a
 */
void UpdateModuleDsp_Init(void)
{
    DspUpdate.State = UPDATE_DSP_NOT_ACTIVE;
}

/*
 * @func UpdateModuleDsp_HandleMessage
 *
 * @brief Handles messages while in UPDATE_DO_DSP state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
BOOL UpdateModuleDsp_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = FALSE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            handled = UpdateModuleDsp_HandlePacket(msg);
            break;
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            handled = UpdateModuleDsp_HandleWatchdog();
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
            handled = UpdateModuleDsp_HandleBulk(msg);
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
 * @func UpdateModuleDsp_HandlePacket
 *
 * @brief Handles IPC packets while in UPDATE_DO_DSP state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
static BOOL UpdateModuleDsp_HandlePacket(GENERIC_MSG_t* msg)
{
    IpcPacket_t* packet = (IpcPacket_t*) msg->params[0];
    BOOL handled = FALSE;

    switch (DspUpdate.State)
    {
        case UPDATE_DSP_WAIT_READY:
            if ((msg->msgID == UPDATE_MESSAGE_ID_IPCPacketAvailable) && (packet->s.opcode == IPC_UPDATE_STATUS))
            {
                UpdateStatus_t* status = (UpdateStatus_t*) packet->s.data.w;
                status->status = REV_WORD_BYTES(status->status);
                if (status->status == UPDATE_STATUS_READY_OK || status->status == UPDATE_STATUS_READY_FAIL)
                {
                    /* If not the first transfer, did the last one complete? */
                    if ((status->status == UPDATE_STATUS_READY_OK) &&
                            (DspUpdate.TransferAttempts != 0) &&
                            (DspUpdate.LastTransferSuccess))
                    {
                        DspUpdate.FilesSent++;
                        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "DSP Files Sent: %d / %d", DspUpdate.FilesSent, DSP_NUM_UPDATE_FILES);
                    }

                    if (DspUpdate.FilesSent >= DSP_NUM_UPDATE_FILES) // have we sent all files?
                    {
                        DspUpdate.State = UPDATE_DSP_DONE;
                        UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
                    }
                    else // send the next file
                    {
                        DspUpdate.Bulk.tID = IPCBulk_BulkTransfer(IPC_DEVICE_DSP, \
                                              (uint8_t*) DspUpdateFiles[DspUpdate.FilesSent].fileAddress, \
                                              DspUpdateFiles[DspUpdate.FilesSent].size, \
                                              BULK_TYPE_SOFTWARE_UPDATE, \
                                              DspUpdate.FilesSent, \
                                              DspUpdateFiles[DspUpdate.FilesSent].filename,
                                              &ManagedUpdateTask->Queue);
                        DspUpdate.TransferAttempts++;

                        if (DspUpdate.Bulk.tID != 0) // if the transfer was started
                        {
                            DspUpdate.State = UPDATE_DSP_DOING_BULK;
                        }
                    }
                }
                // else, let the watchdog request status again

                handled = TRUE;
            }
            break;
        case UPDATE_DSP_DOING_BULK:
            /*
             * We might have sent an UPDATE_GET_STATUS before transitioning to DOING_BULK,
             * it should be safe to ignore this and let the bulk transfer catch any errors.
             */
            break;
        default:
            break;
    }

    return handled;
}

/*
 * @func UpdateModuleDsp_HandleBulk
 *
 * @brief Handles bulk transfer messages while in UPDATE_DO_DSP state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
static BOOL UpdateModuleDsp_HandleBulk(GENERIC_MSG_t* msg)
{
    if (DspUpdate.State == UPDATE_DSP_DOING_BULK)
    {
        if (msg->msgID == IPCROUTER_MESSAGE_ID_BulkTransferComplete)
        {
            DspUpdate.LastTransferSuccess = TRUE;
        }
        else // it failed!
        {
            DspUpdate.LastTransferSuccess = FALSE;
        }
        IPCBulk_DeactivateEndpoint(&DspUpdate.Bulk);
        DspUpdate.State = UPDATE_DSP_WAIT_READY;
    }

    return TRUE; // handled - assuming this is the only bulk transfer happening
}

/*
 * @func UpdateModuleDsp_HandleWatchdog
 *
 * @brief Handles the watchdog while in UPDATE_DO_DSP state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
static BOOL UpdateModuleDsp_HandleWatchdog(void)
{
    static int32_t countdown = DSP_UPDATE_TIMEOUT_MS;
    countdown -= UPDATE_WATCHDOG_PERIOD_MS;

    if (countdown < 0)
    {
        // TODO: Add in a hard reset and retry. Not really useful until we can reset IPC :(
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "DSP update timed out.");
        UpdateManagerTask_CompleteState(UPDATE_TIMEOUT);
        return TRUE;
    }

    switch (DspUpdate.State)
    {
        case UPDATE_DSP_WAIT_READY:
            IPCRouter_Send(IPC_DEVICE_DSP, IPC_UPDATE_GET_STATUS, NULL, NULL, 0);
            break;
    }

    UpdateManagerTask_SetProgress((DspUpdate.FilesSent * 100) / DSP_NUM_UPDATE_FILES);

    return TRUE; // handled
}

/*
 * @func UpdateManagerTask_StartDSPUpdate(void)
 *
 * @brief A hack to update the DSP. This will change once the full update procedure
 *        is in place.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleDsp_Start(UpdateBlobHeader* blob)
{
    ManifestEntry DSPManifest;
    if (!GetUpdateFileInfoFromManifest(blob, BLOB_ADDRESS, DspUpdateFiles, DSP_NUM_UPDATE_FILES) ||
            !GetManifestEntryByName(blob, BLOB_ADDRESS, &DSPManifest, DspUpdateFiles[0].filename))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Error reading DSP update files.");
        UpdateManagerTask_CompleteState(UPDATE_FILE_NOT_FOUND);
        return;
    }

    char versionBuffer[IPC_DSP_SW_VERSION_LEN_BYTES];
    DeviceModel_DSP_Version_Get(versionBuffer);
    
    DSPImage_t dspImage = DeviceModel_DSP_Image_Get();
    
    if (!UpdateState.forced &&
        (dspImage != ERC_IMAGE) &&
        (VERSION_COMPARE_SAME_VERSION == VersionCompareVersions(versionBuffer, DSPManifest.ImageVersion)))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "DSP already at version %s", DSPManifest.ImageVersion);
        UpdateManagerTask_PublishVersionSkip(versionBuffer);
        UpdateManagerTask_CompleteState(UPDATE_SKIPPED_VERSION);
        return;
    }

    DspUpdate.State = UPDATE_DSP_WAIT_READY;
    DspUpdate.FilesSent = 0;
    DspUpdate.TransferAttempts = 0;

    IPCRouter_Send(IPC_DEVICE_DSP, IPC_UPDATE_ENTER, NULL, NULL, 0);
    IPCRouter_Send(IPC_DEVICE_DSP, IPC_UPDATE_GET_STATUS, NULL, NULL, 0);
}

/*
 * @func UpdateManagerTask_EnterDoDsp(void)
 *
 * @brief Called when the UPDATE_DO_DSP state is entered. This
 *        initializes the DSP update state.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleDsp_EnterDoDsp(void)
{
    UpdateBlobHeader upd;
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Starting DSP Update");
        timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
        UpdateModuleDsp_Start(&upd);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file (DSP)!");
        UpdateManagerTask_CompleteState(UPDATE_BLOB_ERROR);
    }
}

void UpdateModuleDsp_ExitDoDsp(void)
{
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "DSP Update Complete");
    timerStop(UpdateManagerTask_GetWDT(), 0);
}
