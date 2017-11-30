#ifndef NV_MFG_H
#define NV_MFG_H

#include "nv.h"
#include "RivieraLPM_IpcProtocol.h"

typedef enum
{
    BL_STATE_CLI,
    BL_STATE_BOOT_USER,
    BL_STATE_BOOT_DIAG,
    BL_STATE_UPDATE_USER,
    BL_STATE_NUM_BOOT_STATES
} BL_STATE;

typedef enum
{
    PRODUCT_UNDEFINED,
    PRODUCT_BARDEEN,
    PRODUCT_GINGER,
    PRODUCT_SKIPPER,
    PRODUCT_MAXWELL,
    PRODUCT_EDDIE_SANTOS,
    PRODUCT_PROFESSOR,
    PRODUCT_NUM,
} PRODUCT_VARIANT;

void NV_InitMfgParams(void);
void NV_CommitMfgParams(void);

LANGUAGE_SELECTION NV_GetDefaultLanguage(void);
void NV_SetDefaultLanguage(LANGUAGE_SELECTION lang);

uint32_t NV_GetChipconAddress();
void NV_SetChipconAddress(uint32_t addr);

COUNTRY_VARIANT NV_GetCountryVariant (void);
void NV_SetCountryVariant (COUNTRY_VARIANT variant);

REGION_VARIANT NV_GetRegionVariant (void);
void NV_SetRegionVariant (REGION_VARIANT variant);

PRODUCT_VARIANT NV_GetProductVariant (void);
void NV_SetProductVariant (PRODUCT_VARIANT variant);


SYSTEM_COLOR NV_GetSystemColor (void);
void NV_SetSystemColor (SYSTEM_COLOR color);

BL_STATE NV_GetBootState(void);
void NV_SetBootState(BL_STATE bootState);

void NV_SetBootSourceAndState(BL_STATE bootState, uint8_t src);

Rotten_StoreEnable NV_GetIsTapLoggerEnabled(void);
void NV_SetIsTapLoggerEnabled(Rotten_StoreEnable enabled);

Rotten_StoreEnable NV_GetIsTapLoggerEnabled(void);
void NV_SetIsTapLoggerEnabled(Rotten_StoreEnable enabled);

void NV_DumpMfgFieldNames(void);
void NV_DumpMfgFields(void);
BOOL NV_GetMfgField(const char* fieldName, uint8_t* value, uint8_t* size);
BOOL NV_SetMfgField(const char* fieldName, const uint8_t* value);
uint8_t NV_GetMfgFieldSize(const char* fieldName);

uint8_t NV_GetWiredGain(void);
void NV_SetWiredGain(uint8_t gain);

void NV_SetBootSource(uint8_t src);
uint8_t NV_GetBootSource(void);

uint32_t NV_MfgStructSize(void);
void NV_GetSystemSerialNo (uint8_t* buffer, uint8_t len, uint32_t mutexWaitusec);
void NV_GetConsoleSerialNo(uint8_t* buffer, uint8_t len);

#endif
