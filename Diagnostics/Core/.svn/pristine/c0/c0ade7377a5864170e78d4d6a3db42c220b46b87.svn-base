//
// UpdateManagerBlob.h
// dr1005920
//

#ifndef UPDATE_BLOB
#define UPDATE_BLOB

#define CHECKSUM_BUFFER_SIZE 256
#define MANIFEST_OFFSET (BLOB_ADDRESS + sizeof(UpdateBlobHeader))
#define MASTER_VERSION_STRING_LENGTH 26
#define IMAGE_VERSION_STRING_LENGTH 24
#define IMAGE_NAME_LENGTH 24

// Manifest flags
#define ACC_UPDATE_REQUIRED (1<<0)

typedef struct
{
    uint32_t fileAddress;
    uint32_t size;
    char* filename;
    BOOL found;
} UpdateFile_t;

typedef struct
{
    char ImageName[IMAGE_NAME_LENGTH];
    char ImageVersion[IMAGE_VERSION_STRING_LENGTH];
    uint32_t ImageChecksum;
    uint16_t Flags;
    uint8_t HeatshrinkWindow;
    uint8_t HeatshrinkLength;
    uint32_t ImageOffset;
    uint32_t ImageSize;
} ManifestEntry;

typedef struct
{
    uint32_t TotalLength;
    char MasterVersion[MASTER_VERSION_STRING_LENGTH];
    uint16_t NumberOfImages;
} UpdateBlobHeader;

BOOL GetManifestEntryByName(UpdateBlobHeader* blob, uint32_t blobAddr, ManifestEntry* entry, char* name);
BOOL GetManifestEntryByIndex(UpdateBlobHeader* blob, uint32_t blobAddr, ManifestEntry* entry, uint8_t index);
BOOL GetManifest(UpdateBlobHeader* blob, uint32_t blobAddr, uint8_t* manifestBuffer);
BOOL GetImageByName(UpdateBlobHeader* blob, uint32_t blobAddr, char* name, uint8_t* image, uint32_t* imageSize);
BOOL GetImageOffsetByName(UpdateBlobHeader* blob, uint32_t blobAddr, char* name, uint32_t* imageOffset, uint32_t* imageSize);
BOOL GetImageOffsetByIndex(UpdateBlobHeader* blob, uint32_t blobAddr, uint8_t index, uint32_t* imageOffset, uint32_t* imageSize);
BOOL GetImageVersionByName(UpdateBlobHeader* blob, uint32_t blobAddr, char* name, char* versionBuffer);
BOOL GetImageNameByIndex(UpdateBlobHeader* blob, uint32_t blobAddr, uint8_t index, char* nameBuffer);
BOOL ReadBlobHeaderFromFlash(UpdateBlobHeader* blob, uint32_t blobAddr);
BOOL GetUpdateFileInfoFromManifest(UpdateBlobHeader* blob, uint32_t blobAddr, UpdateFile_t files[], uint8_t num_files);
BOOL IsValidBlobHeader(UpdateBlobHeader* header);
void GetStoredBlobVersion(char* blob);

#endif /* UPDATE_BLOB */
