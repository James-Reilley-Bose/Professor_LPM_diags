/**
  ******************************************************************************
  * @file    UEITask.c
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   UEI Task.
  *          This file provides firmware functions to manage the following
  *          functionalities of the UEI:
  *           + Task communications
  *           + Init & Deinit of drivers
  *           + Resource allocations
 @verbatim
 ===============================================================================
 **/
#include "UEITask.h"
#include "project.h"
#include "TaskManager.h"
#include "TaskDefs.h"
#include "UEIServices.h"
#include "UEISerial.h"
#include "BoseUSART_API.h"
#include "IPCBulk.h"
#include "buffermanager.h"
#include "IPCRouterTask.h"
#include "UITask.h"
#include "UEI_KeyIndex.h"
#include "BMutex.h"
#include "nv_system.h"
#include "unifySourceList.h"

extern uint8_t NV_GetFirstTimeInit(void);

SCRIBE(uei_serial,ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(uei_service,ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(uei_task,ROTTEN_LOGLEVEL_DISABLED);

/* Local functions */
static void InitUEIUart(void);
static void UEI_BulkCodeSetHandler(GENERIC_MSG_t* msg);
static void UEI_HandleBulkCodeSet(uint8_t  *codeSetStr);
static void UEIBulkCodesetCallback(uint32_t* params, uint8_t numParams);
static void UEI_BlasterSendKeyCallback(uint32_t* params, uint8_t numParams);
static void UEI_GetResponse(MESSAGE_ID_t id, UEIServices_Globals *uei_dataPtr);
//static uint32_t HandleAmbiguousUEIKeyTranslation(uint8_t ueiKey, uint64_t *bitMap);
static void UEI_HandleGetBitMap(GENERIC_MSG_t* msg);
static uint8_t UEI_TVAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap);
static uint8_t UEI_CableSatAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap);
static uint8_t UEI_VCRAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap);
static uint8_t UEI_DVDAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap);
static uint8_t UEI_CD_AudioAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap);
static uint8_t UEI_HomeAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap);
static uint8_t UEI_VideoAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap);
static uint64_t UEI_GetMenuItemsForSourceMask(uint8_t *codeStr);

ManagedTask* ManagedUEIBlasterTask;

static xSemaphoreHandle         s_UEISemaphore;
static BOOL m_IRXmitInProcess = FALSE;
static BOOL m_ReleaseDiscarded = FALSE;
static BOOL m_newMoreButtonMenuItem = FALSE;

/* Configurations for the UART 610LP chip */
static const BU_InitTypeDef buartInit =
{
    BR_19200,   // 9,600
    WL_8BITS,  // 8 bit
    SB_1,      // 1 stop bit
    P_NONE,    // No parity
    M_RX_TX,   // RX and TX
    FC_NONE    // No Flow Control
};

static const UEI_KEY_CODE UEI_BOSE_KEY_MAPPING[] = {
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_00,           // 0x00
    UEI_KEY_INVALID,    // BOSE_MUTE,                  // 0x01
    UEI_KEY_INVALID,    // BOSE_VOLUME_DOWN,           // 0x02
    UEI_KEY_INVALID,    // BOSE_VOLUME_UP,             // 0x03
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_04,           // 0x04
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_05,           // 0x05
    UEI_KEY_INVALID,    // BOSE_GAME_SOURCE,           // 0x06
    UEI_KEY_INVALID,    // BOSE_SPECIAL_FEATURE_2,     // 0x07
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_08,           // 0x08
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_09,           // 0x09
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_0A,           // 0x0A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_0B,           // 0x0B
    UEI_KEY_INVALID,    // BOSE_SPECIAL_FEATURE_1,     // 0x0C
    UEI_KEY_INVALID,    // BOSE_AUX_SOURCE,            // 0x0D
    UEI_KEY_INVALID,    // BOSE_TV_SOURCE,             // 0x0E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_0F,           // 0x0F

    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_10,           // 0x10
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_11,           // 0x11
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_12,           // 0x12
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_13,           // 0x13
    UEI_KEY_AMBIGUOUS,  // BOSE_LAST_CHANNEL,          // 0x14
    UEI_KEY_INVALID,    // BOSE_CBL_SAT_SOURCE,        // 0x15
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_16,           // 0x16
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_17,           // 0x17
    UEI_KEY_CHDN,       // BOSE_CHANNEL_PRESET_DOWN,   // 0x18
    UEI_KEY_CHUP,       // BOSE_CHANNEL_PRESET_UP,     // 0x19
    UEI_KEY_STOP,       // BOSE_STOP,                  // 0x1A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_1B,           // 0x1B
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_1C,           // 0x1C
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_1D,           // 0x1D
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_1E,           // 0x1E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_1F,           // 0x1F

    UEI_KEY_DOWN,       // BOSE_DOWN_ARROW,            // 0x20
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_21,           // 0x21
    UEI_KEY_INVALID,    // BOSE_SOURCE_CYCLE,          // 0x22
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_23,           // 0x23
    UEI_KEY_PWRTOGGLE,  // BOSE_TV_POWER,              // 0x24
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_25,           // 0x25
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_26,           // 0x26
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_27,           // 0x27
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_28,           // 0x28
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_29,           // 0x29
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_2A,           // 0x2A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_2B,           // 0x2B
    UEI_KEY_INVALID,    // BOSE_SOURCE_POWER,          // 0x2C
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_2D,           // 0x2D
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_2E,           // 0x2E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_2F,           // 0x2F

    UEI_KEY_EXIT,       // BOSE_EXIT,                  // 0x30
    UEI_KEY_INVALID,    // BOSE_MENU_EXIT,             // 0x31
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_32,           // 0x32
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_33,           // 0x33
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_34,           // 0x34
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_35,           // 0x35
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_36,           // 0x36
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_37,           // 0x37
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_38,           // 0x38
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_39,           // 0x39
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_3A,           // 0x3A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_3B,           // 0x3B
    UEI_KEY_INPUT,      // BOSE_TV_INPUT,              // 0x3C
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_3D,           // 0x3D
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_3E,           // 0x3E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_3F,           // 0x3F

    UEI_KEY_DIG0,       // BOSE_NUMBER_0,              // 0x40
    UEI_KEY_DIG1,       // BOSE_NUMBER_1,              // 0x41
    UEI_KEY_DIG2,       // BOSE_NUMBER_2,              // 0x42
    UEI_KEY_DIG3,       // BOSE_NUMBER_3,              // 0x43
    UEI_KEY_DIG4,       // BOSE_NUMBER_4,              // 0x44
    UEI_KEY_DIG5,       // BOSE_NUMBER_5,              // 0x45
    UEI_KEY_DIG6,       // BOSE_NUMBER_6,              // 0x46
    UEI_KEY_DIG7,       // BOSE_NUMBER_7,              // 0x47
    UEI_KEY_DIG8,       // BOSE_NUMBER_8,              // 0x48
    UEI_KEY_DIG9,       // BOSE_NUMBER_9,              // 0x49
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_4A,           // 0x4A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_4B,           // 0x4B
    UEI_KEY_INVALID,    // BOSE_ON_OFF,                // 0x4C
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_4D,           // 0x4D
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_4E,           // 0x4E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_4F,           // 0x4F

    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_50,           // 0x50
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_51,           // 0x51
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_,             // 0x52
    UEI_KEY_INVALID,    // BOSE_BD_DVD_SOURCE,         // 0x53
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_54,           // 0x54
    UEI_KEY_PLAY,       // BOSE_PLAY,                  // 0x55
    UEI_KEY_PAUSE,      // BOSE_PAUSE,                 // 0x56
    UEI_KEY_REWIND,     // BOSE_FAST_REVERSE,          // 0x57
    UEI_KEY_FORWARD,    // BOSE_FAST_FORWARD,          // 0x58
    UEI_KEY_AMBIGUOUS,  // BOSE_QUICK_REPLAY,          // 0x59
    UEI_KEY_AMBIGUOUS,  // BOSE_QUICK_SKIP,            // 0x5A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_5B,           // 0x5B
    UEI_KEY_INVALID,    // BOSE_DASH,                  // 0x5C
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_5D,           // 0x5D
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_5E,           // 0x5E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_5F,           // 0x5F

    UEI_KEY_RIGHT,      // BOSE_RIGHT_ARROW,           // 0x60
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_61,           // 0x61
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_62,           // 0x62
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_63,           // 0x63
    UEI_KEY_AMBIGUOUS,  // BOSE_INFO,                  // 0x64
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_65,           // 0x65
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_66,           // 0x66
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_67,           // 0x67
    UEI_KEY_PGDN,       // BOSE_PAGE_DOWN,             // 0x68
    UEI_KEY_PGUP,       // BOSE_PAGE_UP,               // 0x69
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_6A,           // 0x6A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_6B,           // 0x6B
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_6C,           // 0x6C
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_6D,           // 0x6D
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_6E,           // 0x6E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_6F,           // 0x6F

    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_70,           // 0x70
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_71,           // 0x71
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_72,           // 0x72
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_73,           // 0x73
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_74,           // 0x74
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_75,           // 0x75
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_76,           // 0x76
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_77,           // 0x77
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_78,           // 0x78
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_79,           // 0x79
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_7A,           // 0x7A
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_7B,           // 0x7B
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_7C,           // 0x7C
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_7D,           // 0x7D
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_7E,           // 0x7E
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_7F,           // 0x7F

    UEI_KEY_INVALID,    // BOSE_SYSTEM,                // 0x80
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_81,           // 0x81
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_82,           // 0x82
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_83,           // 0x83
    UEI_KEY_AMBIGUOUS,  // BOSE_MORE_SOFTKEY_LIVE,     // 0x84
    UEI_KEY_FAVORITE,   // BOSE_MORE_SOFTKEY_FAVORITE, // 0x85
    UEI_KEY_FORMAT,     // BOSE_MORE_SOFTKEY_FORMAT,   // 0x86
    UEI_KEY_SAP,        // BOSE_MORE_SOFTKEY_SAP,      // 0x87
    UEI_KEY_PIP,        // BOSE_MORE_SOFTKEY_PIP,      // 0x88
    UEI_KEY_PIPOFF,     // BOSE_MORE_SOFTKEY_PIP_OFF,  // 0x89
    UEI_KEY_PANDORA,    // BOSE_MORE_SOFTKEY_PANDORA,  // 0x8A
    UEI_KEY_YOUTUBE,    // BOSE_MORE_SOFTKEY_YOUTUBE,  // 0x8B
    UEI_KEY_INVALID,    // BOSE_ASSERT_ON,             // 0x8C
    UEI_KEY_ROKU,       // BOSE_MORE_SOFTKEY_ROKU,     // 0x8D
    UEI_KEY_YAHOO,      // BOSE_MORE_SOFTKEY_YAHOO,    // 0x8E
    UEI_KEY_SPOTIFY,    // BOSE_MORE_SOFTKEY_SPOTIFY,  // 0x8F

    UEI_KEY_AMBIGUOUS,  // BOSE_DVD_MENU,                   // 0x90
    UEI_KEY_WEBINTERNET,// BOSE_MORE_SOFTKEY_WEB_INTERNET,  // 0x91
    UEI_KEY_WIDGETSAPPS,// BOSE_MORE_SOFTKEY_WIDGETS_APPS,  // 0x92
    UEI_KEY_PICTUREMODE,// BOSE_MORE_SOFTKEY_PICTURE_MODE,  // 0x93
    UEI_KEY_SOUNDMODE,  // BOSE_MORE_SOFTKEY_SOUND_MODE,    // 0x94
    UEI_KEY_CLEAR,      // BOSE_MORE_SOFTKEY_CLEAR,         // 0x95
    UEI_KEY_PWRON,      // BOSE_MORE_SOFTKEY_PWR_ON,        // 0x96
    UEI_KEY_SUBTITLE,   // BOSE_MORE_SOFTKEY_SUBTITLE,      // 0x97
    UEI_KEY_SLOW,       // BOSE_MORE_SOFTKEY_SLOW,          // 0x98
    UEI_KEY_PWROFF,     // BOSE_MORE_SOFTKEY_PWR_OFF,       // 0x99
    UEI_KEY_DELIMITER,  // BOSE_MORE_SOFTKEY_DELIMITER,     // 0x9A
    UEI_KEY_LASTCH,     // BOSE_MORE_SOFTKEY_LAST_CHANNEL,  // 0x9B
    UEI_KEY_RECORD,     // BOSE_RECORD,                     // 0x9C
    UEI_KEY_BACK,       // BOSE_MORE_SOFTKEY_BACK,          // 0x9D
    UEI_KEY_INVALID,    // BOSE_MORE_SOFTKEY_CHANNEL_LIST,  // 0x9E
    UEI_KEY_EJECT,      // BOSE_MORE_SOFTKEY_EJECT,         // 0x9F

    UEI_KEY_AMBIGUOUS,  // BOSE_LEFT_ARROW,                 // 0xA0
    UEI_KEY_QUICKSKIP,  // BOSE_MORE_SOFTKEY_QUICK_SKIP,    // 0xA1
    UEI_KEY_RECORDINGS, // BOSE_MORE_SOFTKEY_RECORDINGS,    // 0xA2
    UEI_KEY_NETFLIX,    // BOSE_MORE_SOFTKEY_NETFLIX,       // 0xA3
    UEI_KEY_AMAZON,     // BOSE_MORE_SOFTKEY_AMAZON,        // 0xA4
    UEI_KEY_3DVIDEO,    // BOSE_MORE_SOFTKEY_3DVIDEO,       // 0xA5
    UEI_KEY_INVALID,    // BOSE_MORE_SOFTKEY_STANDBY,       // 0xA6
    UEI_KEY_HULU,       // BOSE_MORE_SOFTKEY_HULU,          // 0xA7
    UEI_KEY_OPTION,     // BOSE_MORE_SOFTKEY_OPTIONS        // 0xA8
    UEI_KEY_TOOLS,      // BOSE_MORE_SOFTKEY_TOOLS,         // 0xA9
    UEI_KEY_PWRTOGGLE,  // BOSE_MORE_SOFTKEY_PWRTOGGLE,     // 0xAA
    UEI_KEY_INVALID,    // BOSE_MORE_SOFTKEY_POPUP_MENU,    // 0xAB
    UEI_KEY_INVALID,    // BOSE_MORE_SOFTKEY_TITLE_MENU,    // 0xAC
    UEI_KEY_HELP,       // BOSE_MORE_SOFTKEY_HELP,          // 0xAD
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_AE,                // 0xAE
    UEI_KEY_AMBIGUOUS,  // BOSE_INTERNET,                   // 0xAF

    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_B0,           // 0xB0
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_B1,           // 0xB1
    UEI_KEY_INVALID,    // BOSE_CLOSED_CAPTION,        // 0xB2
    UEI_KEY_INVALID,    // BOSE_SOUND_TOUCH,           // 0xB3
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_B4,           // 0xB4
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_B5,           // 0xB5
    UEI_KEY_AMBIGUOUS,  // BOSE_DVR,                   // 0xB6
    UEI_KEY_AMBIGUOUS,  // BOSE_TELETEXT,              // 0xB7
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_B8,           // 0xB8
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_B9,           // 0xB9
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_BA,           // 0xBA
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_BB,           // 0xBB
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_BC,           // 0xBC
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_BD,           // 0xBD
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_BE,           // 0xBE
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_BF,           // 0xBF

    UEI_KEY_UP,         // BOSE_UP_ARROW,              // 0xC0
    UEI_KEY_INVALID,    // BOSE_I_TV,                  // 0xC1
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_C2,           // 0xC2
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_C3,           // 0xC3
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_C4,           // 0xC4
    UEI_KEY_RED,        // BOSE_RED,                   // 0xC5
    UEI_KEY_GREEN,      // BOSE_GREEN,                 // 0xC6
    UEI_KEY_YELLOW,     // BOSE_YELLOW,                // 0xC7
    UEI_KEY_BLUE,       // BOSE_BLUE,                  // 0xC8
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_C9,           // 0xC9
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_CA,           // 0xCA
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_CB,           // 0xCB
    UEI_KEY_INVALID,    // BOSE_ASSERT_OFF,            // 0xCC
    UEI_KEY_INVALID,    // BOSE_ASSERT_UNMUTE,         // 0xCD
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_CE,           // 0xCE
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_CF,           // 0xCF

    UEI_KEY_GUIDE,      // BOSE_EPG,                   // 0xD0
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D1,           // 0xD1
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D2,           // 0xD2
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D3,           // 0xD3
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D4,           // 0xD4
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D5,           // 0xD5
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D6,           // 0xD6
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D7,           // 0xD7
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D8,           // 0xD8
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_D9,           // 0xD9
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_DA,           // 0xDA
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_DB,           // 0xDB
    UEI_KEY_INVALID,    // BOSE_ASPECT_RATIO,          // 0xDC
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_DD,           // 0xDD
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_DE,           // 0xDE
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_DF,           // 0xDF

    UEI_KEY_AMBIGUOUS,  // BOSE_ENTER,                 // 0xE0
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E1,           // 0xE1
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E2,           // 0xE2
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E3,           // 0xE3
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E4,           // 0xE4
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E5,           // 0xE5
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E6,           // 0xE6
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E7,           // 0xE7
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E8,           // 0xE8
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_E9,           // 0xE9
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_EA,           // 0xEA
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_EB,           // 0xEB
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_EC,           // 0xEC
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_ED,           // 0xED
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_EE,           // 0xEE
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_EF,           // 0xEF

    UEI_KEY_INVALID,    // BOSE_SPECIAL_FEATURE_3,     // 0xF0
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F1,           // 0xF1
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F2,           // 0xF2
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F3,           // 0xF3
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F4,           // 0xF4
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F5,           // 0xF5
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F6,           // 0xF6
    UEI_KEY_INVALID,    // BOSE_SPECIAL_FEATURE_4,     // 0xF7
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F8,           // 0xF8
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_F9,           // 0xF9
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_FA,           // 0xFA
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_FB,           // 0xFB
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_FC,           // 0xFC
    UEI_KEY_INVALID,    // KEY_VAL_AVAIL_FD,           // 0xFD
    UEI_KEY_INVALID,    // BOSE_ASSERT_MUTE,           // 0xFE
    UEI_KEY_INVALID,    // BOSE_INVALID_KEY,           // 0xFF
};

#define UEI_TV_MORE_MENU_VALID_ITEMS (  \
              LIVE_SOFTKEY_BIT        | \
              FAVORITE_SOFTKEY_BIT    | \
              FORMAT_SOFTKEY_BIT      | \
              WEBINTERNET_SOFTKEY_BIT | \
              APPSWIDGETS_SOFTKEY_BIT | \
              SUBTITLE_SOFTKEY_BIT    | \
              SAP_SOFTKEY_BIT         | \
              PIP_SOFTKEY_BIT         | \
              PIPOFF_SOFTKEY_BIT      | \
              PANDORA_SOFTKEY_BIT     | \
              YOUTUBE_SOFTKEY_BIT     | \
              ROKU_SOFTKEY_BIT        | \
              YAHOO_SOFTKEY_BIT       | \
              SPOTIFY_SOFTKEY_BIT     | \
              PICTUREMODE_SOFTKEY_BIT | \
              SOUNDMODE_SOFTKEY_BIT   | \
              CLEAR_SOFTKEY_BIT       | \
              DELIMITER_SOFTKEY_BIT   | \
              LASTCHANNEL_SOFTKEY_BIT | \
              BACK_SOFTKEY_BIT        | \
              RECORDINGS_SOFTKEY_BIT  | \
              NETFLIX_SOFTKEY_BIT     | \
              AMAZON_SOFTKEY_BIT      | \
              VIDEO3D_SOFTKEY_BIT     | \
              HULU_SOFTKEY_BIT        | \
              OPTIONS_SOFTKEY_BIT     | \
              TOOLS_SOFTKEY_BIT )

#define UEI_CABSAT_MORE_MENU_VALID_ITEMS  ( \
              LIVE_SOFTKEY_BIT           | \
              FAVORITE_SOFTKEY_BIT       | \
              WEBINTERNET_SOFTKEY_BIT    | \
              APPSWIDGETS_SOFTKEY_BIT    | \
              SAP_SOFTKEY_BIT            | \
              SUBTITLE_SOFTKEY_BIT       | \
              LASTCHANNEL_SOFTKEY_BIT    | \
              BACK_SOFTKEY_BIT           | \
              RECORDINGS_SOFTKEY_BIT     | \
              PWRTOGGLE_SOFTKEY_BIT      | \
              POPUPMENU_SOFTKEY_BIT      | \
              TITLEMENU_SOFTKEY_BIT      | \
              CHANNELLIST_SOFTKEY_BIT    | \
              OPTIONS_SOFTKEY_BIT        | \
              TOOLS_SOFTKEY_BIT          | \
              HELP_SOFTKEY_BIT )

#define UEI_VCR_MORE_MENU_VALID_ITEMS ( \
              LIVE_SOFTKEY_BIT           | \
              SAP_SOFTKEY_BIT            | \
              SLOW_SOFTKEY_BIT           | \
              SUBTITLE_SOFTKEY_BIT       | \
              EJECT_SOFTKEY_BIT          | \
              OPTIONS_SOFTKEY_BIT        | \
              TOOLS_SOFTKEY_BIT          | \
              PWRTOGGLE_SOFTKEY_BIT )

#define UEI_DVD_MORE_MENU_VALID_ITEMS ( \
              LIVE_SOFTKEY_BIT           | \
              FORMAT_SOFTKEY_BIT         | \
              WEBINTERNET_SOFTKEY_BIT    | \
              APPSWIDGETS_SOFTKEY_BIT    | \
              SAP_SOFTKEY_BIT            | \
              SLOW_SOFTKEY_BIT           | \
              SUBTITLE_SOFTKEY_BIT       | \
              EJECT_SOFTKEY_BIT          | \
              QUICKSKIP_SOFTKEY_BIT      | \
              OPTIONS_SOFTKEY_BIT        | \
              TOOLS_SOFTKEY_BIT          | \
              PWRTOGGLE_SOFTKEY_BIT )

#define UEI_AUDIO_MORE_MENU_VALID_ITEMS ( \
              FORMAT_SOFTKEY_BIT         | \
              SAP_SOFTKEY_BIT            | \
              EJECT_SOFTKEY_BIT          | \
              VIDEO3D_SOFTKEY_BIT        | \
              OPTIONS_SOFTKEY_BIT        | \
              TOOLS_SOFTKEY_BIT          | \
              PWRTOGGLE_SOFTKEY_BIT )

#define UEI_CD_MORE_MENU_VALID_ITEMS ( \
              EJECT_SOFTKEY_BIT          | \
              PWRTOGGLE_SOFTKEY_BIT )

#define UEI_HOME_MORE_MENU_VALID_ITEMS ( \
              PWRTOGGLE_SOFTKEY_BIT )

#define UEI_VIDEO_MORE_MENU_VALID_ITEMS ( \
              LIVE_SOFTKEY_BIT           | \
              FAVORITE_SOFTKEY_BIT       | \
              FORMAT_SOFTKEY_BIT         | \
              SAP_SOFTKEY_BIT            | \
              SUBTITLE_SOFTKEY_BIT       | \
              LASTCHANNEL_SOFTKEY_BIT    | \
              BACK_SOFTKEY_BIT           | \
              EJECT_SOFTKEY_BIT          | \
              VIDEO3D_SOFTKEY_BIT        | \
              CHANNELLIST_SOFTKEY_BIT    | \
              OPTIONS_SOFTKEY_BIT        | \
              TOOLS_SOFTKEY_BIT          | \
              PWRTOGGLE_SOFTKEY_BIT )




static bool g_bBlasterOperation = FALSE;

#ifdef USE_DEBUG_ASSERT
const char strFailUEISemCreate[]      = {"UEI Sem. create failed"};
#endif


static BulkEndpoint_t bulkCodeSetEndpoint = {0};
static uint8_t *rxCodeSetPtr = NULL;


/* Local functions */
static void PrintUEIResponse(uint8_t RespCode);

////////////////////////
//Basic Task functions//
////////////////////////
/*
===============================================================================
@fn Function: UEIBlasterModule_Init()
@brief Initialize routine, init IOs, semaphore
===============================================================================
*/
void UEIBlasterModule_Init(void* p)
{
  InitUEIUart();
  UEI_InitService();
  UEI_InitSerial();

  ManagedUEIBlasterTask = GetManagedTaskPointer("UEIBlasterTask");
}

/*
===============================================================================
@fn Function: UEIBlasterModule_PowerInit()
@brief Power informing that supply for chip is ready and perform a reset
===============================================================================
*/
void UEIBlasterModule_PowerInit(void)
{
  g_bBlasterOperation = TRUE;

  UEIBlasterPostMsg(UEIBLASTER_MESSAGE_ID_PowerUp, 0, 0);

  // If the LPM code was updated to add a new more buttons menu item, all of the
  // source moreButtons bit maps need to be recalculated.
  // TODO - add this in when system params work
  // NV_GetFirstTimeInit();  // Call just to wait for the NV structure to be initialized
  if (UEI_getUpdateMoreButtons())
  {
    LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "Updating source more buttons bit map after LPM update");
    UEIBlasterPostMsg(UEIBLASTER_MESSAGE_ID_UpdateMoreButtons, 0, 0);
  }
}

/*
===============================================================================
@fn Function: InitUEIUart()
@brief Initialize routine, init IOs, semaphore
===============================================================================
*/
static void InitUEIUart(void)
{
  // Setup interupt on the UART
  BUSART_PlatformBindInterruptHandler(UEI_BUART, UEI_IRQHandler, TRUE);

  // GPIO Init
  ConfigureGpioPin (UEI_UART_RX_GPIO_BANK, UEI_RX_PIN, GPIO_MODE_AF_OUT_PP, UEI_AF);
  ConfigureGpioPin (UEI_UART_TX_GPIO_BANK, UEI_TX_PIN, GPIO_MODE_AF_OUT_PP, UEI_AF);
  ConfigureGpioPin (QS_RST_L_GPIO_BANK, QS_RST_L_GPIO_PIN, GPIO_MODE_OUT_PP, UEI_AF);

  // pull the RESET_L pin high
  GPIO_SetBits(QS_RST_L_GPIO_BANK, 1 << QS_RST_L_GPIO_PIN);

  // Configure the UART for UEI
  BUSART_DeInit(UEI_BUART);
  BUSART_Init(UEI_BUART, &buartInit);

  BUSART_ITConfig(UEI_BUART, IRQ_RXNE, PS_ENABLE);
  BUSART_ITConfig(UEI_BUART, IRQ_TXE, PS_DISABLE);

  BUSART_Cmd(UEI_BUART, PS_ENABLE);

   /*  Create a semaphore to block tasks while the UEI subsystem responds to
    *  a request (eg. an ETAP get).
    *
    *  This is a binary sempahore as it is used for task synchronization. No one
    *  owns the semaphore. To synchronize tasks/isrs, one subsystem can block
    *  on a semaphore take and only when another subsystem gives the sempahore will
    *  the task unblock. The unblocked task does not give the semaphore back
    *  unless there is some kind of mutual locking going on.
    */
    if(s_UEISemaphore == NULL)
        s_UEISemaphore = xSemaphoreCreateBinary ();

    RTOS_assert ((s_UEISemaphore != NULL), strFailUEISemCreate);
}

/*
===============================================================================
@fn Function: UEIBlasterModule_DeInit()
@brief De-initilization routine
===============================================================================
*/
void UEIBlasterModule_DeInit(void)
{
  g_bBlasterOperation = FALSE;
}

/*
===============================================================================
@fn Function: UEIBlasterModule_ShutDown()
@brief Power informing that supply for chip is being shutdown and blaster
        needs to clean up
===============================================================================
*/
#if 0
void UEIBlasterModule_ShutDown(void)
{
  g_bBlasterOperation = FALSE;
}
#endif

/*
===============================================================================
@fn Function: UEIBlasterModule()
@brief Task routine for UEI serial driver, called by operating system
===============================================================================
*/
void UEIBlasterModule (void *pvParamaters)
{
    static UEIServices_Globals respMsg;

    debug_assert(ManagedUEIBlasterTask != NULL);


    for(;;)
    {
        TaskManagerPollQueue(ManagedUEIBlasterTask);
        if (m_ReleaseDiscarded)
        {
            if (0 == GetQueueMessagesWaiting(&ManagedUEIBlasterTask->Queue))
            {
                //We previously discarded a release when the queue overflowed.
                //No other messages have came in since then which means we need
                //send an "EndContinuousKey
                LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "%s - Recovering orphaned key press", __func__);
                UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_EndContinuousKey, 0, (uint32_t)&respMsg, UEI_BlasterSendKeyCallback);
                m_ReleaseDiscarded = FALSE;
            }
        }
    }
}

/*
===============================================================================
@fn Function: UEIBlasterModule_HandleMessage()
@brief Task handler function for UEI serial driver
===============================================================================
*/
void UEIBlasterModule_HandleMessage(GENERIC_MSG_t* msg)
{
    static UEIServices_Globals respMsg;

    if (g_bBlasterOperation == FALSE)
    {
        LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t isn't ready for  %s",GetEventString(msg->msgID));
        return;
    }

    if ((msg->msgID != UEIBLASTER_MESSAGE_ID_EndContinuousKey) &&
         m_IRXmitInProcess)
    {
        UEI_WakeUp();
        if (WaitAndCheckUeiResponse(TIMER_MSEC_TO_TICKS(UEI_DEFAULT_TIME_MS)))
        {
           PrintUEIResponse(g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_CODE_POS]);
           /* Extract the response code */
           PopulateResponseCode();
        }
    }
    switch (msg->msgID)
    {
        case UEIBLASTER_MESSAGE_ID_PowerUp:
            // Give the blaster module a chance to stabilize - PowerOnReset time from spec is 500 msec
            vTaskDelay(TIMER_MSEC_TO_TICKS(500));
            break;
        case UEIBLASTER_MESSAGE_ID_GetSoftwareVersion:
            GetVersion();
            UEI_GetResponse(msg->msgID, (UEIServices_Globals *)msg->params[1]);
            break;

        case UEIBLASTER_MESSAGE_ID_SendQuickKey:
        case UEIBLASTER_MESSAGE_ID_SendKey:
            if (m_IRXmitInProcess != TRUE)
            {
                UEI_SendKey(msg);
                UEI_GetResponse(msg->msgID, (UEIServices_Globals *)msg->params[1]);
                // If there was no response, the blaster request was good and IR is transmitting
                if (g_Services.ResponseCode == UEI_NO_RESPONSE && msg->msgID != UEIBLASTER_MESSAGE_ID_SendQuickKey)
                {
                    m_IRXmitInProcess = TRUE;
                }
            }
            else
            {
                //Dropping key, we are already sending one.
                LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"SendKey dropped: 0x%02x",*((uei_ueiMsg_t*)msg->params[0])->dataPtr);

                //Set the response code to undefied so we don't mistakenly print the
                //wrong status in the Callback.
                ((UEIServices_Globals *)msg->params[1])->ResponseCode = UEI_RESP_UNDEFINED;

            }
            break;

        case UEIBLASTER_MESSAGE_ID_DownloadCodeSet:
            DownLoadDeviceToFDRA(msg);
            UEI_GetResponse(msg->msgID, (UEIServices_Globals *)msg->params[1]);
            break;

        case UEIBLASTER_MESSAGE_ID_ListAllUpgradeCode:
            ListAllUpgradeCode(msg);
            UEI_GetResponse(msg->msgID, (UEIServices_Globals *)msg->params[1]);
            break;

       case UEIBLASTER_MESSAGE_ID_RemoveCodeSet:
            RemoveSingleCodeset(msg);
            UEI_GetResponse(msg->msgID, (UEIServices_Globals *)msg->params[1]);
            break;

        case UEIBLASTER_MESSAGE_ID_GetKeyMap:
            GetKeyMap((uint16_t)msg->params[0]);
            UEI_GetResponse(msg->msgID, (UEIServices_Globals *)msg->params[1]);
            break;

        case UEIBLASTER_MESSAGE_ID_EndContinuousKey:

            if (m_IRXmitInProcess)
            {
                UEI_WakeUp();
                UEI_GetResponse(msg->msgID, (UEIServices_Globals *)msg->params[1]);
                if (g_Services.ResponseCode == UEI_NO_RESPONSE)
                {
                    //Did not get a response, clear out transmit so we don't get stuck.
                    LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"UEI_NO_RESPONSE: EndContinuousKey, clearing m_IRXmitInProcess");
                    m_IRXmitInProcess = FALSE;
                    //Set the response code to invalid so we don't mistakenly print the
                    //wrong status in the Callback.
                    ((UEIServices_Globals *)msg->params[1])->ResponseCode = UEI_RESP_UNDEFINED;
                }
            }
            else
            {
                LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"DISCARDING: EndContinuousKey, m_IRXmitInProcess == FALSE\n");
            }
            break;

        case UEIBLASTER_MESSAGE_ID_FactoryReset:
            UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_MasterReset, UEI_MASTER_RESET_CLEAR_CODESET, (uint32_t)&respMsg, 0);
            break;
        case UEIBLASTER_MESSAGE_ID_MasterReset:
            MasterReset(msg);
            UEI_GetResponse(UEIBLASTER_MESSAGE_ID_MasterReset, (UEIServices_Globals *)msg->params[1]);
            //TODO
//            UIPostMsg(UI_MSG_ID_FactoryDefaultDone, NOP_CALLBACK, 0);
            break;

        case UEIBLASTER_MESSAGE_ID_GetBitMap:
            UEI_HandleGetBitMap(msg);
            break;

        case UEIBLASTER_MESSAGE_ID_UpdateMoreButtons:
          // Zero out the more buttons in the current source list
          //TODO
          //SystemBehavior_ClearMoreButtons();
          // Get the new more buttons
          //TODO
          //SystemBehavior_CreateMoreButtons();
          break;


        case IPCROUTER_MESSAGE_ID_BulkTransferRequest:
        case IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer:
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
          UEI_BulkCodeSetHandler(msg);
          break;
        default:
            // Shouldn't be here with the checks at the beginning of this function
            LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t%s was invalid message",GetEventString(msg->msgID));
            break;
    }
}

/*
 * @func UEI_HandleDelCodeset
 *
 * @brief Forwards request to remove UEI codeset to the blaster
 *
 */
void UEI_HandleDelCodeset(IpcPacket_t* packet)
{
  static uei_ueiMsg_t  msg;
  static UEIServices_Globals respMsg;

  strncpy((char *)msg.deviceStr, (char *)&packet->s.data, UEI_KEY_RECORD_SIZE);
  msg.dataPtr = NULL;
  msg.dataSize = NULL;
  UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_RemoveCodeSet, (uint32_t)&msg, (uint32_t)&respMsg, UEIStatusCallback);
}

/*
===============================================================================
@fn Function: TakeUEISemaphore()
@brief Request access to the buffer, until timeout
===============================================================================
*/
BOOL TakeUEISemaphore (uint32_t timeOutTicks)
{
    /* Returns TRUE if got semaphore before timeout, FALSE otherwise */
    if(s_UEISemaphore == NULL)
        return FALSE;
    if ( (BMutex_Take (s_UEISemaphore, timeOutTicks) != pdTRUE) )
        return FALSE;

    return TRUE;
}

/*
===============================================================================
@fn Function: GiveUEISemaphore()
@brief Release the semaphore after the resource is no longer needed
===============================================================================
*/
BOOL GiveUEISemaphore (void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if(s_UEISemaphore == NULL)
        return FALSE;
  if ( (xSemaphoreGiveFromISR (s_UEISemaphore, &xHigherPriorityTaskWoken) != pdTRUE) )
        return FALSE;

  return TRUE;
}

/*
===============================================================================
@fn Function: UEI_GetResponse()
@brief Waits for response from the UEI blaster
===============================================================================
*/
static void UEI_GetResponse(MESSAGE_ID_t id, UEIServices_Globals *uei_dataPtr)
{
  uint32_t timeout;

  // Determine message timeout
  switch (id)
  {
    case UEIBLASTER_MESSAGE_ID_SendKey:
      // Sending a continuous key does not have a response message if the command
      // is good.  So, make a minimal timeout for a key message.
      timeout = UEI_KEY_TIME_MS;
      break;
    case UEIBLASTER_MESSAGE_ID_EndContinuousKey:
      timeout = UEI_END_KEY_TIME_MS;
      break;
    case UEIBLASTER_MESSAGE_ID_DownloadCodeSet:
      timeout = UEI_LOAD_CODESET_TIME_MS;
      break;
    default:
      timeout = UEI_DEFAULT_TIME_MS;
      break;
  }

  /* Set the default response, which should trigger an error at the sending end */
  SetDefaultUEIResponse();

  if (WaitAndCheckUeiResponse(TIMER_MSEC_TO_TICKS(timeout)))
  {
    //UEI chip seems to silently discard Keys if sent too quickly(<300us) after
    //a key response code (0x01 0x00).
    Delay_us(500);

    PrintUEIResponse(g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_CODE_POS]);
    /* Extract the response code */
    PopulateResponseCode();

    switch (id)
    {
      case UEIBLASTER_MESSAGE_ID_GetSoftwareVersion:
        /* Extract the library and sw version in UEI tap */
        PopulateVersionString();
        break;
      case UEIBLASTER_MESSAGE_ID_ListAllUpgradeCode:
        /* Extract the code set within the blaster */
        PopulateCodesetString();
        break;
      case UEIBLASTER_MESSAGE_ID_GetKeyMap:
        /* Extract the key map values */
        PopulateKeyMapString();
        break;
      default:
        break;
    }
  }
    // Copy the data into the response message buffer
    if (uei_dataPtr)
    {
      memcpy(uei_dataPtr, &g_Services, sizeof(UEIServices_Globals));
    }

}


/*
===============================================================================
@fn PrintUEIResponse
@brief Prints the response from 610LP
===============================================================================
*/
static void PrintUEIResponse(uint8_t RespCode)
{
    switch(RespCode & 0x0F)
    {
        case UEI_RESP_SUCCESS:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command is successful");
          break;
        case UEI_RESP_INVALID_DEVICE_CODE:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command has invalid device code");
          break;
        case UEI_RESP_INVALID_DEVICE_TYPE:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command has invalid device type");
          break;
        case UEI_RESP_INVALID_KEY_CODE_NO_IR_DATA_PICKED:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command has invalid key code or IR data picked");
          break;
        case UEI_RESP_BAD_FDRA_BAD_MEMORY:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command FDRA has bad memory");
          break;
        case UEI_RESP_OUT_OF_MEMORY:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command is out of memory");
          break;
        case UEI_RESP_LEARNING_ERROR_TIME_OUT:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command learning error time out");
          break;
        case UEI_RESP_DATA_PACKET_FORMAT_ERROR:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command data packet format error");
          break;
        case UEI_RESP_DOWNLOADED_ID_ALREADY_EXIST:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command downloaded code set already exists");
          break;
        case UEI_RESP_LOW_VOLTAGE_FDRA_ACCESS_ERROR:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command FDRA access error");
          break;
        case UEI_RESP_INVALID_LEARNCODEID:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command invalid learn code id");
          break;
        case UEI_RESP_INCORRECT_SEQUENCE_NUMBER:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command has incorrect sequence number");
          break;
        case UEI_RESP_INVALID_IMAGE_NO_APPLICATION_AVAILABLE:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command has invalid image or no application available");
          break;
        case UEI_RESP_DECRYPT_FAILED:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command has failed decryptioin");
          break;
        case UEI_RESP_INVALID_COMMAND:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command is invalid command");
          break;
        case UEI_RESP_UNDEFINED:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t command is undefined");
          break;
        default:
          LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t Unknown response!");
          break;
    }
}

/*
===============================================================================
@fn UEI_BulkCodeSetHandler
@brief Processes code set bulk transfer messages from ASOC
===============================================================================
*/

void UEI_BulkCodeSetHandler(GENERIC_MSG_t* msg)
{

  static uint8_t *codesetStr;

  switch (msg->msgID)
  {
    case IPCROUTER_MESSAGE_ID_BulkTransferRequest:

      if (!bulkCodeSetEndpoint.Active)
      {
        IPCBulk_ActivateRxEndpoint(&bulkCodeSetEndpoint, msg->params[0], msg->params[1]);

        codesetStr = IPCBulk_GetFilename(bulkCodeSetEndpoint.tID);

        LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "Code set bulk transfer accepted");
      }
      else
      {
        IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge, bulkCodeSetEndpoint.tID, BULK_RESPONSE_DECLINE);
        LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "Bulk code set rx declined, busy");
      }
      break;

    case IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer:

      if ((bulkCodeSetEndpoint.BytesRxd+(msg->params[0] & 0xffff) <= bulkCodeSetEndpoint.TotalLength) &&
           bulkCodeSetEndpoint.Active)
      {
        rxCodeSetPtr = (uint8_t *)(msg->params[1]);
        bulkCodeSetEndpoint.BytesRxd += msg->params[0] & 0xffff;
        LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "Code set bulk transfer full buffer handled");
      }
      else
      {
        //should never get here ..
        IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer, (msg->params[0] >> 16), msg->params[1]);
        IPCBulk_DeactivateEndpoint(&bulkCodeSetEndpoint);
        LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "Code set bulk transfer error, too many bytes");
      }
      break;

    case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
      if (bulkCodeSetEndpoint.Active && (bulkCodeSetEndpoint.BytesRxd == bulkCodeSetEndpoint.TotalLength))
      {
        // Go process the message and send to blaster
        UEI_HandleBulkCodeSet(codesetStr);
        LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "Code set bulk transfer complete");
      }
      else
      {
        if(rxCodeSetPtr)
        {
            IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer, (msg->params[0] >> 16), (uint32_t) rxCodeSetPtr);
            rxCodeSetPtr = NULL;
        }
      }
      IPCBulk_DeactivateEndpoint(&bulkCodeSetEndpoint);
      break;

    case IPCROUTER_MESSAGE_ID_BulkTransferError:
    default:
      if (rxCodeSetPtr)
      {
        IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer, (msg->params[0] >> 16), msg->params[1]);
        rxCodeSetPtr = NULL;
      }
      IPCBulk_DeactivateEndpoint(&bulkCodeSetEndpoint);
      LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "Code set bulk transfer error, ipc error received");
      break;
  }
}


/*
===============================================================================
@fn UEI_HandleBulkCodeSet
@brief Sends received code set from ASOC to blaster
===============================================================================
*/

static void UEI_HandleBulkCodeSet(uint8_t *codeSetStr)
{
  static uei_ueiMsg_t csMsg;
  static UEIServices_Globals respMsg;

  strncpy((char *)csMsg.deviceStr, (char *)codeSetStr, UEI_KEY_RECORD_SIZE);

  csMsg.dataSize = bulkCodeSetEndpoint.TotalLength;
  csMsg.dataPtr = rxCodeSetPtr;
  csMsg.codeID = UEIDeviceStringToDeviceType(csMsg.deviceStr);

  UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_DownloadCodeSet, (uint32_t )&csMsg, (uint32_t)&respMsg, UEIBulkCodesetCallback);
}

/*
===============================================================================
@fn UEIBulkCodesetCallback
@brief Callback function for post message.
===============================================================================
*/
void UEIBulkCodesetCallback(uint32_t* params, uint8_t numParams)
{

  bulkCodeSetEndpoint.Active = FALSE;
  IpcOperationCompletePayload_t payload;
  UEIServices_Globals *blaster_status = (UEIServices_Globals *)params[1];

  if (rxCodeSetPtr)
  {
    IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer, bulkCodeSetEndpoint.tID, (uint32_t) rxCodeSetPtr);
    rxCodeSetPtr = NULL;
  }


  // Send the status message to the ASOC
  memset(&payload, 0, sizeof(IpcOperationCompletePayload_t));
  payload.opCompleteID = IPC_COMPLETE_SET_CODESET;

  switch (blaster_status->ResponseCode)
  {
    case 0:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_OK;
      break;
    case 5:
      payload.opCompleteStatus = IPC_COMPLETE_OUT_OF_MEMORY;
      break;
    case 8:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_EXISTS;
      break;
    case 12:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_INV_ARGS;
      break;
    case 6:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_TIMEOUT;
      break;
    default:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_ERROR;
      break;
  }
  strncpy(payload.msg, (char *)blaster_status->deviceStr, UEI_KEY_RECORD_SIZE);

  IPCRouter_Send(IPC_DEVICE_AP, IPC_OPERATION_COMPLETE, NULL, &payload, sizeof(payload));
  LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "UEI blaster codeset download complete, status 0x%02x",
       blaster_status->ResponseCode);
}

/*
===============================================================================
@fn UEIStatusCallback
@brief Callback function for post message.
===============================================================================
*/
void UEIStatusCallback(uint32_t* params, uint8_t numParams)
{

  UEIServices_Globals *blaster_status = (UEIServices_Globals *)params[1];
  IpcOperationCompletePayload_t payload;

  // Send the status message to the ASOC
  memset(&payload, 0, sizeof(IpcOperationCompletePayload_t));
  payload.opCompleteID = IPC_COMPLETE_DEL_CODESET;

  switch (blaster_status->ResponseCode)
  {
    case 0:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_OK;
      break;
    case 1:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_NOT_EXISTS;
      break;
    case 6:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_TIMEOUT;
      break;
    default:
      payload.opCompleteStatus = IPC_COMPLETE_STAT_ERROR;
      break;
  }
  strncpy(payload.msg, (char *)blaster_status->deviceStr, UEI_KEY_RECORD_SIZE);

  IPCRouter_Send(IPC_DEVICE_AP, IPC_OPERATION_COMPLETE, NULL, &payload, sizeof(payload));
  LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "UEI blaster codeset delete complete, status 0x%02x",
       blaster_status->ResponseCode);
}

static uint8 UEI_TranslateBoseToUEI(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap, char *ueiCodeID)
{
  uint8_t ueiKey = UEI_BOSE_KEY_MAPPING[boseKey];

  if (ueiKey == UEI_KEY_AMBIGUOUS)
  {
    switch (ueiCodeID[0])
    {
      case 'T':
        ueiKey = UEI_TVAmbiguousKey(boseKey, ueiAmbiguousKeyBitMap);
        break;
      case 'C':
      case 'S':

        ueiKey = UEI_CableSatAmbiguousKey(boseKey, ueiAmbiguousKeyBitMap);
        break;
      case 'V':
        ueiKey = UEI_VCRAmbiguousKey(boseKey, ueiAmbiguousKeyBitMap);
        break;
      case 'Y':
        ueiKey = UEI_DVDAmbiguousKey(boseKey, ueiAmbiguousKeyBitMap);
        break;
      case 'R':
      case 'M':
      case 'A':
      case 'D':
        ueiKey = UEI_CD_AudioAmbiguousKey(boseKey, ueiAmbiguousKeyBitMap);
        break;
      case 'H':
        ueiKey = UEI_HomeAmbiguousKey(boseKey, ueiAmbiguousKeyBitMap);
        break;
      case 'N':
        ueiKey = UEI_VideoAmbiguousKey(boseKey, ueiAmbiguousKeyBitMap);
        break;
      default:
        ueiKey = UEI_KEY_INVALID;
        break;
    }
  }
  return ueiKey;
}

static uint8 UEI_TVAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap)
{
  uint8_t ueiKey = UEI_KEY_INVALID;

  switch (boseKey)
  {
    case BOSE_ENTER:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_OK_KEY)
      {
        ueiKey = UEI_KEY_OK;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_ENTER_KEY)
      {
        ueiKey = UEI_KEY_ENTER;
      }
      break;
    case BOSE_LAST_CHANNEL:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_LASTCH_KEY)
      {
        ueiKey = UEI_KEY_LASTCH;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_BACK_KEY)
      {
        ueiKey = UEI_KEY_BACK;
      }
      break;
    case BOSE_LEFT_ARROW:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_LEFT_KEY)
      {
        ueiKey = UEI_KEY_LEFT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_BACK_KEY)
      {
        ueiKey = UEI_KEY_BACK;
      }
      break;
    case BOSE_TELETEXT:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_TEXT_KEY)
      {
        ueiKey = UEI_KEY_TEXT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SUBTITLE_KEY)
      {
        ueiKey = UEI_KEY_SUBTITLE;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SAP_KEY)
      {
        ueiKey = UEI_KEY_SAP;
      }
      break;
    case BOSE_DVR:
      ueiKey = UEI_KEY_CHANNELLIST;
      break;
    case BOSE_INTERNET:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_HOME_KEY)
      {
        ueiKey = UEI_KEY_HOME;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WIDGETSAPPS_KEY)
      {
        ueiKey = UEI_KEY_WIDGETSAPPS;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WEBINTERNET_KEY)
      {
        ueiKey = UEI_KEY_WEBINTERNET;
      }
      break;
    case BOSE_QUICK_SKIP:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_QUICKSKIP_KEY)
      {
        ueiKey = UEI_KEY_QUICKSKIP;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPFWD_KEY)
      {
        ueiKey = UEI_KEY_SKIPFWD;
      }
      break;
    case BOSE_QUICK_REPLAY:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_INSTANTRPLY_KEY)
      {
        ueiKey = UEI_KEY_INSTANTRPLY;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPREV_KEY)
      {
        ueiKey = UEI_KEY_SKIPREV;
      }
      break;
    case BOSE_DVD_MENU:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_MENU_KEY)
      {
        ueiKey = UEI_KEY_MENU;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SERVICES_KEY)
      {
        ueiKey = UEI_KEY_SERVICES;
      }
      break;
    case BOSE_INFO:
      ueiKey = UEI_KEY_INFO;
      break;
    case BOSE_MORE_SOFTKEY_LIVE:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_LIVE_KEY)
      {
        ueiKey = UEI_KEY_LIVE;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_DTV_KEY)
      {
        ueiKey = UEI_KEY_DTV;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_DISCRETE7_KEY)
      {
        ueiKey = UEI_KEY_DISCREETINPUT7;
      }
      break;
    default:
      ueiKey = UEI_KEY_INVALID;
      break;
  }
  return ueiKey;
}

static uint8 UEI_CableSatAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap)
{
  uint8_t ueiKey = UEI_KEY_INVALID;

  switch (boseKey)
  {
    case BOSE_ENTER:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_OK_KEY)
      {
        ueiKey = UEI_KEY_OK;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_ENTER_KEY)
      {
        ueiKey = UEI_KEY_ENTER;
      }
      break;
    case BOSE_LAST_CHANNEL:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_LASTCH_KEY)
      {
        ueiKey = UEI_KEY_LASTCH;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_BACK_KEY)
      {
        ueiKey = UEI_KEY_BACK;
      }
      break;
    case BOSE_LEFT_ARROW:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_LEFT_KEY)
      {
        ueiKey = UEI_KEY_LEFT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_BACK_KEY)
      {
        ueiKey = UEI_KEY_BACK;
      }
      break;
    case BOSE_TELETEXT:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_TEXT_KEY)
      {
        ueiKey = UEI_KEY_TEXT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SUBTITLE_KEY)
      {
        ueiKey = UEI_KEY_SUBTITLE;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SAP_KEY)
      {
        ueiKey = UEI_KEY_SAP;
      }
      break;
    case BOSE_DVR:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_DVR_KEY)
      {
        ueiKey = UEI_KEY_PVRMENU;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_RECORDINGS_KEY)
      {
        ueiKey = UEI_KEY_RECORDINGS;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_CHANNELLIST_KEY)
      {
        ueiKey = UEI_KEY_CHANNELLIST;
      }
      break;
    case BOSE_INTERNET:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_HOME_KEY)
      {
        ueiKey = UEI_KEY_HOME;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WIDGETSAPPS_KEY)
      {
        ueiKey = UEI_KEY_WIDGETSAPPS;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WEBINTERNET_KEY)
      {
        ueiKey = UEI_KEY_WEBINTERNET;
      }
      break;
    case BOSE_QUICK_SKIP:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_QUICKSKIP_KEY)
      {
        ueiKey = UEI_KEY_QUICKSKIP;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPFWD_KEY)
      {
        ueiKey = UEI_KEY_SKIPFWD;
      }
      break;
    case BOSE_QUICK_REPLAY:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_INSTANTRPLY_KEY)
      {
        ueiKey = UEI_KEY_INSTANTRPLY;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPREV_KEY)
      {
        ueiKey = UEI_KEY_SKIPREV;
      }
      break;
    case BOSE_DVD_MENU:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_MENU_KEY)
      {
        ueiKey = UEI_KEY_MENU;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SERVICES_KEY)
      {
        ueiKey = UEI_KEY_SERVICES;
      }
      break;
    case BOSE_INFO:
      ueiKey = UEI_KEY_INFO;
      break;
    case BOSE_MORE_SOFTKEY_LIVE:
      ueiKey = UEI_KEY_LIVE;
      break;
    default:
      ueiKey = UEI_KEY_INVALID;
      break;
  }
  return ueiKey;
}

static uint8 UEI_VCRAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap)
      {
  uint8_t ueiKey = UEI_KEY_INVALID;

  switch (boseKey)
  {
    case BOSE_ENTER:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_OK_KEY)
      {
        ueiKey = UEI_KEY_OK;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_ENTER_KEY)
      {
        ueiKey = UEI_KEY_ENTER;
      }
      break;
    case BOSE_TELETEXT:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_TEXT_KEY)
      {
        ueiKey = UEI_KEY_TEXT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SUBTITLE_KEY)
      {
        ueiKey = UEI_KEY_SUBTITLE;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SAP_KEY)
      {
        ueiKey = UEI_KEY_SAP;
      }
      break;
    case BOSE_QUICK_SKIP:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_QUICKSKIP_KEY)
      {
        ueiKey = UEI_KEY_QUICKSKIP;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPFWD_KEY)
      {
        ueiKey = UEI_KEY_SKIPFWD;
      }
      break;
    case BOSE_QUICK_REPLAY:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_INSTANTRPLY_KEY)
      {
        ueiKey = UEI_KEY_INSTANTRPLY;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPREV_KEY)
      {
        ueiKey = UEI_KEY_SKIPREV;
      }
      break;
    case BOSE_DVD_MENU:
      ueiKey = UEI_KEY_MENU;
      break;
    case BOSE_LEFT_ARROW:
      ueiKey = UEI_KEY_LEFT;
      break;
    case BOSE_MORE_SOFTKEY_LIVE:
      ueiKey = UEI_KEY_LIVE;
      break;
    default:
      ueiKey = UEI_KEY_INVALID;
      break;
  }
  return ueiKey;
}

static uint8 UEI_DVDAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap)
{
  uint8_t ueiKey = UEI_KEY_INVALID;

  switch (boseKey)
  {
    case BOSE_ENTER:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_OK_KEY)
      {
        ueiKey = UEI_KEY_OK;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_ENTER_KEY)
      {
        ueiKey = UEI_KEY_ENTER;
      }
      break;
    case BOSE_TELETEXT:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_TEXT_KEY)
      {
        ueiKey = UEI_KEY_TEXT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SUBTITLE_KEY)
      {
        ueiKey = UEI_KEY_SUBTITLE;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SAP_KEY)
      {
        ueiKey = UEI_KEY_SAP;
      }
      break;
    case BOSE_INTERNET:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_HOME_KEY)
      {
        ueiKey = UEI_KEY_HOME;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WIDGETSAPPS_KEY)
      {
        ueiKey = UEI_KEY_WIDGETSAPPS;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WEBINTERNET_KEY)
      {
        ueiKey = UEI_KEY_WEBINTERNET;
      }
      break;
    case BOSE_QUICK_SKIP:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPFWD_KEY)
      {
        ueiKey = UEI_KEY_SKIPFWD;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_QUICKSKIP_KEY)
      {
        ueiKey = UEI_KEY_QUICKSKIP;
      }
      break;
    case BOSE_QUICK_REPLAY:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPREV_KEY)
      {
        ueiKey = UEI_KEY_SKIPREV;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_INSTANTRPLY_KEY)
      {
        ueiKey = UEI_KEY_INSTANTRPLY;
      }
      break;
    case BOSE_DVD_MENU:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_POPUPMENU_KEY)
      {
        ueiKey = UEI_KEY_POPUPMENU;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_MENU_KEY)
      {
        ueiKey = UEI_KEY_MENU;
      }
      break;
    case BOSE_LEFT_ARROW:
      ueiKey = UEI_KEY_LEFT;
      break;
    case BOSE_INFO:
      ueiKey = UEI_KEY_INFO;
      break;
    case BOSE_DVR:
      ueiKey = UEI_KEY_TITLEMENU;
      break;
    case BOSE_MORE_SOFTKEY_LIVE:
      ueiKey = UEI_KEY_LIVE;
      break;
    case BOSE_LAST_CHANNEL:
      ueiKey = UEI_KEY_BACK;
      break;
    default:
      ueiKey = UEI_KEY_INVALID;
      break;
  }
  return ueiKey;
}

static uint8 UEI_CD_AudioAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap)
{
  uint8_t ueiKey = UEI_KEY_INVALID;

  switch (boseKey)
  {
    case BOSE_ENTER:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_OK_KEY)
      {
        ueiKey = UEI_KEY_OK;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_ENTER_KEY)
      {
        ueiKey = UEI_KEY_ENTER;
      }
      break;
    case BOSE_QUICK_SKIP:
      ueiKey = UEI_KEY_SKIPFWD;
      break;
    case BOSE_QUICK_REPLAY:
      ueiKey = UEI_KEY_SKIPREV;
      break;
    case BOSE_DVD_MENU:
      ueiKey = UEI_KEY_MENU;
      break;
    case BOSE_LEFT_ARROW:
      ueiKey = UEI_KEY_LEFT;
      break;
    case BOSE_INFO:
      ueiKey = UEI_KEY_INFO;
      break;
    default:
      ueiKey = UEI_KEY_INVALID;
      break;
  }
  return ueiKey;
}

static uint8 UEI_HomeAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap)
{
  uint8_t ueiKey = UEI_KEY_INVALID;

  switch (boseKey)
  {
    case BOSE_ENTER:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_OK_KEY)
      {
        ueiKey = UEI_KEY_OK;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_ENTER_KEY)
      {
        ueiKey = UEI_KEY_ENTER;
      }
      break;
    case BOSE_DVD_MENU:
      ueiKey = UEI_KEY_MENU;
      break;
    case BOSE_LEFT_ARROW:
      ueiKey = UEI_KEY_LEFT;
      break;
    default:
      ueiKey = UEI_KEY_INVALID;
      break;
  }
  return ueiKey;
}

static uint8 UEI_VideoAmbiguousKey(KEY_VALUE boseKey, uint32_t ueiAmbiguousKeyBitMap)
{
  uint8_t ueiKey = UEI_KEY_INVALID;

  switch (boseKey)
  {
    case BOSE_ENTER:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_OK_KEY)
      {
        ueiKey = UEI_KEY_OK;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_ENTER_KEY)
      {
        ueiKey = UEI_KEY_ENTER;
      }
      break;
    case BOSE_LAST_CHANNEL:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_LASTCH_KEY)
      {
        ueiKey = UEI_KEY_LASTCH;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_BACK_KEY)
      {
        ueiKey = UEI_KEY_BACK;
      }
      break;
    case BOSE_LEFT_ARROW:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_LEFT_KEY)
      {
        ueiKey = UEI_KEY_LEFT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_BACK_KEY)
      {
        ueiKey = UEI_KEY_BACK;
      }
      break;
    case BOSE_INTERNET:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_HOME_KEY)
      {
        ueiKey = UEI_KEY_HOME;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_TITLEMENU_KEY)
      {
        ueiKey = UEI_KEY_TITLEMENU;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WIDGETSAPPS_KEY)
      {
        ueiKey = UEI_KEY_WIDGETSAPPS;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_WEBINTERNET_KEY)
      {
        ueiKey = UEI_KEY_WEBINTERNET;
      }
      break;
    case BOSE_QUICK_SKIP:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_QUICKSKIP_KEY)
      {
        ueiKey = UEI_KEY_QUICKSKIP;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPFWD_KEY)
      {
        ueiKey = UEI_KEY_SKIPFWD;
      }
      break;
    case BOSE_QUICK_REPLAY:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_INSTANTRPLY_KEY)
      {
        ueiKey = UEI_KEY_INSTANTRPLY;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SKIPREV_KEY)
      {
        ueiKey = UEI_KEY_SKIPREV;
      }
      break;
    case BOSE_DVD_MENU:
      ueiKey = UEI_KEY_MENU;
      break;
    case BOSE_INFO:
      ueiKey = UEI_KEY_INFO;
      break;
    case BOSE_TELETEXT:
      if (ueiAmbiguousKeyBitMap & UEI_HAS_TEXT_KEY)
      {
        ueiKey = UEI_KEY_TEXT;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SUBTITLE_KEY)
      {
        ueiKey = UEI_KEY_SUBTITLE;
      }
      else if (ueiAmbiguousKeyBitMap & UEI_HAS_SAP_KEY)
      {
        ueiKey = UEI_KEY_SAP;
      }
      break;
    case BOSE_MORE_SOFTKEY_LIVE:
      ueiKey = UEI_KEY_LIVE;
      break;
    default:
      ueiKey = UEI_KEY_INVALID;
      break;
  }
  return ueiKey;
}

/*
static uint32_t HandleAmbiguousUEIKeyTranslation(uint8_t ueiKey, uint64_t *bitMap)
{
  uint32_t retVal = 0;

  // Handle keys with multiple translations
  switch (ueiKey)
  {
    // Enter key vs. okay key
    case UEI_KEY_ENTER:
      retVal = UEI_HAS_ENTER_KEY;
      break;
    case UEI_KEY_OK:
      retVal = UEI_HAS_OK_KEY;
      break;

    // PVR key vs. Recordings key vs. Channel List key
    case UEI_KEY_PVRMENU:
      retVal = UEI_HAS_DVR_KEY;
      break;
    case UEI_KEY_RECORDINGS:
      retVal = UEI_HAS_RECORDINGS_KEY;
      break;
    case UEI_KEY_CHANNELLIST:
      retVal = UEI_HAS_CHANNELLIST_KEY;
      break;

    // Left arrow key vs. back key vs. last channel
    case UEI_KEY_LEFT:
      retVal = UEI_HAS_LEFT_KEY;
      break;
    case UEI_KEY_BACK:
      retVal = UEI_HAS_BACK_KEY;
      break;
    case UEI_KEY_LASTCH:
      retVal = UEI_HAS_LASTCH_KEY;
      break;

    // Quick skip key vs. skip forward key
    case UEI_KEY_QUICKSKIP:
      retVal = UEI_HAS_QUICKSKIP_KEY;
      break;
    case UEI_KEY_SKIPFWD:
      retVal = UEI_HAS_SKIPFWD_KEY;
      break;

    // Quick reverse key vs. instant replay key
    case UEI_KEY_INSTANTRPLY:
      retVal = UEI_HAS_INSTANTRPLY_KEY;
      break;
    case UEI_KEY_SKIPREV:
      retVal = UEI_HAS_SKIPREV_KEY;
      break;

    // Home key vs. title menu vs. web/internet vs. widget/apps key vs. popupmenu
    case UEI_KEY_HOME:
      retVal = UEI_HAS_HOME_KEY;
      break;
    case UEI_KEY_TITLEMENU:
      retVal = UEI_HAS_TITLEMENU_KEY;
      break;
    case UEI_KEY_WEBINTERNET:
      retVal = UEI_HAS_WEBINTERNET_KEY;
      break;
    case UEI_KEY_WIDGETSAPPS:
      retVal = UEI_HAS_WIDGETSAPPS_KEY;
      break;
    case UEI_KEY_POPUPMENU:
      retVal = UEI_HAS_POPUPMENU_KEY;
      break;

   // Text key vs. Subtitle vs. SAP key
    case UEI_KEY_TEXT:
      retVal = UEI_HAS_TEXT_KEY;
      break;
    case UEI_KEY_SUBTITLE:
      retVal = UEI_HAS_SUBTITLE_KEY;
      break;
    case UEI_KEY_SAP:
      retVal = UEI_HAS_SAP_KEY;
      break;

    // Menu key vs. Services key
    case UEI_KEY_MENU:
      retVal = UEI_HAS_MENU_KEY;
      break;
    case UEI_KEY_SERVICES:
      retVal = UEI_HAS_SERVICES_KEY;
      break;

    // Live key vs. DTV key vs. Discrete Input 7 key
    case UEI_KEY_LIVE:
      retVal = UEI_HAS_LIVE_KEY;
      *bitMap |= LIVE_SOFTKEY_BIT;
      break;
    case UEI_KEY_DTV:
      retVal = UEI_HAS_DTV_KEY;
      *bitMap |= LIVE_SOFTKEY_BIT;
      break;
    case UEI_KEY_DISCREETINPUT7:
      retVal = UEI_HAS_DISCRETE7_KEY;
      *bitMap |= LIVE_SOFTKEY_BIT;
      break;

    default:
      break;
  }
  return retVal;
}
*/
/*
===============================================================================
@fn UEI_ConsumeKey
@brief Receive a Bose key value and transmit UEI translated key
===============================================================================
*/
void UEI_ConsumeKey(KEY_DATA_t key, SOURCE_ID src)
{
  if(src >= NUM_SOURCES)
  {
    return;
  }

  static uei_ueiMsg_t  msg;
  static uint8_t data;
  static UEIServices_Globals respMsg1;
  static UEIServices_Globals respMsg2;
  UnifySourceStruct* srcPtr;

  uint32_t queueSpace = GetQueueSpacesAvailable(&ManagedUEIBlasterTask->Queue);
  if (!queueSpace)
  {
     //If we discard a release, we will need to know when the queue empties
     if(LPM_KEY_STATE_RELEASED == key.state)
     {
        m_ReleaseDiscarded = TRUE;
     }

     LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "%s - Queue full discarding: Key:0x%x Event:0x%x ",
         __func__, key.value, key.state);
     return;
  }

  if (m_ReleaseDiscarded)
  {
     //There is space in the queue. Any message results in a "wakeUp" which means
     //IR will be aborted. If we discarded a release, it won't matter anymore.
     m_ReleaseDiscarded = FALSE;
  }

  if(key.value == BOSE_TV_INPUT || key.value == BOSE_TV_POWER)
  {
     srcPtr = UITask_GetUnifySourceStruct(UI_GetUnifySourceIndex(SOURCE_TV));
  }
  else
  {
     srcPtr = UITask_GetUnifySourceStruct(UI_GetUnifySourceIndex(src));
  }

  data = UEI_TranslateBoseToUEI(key.value, srcPtr->UEItoBoseKeyBitMap, srcPtr->ueiKeyRecordID);

  if (data != UEI_KEY_INVALID)
  {
    if (LPM_KEY_STATE_PRESSED == key.state || (LPM_KEY_STATE_SINGLE_PRESS == key.state))
    {
      msg.codeID = UEIDeviceStringToDeviceType((uint8_t *)srcPtr->ueiKeyRecordID);
      msg.dataPtr = &data;
      msg.dataSize = sizeof(data);

      if (LPM_KEY_STATE_SINGLE_PRESS == key.state)
      {
        msg.quick = TRUE;
        UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_SendQuickKey, (uint32_t)&msg, (uint32_t)&respMsg1, UEI_BlasterSendKeyCallback);
      }
      else
      {
        msg.quick = FALSE;
        UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_SendKey, (uint32_t)&msg, (uint32_t)&respMsg1, UEI_BlasterSendKeyCallback);
      }

      LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "UEI Key Press: 0x%x from Bose key: 0x%x UEI press sent %s", data, key.value, (msg.quick ? "quickly" : ""));
    }
    else if (LPM_KEY_STATE_RELEASED == key.state)
    {
      UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_EndContinuousKey, 0, (uint32_t)&respMsg2, UEI_BlasterSendKeyCallback);
    }
  }
  else
  {
    if (LPM_KEY_STATE_PRESSED == key.state)
    {
      LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "No UEI mapping for key 0x%x", key.value);
    }
  }
}

static void UEI_BlasterSendKeyCallback(uint32_t* params, uint8_t numParams)
{
  UEIServices_Globals *blaster_resp = (UEIServices_Globals *)params[1];

  if (!blaster_resp->ResponseCode)
  {
    LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "UEI Key release key processed");
  }
  else if (blaster_resp->ResponseCode == UEI_NO_RESPONSE)
  {
    LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "UEI Key press sent");
  }
  else
  {
    LOG(uei_task, ROTTEN_LOGLEVEL_NORMAL, "UEI Send Key error %d, current m_IRXmitInProcess=%d", blaster_resp->ResponseCode, m_IRXmitInProcess);
    m_IRXmitInProcess = FALSE;
  }
}

/*
===============================================================================
@fn UEI_ProgrammedForSource
@brief Returns TRUE if codeset for the source is stored in the UEI Blaster
===============================================================================
*/

BOOL UEI_ProgrammedForSource(KEY_DATA_t key, SOURCE_ID src)
{

  UnifySourceStruct* srcPtr;

  switch (key.value)
  {
    case BOSE_TV_INPUT:
    case BOSE_TV_POWER:
      srcPtr = UITask_GetUnifySourceStruct(UI_GetUnifySourceIndex(SOURCE_TV));
      break;

    default:
      srcPtr = UITask_GetUnifySourceStruct(UI_GetUnifySourceIndex(src));
      break;
  }

  // If the .ueiKeyRecordID is non zero, the code has been stored.
  if ((srcPtr->ueiKeyRecordID) &&
      (UEIDeviceStringToDeviceType((uint8_t *)srcPtr->ueiKeyRecordID) != 0))
  {
    return TRUE;
  }

  return FALSE;
}

void UEI_IRXmitComplete(void)
{
    m_IRXmitInProcess = FALSE;
}

void UEI_setUpdateMoreButtons(BOOL update)
{
    m_newMoreButtonMenuItem = update;
}

BOOL UEI_getUpdateMoreButtons(void)
{
    return m_newMoreButtonMenuItem;
}

static void UEI_HandleGetBitMap(GENERIC_MSG_t* msg)
{

  uint8_t *codeStr = (uint8_t *)((UnifySourceStruct *)(msg->params[0]))->ueiKeyRecordID;
  uint16_t codeID = UEIDeviceStringToDeviceType(codeStr);
  // uint8_t i,j;
  uint64_t bitMap = 0;


  if (codeID)
  {
    GetKeyMap(codeID);

    UEI_GetResponse(UEIBLASTER_MESSAGE_ID_GetKeyMap, 0);

    LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "Number of keys found = %d", g_Services.NumberOfKeysInMap);
    // now calculate bit map
    // For each of the keys in the code set
  //  for (i=0; i<g_Services.NumberOfKeysInMap; i++)
  //  {
  //    for (j=0; j<MAX_NUM_MORE_KEYS; j++)
  //    {
  //      if (UEI_BOSE_KEY_MAPPING[moreKeyTable[j].key] == g_Services.KeyMapStringPtr[i])
  //      {
  //        LOG(uei_task, ROTTEN_LOGLEVEL_VERBOSE, "more key found cs %d, key 0x%02x", codeID, g_Services.KeyMapStringPtr[i]);
            // This key is one of the more buttons keys, add it to the bit map.
  //        bitMap |= moreKeyTable[j].bitMask;
  //       break;
  //      }
  //   }
  //
       //Some of the remote keys have multiple translations to UEI based on the
       //codeset.  Keep track for each codeset as to whether it has definition(s) for one
       //of the ambiguous keys.
  //    ((UnifySourceStruct*)msg->params[0])->UEItoBoseKeyBitMap |=
  //      HandleAmbiguousUEIKeyTranslation(g_Services.KeyMapStringPtr[i], &bitMap);
  //
  //   }

    // Now, make sure we only add menu items that make sense for the TV/source
    bitMap &= UEI_GetMenuItemsForSourceMask(codeStr);

    // Save the source's bit maps
    ((UnifySourceStruct *)msg->params[0])->moreButtonsBitMap = (uint32_t)(bitMap & 0xffffffffLL);
    ((UnifySourceStruct *)msg->params[0])->moreButtonsBitMap2 = (uint32_t)(bitMap >> 32);

    // post message create bit map to get the next one
    //TODO
    //UIPostMsg(UI_MSG_ID_CreateMoreButtons, NOP_CALLBACK, 0);
  }
  //else
  //{TODO
    //UIPostMsg(UI_MSG_ID_SourceListComplete,NOP_CALLBACK, IPC_COMPLETE_STAT_ERROR);
  //}
}

static uint64_t UEI_GetMenuItemsForSourceMask(uint8_t *codeStr)
{
  uint64_t retVal = 0;

  switch (codeStr[0])
  {
    case 'T':
      retVal = UEI_TV_MORE_MENU_VALID_ITEMS;
      break;

    case 'C':
    case 'S':
      retVal = UEI_CABSAT_MORE_MENU_VALID_ITEMS;
      break;

    case 'V':
      retVal = UEI_VCR_MORE_MENU_VALID_ITEMS;
      break;

    case 'Y':
      retVal = UEI_DVD_MORE_MENU_VALID_ITEMS;
      break;

    case 'R':
    case 'M':
    case 'A':
      retVal = UEI_AUDIO_MORE_MENU_VALID_ITEMS;
      break;

    case 'D':
      retVal = UEI_CD_MORE_MENU_VALID_ITEMS;
      break;

    case 'H':
      retVal = UEI_HOME_MORE_MENU_VALID_ITEMS;
      break;

    case 'N':
      retVal = UEI_VIDEO_MORE_MENU_VALID_ITEMS;
      break;

    default:
      break;
  }

  return retVal;
}


BOOL UEI_DoesCodsetHaveKey(KEY_VALUE key, SOURCE_ID src)
{
    UnifySourceStruct* srcPtr = UITask_GetUnifySourceStruct(UI_GetUnifySourceIndex(src));
    if(srcPtr != NULL)
    {
        uint64_t moreButtonsBitMap = ((uint64_t)srcPtr->moreButtonsBitMap2 << 32) |
                                      (uint64_t)srcPtr->moreButtonsBitMap;

        for(uint32_t i = 0; i < MAX_NUM_MORE_KEYS; i++)
        {
          keyMapStruct const* mapStruct = UI_GetMoreButtonTableEnrty(i);
          if(mapStruct->key == key)
          {
            if(moreButtonsBitMap & mapStruct->bitMask)
            {
              return TRUE;
            }
            else
            {
              return FALSE;
            }
          }
        }
    }
    return FALSE;
}
