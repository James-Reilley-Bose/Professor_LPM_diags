#include "UpdateManagerTask.h"
#include "UpdateModuleDownloading.h"
#include "IpcRouterTask.h"
#include "nvram.h"
#include "BufferManager.h"

static void UpdateModule_Downloading_HandleTimerExpired(void);
static void UpdateModule_Downloading_HandleBulkFullBuffer(GENERIC_MSG_t* msg);
static void UpdateModule_Downloading_HandleBulkComplete(void);
static void UpdateModule_Downloading_HandleBulkError(void);

SCRIBE_DECL(update);

#ifdef LPM_HAS_SPI_FLASH
static uint32_t currentAddress = 0; // flash address to write to
#endif

void UpdateModule_Downloading_Enter(void)
{
    timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Downloading...");
}

void UpdateModule_Downloading_Exit(void)
{
    timerStop(UpdateManagerTask_GetWDT(), 0);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Downloading complete");
}

/*
 * @func UpdateModule_Downloading_HandleMessage
 *
 * @brief Handles messages while in UPDATE_DOWNLOADING state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if it was handled FALSE if it wasn't
 */
BOOL UpdateModule_Downloading_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            UpdateModule_Downloading_HandleTimerExpired();
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer:
            UpdateModule_Downloading_HandleBulkFullBuffer(msg);
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
            UpdateModule_Downloading_HandleBulkComplete();
            break;
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
            UpdateModule_Downloading_HandleBulkError();
            break;
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

/*
 * @func UpdateModule_Downloading_HandleBulkError
 *
 * @brief An error happened. Notify Soundtouch. For now, abort.
 *
 * @param n/a
 *
 * @return n/a
 */
static void UpdateModule_Downloading_HandleBulkError(void)
{
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Update download failed!");

    // TODO: retries?
    UpdateManagerTask_AbortUpdate();

    IPCBulk_DeactivateEndpoint(&UpdateState.bulkUpdateEndpoint);

    uint32_t status = REV_WORD_BYTES(UPDATE_STATUS_READY_FAIL);
    IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_UPDATE_STATUS, NULL, &status, sizeof(status));
}

/*
 * @func UpdateModule_Downloading_HandleTimerExpired
 *
 * @brief When the timer goes off, update the progress.
 *
 * @param n/a
 *
 * @return n/a
 */
static void UpdateModule_Downloading_HandleTimerExpired(void)
{
    BulkEndpoint_t* b = &UpdateState.bulkUpdateEndpoint;
    int progress = (b && b->TotalLength) ? ((b->BytesRxd * 100) /
                                            b->TotalLength) : 0;

    UpdateManagerTask_SetProgress(progress);
}


/*
 * @func UpdateModule_Downloading_HandleBulkFullBuffer
 *
 * @brief Handle bulk transfer buffers during download.
 *
 * @param GENERIC_MSG_t* msg - pointer to message recieved
 *
 * @return void
 */
static void UpdateModule_Downloading_HandleBulkFullBuffer(GENERIC_MSG_t* msg)
{
    uint16_t tID = msg->params[0] >> 16;
    uint8_t* data = (uint8_t*) msg->params[1];

#ifdef LPM_HAS_SPI_FLASH
    uint16_t bytesToWrite = msg->params[0] & 0xffff;
    uint16_t bytesWritten = 0;

    /* Now write the rest of the data if there is any */
    if (bytesToWrite > 0)
    {
        /* If it is staying in current sector */
        if ((currentAddress / SPIFLASH_SECTOR_SIZE) == ((currentAddress + bytesToWrite) / SPIFLASH_SECTOR_SIZE))
        {
            /* Need to erase for first right no matter what so this handles that */
            if ((currentAddress % SPIFLASH_SECTOR_SIZE) == 0)
            {
                nvram_erase(currentAddress, currentAddress + SPIFLASH_SECTOR_SIZE - 1);
            }
            nvram_write(currentAddress, bytesToWrite, data + bytesWritten);
            currentAddress += bytesToWrite;
        }
        /* If it crosses sector boundary we have to erase next sector seperately*/
        else
        {
            /* Fill rest of sector */
            uint32_t bytesLeftInSector = SPIFLASH_SECTOR_SIZE - (currentAddress % SPIFLASH_SECTOR_SIZE);
            nvram_write(currentAddress, bytesLeftInSector, data + bytesWritten);
            currentAddress += bytesLeftInSector;
            bytesToWrite -= bytesLeftInSector;
            bytesWritten += bytesLeftInSector;

            /* Erase and start new sector */
            nvram_erase(currentAddress, currentAddress + SPIFLASH_SECTOR_SIZE - 1);
            nvram_write(currentAddress, bytesToWrite, data + bytesWritten);
            currentAddress += bytesToWrite;
        }
    }
    UpdateState.bulkUpdateEndpoint.BytesRxd += msg->params[0] & 0xffff;
#endif
    IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer, tID, (uint32_t) data);
}

/*
 * @func UpdateModule_Downloading_HandleBulkComplete
 *
 * @brief Move on to the next state when the transfer is complete.
 *        Notify Soundtouch that we're okay.
 *
 * @param n/a
 *
 * @return n/a
 */
static void UpdateModule_Downloading_HandleBulkComplete(void)
{
    LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "Update download complete.");

    IPCBulk_DeactivateEndpoint(&UpdateState.bulkUpdateEndpoint);

    uint32_t status = REV_WORD_BYTES(UPDATE_STATUS_READY_OK);
    IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_UPDATE_STATUS, NULL, &status, sizeof(status));

    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
}
