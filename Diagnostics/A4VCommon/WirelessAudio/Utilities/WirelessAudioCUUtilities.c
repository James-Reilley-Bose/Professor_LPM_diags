//
// WirelessAudioCUUtilities.c
//

#include "WirelessAudioUtilities.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioISR.h"
#include "WirelessAudioNV.h"
#include "nv_mfg.h"
#include "AccessoryManager.h"

SCRIBE_DECL(wa_task);

static BOOL WirelessAudioVariantUtils_DoColdbootInit(void);
static void WirelessAudioVariantUtils_InitializeNV(void);

static const char * const mfg24Name = "24";
static const char * const mfg52Name = "52";
static const char * const mfg58Name = "58";
static const char * const mfgFlagName = "waf";
static const char * const mfgPDDCName = "wap";
static const char * const mfgT500Name = "wat";

static const uint8_t defaultCUCoco[WA_COCO_LEN] = {0xFF, 0xFF, 0xFF};
static uint8_t activeBandMask = WA_BAND_NONE_BIT_MASK;
static WA_NVParams_t nvParams = {0};
static REGION_VARIANT region = REGION_UNDEFINED;
static BOOL ColdbootInitCompleted = FALSE;

void WirelessAudioVariantUtils_Init(void)
{
    WirelessAudioVariantUtils_InitializeNV();
}

BOOL WirelessAudioVariantUtils_InitializeDarr(void)
{
    BOOL success = TRUE;

    success &= WirelessAudioVariantUtils_DoColdbootInit();

    WirelessAudioUtilities_ResetDarr();
    if(!WirelessAudioUtilities_WaitForInitBit()) return FALSE;

    uint8_t data = 0xF6;
    if(region == REGION_JP)
    {
        success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_AGCM, sizeof(data));
    }
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = 0xFF;
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_SET_JAPAN_STREAM, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = 3;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_STAC, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = 0x60;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_PIAC, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = (region == REGION_JP) ? 0xE4 : 0xE0;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_RS2P, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = (region == REGION_JP) ? 0xCA : 0xC9;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_TP2S, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = 0x20;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_AIC1, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = (nvParams.flags & WANV_FLAGS_PDDC) ? nvParams.PDDCSetting : 0;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_PDDC, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = WirelessAudioUtilities_GetActiveBands();
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_FIX_RF_BAND, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    if(nvParams.flags & WANV_FLAGS_T500)
    {
        data = nvParams.T500Setting;
        success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_T500, sizeof(data));
        vTaskDelay(TIMER_MSEC_TO_TICKS(20));
    }

    WirelessAudio_EnableEXTI(TRUE);

    data = 1;
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_ENABLE, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));

    if(region != REGION_JP)
    {
        data = 0;
        success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_DISABLE_SNOOZE, sizeof(data));

        data = 0x53; // 0x52 will disable (... whatever this is)
        success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_SSC1, sizeof(data));
        vTaskDelay(TIMER_MSEC_TO_TICKS(20));

        data = 0x20;
        success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_ABTL, sizeof(data));
        vTaskDelay(TIMER_MSEC_TO_TICKS(20));
    }

    return success;
}

static BOOL WirelessAudioVariantUtils_DoColdbootInit(void)
{
    if(ColdbootInitCompleted) return TRUE;

    BOOL success = TRUE;

    WirelessAudioUtilities_ResetDarr();
    if(!WirelessAudioUtilities_WaitForInitBit()) return FALSE;

    uint8_t data = 1;
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_SET_CU_MU, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = 0;
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_ENABLE, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));

    switch(region)
    {
        case REGION_GB:
            data = 0;
            break;
        case REGION_JP:
            data = 1;
            break;
        default:
            data = 2;
            break;
    }
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_APP_SELECT, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = (region == REGION_JP) ? 2 : 0;
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_SET_ETSI, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = 1;
    success &= WirelessAudio_I2C_WriteCommand(&data, WA_CMD_PCL_ON_OFF, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    ColdbootInitCompleted = success;

    return success;
}

uint32_t WirelessAudioVariantUtils_GetBroadcastAddress(void)
{
    return WA_BROADCAST_TO_MU_ADDR;
}

void WirelessAudioVariantUtils_GetDefaultCoco(uint8_t* coco)
{
    if(coco) memcpy(coco, defaultCUCoco, sizeof(defaultCUCoco));
}

BOOL WirelessAudioVariantUtils_IsDefaultCoco(uint8_t* coco)
{
    if(coco)
    {
        return (memcmp(coco, defaultCUCoco, sizeof(defaultCUCoco)) == 0);
    }
    else
    {
        return FALSE;
    }
}

uint8_t WirelessAudioVariantUtils_GetChannel(void)
{
    return 0; // no channel for CUs
}

void WirelessAudioVariantUtils_SendTelemetry(uint32_t serialNumberCRC)
{
    // CU doesn't send telemetry
}

void WirelessAudioVariantUtils_PeriodicService(void)
{
    // nothing to do for now
}

uint8_t WirelessAudioVariantUtils_GetActiveBandsMask(void)
{
    return activeBandMask;
}

void WirelessAudioVariantUtils_SetActiveBandsMask(uint8_t bandMask)
{
    activeBandMask = bandMask;
}

static void WirelessAudioVariantUtils_InitializeNV(void)
{
    uint8_t size = 0;
    if(NV_GetMfgField(mfg24Name, (uint8_t*) &nvParams.band24Mask, &size) &&
       NV_GetMfgField(mfg52Name, (uint8_t*) &nvParams.band52Mask, &size) &&
       NV_GetMfgField(mfg58Name, (uint8_t*) &nvParams.band58Mask, &size))
    {
        activeBandMask |= nvParams.band24Mask ? WA_BAND_24_BIT_MASK : 0;
        activeBandMask |= nvParams.band52Mask ? WA_BAND_52_BIT_MASK : 0;
        activeBandMask |= nvParams.band58Mask ? WA_BAND_58_BIT_MASK : 0;
    }
    else
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Error getting masks from mfg.");
    }

    if(!NV_GetMfgField(mfgFlagName, (uint8_t*) &nvParams.flags, &size))
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Error reading flags from mfg.");
    }

    if(!NV_GetMfgField(mfgPDDCName, (uint8_t*) &nvParams.PDDCSetting, &size))
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Error reading PDDC from mfg.");
    }

    if(!NV_GetMfgField(mfgT500Name, (uint8_t*) &nvParams.T500Setting, &size))
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Error reading T500 from mfg.");
    }

    region = NV_GetRegionVariant();
    debug_assert(region != REGION_UNDEFINED);
    if(region == REGION_JP) activeBandMask = WA_BAND_24_52_BITMASK;
}

BOOL WirelessAudioVariantUtils_IsPairingNeeded(void)
{
    uint8_t numSpeakersConnected = AccessoryManager_NumberOfTypeConnected(ACCESSORY_MAXWELL);
    numSpeakersConnected += AccessoryManager_NumberOfTypeConnected(ACCESSORY_SKIPPER) \
                            + AccessoryManager_NumberOfTypeConnected(ACCESSORY_LOVEY);
    return (numSpeakersConnected < WA_NUM_CHANNELS);
}

BOOL WirelessAudioVariantUtils_DoesRegionAllowPair(void)
{
    return TRUE;
}

void WirelessAudioVariantUtils_Reboot(void)
{
    //NOP - can only send this 
}

BOOL WirelessAudioVariantUtils_WasDarrQuickBooted(void)
{
    return FALSE;
}

void WirelessAudioVariantUtils_EnableFullInitSequence(void)
{
    ColdbootInitCompleted = FALSE;
}
