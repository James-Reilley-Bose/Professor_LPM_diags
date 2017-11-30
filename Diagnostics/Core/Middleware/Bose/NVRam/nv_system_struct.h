#ifndef NV_SYSTEN_STRUCT_H
#define NV_SYSTEM_STRUCT_H

#include "UpdateManagerBlob.h" // for version length
#include "unifySourceList.h"
#include "ProductNvMfg.h"
#include "ProductSystemParams.h"

#define NV_SYSTEM_PARAMS_SIZE 1024
#define NUM_OF_PADDING_BYTES  (NV_SYSTEM_PARAMS_SIZE-sizeof(NV_ProductSysParams_t)-sizeof(uint16_t)-sizeof(NV_SystemSettings_t))
#define MAX_NUM_SOURCE_FOR_NV_STORAGE (26)
#define NV_SYSTEM_VERSION 0x01
#pragma pack(1)

typedef struct
{
    uint8_t firstTimeInit;      // Flag used to detect 1st post manufacturing use
    uint8_t rebootOnAssert;
    uint8_t systemStructVersion;
} NV_SystemSettings_t;

typedef struct
{
    NV_SystemSettings_t sysSettings;
    NV_ProductSysParams_t product;
    uint8_t             padding2[NUM_OF_PADDING_BYTES];
    uint16_t            systemCtrlWord;
} NV_SystemParams_t;
#pragma pack()

#endif
