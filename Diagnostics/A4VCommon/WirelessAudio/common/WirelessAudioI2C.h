//
// WirelessAudioI2C.h
//

#ifndef WIRELESS_AUDIO_I2C_H
#define WIRELESS_AUDIO_I2C_H

#include "WirelessAudioData.h"

typedef enum
{
    /* Read commands */
    WA_CMD_READ_VERSION = 0x38,
    WA_CMD_READ_MESSAGE = 0x3E,
    WA_CMD_READ_CUS_GLOBAL_FLAG = 0x3F,

    /* Write commands */
    WA_CMD_ENABLE = 0xC1,
    WA_CMD_PAIRING = 0xC2,
    WA_CMD_SET_JAPAN_STREAM = 0xC3,
    WA_CMD_APP_SELECT = 0xC5,
    // WA_CMD_WRITE_COCO = 0xC6,  --- I think this is deprecated...
    WA_CMD_FIX_RF_CHAN = 0xC7,
    WA_CMD_FIX_RF_BAND = 0xC8,
    WA_CMD_PCL_ON_OFF = 0xC9,
    WA_CMD_SET_CU_MU = 0xCA,
    WA_CMD_INTO_STANDBY = 0xCB,
    WA_CMD_ERASE_DB = 0xCC,
    WA_CMD_OUT_STANDBY = 0xCD,
    WA_CMD_EXTENSION = 0xCF,
    WA_CMD_REGULATION_MODE = 0xD2,
    WA_CMD_SET_ETSI = 0xD0,
    WA_CMD_SET_FREQ_LEVEL = 0xD1,
    WA_CMD_SET_APL = 0xD5,
    WA_CMD_DISABLE_SNOOZE = 0xDB,
} WA_Cmd_t;

typedef enum
{
    WA_CMD_EXT_CLEAR_COCO = 0x05,
    WA_CMD_EXT_FD = 0x06,
    WA_CMD_EXT_SET_COCO = 0x07,
} WA_CmdExt_t;

typedef enum
{
    WA_REG_ENCY   =    0,  /* Encryption KEY                */
    WA_REG_GEC    =   16,  /*  10h Activate Encryption, Audio Snooze, etc */
    WA_REG_VER    =   17,  /*  11h Version number (HW)           */
    WA_REG_CEID   =   25, /*  19h Upper 4bit is mask, lower 4bit is cell ID for zoning. Default cell ID is 0 */
    WA_REG_PDDC   =   27, /*  1bh PDDC  */
    WA_REG_MODE   =   28, /*  1ch Force Enc, Force Con,ACK/NAC,CU/MU, Application ID    */
    WA_REG_ABTL   =   46, /*  2eh TODO: figure out what this is */
    WA_REG_AGCM   =   58, /*  3ah Used to disable Automatic Power Control (APC) Japan mode only */
    WA_REG_AIS0   =   75, /*  4bh Audio Interface Status  0     */
    WA_REG_AIS1   =   76, /*  4ch Audio Interface Status  1     */
    WA_REG_ALOP   =   77, /*  4dh Audio Loop                    */
    WA_REG_SPC    =   78, /*  4eh S/PDIF Control                */
    WA_REG_AICS   =   79, /*  4fh Audio Interface Control Status*/
    WA_REG_AIC1   =   80, /*  50h Audio Interface Control 1     */
    WA_REG_STAC   =   82, /*  52h Stream Active                 */
    WA_REG_PIAC   =   83, /*  53h Pipe Active                   */
    WA_REG_RS2P   =   84, /*  54h RX Stream To Pipe             */
    WA_REG_TP2S   =   85, /*  55h TX Stream From Pipe           */
    WA_REG_AVOW   =   86, /*  56h Audio Volume Pipe W           */
    WA_REG_AVOX   =   87, /*  57h Audio Volume Pipe X           */
    WA_REG_AVOY   =   88, /*  58h Audio Volume Pipe Y           */
    WA_REG_AVOZ   =   89, /*  59h Audio Volume Pipe Z           */
    WA_REG_SCS1   =   91, /*  5bh S/PDIF A Channel Status 1     */
    WA_REG_SCS2   =   92, /*  5ch S/PDIF A Channel Status 2     */
    WA_REG_SPS1   =   93, /*  5dh S/PDIF B Channel Status 1     */
    WA_REG_SPS2   =   94, /*  5eh S/PDIF B Channel Status 2     */
    WA_REG_AMIL   =   95, /*  5fh Audio Min Level               */
    WA_REG_AMAP   =   96, /*  60h Audio Max Perion              */
    WA_REG_AMLA   =   99, /*  63h Audio Max Level A             */
    WA_REG_AMLB   =  100, /*  64h Audio Max Level B             */
    WA_REG_AMLC   =  101, /*  65h Audio Max Level C             */
    WA_REG_AMLD   =  102, /*  66h Audio Max Level D             */
    WA_REG_DCSC   =  113, /*  71h Digital Clock Sync Control    */
    WA_REG_SRC    =  114, /*  72h Sample Rate Converter Control */
    WA_REG_I2SC   =  115, /*  73h I2SCTR I2c control            */
    WA_REG_COCO0  =  117, /*  75h Connect ID byte 0             */
    WA_REG_COCO1  =  118, /*  76h Connect ID byte 1             */
    WA_REG_COCO2  =  119, /*  77h Connect ID byte 2             */
    WA_REG_ITF0   =  120, /*  78h Interrupt Flag 0              */
    WA_REG_ITF1   =  121, /*  79h Interrupt Flag 1              */
    WA_REG_ITF2   =  122, /*  7ah Interrupt Flag 2              */
    WA_REG_ITF3   =  123, /*  7bh Interrupt Flag 3              */
    WA_REG_ITM0   =  124, /*  7ch Interrupt Mask 0              */
    WA_REG_ITM1   =  125, /*  7dh Interrupt Mask 1              */
    WA_REG_ITM2   =  126, /*  7eh Interrupt Mask 2              */
    WA_REG_ITM3   =  127, /*  7fh Interrupt Mask 3              */
    WA_REG_MUAD   =  136, /*  88h MU Address                     */
    WA_REG_DMUA   =  139, /*  8bh 24bit Data Destination MU addr */
    WA_REG_QUIN   =  216, /*  d8h Average ratio between good and total slot, 255 is best and 0 is worst */
    WA_REG_APPL   =  258, /* 102h Application specific tweeks   */
    WA_REG_LTAB   =  259, /* 103h Audio Latency for Stream A and B */
    WA_REG_T500   =  263, /* 107h T500 Setting */
    WA_REG_STC2   =  282, /* 11ah [BB, SR-L, SR-R] 2 Down Streams, Use XO (not VCXO), no Up stream */ 
    WA_REG_LTCD   =  283, /* 11bh Audio Latency for Stream C and D */
    WA_REG_SSC1   =  293, /* 125h TODO: figure out what this is */
    WA_REG_MCUCC2 =  769, /* 301h MCU Clock control 2 Used for passthrough flashing */
    WA_REG_MCUCS  =  770, /* 302h MCU Clock Source Select */
    WA_REG_MPRR   =  771, /* 303h Register does not appear in spec used in flashing */
    WA_REG_RCCCS  =  772, /* 304h RF Calibration Control and Status */
    WA_REG_RCCIV  =  773, /* 305h RC Calibration Increment Value */
    WA_REG_RSTC   =  780, /* 30ch Reset control and status      */
    WA_REG_ID_OFF =  781, /* 30dh Identification                */
    WA_REG_GPCS40 = 1010, /* 3f2h General Purpose Chip Select 4 */
    WA_REG_GPCS41 = 1011, /* 3f3h For Passthrough               */
    WA_REG_ISCLC  = 1020, /* 3fch For passthrough               */
    WA_REG_ISDLC  = 1021, /* 3fdh For Passthrough               */
} WA_Reg_t;

typedef enum
{
    WA_BAND_AUTO_SELECT = 0,
    WA_BAND_58 = 1,
    WA_BAND_52 = 2,
    WA_BAND_24 = 3,
    WA_BAND_52_58 = 4,
    WA_BAND_24_58 = 5,
    WA_BAND_24_52 = 6,
    WA_BAND_NUM_BAND_CONFIGS = 7,
    WA_BAND_INVALID = WA_BAND_NUM_BAND_CONFIGS,
} WA_Band_t;

/* Customer global flag bit masks */
#define WA_CUSTOM_G_FLAG_INIT              0x0001
#define WA_CUSTOM_G_FLAG_OPERATING         0x0002
#define WA_CUSTOM_G_FLAG_STANDBY           0x0004
#define WA_CUSTOM_G_FLAG_PAIRING           0x0008
#define WA_CUSTOM_G_FLAG_FAST_LED          0x0010
#define WA_CUSTOM_G_FLAG_SLOW_LED          0x0020
#define WA_CUSTOM_G_FLAG_MED_LED           0x0040
#define WA_CUSTOM_G_FLAG_VERY_SLOW_LED		0x0080
#define WA_CUSTOM_G_FLAG_LINK              0x0100
#define WA_CUSTOM_G_FLAG_PAIRING_OK        0x0200
//#define CUSTOM_G_FLAG_VOLUME_REFRESH    0x0400
//#define CUSTOM_G_FLAG_MU_CHANGE_CU    	0x0400
#define WA_CUSTOM_G_FLAG_MUTE              0x0800
#define WA_CUSTOM_G_FLAG_MU_CU_PAIRED      0x2000
#define WA_CUSTOM_G_FLAG_POWER_OFF			0x4000
#define WA_CUSTOM_G_FLAG_REGULATION        0x8000

#define WA_REGISTER_BASE_ADDR 0xFC00
#define WA_REGISTER_BASE_ADDR_MSB 0xFC
#define WA_WIRELESS_MSG_ID 0x10 /* Used to address remote devices */
#define WA_BOSE_COMMAND    0x13 /* For remote message exchange this is a special bose command */
#define WA_BROADCAST_TO_MU_ADDR 0
#define WA_BROADCAST_TO_CU_ADDR 1

#define WA_PAIRING_OPEN_DATA 1
#define WA_PAIRING_CLOSE_DATA 0

#define WA_DATA_PKT_LEN 34 // 3 bytes remote address, 1 bytes message ID, 2 bytes opcode, 28 bytes data

void WirelessAudio_I2C_Init(void);
void WirelessAudio_I2C_Enable(BOOL enable);
BOOL WirelessAudio_I2C_Write8bReg(const void* data, WA_Reg_t regOffset, uint32_t length);
BOOL WirelessAudio_I2C_Write16bReg(const void* data, WA_Reg_t regOffset, uint32_t length);
BOOL WirelessAudio_I2C_Write16bRegRawAddr(const void* data, uint16_t address, uint32_t length);
BOOL WirelessAudio_I2C_Read(WA_Cmd_t readCommand, void* data, uint32_t length);
BOOL WirelessAudio_I2C_ReadReg(WA_Reg_t regOffset, void* data, uint32_t length);
BOOL WirelessAudio_I2C_Read16bReg(WA_Reg_t regOffset, void* data, uint32_t length);
BOOL WirelessAudio_I2C_WriteCommand(const void* data, WA_Cmd_t command, uint32_t length);
BOOL WirelessAudio_I2C_SendDataMessage(WA_BoseCmd_t command, const void* data, uint32_t length);
BOOL WirelessAudio_I2C_ReadDataMessage(void* message);

#endif // WIRELESS_AUDIO_I2C_H
