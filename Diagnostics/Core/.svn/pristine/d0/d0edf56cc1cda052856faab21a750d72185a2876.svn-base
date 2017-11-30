//
// UpdateModuleMicro.c
// dr1005920
//

#include "UpdateModuleBase.h"
#include "UpdateModuleMicro.h"
#include "UpdateManagerTask.h"
#include "UpdateManagerBlob.h"
#include "IpcProtocolLpm.h"
#include "IpcRouterTask.h"
#include "nv_mfg.h"
#include "buffermanager.h"
#include "internalFlashAPI.h"
#include "versionlib.h"
#include "UpdateVariant.h"

SCRIBE_DECL(update);

static void EraseImageInFlash(UpdateFile_t* updFile, uint32_t startAddressToErase);
static void WriteUpdateToFlash(UpdateFile_t* updFile, uint32_t startAddressToWrite, uint32_t blobAddr);

#define UPDATE_BUFFER_SIZE 0x500

#define MICRO_NUM_UPDATE_FILES 1
UpdateFile_t MicroUpdateFiles[MICRO_NUM_UPDATE_FILES] =
{
    {0, 0, LPM_UPDATE_FILENAME, FALSE},
};

#define MICRO_MFG_NUM_UPDATE_FILES 1
UpdateFile_t MicroMfgUpdateFiles[MICRO_MFG_NUM_UPDATE_FILES] =
{
    {0, 0, LPM_MFG_UPDATE_FILENAME, FALSE},
};

#define MICRO_BL_NUM_UPDATE_FILES 1
UpdateFile_t MicroBLUpdateFiles[MICRO_BL_NUM_UPDATE_FILES] =
{
    {0, 0, LPM_BL_UPDATE_FILENAME, FALSE},
};

void UpdateModuleMicro_Init(void)
{
}

BOOL UpdateModuleMicro_HandlePacket(GENERIC_MSG_t* msg)
{
    return FALSE;
}

void UpdateModuleMicro_EnterDoMicro(void)
{
    UpdateBlobHeader upd;
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Setting LPM Update Flag");
        UpdateModuleMicro_Start(&upd);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file (LPM)!");
        UpdateManagerTask_CompleteState(UPDATE_BLOB_ERROR);
    }
}

/*
 * @func UpdateModuleMicro_Start(UpdateBlobHeader* blob)
 *
 * @brief Set the LPM update flag
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleMicro_Start(UpdateBlobHeader* blob)
{
    ManifestEntry LPMManifest;
    if (!GetUpdateFileInfoFromManifest(blob, BLOB_ADDRESS, MicroUpdateFiles, MICRO_NUM_UPDATE_FILES) ||
            !GetManifestEntryByName(blob, BLOB_ADDRESS, &LPMManifest, MicroUpdateFiles[0].filename))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Error reading micro update files!");
        UpdateManagerTask_CompleteState(UPDATE_FILE_NOT_FOUND);
        return;
    }

    if (!UpdateState.forced &&
            (VERSION_COMPARE_SAME_VERSION == VersionCompareVersions(VersionGetVersion(), LPMManifest.ImageVersion)))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "LPM already at version %s", LPMManifest.ImageVersion);
        UpdateManagerTask_CompleteState(UPDATE_SKIPPED_VERSION);
        return;
    }

    // Config bl to do update
#if 0 // TODO - dj1005472 - do we need this..?
    if (NV_GetFirstTimeInit())
    {
        NV_SetBootSourceAndState(BL_STATE_UPDATE_USER, SOURCE_UNIFY);
    }
    else
    {
        NV_SetBootState(BL_STATE_UPDATE_USER);
    }
    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
#else
    NV_SetBootState(BL_STATE_UPDATE_USER);
    UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
#endif
}

void UpdateModuleMicro_EnterDoMicroMfg(void)
{
    UpdateBlobHeader upd;
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Starting Mfg update");
        UpdateModuleMicro_StartMfg(&upd);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file (LPM)!");
    }
}

/*
 * @func UpdateModuleMicro_StartMfg(UpdateBlobHeader* blob)
 *
 * @brief Set the LPM update flag
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleMicro_StartMfg(UpdateBlobHeader* blob)
{
    //Make sure update is found before reboot
    if (GetUpdateFileInfoFromManifest(blob, BLOB_ADDRESS, MicroMfgUpdateFiles, MICRO_MFG_NUM_UPDATE_FILES))
    {
        // Update the mfg image
        // Erase old mfg image from flash
        EraseImageInFlash(&MicroMfgUpdateFiles[0], INTERNAL_FLASH_RECOVERY_IMG_START);
        // Now write the new image
        WriteUpdateToFlash(&MicroMfgUpdateFiles[0], INTERNAL_FLASH_RECOVERY_IMG_START, BLOB_ADDRESS);
        system_reboot();
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "No micro mfg update files found!");
    }
}

void UpdateModuleMicro_EnterDoMicroBL(void)
{
    UpdateBlobHeader upd;
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Starting LPM Bootloader update");
        UpdateModuleMicro_StartBL(&upd);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file!");
    }
}

/*
 * @func UpdateModuleMicro_StartBL(UpdateBlobHeader* blob)
 *
 * @brief Set the LPM update flag
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleMicro_StartBL(UpdateBlobHeader* blob)
{
    //Make sure update is found before reboot
    if (GetUpdateFileInfoFromManifest(blob, BLOB_ADDRESS, MicroBLUpdateFiles, MICRO_BL_NUM_UPDATE_FILES))
    {
        // Update the bl image
        // Erase old bl image from flash
        EraseImageInFlash(&MicroBLUpdateFiles[0], INTERNAL_FLASH_BOOTLOADER_START);
        // Now write the new image
        WriteUpdateToFlash(&MicroBLUpdateFiles[0], INTERNAL_FLASH_BOOTLOADER_START, BLOB_ADDRESS);
        system_reboot();
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "No micro bl update files found!");
    }
}

static void EraseImageInFlash(UpdateFile_t* updFile, uint32_t startAddressToErase)
{
    uint32_t addressToErase = startAddressToErase;

    // Loop through needed sectors and erase
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Erasing flash");
    while (addressToErase < (startAddressToErase + updFile->size))
    {
        //        LOG(update,ROTTEN_LOGLEVEL_NORMAL,
        //             "\n\tErasing flash...................%d%%", (100 * (addressToErase - startAddressToErase) /  updFile->size));
        InternalFlashAPI_EraseSectorByAddress(addressToErase);
        addressToErase += InternalFlashAPI_GetSectorSize(addressToErase);
    }
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Erasing flash complete");
}

static void WriteUpdateToFlash(UpdateFile_t* updFile, uint32_t updAddr, uint32_t blobAddr)
{
    uint8_t* update_buffer = BufferManagerGetBuffer(UPDATE_BUFFER_SIZE);
    debug_assert(update_buffer);

    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Writing flash");
    unsigned int i = 0;
    for (; i < (updFile->size / UPDATE_BUFFER_SIZE); i++)
    {
        UpdateVariant_FlashRead(updFile->fileAddress + (i * UPDATE_BUFFER_SIZE) + blobAddr, update_buffer, UPDATE_BUFFER_SIZE);
        InternalFlashAPI_Write(updAddr + (i * UPDATE_BUFFER_SIZE), update_buffer, UPDATE_BUFFER_SIZE);
    }
    UpdateVariant_FlashRead(updFile->fileAddress + (i * UPDATE_BUFFER_SIZE) + blobAddr, update_buffer, (updFile->size % UPDATE_BUFFER_SIZE));
    InternalFlashAPI_Write(updAddr + (i * UPDATE_BUFFER_SIZE), update_buffer, (updFile->size % UPDATE_BUFFER_SIZE));
    BufferManagerFreeBuffer(update_buffer);
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Writing flash complete\n");
}
