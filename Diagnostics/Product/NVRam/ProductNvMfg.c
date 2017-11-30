//
// ProductNvMfg.c
//

#include "project.h"
#include "nv_mfg_struct.h"
#include "nv.h"
#include "nv_mfg.h"

const NV_ManufacturingParams_t MfgParamsDefaults =
{
    .mfgStructVersion        = NV_MFG_VERSION,

    .productVariant          = PRODUCT_VARIANT_DEFAULT,
    .systemSerialNo          = {0},
    .systemSerialNo2         = {0},

    .countryVariant          = COUNTRY_US,
    .regionVariant           = REGION_US,
    .defaultLanguage         = LANGUAGE_ENGLISH,

    .preBoothPassed          = 0,
    .finalBoothPassed        = 0,
    .functionalPassed        = 0,
    .luapTest1               = 0,
    .luapTest2               = 0,
    .luapTest3               = 0,

    .rfidChecksum            = NV_VALID_CTRL_WORD,

    .bootState               = BL_STATE_BOOT_USER,
    .isTapLoggerEnabled      = ROTTEN_STORE_DISABLED,

    .productStructVersion    = NV_MFG_PRODUCT_STRUCT_VERSION,
    .product                 = {0},

    .padding                 = {0},

    .mfgCtrlWord             = NV_VALID_CTRL_WORD,
};