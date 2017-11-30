//
// WirelessAudioUtilities.c
//

#include <stdlib.h>

#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioVariantUtilities.h"
#include "WirelessAudioISR.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioNV.h"
#include "WirelessAudioAPI.h"
#include "WirelessAudioUpdate.h"
#include "nv_mfg.h"
#include "nv_mfg_struct.h"
#include "versionlib.h"
#include "BCRC.h"

SCRIBE_DECL(wa_task);

#define DARR_HOLD_RESET_TIME TIMER_MSEC_TO_TICKS(50)
#define DARR_WAIT_AFTER_RESET_TIME TIMER_MSEC_TO_TICKS(2500)
#define DARR_WAIT_INIT_BIT_DELAY TIMER_MSEC_TO_TICKS(100)
#define DARR_INIT_BIT_RETRIES 100
#define DARR_WAIT_AFTER_ERASE_TIME TIMER_MSEC_TO_TICKS(6500)
#define DARR_WAIT_AFTER_PROGRAM_COCO_TIME TIMER_SEC_TO_TICKS(2)
#define DARR_MAX_BAD_MESSAGES 10
#define SERIAL_NO_WORD_LENGTH (SYSTEM_SERIAL_NO_LEN >> 2)

static BOOL WirelessAudioUtilities_IsMessageValid(WA_DataMessage_t* message);
static void WirelessAudioUtilities_LogDataPacket(WA_DataMessage_t* p);

static uint8_t waCurrentCoco[WA_COCO_LEN] = {0};
static uint8_t darrVersion = WA_INVALID_VERSION;
static uint16_t waCocoSum = 0;
static uint32_t waSerialNumberCRC = 0;
static BOOL dumpDataMessages = FALSE;

static const char * const mfgCocoName = "wac";

void WirelessAudioUtilities_Init(void)
{
    uint8_t size = 0; // not used, just needed to make the function call
    if(!NV_GetMfgField(mfgCocoName, waCurrentCoco, &size))
    {
        WirelessAudioVariantUtils_GetDefaultCoco(waCurrentCoco);
    }

    uint8_t sn[SYSTEM_SERIAL_NO_LEN] = {0};
    NV_GetSystemSerialNo(sn, SYSTEM_SERIAL_NO_LEN, 0);
    waSerialNumberCRC = WirelessAudioUtilities_CalculateSerialCRC((char*)sn);
    WirelessAudioVariantUtils_Init();
}

void WirelessAudioUtilities_ConfigureIO(void)
{
    WirelessAudioUtilities_AssertReset(TRUE);
    ConfigureGpioPin(DWAM_RESET_BANK,
                     DWAM_RESET_PIN,
                     GPIO_MODE_OUT_PP,
                     0);
    
    WirelessAudio_ConfigureEXTI();
}

void WirelessAudioUtilities_ResetDarr(void)
{
    WirelessAudioUtilities_TurnOff();
    WirelessAudioUtilities_TurnOn();
}

void WirelessAudioUtilities_AssertReset(BOOL assert)
{
    // The reset line is active low
    if(assert)
    {
        GPIO_WriteBit(DWAM_RESET_BANK, 1 << DWAM_RESET_PIN, Bit_RESET);
    }
    else
    {
        GPIO_WriteBit(DWAM_RESET_BANK, 1 << DWAM_RESET_PIN, Bit_SET);
    }
}

BOOL WirelessAudioUtilities_WaitForInitBit(void)
{
    uint16_t globalFlags = 0;

    for(uint8_t i = 0; i < DARR_INIT_BIT_RETRIES; i++)
    {
        if(WirelessAudio_I2C_Read(WA_CMD_READ_CUS_GLOBAL_FLAG, &globalFlags, sizeof(globalFlags)) &&
          ((globalFlags & WA_CUSTOM_G_FLAG_INIT) == 0))
        {
            return TRUE;
        }
        vTaskDelay(DARR_WAIT_INIT_BIT_DELAY);
    }
    return FALSE;
}

void WirelessAudioUtilities_TurnOff(void)
{
    WirelessAudio_EnableEXTI(FALSE);
    WirelessAudio_I2C_Enable(FALSE);
    WirelessAudioUtilities_AssertReset(TRUE);
    vTaskDelay(DARR_HOLD_RESET_TIME);
}

void WirelessAudioUtilities_TurnOn(void)
{
    WirelessAudioUtilities_AssertReset(FALSE);
    vTaskDelay(DARR_WAIT_AFTER_RESET_TIME);
    WirelessAudio_I2C_Enable(TRUE);
    WirelessAudio_EnableEXTI(TRUE);
}

void WirelessAudioUtilities_ReadAllDataMessages(void (*processMessageCallback)(WA_DataMessage_t*))
{
    unsigned int errorCount = 0;
    while(!GPIO_ReadInputDataBit(DWAM_INTERRUPT_BANK, DWAM_INTERRUPT_PIN_MASK))
    {
        WA_DataMessage_t message = {0};
        if(WirelessAudio_I2C_ReadDataMessage(&message) &&
           WirelessAudioUtilities_IsMessageValid(&message))
        {
            errorCount = 0;
            processMessageCallback(&message);
        }
        else if(++errorCount >= DARR_MAX_BAD_MESSAGES)
        {
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Bad messages, reset DARR");
            WirelessAudioUtilities_InitializeDarr(FALSE);
        }

        if(dumpDataMessages)
        {
            WirelessAudioUtilities_LogDataPacket(&message);
        }
    }
}

static BOOL WirelessAudioUtilities_IsMessageValid(WA_DataMessage_t* message)
{
    BOOL valid = TRUE;

    if(message->boseID != WA_BOSE_COMMAND_ID)
    {
        valid = FALSE;
    }
    else if(WirelessAudioVariantUtils_IsDefaultCoco(waCurrentCoco) &&
           (WirelessAudioVariant_GetState() != WA_STATE_PAIRING))
    {
        valid = FALSE;
    }
    else if(message->opcode == WA_BCMD_TEST)
    {
        message->data[WA_DATA_PKT_PAYLOAD_SIZE - 1] = '\0';
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "%s", message->data);
        valid = FALSE;
    }

    return valid;
}

BOOL WirelessAudioUtilities_InitializeDarr(BOOL updateCoco)
{
    BOOL success = FALSE;
    unsigned int i = 0;
    do
    {
        success = WirelessAudioVariantUtils_InitializeDarr();
    } while(!success && (i < WA_DARR_INIT_RETRIES));

    if(!WirelessAudioUtilities_WasDarrQuickBooted())
    {
        if(success && updateCoco) WirelessAudioUtilities_UpdateCoco();
        WirelessAudio_I2C_Read(WA_CMD_READ_VERSION, &darrVersion, sizeof(darrVersion));
    }
    return success;
}

uint32_t WirelessAudioUtilities_GetBroadcastAddress(void)
{
    return WirelessAudioVariantUtils_GetBroadcastAddress();
}

void WirelessAudioUtilities_GetDefaultCoco(uint8_t* coco)
{
    WirelessAudioVariantUtils_GetDefaultCoco(coco);
}

BOOL WirelessAudioUtilities_IsDefaultCoco(uint8_t* coco)
{
    return WirelessAudioVariantUtils_IsDefaultCoco(coco);
}

BOOL WirelessAudioUtilities_ReadCoco(uint8_t* coco)
{
    static const WA_Reg_t cocoRegisters[WA_COCO_LEN] = {WA_REG_COCO0, WA_REG_COCO1, WA_REG_COCO2};

    for(uint16_t i = 0; i < WA_COCO_LEN; i++)
    {
        if(!WirelessAudio_I2C_ReadReg(cocoRegisters[i], &coco[i], sizeof(uint8_t)))
        {
            return FALSE;
        }
    }
    return TRUE;
}

void WirelessAudioUtilities_UpdateCoco(void)
{
    uint8_t currentCoco[WA_COCO_LEN];
    if(!WirelessAudioUtilities_ReadCoco(currentCoco)) return;

    if(memcmp(waCurrentCoco, currentCoco, WA_COCO_LEN))
    {
        memcpy(waCurrentCoco, currentCoco, WA_COCO_LEN);
        NV_SetMfgField(mfgCocoName, waCurrentCoco);
    }
    waCocoSum = waCurrentCoco[0] + waCurrentCoco[1] + waCurrentCoco[2];
}

void WirelessAudioUtilities_ProgramCoco(uint8_t* coco)
{
    if(!coco) return;
    uint8_t cocoButter[WA_COCO_LEN + 1] = {WA_CMD_EXT_SET_COCO};
    memcpy(&cocoButter[1], coco, WA_COCO_LEN);
    WirelessAudio_I2C_WriteCommand(cocoButter, WA_CMD_EXTENSION, WA_COCO_LEN + 1);
    vTaskDelay(DARR_WAIT_AFTER_PROGRAM_COCO_TIME);
    WirelessAudioUtilities_InitializeDarr(TRUE);
}

void WirelessAudioUtilities_ProgramCocoFromMfg(void)
{
    uint8_t cocoButter[WA_COCO_LEN] = WA_DEFAULT_COCO;
    uint8_t size;
    if(NV_GetMfgField(mfgCocoName, &cocoButter[0], &size))
    {
        WirelessAudioUtilities_ProgramCoco(cocoButter);
        vTaskDelay(DARR_WAIT_AFTER_PROGRAM_COCO_TIME);
    }
}

void WirelessAudioUtilities_GetCurrentCoco(uint8_t* coco)
{
    if(!coco) return;
    memcpy(coco, waCurrentCoco, WA_COCO_LEN);
}

void WirelessAudioUtilities_SendIdAck(WA_ChannelID_t channel)
{
    WAPacket_IDAck_t ack = {.channel = channel};

    WirelessAudio_I2C_SendDataMessage(WA_BCMD_ID_ACK, &ack, sizeof(ack));
}

void WirelessAudioUtilities_SendSNAck(WA_ChannelID_t channel, uint8_t* sn)
{
    if(!sn) return;

    WAPacket_SerialNum_t serial = {0};
    serial.subCmd = WA_SN_ACK;
    serial.channel = channel;
    memcpy(serial.serialNumber, sn, member_size(WAPacket_SerialNum_t, serialNumber));
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_SERIAL_NO, &serial, sizeof(serial));
}

uint16_t WirelessAudioUtilities_GetChannelBitMask(WA_ChannelID_t channel)
{
    uint16_t bit = 0;

    switch(channel)
    {
        case WA_CHANNEL_BASS_SKIPPER:
        case WA_CHANNEL_BASS_PBB:    
            bit = WA_CHANNEL_BASS_BIT;
            break;
        case WA_CHANNEL_SURROUND_LEFT:
            bit = WA_CHANNEL_SURROUND_LEFT_BIT;
            break;
        case WA_CHANNEL_SURROUND_RIGHT:
            bit = WA_CHANNEL_SURROUND_RIGHT_BIT;
            break;
    }

    return bit;
}

// When opening the pairing window, the DARR configures itself
// automatically. Currently, it is unsafe to close the pairing window
// manually because the DARR will not reconfigure itself appropriately.
void WirelessAudioUtilities_OpenPairingWindow(BOOL open)
{
    debug_assert(open);
    uint8_t data = open ? WA_PAIRING_OPEN_DATA : WA_PAIRING_CLOSE_DATA;
    WirelessAudio_I2C_WriteCommand(&data, WA_CMD_PAIRING, sizeof(data));
}

BOOL WirelessAudioUtilities_IsLinked(void)
{
    uint16_t globalFlags = 0;
    if(WirelessAudio_I2C_Read(WA_CMD_READ_CUS_GLOBAL_FLAG, &globalFlags, sizeof(globalFlags)) &&
       ((globalFlags & WA_CUSTOM_G_FLAG_LINK) != 0))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void WirelessAudioUtilities_SendID(void)
{
    WAPacket_ID_t id = {0};
    id.channel = WirelessAudioUtilities_GetChannel();
    id.darr_ver = WirelessAudioUtilities_GetVersion();
    id.lpm_major = atoi(VersionGetMajor());
    id.lpm_minor = atoi(VersionGetMinor());
    id.lpm_patch = atoi(VersionGetPatch());
    id.lpm_build = atoi(VersionGetBuildNumber());
    WirelessAudioUtilities_GetCurrentCoco(id.coco);
    id.serialNumberCRC = waSerialNumberCRC;
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_ID, &id, sizeof(id));
}

uint8_t WirelessAudioUtilities_GetVersion(void)
{
    return darrVersion;
}

void WirelessAudioUtilities_SendSN(void)
{
    WAPacket_SerialNum_t sn = {0};
    sn.subCmd = WA_SN_SEND;
    sn.channel = WirelessAudioUtilities_GetChannel();
    NV_GetSystemSerialNo(sn.serialNumber, SYSTEM_SERIAL_NO_LEN, 0);
    sn.cocoSum = waCocoSum;
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_SERIAL_NO, &sn, sizeof(sn));
}

void WirelessAudioUtilities_SendTelemetry(void)
{
    WirelessAudioVariantUtils_SendTelemetry(waSerialNumberCRC);
}

void WirelessAudioUtilities_PeriodicService(void)
{
    WirelessAudioVariantUtils_PeriodicService();
}

uint8_t WirelessAudioUtilities_GetChannel(void)
{
    return WirelessAudioVariantUtils_GetChannel();
}

void WirelessAudioUtilities_SendTestMessage(void)
{
    char message[] = "This is a test message";
    WirelessAudio_I2C_SendDataMessage(WA_BCMD_TEST, message, sizeof(message));
}

uint32_t WirelessAudioUtilities_CalculateSerialCRC(char* serialNumber)
{
    if(!serialNumber) return 0;
    return BCRC_CalculateCRC(serialNumber, SERIAL_NO_WORD_LENGTH);
}

uint16_t WirelessAudioUtilities_GetCocoSum(void)
{
    return waCocoSum;
}

void WirelessAudioUtilities_FactoryDefault(void)
{
    uint8_t extCmd = WA_CMD_EXT_FD;
    WirelessAudio_I2C_WriteCommand(&extCmd, WA_CMD_EXTENSION, sizeof(extCmd));
    vTaskDelay(DARR_WAIT_AFTER_ERASE_TIME);
    WirelessAudioVariantUtils_EnableFullInitSequence();
    WirelessAudioUtilities_InitializeDarr(TRUE);
}

void WirelessAudioUtilities_ClearCoco(void)
{
    uint8_t extCmd = WA_CMD_EXT_CLEAR_COCO;
    WirelessAudio_I2C_WriteCommand(&extCmd, WA_CMD_EXTENSION, sizeof(extCmd));
    vTaskDelay(DARR_WAIT_AFTER_ERASE_TIME);
    WirelessAudioVariantUtils_EnableFullInitSequence();
    WirelessAudioUtilities_InitializeDarr(TRUE);
}

BOOL WirelessAudioUtilities_IsPreviouslyPaired(void)
{
    return !WirelessAudioVariantUtils_IsDefaultCoco(waCurrentCoco);
}

uint8_t WirelessAudioUtilities_ConvertBandToMask(WA_Band_t band)
{
    uint8_t mask = 0;
    switch(band)
    {
        case WA_BAND_AUTO_SELECT:
            mask = WA_BAND_TRIBAND_BITMASK;
            break;
        case WA_BAND_58:
            mask = WA_BAND_58_BIT_MASK;
            break;
        case WA_BAND_52:
            mask = WA_BAND_52_BIT_MASK;
            break;
        case WA_BAND_24:
            mask = WA_BAND_24_BIT_MASK;
            break;
        case WA_BAND_52_58:
            mask = WA_BAND_52_58_BITMASK;
            break;
        case WA_BAND_24_58:
            mask = WA_BAND_24_58_BITMASK;
            break;
        case WA_BAND_24_52:
            mask = WA_BAND_24_52_BITMASK;
            break;
    }
    return mask;
}

WA_Band_t WirelessAudioUtilities_ConvertMaskToBand(uint8_t bandMask)
{
    WA_Band_t band = WA_BAND_AUTO_SELECT;
    switch(bandMask)
    {
        case WA_BAND_24_BIT_MASK:
            band = WA_BAND_24;
            break;
        case WA_BAND_52_BIT_MASK:
            band = WA_BAND_52;
            break;
        case WA_BAND_58_BIT_MASK:
            band = WA_BAND_58;
            break;
        case WA_BAND_24_52_BITMASK:
            band = WA_BAND_24_52;
            break;
        case WA_BAND_24_58_BITMASK:
            band = WA_BAND_24_58;
            break;
        case WA_BAND_52_58_BITMASK:
            band = WA_BAND_52_58;
            break;
    }
    return band;
}

uint8_t WirelessAudioUtilities_GetActiveBandsMask(void)
{
    return WirelessAudioVariantUtils_GetActiveBandsMask();
}

void WirelessAudioUtilities_SetActiveBandsMask(uint8_t bandMask)
{
    WirelessAudioVariantUtils_SetActiveBandsMask(bandMask);
}

WA_Band_t WirelessAudioUtilities_GetActiveBands(void)
{
    uint8_t bandMask = WirelessAudioUtilities_GetActiveBandsMask();
    return WirelessAudioUtilities_ConvertMaskToBand(bandMask);
}

BOOL WirelessAudioUtilities_IsPairingNeeded(void)
{
    return WirelessAudioVariantUtils_IsPairingNeeded();
}

BOOL WirelessAudioUtilities_IsPairingActive(void)
{
    uint16_t globalFlags = 0;
    if(WirelessAudio_I2C_Read(WA_CMD_READ_CUS_GLOBAL_FLAG, &globalFlags, sizeof(globalFlags)) &&
       ((globalFlags & WA_CUSTOM_G_FLAG_PAIRING) != 0))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void WirelessAudioUtilities_UpdateAndReprogram(void)
{
    WirelessAudioUpdate_UpgradeImage();
    WirelessAudioVariantUtils_EnableFullInitSequence();
    WirelessAudioUtilities_InitializeDarr(FALSE);
    WirelessAudioUtilities_ProgramCocoFromMfg();
}

BOOL WirelessAudioUtilities_DoesRegionAllowPair(void)
{
    return WirelessAudioVariantUtils_DoesRegionAllowPair();
}

void WirelessAudioUtilities_ToggleDataMessageDump(void)
{
    dumpDataMessages = !dumpDataMessages;
}

BOOL WirelessAudioUtilities_GetDataMessageDumpEnabled(void)
{
    return dumpDataMessages;
}

static void WirelessAudioUtilities_LogDataPacket(WA_DataMessage_t* p)
{
    uint32_t* data = (uint32_t*) p->data;
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "%02X %02x %08X %08X %08X %08X %08X %08X %08X",
                                         p->boseID,
                                         p->opcode,
                                         __REV(data[0]), __REV(data[1]),
                                         __REV(data[2]), __REV(data[3]),
                                         __REV(data[4]), __REV(data[5]),
                                         __REV(data[6]));
}

BOOL WirelessAudioUtilities_WasDarrQuickBooted(void)
{
    return WirelessAudioVariantUtils_WasDarrQuickBooted();
}
