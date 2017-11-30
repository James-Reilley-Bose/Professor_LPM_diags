#include "project.h"
#include "nv.h"
#include "nv_internal_params.h"
#include "nv_mfg.h"
#include "nv_mfg_struct.h"
#include "InternalFlashAPI.h"
#include "buffermanager.h"
#include "etap.h"

static NV_ManufacturingParams_t NVMfgParams = { 0 };
static nv_internal_params_info NVInfoMfgParams =
{
    .start_address = INTERNAL_FLASH_MANUFACTURING_START,
    .end_address = INTERNAL_FLASH_MANUFACTURING_END,
    .struct_size = sizeof(NV_ManufacturingParams_t),
};

static void NV_WriteDefaultMfgParams(void);
static void NV_ValidateMfgParams(void);
static BOOL NV_AreValidMfgParams(void);

// Generate a compile time error if the param size is wrong
// Manually edit the padding in NV_ManufacturingParams_t
extern const uint8_t NvMfgParamErrorCheck[(sizeof(NV_ManufacturingParams_t) == NV_MFG_PARAMS_SIZE) ? 1 : -1];

extern const NV_ManufacturingParams_t MfgParamsDefaults;

void NV_InitMfgParams(void)
{
    uint32_t params = NV_FindCurrentParams(&NVInfoMfgParams, &NVMfgParams);
    if (params == NULL)
    {
        NV_WriteDefaultMfgParams();
    }
    NV_ValidateMfgParams();

    // verify that the software is running on the correct system.
    PRODUCT_VARIANT product = NV_GetProductVariant();
    
    debug_assert(product == PRODUCT_VARIANT_DEFAULT);
}

static void NV_WriteDefaultMfgParams(void)
{
    memcpy(&NVMfgParams, &MfgParamsDefaults, sizeof(NV_ManufacturingParams_t));
    NV_CommitMfgParams();
}

void NV_CommitMfgParams(void)
{
    NV_CommitParams(&NVInfoMfgParams, &NVMfgParams);
}

static void NV_ValidateMfgParams(void)
{
    // Baid-Aids
    if (NVMfgParams.mfgStructVersion < 0x01)
    {
        NV_WriteDefaultMfgParams();
    }

    NVMfgParams.mfgStructVersion = NV_MFG_VERSION;
    NV_CommitMfgParams();

#ifdef HAS_RFID
    // Configure the RFID chip in standalone i2c mode
    rfdConfigureI2C();

    RFD_block_Struct rfidData;
    if (ERROR == rfdRead(&rfidData))
    {
        if (!NV_AreValidMfgParams())
        {
            // We're bricked.
            SystemBehavior_HandleSystemError(1);
        }
        else
        {
            rfdDeconfigureI2C();
            return;
        }
    }
    rfdDeconfigureI2C();

    if (!isRFIDChecksumValid(&rfidData))
    {
        if (!NV_AreValidMfgParams())
        {
            // We're bricked.
            SystemBehavior_HandleSystemError(2);
        }
        else
        {
            return;
        }
    }

    if (!areRFIDChannelMasksValid(&rfidData))
    {
        if (!NV_AreValidMfgParams())
        {
            // Brick!
            SystemBehavior_HandleSystemError(5);
        }
        else
        {
            return;
        }
    }


    if (rfidData.checksum != NVMfgParams.rfidChecksum)
    {
        NVMfgParams.defaultLanguage = rfidData.defaultLanguage;
        NVMfgParams.productVariant = PRODUCT_VARIANT_DEFAULT; // This does not exist in the RFID, so we set it to our builtin value
        NVMfgParams.speakerPackageVariant = rfidData.speakerPackage;
        NVMfgParams.rfidChecksum = rfidData.checksum;
        NVMfgParams.countryVariant = rfidData.country;
        NVMfgParams.regionVariant = rfidData.region;
        NVMfgParams.radioMask24GHz = rfidData.wifiMask_2_4Ghz;
        NVMfgParams.radioMask52GHz = rfidData.wifiMask_5_2Ghz;
        NVMfgParams.radioMask58GHz = rfidData.wifiMask_5_8Ghz;
        NV_CommitMfgParams();
    }
#endif

    // So now that we've processed the RFID data if we fail validation we're considered bricked
    // For Ginger, this should never happen because the defaults set above should be good.
    
    if (!NV_AreValidMfgParams())
    {
        // TODO dj1005472 - bring in SystemBehavior
        debug_assert(FALSE);
        //SystemBehavior_HandleSystemError(3);
    }
    
}


static BOOL NV_AreValidMfgParams(void)
{
    // TODO dj1005472 - check product specific params
    return (
               NVMfgParams.productVariant < PRODUCT_NUM
               && NVMfgParams.productVariant > PRODUCT_UNDEFINED
               && NVMfgParams.regionVariant < REGION_NUM
               && NVMfgParams.regionVariant > REGION_UNDEFINED
               && NVMfgParams.countryVariant < COUNTRY_NUM
               && NVMfgParams.countryVariant > COUNTRY_UNDEFINED
               && NVMfgParams.defaultLanguage < LANGUAGE_NUM
           );
}



/******************************************************************************
*
*   Manufacturing Parameters Getters and Setters
*
******************************************************************************/
Rotten_StoreEnable NV_GetIsTapLoggerEnabled(void)
{
    return (Rotten_StoreEnable) NVMfgParams.isTapLoggerEnabled;
}

void NV_SetIsTapLoggerEnabled(Rotten_StoreEnable enabled)
{
    if (NVMfgParams.isTapLoggerEnabled != enabled)
    {
        NVMfgParams.isTapLoggerEnabled = enabled;
        NV_CommitMfgParams();
    }
}

LANGUAGE_SELECTION NV_GetDefaultLanguage(void)
{
    return (LANGUAGE_SELECTION) NVMfgParams.defaultLanguage;
}

void NV_SetDefaultLanguage(LANGUAGE_SELECTION lang)
{
    if (NVMfgParams.defaultLanguage != lang)
    {
        NVMfgParams.defaultLanguage = lang;
    }
}

/*
*   s_MfgParams.PreBoothTestPassed
*/
uint8_t NV_GetPreBoothPassed (void)
{
    return (NVMfgParams.preBoothPassed);
}

void NV_SetPreBoothPassed (uint8_t state)
{
    if (NVMfgParams.preBoothPassed != state)
    {
        NVMfgParams.preBoothPassed = state;
    }
}

/*
*   s_MfgParams.RegionVariant
*/
REGION_VARIANT NV_GetRegionVariant (void)
{
    return ((REGION_VARIANT)NVMfgParams.regionVariant);
}

void NV_SetRegionVariant (REGION_VARIANT variant)
{
    debug_assert (variant < REGION_NUM);
    if (NVMfgParams.regionVariant != (uint8_t)variant)
    {
        NVMfgParams.regionVariant = (uint8_t)variant;
    }
}

/*
*   s_MfgParams.CountryVariant
*/
COUNTRY_VARIANT NV_GetCountryVariant (void)
{
    return ((COUNTRY_VARIANT)NVMfgParams.countryVariant);
}

void NV_SetCountryVariant (COUNTRY_VARIANT variant)
{
    debug_assert (variant < COUNTRY_NUM);
    if (NVMfgParams.countryVariant != (uint8_t)variant)
    {
        NVMfgParams.countryVariant = (uint8_t)variant;
    }
}

/*
*   s_MfgParams.ProductVariant
*/
PRODUCT_VARIANT NV_GetProductVariant (void)
{
    return ((PRODUCT_VARIANT)NVMfgParams.productVariant);
}

void NV_SetProductVariant (PRODUCT_VARIANT variant)
{
    debug_assert (variant < PRODUCT_NUM);
    if (NVMfgParams.productVariant != (uint8_t)variant)
    {
        NVMfgParams.productVariant = (uint8_t)variant;
    }
}

/*
*   s_MfgParams.FinalBoothPassed
*/
uint8_t NV_GetFinalBoothPassed (void)
{
    return (NVMfgParams.finalBoothPassed);
}

void NV_SetFinalBoothPassed (uint8_t state)
{
    if (NVMfgParams.finalBoothPassed != state)
    {
        NVMfgParams.finalBoothPassed = state;
    }
}

/*
*   s_MfgParams.FunctionalPassed
*/
uint8_t NV_GetFunctionalPassed (void)
{
    return (NVMfgParams.functionalPassed);
}

void NV_SetFunctionalPassed (uint8_t state)
{
    if (NVMfgParams.functionalPassed != state)
    {
        NVMfgParams.functionalPassed = state;
    }
}

/*
*   s_MfgParams.systemSerialNo
*/
void NV_GetSystemSerialNo (uint8_t* buffer, uint8_t len, uint32_t mutexWaitusec)
{
    debug_assert (len <= SYSTEM_SERIAL_NO_LEN);
    debug_assert (buffer != (uint8_t*)NULL);

    memcpy(buffer, NVMfgParams.systemSerialNo, len);
}

void NV_SetSystemSerialNo (uint8_t* serial, uint8_t len, uint32_t mutexWaitusec)
{
    debug_assert (len <= SYSTEM_SERIAL_NO_LEN);
    debug_assert (serial != (uint8_t*)NULL);

    if (memcmp(serial, NVMfgParams.systemSerialNo, len) != 0)
    {
        memcpy(NVMfgParams.systemSerialNo, serial, len);
    }
}

BL_STATE NV_GetBootState(void)
{
    return (BL_STATE) NVMfgParams.bootState;
}

void NV_SetBootState(BL_STATE bootState)
{
    if (NVMfgParams.bootState != bootState)
    {
        NVMfgParams.bootState = bootState;
        NV_CommitMfgParams();
    }
}

static const struct nvMfgFieldMapType
{
    const char* fieldName;
    const char* fieldFullName;
    uint8_t fieldOffset;
    uint8_t fieldSize;
} nvMfgFieldMap[] =
{
    {.fieldName = "v",   .fieldFullName = "version", .fieldOffset = offsetof(NV_ManufacturingParams_t, mfgStructVersion), .fieldSize = member_size(NV_ManufacturingParams_t, mfgStructVersion)},
    {.fieldName = "tl",  .fieldFullName = "tapLoggerEnabled", .fieldOffset = offsetof(NV_ManufacturingParams_t, isTapLoggerEnabled), .fieldSize = member_size(NV_ManufacturingParams_t, isTapLoggerEnabled)},
    {.fieldName = "co",  .fieldFullName = "country", .fieldOffset = offsetof(NV_ManufacturingParams_t, countryVariant), .fieldSize = member_size(NV_ManufacturingParams_t, countryVariant)},
    {.fieldName = "rv",  .fieldFullName = "region", .fieldOffset = offsetof(NV_ManufacturingParams_t, regionVariant), .fieldSize = member_size(NV_ManufacturingParams_t, regionVariant)},
    {.fieldName = "pv",  .fieldFullName = "product", .fieldOffset = offsetof(NV_ManufacturingParams_t, productVariant), .fieldSize = member_size(NV_ManufacturingParams_t, productVariant)},
    {.fieldName = "l",   .fieldFullName = "language", .fieldOffset = offsetof(NV_ManufacturingParams_t, defaultLanguage), .fieldSize = member_size(NV_ManufacturingParams_t, defaultLanguage)},
    {.fieldName = "rfc", .fieldFullName = "RFIDChecksum", .fieldOffset = offsetof(NV_ManufacturingParams_t, rfidChecksum), .fieldSize = member_size(NV_ManufacturingParams_t, rfidChecksum)},
    {.fieldName = "pbp", .fieldFullName = "preBoothPassed", .fieldOffset = offsetof(NV_ManufacturingParams_t, preBoothPassed), .fieldSize = member_size(NV_ManufacturingParams_t, preBoothPassed)},
    {.fieldName = "fbp", .fieldFullName = "finalBoothPassed", .fieldOffset = offsetof(NV_ManufacturingParams_t, finalBoothPassed), .fieldSize = member_size(NV_ManufacturingParams_t, finalBoothPassed)},
    {.fieldName = "fp",  .fieldFullName = "funcPassed", .fieldOffset = offsetof(NV_ManufacturingParams_t, functionalPassed), .fieldSize = member_size(NV_ManufacturingParams_t, functionalPassed)},
    {.fieldName = "lu1", .fieldFullName = "luapTest1", .fieldOffset = offsetof(NV_ManufacturingParams_t, luapTest1), .fieldSize = member_size(NV_ManufacturingParams_t, luapTest1)},
    {.fieldName = "lu2", .fieldFullName = "luapTest2", .fieldOffset = offsetof(NV_ManufacturingParams_t, luapTest2), .fieldSize = member_size(NV_ManufacturingParams_t, luapTest2)},
    {.fieldName = "lu3", .fieldFullName = "luapTest3", .fieldOffset = offsetof(NV_ManufacturingParams_t, luapTest3), .fieldSize = member_size(NV_ManufacturingParams_t, luapTest3)},
    {.fieldName = "ss",  .fieldFullName = "sysSerial", .fieldOffset = offsetof(NV_ManufacturingParams_t, systemSerialNo), .fieldSize = member_size(NV_ManufacturingParams_t, systemSerialNo)},
    {.fieldName = "ss2", .fieldFullName = "sysSerial2", .fieldOffset = offsetof(NV_ManufacturingParams_t, systemSerialNo2), .fieldSize = member_size(NV_ManufacturingParams_t, systemSerialNo2)},
    {.fieldName = "bs",  .fieldFullName = "bootState", .fieldOffset = offsetof(NV_ManufacturingParams_t, bootState), .fieldSize = member_size(NV_ManufacturingParams_t, bootState)},
    {.fieldName = "psv", .fieldFullName = "productStructVersion", .fieldOffset = offsetof(NV_ManufacturingParams_t, productStructVersion), .fieldSize = member_size(NV_ManufacturingParams_t, productStructVersion)},
    {.fieldName = "mcw", .fieldFullName = "mfgCtrlWord", .fieldOffset = offsetof(NV_ManufacturingParams_t, mfgCtrlWord), .fieldSize = member_size(NV_ManufacturingParams_t, mfgCtrlWord)},
};
#define nvMfgFieldMapSize (sizeof(nvMfgFieldMap) / sizeof(struct nvMfgFieldMapType))

BOOL NV_GetMfgField(const char* fieldName, uint8_t* value, uint8_t* size)
{
    for (uint8_t i = 0; i < nvMfgFieldMapSize; i++)
    {
        if (!strcmp(nvMfgFieldMap[i].fieldName, fieldName))
        {
            memcpy(value, ((const uint8_t*)&NVMfgParams) + nvMfgFieldMap[i].fieldOffset, nvMfgFieldMap[i].fieldSize);
            *size = nvMfgFieldMap[i].fieldSize;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL NV_SetMfgField(const char* fieldName, const uint8_t* value)
{
    for (uint8_t i = 0; i < nvMfgFieldMapSize; i++)
    {
        if (!strcmp(nvMfgFieldMap[i].fieldName, fieldName))
        {
            memcpy(((uint8_t*)&NVMfgParams) + nvMfgFieldMap[i].fieldOffset, value, nvMfgFieldMap[i].fieldSize);
            NV_CommitMfgParams();
            return TRUE;
        }
    }

    return FALSE;
}

uint8_t NV_GetMfgFieldSize(const char* fieldName)
{
    for (uint8_t i = 0; i < nvMfgFieldMapSize; i++)
    {
        if (!strcmp(nvMfgFieldMap[i].fieldName, fieldName))
        {
            return (nvMfgFieldMap[i].fieldSize);
        }
    }

    return 0;
}

void NV_DumpMfgFields(void)
{
    for (uint8_t i = 0; i < nvMfgFieldMapSize; i++)
    {
        TAP_Printf("%04s (%02d) = ", nvMfgFieldMap[i].fieldName, nvMfgFieldMap[i].fieldSize);
        uint8_t* fieldAddr = ((uint8_t*)&NVMfgParams) + nvMfgFieldMap[i].fieldOffset;
        for (uint8_t j = 0; j < nvMfgFieldMap[i].fieldSize; j++)
        {
            TAP_Printf("%02x ", *fieldAddr++);
        }
        TAP_PrintString("\r\n");
    }
}

void NV_DumpMfgFieldNames(void)
{
    for (uint8_t i = 0; i < nvMfgFieldMapSize; i++)
    {
        TAP_Printf("    %04s - %s\r\n", nvMfgFieldMap[i].fieldName, nvMfgFieldMap[i].fieldFullName);
    }
}

uint32_t NV_MfgStructSize(void)
{
    return sizeof(NV_ManufacturingParams_t);
}
