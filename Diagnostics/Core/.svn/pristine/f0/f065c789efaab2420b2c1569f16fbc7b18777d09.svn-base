//
// UpdateManagerBlob.c
// dr1005920
//

#include "project.h"
#include "UpdateManagerBlob.h"
#include "InternalFlashAPI.h"
#include "UpdateVariant.h"

/*
 * @func BlobCheckSum
 *
 * @brief Checks blob checksum
 *
 * @param UpdateBlobHeader* blob - pointer to header of blob
 *        uint32_t blobAddr - memory address of the blob
 *
 * @return BOOL - whether or not checksum is correct
 */
BOOL BlobCheckSum(UpdateBlobHeader* blob, uint32_t blobAddr)
{
    uint32_t calcCheckSum = 0;
    if (blob->TotalLength == 0x00000000 || blob->TotalLength > MAX_BLOB_LENGTH)
    {
        return FALSE;
    }

    // Buffered to limit flash reads when computing checksum
    uint8_t buffer[CHECKSUM_BUFFER_SIZE];

    // Calculate checksum except for last page in flash
    uint32_t bytesRead = 0;
    do
    {
        UpdateVariant_FlashRead(blobAddr + bytesRead, buffer, sizeof(buffer));

        for (uint32_t i = 0; i < sizeof(buffer); i++)
        {
            calcCheckSum += buffer[i];
        }

        bytesRead += sizeof(buffer);
    }
    while ( blob->TotalLength - bytesRead > sizeof(buffer) );

    // Read the last page
    UpdateVariant_FlashRead(blobAddr + bytesRead, buffer, blob->TotalLength - bytesRead);

    // The length of the file is required to be a multiple of four, so this is safe.
    // Also, ignore the last word because it's the appended checksum.
    for (uint32_t i = 0; i < (blob->TotalLength - bytesRead) - sizeof(uint32_t); i++)
    {
        calcCheckSum += buffer[i];
    }

    uint32_t readCheckSum = *((uint32_t*) &buffer[(blob->TotalLength - bytesRead) - sizeof(uint32_t)]);

    return (readCheckSum == calcCheckSum);
}

/*
 * @func ReadBlobHeaderFromFlash
 *
 * @brief Copies blob header from external flash to struct in ram
 *
 * @param UpdateBlobHeader* blob - pointer to header of blob
 *        uint32_t blobAddr - memory address of the blob
 *
 * @return BOOL - whether or not checksum of blob is correct
 */
BOOL ReadBlobHeaderFromFlash(UpdateBlobHeader* blob, uint32_t blobAddr)
{
    UpdateVariant_FlashRead(blobAddr, blob, sizeof(UpdateBlobHeader));

    if (!IsValidBlobHeader(blob))
    {
        return FALSE;
    }

    return BlobCheckSum(blob, blobAddr);
}

/*
 * @func GetManifest
 *
 * @brief Gets entire manifest
 *
 * @param UpdateBlobHeader* blob - pointer to blob
 *        uint32_t blobAddr - memory address of the blob
 *        uint8_t* manifestBuffer - buffer to copy into
 *
 * @return TRUE always
 */
BOOL GetManifest(UpdateBlobHeader* blob, uint32_t blobAddr, uint8_t* manifestBuffer)
{
    UpdateVariant_FlashRead((uint32_t)(blobAddr + sizeof(UpdateBlobHeader)), manifestBuffer, sizeof(ManifestEntry) * blob->NumberOfImages);
    return TRUE;
}

/*
 * @func GetManifestEntryByName
 *
 * @brief Gets a manifest entry
 *
 * @param UpdateBlobHeader* blob - pointer to blob
 *        uint32_t blobAddr - memory address of the blob
 *        ManifestEntry* entry - return entry
 *        char* name - name to look for in manifest
 *
 * @return TRUE if it was found FALSE if it wasn't
 */
BOOL GetManifestEntryByName(UpdateBlobHeader* blob, uint32_t blobAddr, ManifestEntry* entry, char* name)
{
    for (int i = 0; i < blob->NumberOfImages; i++)
    {
        UpdateVariant_FlashRead((uint32_t)((blobAddr + sizeof(UpdateBlobHeader)) + (i * sizeof(ManifestEntry))), (uint8_t*)entry, sizeof(ManifestEntry));
        if (strcmp(name, entry->ImageName) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * @func GetManifestEntryByIndex
 *
 * @brief Gets a manifest entry
 *
 * @param UpdateBlobHeader* blob - pointer to blob
 *        uint32_t blobAddr - memory address of the blob
 *        ManifestEntry* entry - return entry
 *        uint8_t index - index to look for in manifest
 *
 * @return TRUE if it was found FALSE if it wasn't
 */
BOOL GetManifestEntryByIndex(UpdateBlobHeader* blob, uint32_t blobAddr, ManifestEntry* entry, uint8_t index)
{
    if (index < blob->NumberOfImages)
    {
        UpdateVariant_FlashRead((uint32_t)((blobAddr + sizeof(UpdateBlobHeader)) + (index * sizeof(ManifestEntry))), entry, sizeof(ManifestEntry));
        return TRUE;
    }
    return FALSE;
}

/*
 * @func GetImageOffsetByName
 *
 * @brief Gets flash offset and size of an image
 *
 * @param UpdateBlobHeader* blob - pointer to blob
 *        uint32_t blobAddr - memory address of the blob
 *        char* name - name to look for in manifest
 *        uitn32_t* imageOffset - offset return
 *        uint32_t* imageSize- size return
 *
 * @return TRUE if it was found FALSE if it wasn't
 */
BOOL GetImageOffsetByName(UpdateBlobHeader* blob, uint32_t blobAddr, char* name, uint32_t* imageOffset, uint32_t* imageSize)
{
    ManifestEntry entry = {0};
    if (GetManifestEntryByName(blob, blobAddr, &entry, name))
    {
        *imageOffset = entry.ImageOffset;
        *imageSize = entry.ImageSize;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*
 * @func GetImageOffsetByIndex
 *
 * @brief Gets flash offset and size of an image
 *
 * @param UpdateBlobHeader* blob - pointer to blob
 *        uint32_t blobAddr - memory address of the blob
 *        uint8_t index - index to look for in manifest
 *        uitn32_t* imageOffset - offset return
 *        uint32_t* imageSize- size return
 *
 * @return TRUE if it was found FALSE if it wasn't
 */
BOOL GetImageOffsetByIndex(UpdateBlobHeader* blob, uint32_t blobAddr, uint8_t index, uint32_t* imageOffset, uint32_t* imageSize)
{
    ManifestEntry entry = {0};
    if (GetManifestEntryByIndex(blob, blobAddr, &entry, index))
    {
        *imageOffset = entry.ImageOffset;
        *imageSize = entry.ImageSize;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*
 * @func GetImageVersionByName
 *
 * @brief Gets version string of an image
 *
 * @param UpdateBlobHeader* blob - pointer to blob
 *        uint32_t blobAddr - memory address of the blob
 *        char* name - name to look for in manifest
 *        char* versionBuffer - buffer to copy version string into
 *
 * @return TRUE if it was found FALSE if it wasn't
 */
BOOL GetImageVersionByName(UpdateBlobHeader* blob, uint32_t blobAddr, char* name, char* versionBuffer)
{
    ManifestEntry entry = {0};
    if (GetManifestEntryByName(blob, blobAddr, &entry, name))
    {
        strcpy(versionBuffer, entry.ImageVersion);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*
 * @func GetImageNameByIndex
 *
 * @brief Gets name string of an image
 *
 * @param UpdateBlobHeader* blob - pointer to blob
 *        uint32_t blobAddr - memory address of the blob
 *        uint8_t index - index to look for in manifest
 *        char* nameBuffer - buffer to copy version string into
 *
 * @return TRUE if it was found FALSE if it wasn't
 */
BOOL GetImageNameByIndex(UpdateBlobHeader* blob, uint32_t blobAddr, uint8_t index, char* nameBuffer)
{
    ManifestEntry entry = {0};
    if (GetManifestEntryByIndex(blob, blobAddr, &entry, index))
    {
        strcpy(nameBuffer, entry.ImageName);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


BOOL GetUpdateFileInfoFromManifest(UpdateBlobHeader* blob, uint32_t blobAddr, UpdateFile_t files[], uint8_t num_files)
{
    uint8_t foundcount = 0;
    for (uint8_t i = 0; i < num_files; i++)
    {
        if (GetImageOffsetByName(blob, blobAddr, files[i].filename, &files[i].fileAddress, &files[i].size))
        {
            // Add length of header and manifests to address
            files[i].fileAddress += (sizeof(UpdateBlobHeader) + (blob->NumberOfImages * sizeof(ManifestEntry)));
            files[i].found = TRUE;
            foundcount++;
        }
    }
    if (foundcount == num_files)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL IsValidBlobHeader(UpdateBlobHeader* header)
{
    return (header->TotalLength <= MAX_BLOB_LENGTH);
}

void GetStoredBlobVersion(char* version)
{
    UpdateBlobHeader blobHeader = {0};
    if (!ReadBlobHeaderFromFlash(&blobHeader, BLOB_ADDRESS))
    {
        strncpy(version, "0.0.0.0", MASTER_VERSION_STRING_LENGTH);
    }
    else
    {
        strncpy(version, blobHeader.MasterVersion, MASTER_VERSION_STRING_LENGTH);
    }
}
