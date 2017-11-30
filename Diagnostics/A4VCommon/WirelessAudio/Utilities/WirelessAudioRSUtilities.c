//
// WirelessAudioRSUtilities.c - rear surround utilities
//

#include "WirelessAudioUtilities.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioISR.h"
#include "WirelessAudioNV.h"
#include "LeftRightDetect.h"
#include "AmpTask.h"
#include "PowerTask.h"
#include "AmpMute.h"

SCRIBE_DECL(wa_task);

static BOOL WirelessAudioVariantUtils_DoColdbootInit(void);
static BOOL WirelessAudioVariantUtils_SetJapanM2S(void);
static BOOL WirelessAudioVariantUtils_SetSurroundChannel(void);
static void WirelessAudioVariantUtils_InitializeNV(void);

static const char * const mfgFlagName = "waf";
static const char * const mfgPDDCName = "wap";
static const char * const mfgT500Name = "wat";

static SurroundChannel_t activeChannel = SURROUND_NO_CHANNEL;
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

    AmpMute_SetMute(AMP_MUTE_DARR, TRUE);
    activeChannel = LeftRightDetect_GetChannel();
    vTaskDelay(TIMER_MSEC_TO_TICKS(50));

    success &= WirelessAudioVariantUtils_DoColdbootInit();

    WirelessAudioUtilities_ResetDarr();
    if(!WirelessAudioUtilities_WaitForInitBit()) return FALSE;

    uint8_t data = 0xF6;
    if(region == REGION_JP)
    {
        success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_AGCM, sizeof(data));
    }
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    success &= WirelessAudioVariantUtils_SetJapanM2S();
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    if(region == REGION_JP)
    {
        data = (activeChannel == SURROUND_RIGHT_CHANNEL) ? 0x20 : 0x10;
    }
    else
    {
        data = 0x20;
    }
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_STAC, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = 0x02;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_PIAC, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    if(region == REGION_JP)
    {
        data = (activeChannel == SURROUND_RIGHT_CHANNEL) ? 0xE4 : 0xE1;
    }
    else
    {
        data = 0xE4;
    }
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_RS2P, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = (region == REGION_JP) ? 0xE4 : 0xE0;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_TP2S, sizeof(data));
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    success &= WirelessAudioVariantUtils_SetSurroundChannel();
    vTaskDelay(TIMER_MSEC_TO_TICKS(20));

    data = (nvParams.flags & WANV_FLAGS_PDDC) ? nvParams.PDDCSetting : 0x12;
    success &= WirelessAudio_I2C_Write8bReg(&data, WA_REG_PDDC, sizeof(data));
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

    AmpMute_SetMute(AMP_MUTE_DARR, FALSE);

    return success;
}

static BOOL WirelessAudioVariantUtils_DoColdbootInit(void)
{
    if(ColdbootInitCompleted) return TRUE;
    
    BOOL success = TRUE;

    WirelessAudioUtilities_ResetDarr();
    if(!WirelessAudioUtilities_WaitForInitBit()) return FALSE;

    uint8_t data = 0;
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

uint8_t WirelessAudioVariantUtils_GetChannel(void)
{
    return (LeftRightDetect_GetChannel() == SURROUND_LEFT_CHANNEL) ?
           WA_CHANNEL_SURROUND_LEFT :
           WA_CHANNEL_SURROUND_RIGHT;
}

void WirelessAudioVariantUtils_PeriodicService(void)
{
    if(activeChannel != LeftRightDetect_GetChannel())
    {
        activeChannel = LeftRightDetect_GetChannel();
        WirelessAudioVariantUtils_SetJapanM2S();
        WirelessAudioVariantUtils_SetSurroundChannel();
    }
}

void WirelessAudioVariantUtils_SendTelemetry(uint32_t serialNumberCRC)
{
    WAPacket_Telemetry_t t = {0};
    t.ampFault = Amp_IsFault();
    t.dcOffset = Amp_IsDcOffset();
    t.channel = WirelessAudioVariantUtils_GetChannel();
    t.ampTemp = Pwr_GetTemperature();
    t.serialNumberCRC = serialNumberCRC;
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_TELEMETRY, &t, sizeof(t));
}

static BOOL WirelessAudioVariantUtils_SetJapanM2S(void)
{
    uint8_t data = (activeChannel == SURROUND_RIGHT_CHANNEL) ? 0x02 : 0x04;
    return WirelessAudio_I2C_WriteCommand(&data, WA_CMD_SET_JAPAN_STREAM, sizeof(data));
}

static BOOL WirelessAudioVariantUtils_SetSurroundChannel(void)
{
    uint8_t data = (activeChannel == SURROUND_RIGHT_CHANNEL) ? 0x28 : 0x20;
    return WirelessAudio_I2C_Write8bReg(&data, WA_REG_AIC1, sizeof(data));
}

BOOL WirelessAudioVariantUtils_DoesRegionAllowPair(void)
{
    return TRUE;
}

static void WirelessAudioVariantUtils_InitializeNV(void)
{
    uint8_t size = 0;

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
}

BOOL WirelessAudioVariantUtils_WasDarrQuickBooted(void)
{
    return FALSE;
}

void WirelessAudioVariantUtils_EnableFullInitSequence(void)
{
    ColdbootInitCompleted = FALSE;
}
