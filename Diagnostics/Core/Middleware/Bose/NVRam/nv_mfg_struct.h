//
// nv_mfg_struct.h
//

#ifndef NV_MFG_STRUCT_H
#define NV_MFG_STRUCT_H

#include "ProductNvMfg.h"

#define NV_MFG_PARAMS_SIZE 128
#define NV_MFG_VERSION 0x01
#define SYSTEM_SERIAL_NO_LEN 24

#pragma pack(1)
typedef struct
{
    // NV param info
    uint8_t  mfgStructVersion;

    // Product info
    uint8_t  productVariant;
    uint8_t  systemSerialNo[SYSTEM_SERIAL_NO_LEN];
    uint8_t  systemSerialNo2[SYSTEM_SERIAL_NO_LEN];

    // Locale info
    uint8_t  countryVariant;
    uint8_t  regionVariant;
    uint8_t  defaultLanguage;

    // Factory info
    uint8_t  preBoothPassed;
    uint8_t  finalBoothPassed;
    uint8_t  functionalPassed;
    uint8_t  luapTest1;
    uint8_t  luapTest2;
    uint8_t  luapTest3;

    // Postponement info
    uint16_t rfidChecksum;

    // Runtime info
    uint8_t  bootState;
    uint8_t  isTapLoggerEnabled;

    // Do not put anything above this point without
    // updating the bootloader.

    // Product specific params
    uint8_t productStructVersion;
    ProductParams_t product;

    // Padding - adjust this when modify this struct
    uint8_t padding[62 - sizeof(ProductParams_t)];

    uint16_t mfgCtrlWord;
} NV_ManufacturingParams_t;
#pragma pack()

#endif
