//
// etapWirelessAudio.c
//

#include "etap.h"
#include "etapWirelessAudio.h"
#include "WirelessAudioAPI.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioNV.h"
#include "AccessoryManager.h"

#define UNUSED_STRING "Unused bit\n"
#define A_BLOCK_OF_SPACES "           "

static void WaTap_SetBand(CommandLine_t* cl);
static void WaTap_ProgramCoco(CommandLine_t* cl);
static void WaTap_PrintStatus(void);
static void WaTap_Erase(CommandLine_t* cl);
static void WaTap_PrintBands(void);
static void WaTap_XTREME(CommandLine_t* cl);
static const char* WaTap_GetStateString(void);
static void WaTap_PrintXTREMEStatus(void);
static const char * WaTap_GetLocaleString(void);

static BOOL WaTap_Read(CommandLine_t* cl);
static BOOL WaTap_Write(CommandLine_t* cl);
void MakePayloadBuff(CommandLine_t* cl, uint32_t len, BOOL* IsValidNumber, uint8_t* buffer);

static const char * const bandName24 = "2.4";
static const char * const bandName52 = "5.2";
static const char * const bandName58 = "5.8";

TAPCommand(TAP_WACommand)
{
    if(CommandLine->numArgs < 1)
    {
        TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
        TAP_PrintString("Incorrect arg.");
        return;
    }
    else if(WirelessAudioTask_GetState() == WA_STATE_OFF)
    {
        TAP_PrintString("\nDARR is sleeping - try again later...\n");
        return;
    }

    switch(CommandLine->args[0][0])
    {
        case 'b':
            WaTap_SetBand(CommandLine);
            break;
        case 'c':
            WaTap_ProgramCoco(CommandLine);
            break;
        case 'e':
            WaTap_Erase(CommandLine);
            break;
        case 'j':
            WirelessAudioPostMsg(WA_MSG_ID_OpenPairingWindow, NULL, TRUE);
            break;
        case 'p':
            WirelessAudioPostMsg(WA_MSG_ID_StartPair, NULL, 0);
            break;
        case 'q':
            WirelessAudioPostMsg(WA_MSG_ID_ToggleDataMessageDump, NULL, 0);
            break;
        case 's':
            WaTap_PrintStatus();
            break;
        case 't':
            WirelessAudioPostMsg(WA_MSG_ID_SendTestMessage, NULL, 0);
            break;
        case 'u':
            WirelessAudio_PostAndWaitFor(WA_MSG_ID_Update_Darr);
            break;
        case 'x':
            WaTap_XTREME(CommandLine);
            break;
        case 'r':
            WaTap_Read(CommandLine);
            break;
        case 'w':
            WaTap_Write(CommandLine);
            break;
        default:
            TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
            TAP_PrintString("Cmd not supported.");
            break;
    }
}

static void WaTap_SetBand(CommandLine_t* cl)
{
    if(cl->numArgs == 1)
    {
        WaTap_PrintBands();
    }
    else if(cl->numArgs == 2)
    {
        BOOL valid = FALSE;
        uint8_t band = TAP_HexArgToInt(cl, 1, &valid);
        if(!valid || (band > WA_BAND_TRIBAND_BITMASK))
        {
            TAP_PrintString("Invalid band.");
        }
        else
        {
            WirelessAudioPostMsg(WA_MSG_ID_SetRFBand, NULL, band);
        }
    }
    else
    {
        TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
        TAP_PrintString("Incorrect args.");
    }
}

static void WaTap_ProgramCoco(CommandLine_t* cl)
{
    if(cl->numArgs < 4)
    {
        TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
        TAP_PrintString("Incorrect arg.");
        return;
    }

    BOOL valid[3];
    uint8_t coco[3];
    coco[0] = TAP_HexArgToInt(cl, 1, &valid[0]);
    coco[1] = TAP_HexArgToInt(cl, 2, &valid[1]);
    coco[2] = TAP_HexArgToInt(cl, 3, &valid[2]);
    if(valid[0] && valid[1] && valid[2])
    {
        TAP_PrintString("Programming coco...");
        WirelessAudioUtilities_ProgramCoco(coco);
    }
    else
    {
        TAP_PrintString("Bad coco");
    }
}

static void WaTap_PrintStatus(void)
{
    uint8_t coco[3] = {0};
    WirelessAudioUtilities_GetCurrentCoco(coco);
    TAP_Printf("State: %s\n", WaTap_GetStateString());
    TAP_Printf("Coco: %02X%02X%02X\n", coco[0], coco[1], coco[2]);
    TAP_PrintString("Bands: ");
    WaTap_PrintBands();
}

static void WaTap_Erase(CommandLine_t* cl)
{
    if(cl->numArgs != 2)
    {
        TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
        TAP_PrintString("Incorrect arg.");
        return;
    }

    switch(cl->args[1][0])
    {
        case '0':
            TAP_PrintString("FD...");
            WirelessAudio_PostAndWaitFor(WA_MSG_ID_FactoryDefault);
            break;
        case '1':
            TAP_PrintString("Erasing coco...");
            WirelessAudio_PostAndWaitFor(WA_MSG_ID_EraseCoco);
            break;
        default:
            TAP_PrintString("Option not supported.");
            break;
    }
}

static void WaTap_PrintBands(void)
{
    uint8_t mask = WirelessAudioUtilities_GetActiveBandsMask();
    switch(mask)
    {
        case WA_BAND_24_BIT_MASK:
            TAP_Printf("%s", bandName24);
            break;
        case WA_BAND_52_BIT_MASK:
            TAP_Printf("%s", bandName52);
            break;
        case WA_BAND_58_BIT_MASK:
            TAP_Printf("%s", bandName58);
            break;
        case WA_BAND_24_52_BITMASK:
            TAP_Printf("%s + %s", bandName24, bandName52);
            break;
        case WA_BAND_24_58_BITMASK:
            TAP_Printf("%s + %s", bandName24, bandName58);
            break;
        case WA_BAND_52_58_BITMASK:
            TAP_Printf("%s + %s", bandName52, bandName58);
            break;
        case WA_BAND_TRIBAND_BITMASK:
            TAP_PrintString("Auto (tri-band)");
            break;
        default:
            TAP_PrintString("none");
            break;
    }
    TAP_PrintString("\n");
}

static void WaTap_XTREME(CommandLine_t* cl)
{
    if(cl->numArgs < 2)
    {
        TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
        TAP_PrintString("Incorrect arg.");
        return;
    }

    switch(cl->args[1][0])
    {
        case 's':
            WaTap_PrintXTREMEStatus();
            break;
        default:
            TAP_PrintString("Option not supported.");
            break;
    }
}

static const char* WaTap_GetStateString(void)
{
    char* state;
    switch(WirelessAudio_GetState())
    {
        case WA_STATE_OFF:
            state = "Off";
            break;
        case WA_STATE_ON:
            state = "On";
            break;
        case WA_STATE_PAIRING:
            state = "Pairing";
            break;
        case WA_STATE_ERROR:
            state = "Error";
            break;
        case WA_STATE_UPDATE_SPEAKERS:
            state = "Update";
            break;
        default:
            state = "Unknown";
            break;
    }
    return state;
}

static void WaTap_PrintXTREMEStatus(void)
{
    TAP_PrintString("Status 0xb05e\n");
    TAP_PrintString("\nBit:               Meaning:         Value:\n");
    WAState_t state = WirelessAudio_GetState();
    TAP_Printf(" Initialized       (0=yes)          %d\n", (state == WA_STATE_OFF));
    TAP_Printf(" Operating         (0=no)%s%d\n", A_BLOCK_OF_SPACES, (state != WA_STATE_OFF));
    TAP_Printf(" Standby           (0=no)%s%d\n", A_BLOCK_OF_SPACES, (state == WA_STATE_OFF));
    TAP_Printf(" Pairing           (0=no)%s%d\n", A_BLOCK_OF_SPACES, (state == WA_STATE_PAIRING));
    for(unsigned int i = 0; i < 4; i++, TAP_PrintString(UNUSED_STRING));
    TAP_Printf(" Linked MU only    (0=no)%s%d\n", A_BLOCK_OF_SPACES, WirelessAudioUtilities_IsLinked());
    TAP_PrintString(UNUSED_STRING);
    TAP_Printf(" MU Change CU      (0=no)%s0\n", A_BLOCK_OF_SPACES);
    TAP_PrintString(UNUSED_STRING);
    TAP_Printf(" MU Lost Conn.     (0=no)%s%d\n", A_BLOCK_OF_SPACES,
#ifdef IS_CONSOLE
               (AccessoryManager_GetUI_State() == ACCESSORY_STATE_DISCONNECTED));
#else
               0);
#endif
    TAP_Printf(" Paired            (0=no)%s%d\n", A_BLOCK_OF_SPACES, WirelessAudioUtilities_IsPreviouslyPaired());
    TAP_Printf(" POWER_OFF         (0=no)%s%d\n", A_BLOCK_OF_SPACES, (state == WA_STATE_OFF));
    TAP_Printf(" Custom Regulation (0=no)%s0\n\n", A_BLOCK_OF_SPACES);
    TAP_Printf(" Locale: %s, QT = %s, Bands = ", WaTap_GetLocaleString(),
                 WirelessAudioUtilities_GetDataMessageDumpEnabled() ? "ON" : "OFF");
    WaTap_PrintBands();
    uint8_t coco[3] = {0};
    WirelessAudioUtilities_GetCurrentCoco(coco);
    TAP_Printf(" Connect ID: = %02x%02x%02x\n\n", coco[0], coco[1], coco[2]);
    uint8_t NVParam = 0, size = 0;
    NV_GetMfgField("wap", &NVParam, &size);
    TAP_Printf(" PDDC=%02x ", NVParam);
    NV_GetMfgField("wat", &NVParam, &size);
    TAP_Printf("T500=%02x  ", NVParam);
    TAP_Printf("Mask= %2x\n", WirelessAudioUtilities_GetActiveBandsMask());
    TAP_PrintString("OK.\n\n");
}

static const char * WaTap_GetLocaleString(void)
{
    char* locale;
    switch(NV_GetRegionVariant())
    {
        case REGION_US:
            locale = "WW";
            break;
        case REGION_GB:
            locale = "EU";
            break;
        case REGION_JP:
            locale = "JPN";
            break;
        default:
            locale = "Hyrule";
            break;
    }
    return locale;
}

static BOOL WaTap_Read(CommandLine_t* cl)
{
  // if there are not enough args, let the user know
  // assuming 4 since we dont need a payload
  if(cl->numArgs < 4)
  {
    TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
    TAP_PrintString("Not enough args to read.");
    return FALSE;
  }

  if( cl->args[0][0] != 'r') return FALSE;

  BOOL IsValidNumber = FALSE;
  uint8_t buffer[100] = {0};
  uint16_t RegOffsetVal;

  uint32_t length = TAP_HexArgToInt(cl, 2, &IsValidNumber);
  if( !IsValidNumber ) return FALSE;
  RegOffsetVal = (uint16_t)TAP_HexArgToInt(cl, 3, &IsValidNumber);
  if( !IsValidNumber ) return FALSE;

  WA_Reg_t finalOffset;

  for(uint16_t i = 0; i < length; i++)
  {
    finalOffset = (WA_Reg_t)(RegOffsetVal + i);
    WirelessAudio_I2C_ReadReg(finalOffset, &buffer[i], 1);
  }

  TAP_Printf("\n");
  for(int i = 0; i < length; i++)
  {
    TAP_Printf("Byte[%u]= %u\n", i, buffer[i]);
  }
  return TRUE;
}


// command to write to the Darr registers
static BOOL WaTap_Write(CommandLine_t* cl)
{
  // if there are not enough args, let the user know
  // assuming 4 since we need r/w, len, reg, and payload ([0], [2], [3], [4-len])
  if(cl->numArgs < 5)
  {
    TAP_PrintString(WIRELESS_AUDIO_HELP_TEXT);
    TAP_PrintString("Not enough args to write.");
    return FALSE;
  }

  if( cl->args[0][0] != 'w') return FALSE;

  BOOL IsValidNumber = FALSE;
  uint8_t buffer[100] = {0};
  uint16_t RegOffsetVal;

  RegOffsetVal = (uint16_t)TAP_HexArgToInt(cl, 3, &IsValidNumber);
  if( !IsValidNumber  ) return FALSE;
  uint32_t length = (uint32_t)TAP_HexArgToInt(cl, 2, &IsValidNumber);
  if( !IsValidNumber ) return FALSE;

  MakePayloadBuff(cl, length, &IsValidNumber, &buffer[0]);
  if( !IsValidNumber ) return FALSE;

  WA_Reg_t finalOffset;

  for(uint16_t i = 0; i < length; i++)
  {
    finalOffset = (WA_Reg_t)(RegOffsetVal + i);
    WirelessAudio_I2C_Write8bReg(&buffer[i], finalOffset, 1);
  }
  return TRUE;
}

// used to make a payload buffer of 8 bit or 16 bit numbers
void MakePayloadBuff(CommandLine_t* cl, uint32_t len, BOOL* IsValidNumber, uint8_t* buffer)
{
  for(int j = 0; j < len; j++ )
  {
    buffer[j] = (uint8_t)TAP_HexArgToInt(cl, 4 + j, IsValidNumber);
    if( !(*IsValidNumber)) break;
  }
}
