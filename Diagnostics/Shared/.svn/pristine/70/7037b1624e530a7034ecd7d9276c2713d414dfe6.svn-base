
////////////////////////////////////////////////////////////////////////////////
/// @file          RivieraLPM_KeyValues.h
/// @brief         define key values
/// @author        Dillon Johnson
/// Copyright      2015 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#ifndef RIVIERALPM_KEYVALUES_H
#define RIVIERALPM_KEYVALUES_H

#define GENERATE_ENUM(ENUM) ENUM,

typedef enum
{
    LPM_KEY_STATE_PRESSED,
    LPM_KEY_STATE_RELEASED,
    LPM_KEY_STATE_SINGLE_PRESS,
    LPM_KEY_STATE_SHORT_PRESS_AND_HOLD,
    LPM_KEY_STATE_PRESS_AND_HOLD,
    LPM_KEY_STATE_VENTI_PRESS_AND_HOLD,
    LPM_KEY_STATE_LONG_PRESS_AND_HOLD,
    LPM_KEY_STATE_XL_PRESS_AND_HOLD,
    LPM_KEY_STATE_INVALID,
    NUM_LPM_KEY_STATES = LPM_KEY_STATE_INVALID,
} LPM_KEY_STATE;

/* Key Producers */
#define FOREACH_KEY_PRODUCER(KEY_PRODUCER)\
    KEY_PRODUCER(KEY_PRODUCER_CONSOLE)\
    KEY_PRODUCER(KEY_PRODUCER_RF)\
    KEY_PRODUCER(KEY_PRODUCER_IR)\
    KEY_PRODUCER(KEY_PRODUCER_TAP)\
    KEY_PRODUCER(KEY_PRODUCER_IPC)\
    KEY_PRODUCER(KEY_PRODUCER_CEC)\
\
    KEY_PRODUCER(KEY_PRODUCER_INVALID)
#define NUM_KEY_PRODUCERS KEY_PRODUCER_INVALID
typedef enum {
	FOREACH_KEY_PRODUCER (GENERATE_ENUM)
} KEY_PRODUCER;
extern const char* KeyProducerNames[];

/* Key Groups */
#define FOREACH_KEY_GROUP(KEY_GROUP)\
	KEY_GROUP(KEY_GROUP_SOURCE)\
	KEY_GROUP(KEY_GROUP_AUDIO)\
	KEY_GROUP(KEY_GROUP_TRANSPORT_CONTROL)\
	KEY_GROUP(KEY_GROUP_MENU_NAVIGATION)\
	KEY_GROUP(KEY_GROUP_SPECIAL)\
	KEY_GROUP(KEY_GROUP_NUMERIC)\
    KEY_GROUP(KEY_GROUP_COLOR)\
   KEY_GROUP(KEY_GROUP_SOFTKEY)\
\
	KEY_GROUP(KEY_GROUP_INVALID)
#define KEY_GROUP_START KEY_GROUP_SOURCE  /* First element of above list! */
#define NUM_KEY_GROUPS KEY_GROUP_INVALID
typedef enum {
   FOREACH_KEY_GROUP (GENERATE_ENUM)
} KEY_GROUP;
extern const char* KeyGroupNames[];

typedef enum
{
    SINGLE_PRESS,
    CONTINUOUS_PRESS,
    PRESS_AND_HOLD,
    PRESS_INVALID,
} PRESS_TYPE;

typedef struct
{
    KEY_GROUP group;
    PRESS_TYPE press_type;
    const char* name;
} KEY_GROUP_TABLE_ELEMENT;
extern const KEY_GROUP_TABLE_ELEMENT Key_Info[];

typedef enum
{
   KEY_VAL_AVAIL_00,           // 0x00
   BOSE_MUTE,                  // 0x01
   BOSE_VOLUME_DOWN,           // 0x02
   BOSE_VOLUME_UP,             // 0x03
   KEY_VAL_AVAIL_04,           // 0x04
   KEY_VAL_AVAIL_05,           // 0x05
   BOSE_GAME_SOURCE,           // 0x06
   BOSE_SPECIAL_FEATURE_2,     // 0x07
   KEY_VAL_AVAIL_08,           // 0x08
   KEY_VAL_AVAIL_09,           // 0x09
   KEY_VAL_AVAIL_0A,           // 0x0A
   KEY_VAL_AVAIL_0B,           // 0x0B
   BOSE_SPECIAL_FEATURE_1,     // 0x0C
   BOSE_AUX_SOURCE,            // 0x0D
   BOSE_TV_SOURCE,             // 0x0E
   BOSE_321_AUX_SOURCE,        // 0x0F

   BOSE_SETUP,                 // 0x10
   KEY_VAL_AVAIL_11,           // 0x11
   KEY_VAL_AVAIL_12,           // 0x12
   KEY_VAL_AVAIL_13,           // 0x13
   BOSE_LAST_CHANNEL,          // 0x14
   BOSE_CBL_SAT_SOURCE,        // 0x15
   KEY_VAL_AVAIL_16,           // 0x16
   KEY_VAL_AVAIL_17,           // 0x17
   BOSE_CHANNEL_PRESET_DOWN,   // 0x18
   BOSE_CHANNEL_PRESET_UP,     // 0x19
   BOSE_STOP,                  // 0x1A
   KEY_VAL_AVAIL_1B,           // 0x1B
   KEY_VAL_AVAIL_1C,           // 0x1C
   KEY_VAL_AVAIL_1D,           // 0x1D
   KEY_VAL_AVAIL_1E,           // 0x1E
   KEY_VAL_AVAIL_1F,           // 0x1F

   BOSE_DOWN_ARROW,            // 0x20
   BOSE_SHUFFLE,               // 0x21
   BOSE_SOURCE_CYCLE,          // 0x22
   BOSE_MORE_BUTTONS,          // 0x23
   BOSE_TV_POWER,              // 0x24
   KEY_VAL_AVAIL_25,           // 0x25
   KEY_VAL_AVAIL_26,           // 0x26
   KEY_VAL_AVAIL_27,           // 0x27
   KEY_VAL_AVAIL_28,           // 0x28
   KEY_VAL_AVAIL_29,           // 0x29
   KEY_VAL_AVAIL_2A,           // 0x2A
   KEY_VAL_AVAIL_2B,           // 0x2B
   BOSE_SOURCE_POWER,          // 0x2C
   KEY_VAL_AVAIL_2D,           // 0x2D
   KEY_VAL_AVAIL_2E,           // 0x2E
   KEY_VAL_AVAIL_2F,           // 0x2F

   BOSE_EXIT,                  // 0x30
   BOSE_MENU_EXIT,             // 0x31
   KEY_VAL_AVAIL_32,           // 0x32
   KEY_VAL_AVAIL_33,           // 0x33
   KEY_VAL_AVAIL_34,           // 0x34
   KEY_VAL_AVAIL_35,           // 0x35
   KEY_VAL_AVAIL_36,           // 0x36
   KEY_VAL_AVAIL_37,           // 0x37
   KEY_VAL_AVAIL_38,           // 0x38
   KEY_VAL_AVAIL_39,           // 0x39
   KEY_VAL_AVAIL_3A,           // 0x3A
   KEY_VAL_AVAIL_3B,           // 0x3B
   BOSE_TV_INPUT,              // 0x3C
   KEY_VAL_AVAIL_3D,           // 0x3D
   KEY_VAL_AVAIL_3E,           // 0x3E
   KEY_VAL_AVAIL_3F,           // 0x3F

   BOSE_NUMBER_0,              // 0x40
   BOSE_NUMBER_1,              // 0x41
   BOSE_NUMBER_2,              // 0x42
   BOSE_NUMBER_3,              // 0x43
   BOSE_NUMBER_4,              // 0x44
   BOSE_NUMBER_5,              // 0x45
   BOSE_NUMBER_6,              // 0x46
   BOSE_NUMBER_7,              // 0x47
   BOSE_NUMBER_8,              // 0x48
   BOSE_NUMBER_9,              // 0x49
   KEY_VAL_AVAIL_4A,           // 0x4A
   KEY_VAL_AVAIL_4B,           // 0x4B
   BOSE_ON_OFF,                // 0x4C
   KEY_VAL_AVAIL_4D,           // 0x4D
   KEY_VAL_AVAIL_4E,           // 0x4E
   KEY_VAL_AVAIL_4F,           // 0x4F

   KEY_VAL_AVAIL_50,           // 0x50
   KEY_VAL_AVAIL_51,           // 0x51
   KEY_VAL_AVAIL_52,           // 0x52
   BOSE_BD_DVD_SOURCE,         // 0x53
   KEY_VAL_AVAIL_54,           // 0x54
   BOSE_PLAY,                  // 0x55
   BOSE_PAUSE,                 // 0x56
   BOSE_FAST_REVERSE,          // 0x57
   BOSE_FAST_FORWARD,          // 0x58
   BOSE_QUICK_REPLAY,          // 0x59
   BOSE_QUICK_SKIP,            // 0x5A
   KEY_VAL_AVAIL_5B,           // 0x5B
   BOSE_DASH,                  // 0x5C
   BOSE_AUTO_WAKE_TOGGLE,      // 0x5D
   KEY_VAL_AVAIL_5E,           // 0x5E
   KEY_VAL_AVAIL_5F,           // 0x5F

   BOSE_RIGHT_ARROW,           // 0x60
   KEY_VAL_AVAIL_61,           // 0x61
   KEY_VAL_AVAIL_62,           // 0x62
   KEY_VAL_AVAIL_63,           // 0x63
   BOSE_INFO,                  // 0x64
   KEY_VAL_AVAIL_65,           // 0x65
   KEY_VAL_AVAIL_66,           // 0x66
   KEY_VAL_AVAIL_67,           // 0x67
   BOSE_PAGE_DOWN,             // 0x68
   BOSE_PAGE_UP,               // 0x69
   BOSE_PAGE_THUMBS_DOWN,      // 0x6A
   BOSE_PAGE_THUMBS_UP,        // 0x6B
   KEY_VAL_AVAIL_6C,           // 0x6C
   KEY_VAL_AVAIL_6D,           // 0x6D
   KEY_VAL_AVAIL_6E,           // 0x6E
   KEY_VAL_AVAIL_6F,           // 0x6F

   HDMI_1,                     // 0x70
   HDMI_2,                     // 0x71
   HDMI_3,                     // 0x72
   HDMI_4,                     // 0x73
   HDMI_5,                     // 0x74
   HDMI_FRONT,                 // 0x75
   KEY_VAL_AVAIL_76,           // 0x76
   KEY_VAL_AVAIL_77,           // 0x77
   KEY_VAL_AVAIL_78,           // 0x78
   KEY_VAL_AVAIL_79,           // 0x79
   KEY_VAL_AVAIL_7A,           // 0x7A
   KEY_VAL_AVAIL_7B,           // 0x7B
   KEY_VAL_AVAIL_7C,           // 0x7C
   KEY_VAL_AVAIL_7D,           // 0x7D
   KEY_VAL_AVAIL_7E,           // 0x7E
   KEY_VAL_AVAIL_7F,           // 0x7F

   BOSE_SYSTEM,                // 0x80
   BOSE_BLUETOOTH_SOURCE,      // 0x81
   BOSE_BASS_ADJUST,           // 0x82
   BOSE_DIALOG_ADJUST,         // 0x83
   BOSE_MORE_SOFTKEY_LIVE,     // 0x84
   BOSE_MORE_SOFTKEY_FAVORITE, // 0x85
   BOSE_MORE_SOFTKEY_FORMAT,   // 0x86
   BOSE_MORE_SOFTKEY_SAP,      // 0x87
   BOSE_MORE_SOFTKEY_PIP,      // 0x88
   BOSE_MORE_SOFTKEY_PIP_OFF,  // 0x89
   BOSE_MORE_SOFTKEY_PANDORA,  // 0x8A
   BOSE_MORE_SOFTKEY_YOUTUBE,  // 0x8B
   BOSE_ASSERT_ON,             // 0x8C
   BOSE_MORE_SOFTKEY_ROKU,     // 0x8D
   BOSE_MORE_SOFTKEY_YAHOO,    // 0x8E
   BOSE_MORE_SOFTKEY_SPOTIFY,  // 0x8F

   BOSE_DVD_MENU,              // 0x90
   BOSE_MORE_SOFTKEY_WEB_INTERNET,  // 0x91
   BOSE_MORE_SOFTKEY_WIDGETS_APPS,  // 0x92
   BOSE_MORE_SOFTKEY_PICTURE_MODE,  // 0x93
   BOSE_MORE_SOFTKEY_SOUND_MODE,    // 0x94
   BOSE_MORE_SOFTKEY_CLEAR,         // 0x95
   BOSE_MORE_SOFTKEY_PWR_ON,        // 0x96
   BOSE_MORE_SOFTKEY_SUBTITLE,      // 0x97
   BOSE_MORE_SOFTKEY_SLOW,          // 0x98
   BOSE_MORE_SOFTKEY_PWR_OFF,       // 0x99
   BOSE_MORE_SOFTKEY_DELIMITER,     // 0x9A
   BOSE_MORE_SOFTKEY_LAST_CHANNEL,  // 0x9B
   BOSE_RECORD,                // 0x9C
   BOSE_MORE_SOFTKEY_BACK,          // 0x9D
   BOSE_MORE_SOFTKEY_CHANNEL_LIST,  // 0x9E
   BOSE_MORE_SOFTKEY_EJECT,         // 0x9F

   BOSE_LEFT_ARROW,            // 0xA0
   BOSE_MORE_SOFTKEY_QUICK_SKIP,    // 0xA1
   BOSE_MORE_SOFTKEY_RECORDINGS,    // 0xA2
   BOSE_MORE_SOFTKEY_NETFLIX,       // 0xA3
   BOSE_MORE_SOFTKEY_AMAZON,        // 0xA4
   BOSE_MORE_SOFTKEY_3DVIDEO,       // 0xA5
   BOSE_MORE_SOFTKEY_STANDBY,       // 0xA6
   BOSE_MORE_SOFTKEY_HULU,          // 0xA7
   BOSE_MORE_SOFTKEY_OPTIONS,       // 0xA8
   BOSE_MORE_SOFTKEY_TOOLS,         // 0xA9
   BOSE_MORE_SOFTKEY_PWRTOGGLE,     // 0xAA
   BOSE_MORE_SOFTKEY_POPUPMENU,     // 0xAB
   BOSE_MORE_SOFTKEY_TITLEMENU,     // 0xAC
   BOSE_MORE_SOFTKEY_HELP,          // 0xAD
   KEY_VAL_AVAIL_AE,           // 0xAE
   BOSE_INTERNET,              // 0xAF

   KEY_VAL_AVAIL_B0,           // 0xB0
   KEY_VAL_AVAIL_B1,           // 0xB1
   BOSE_CLOSED_CAPTION,        // 0xB2
   BOSE_SOUND_TOUCH_SOURCE,    // 0xB3
   KEY_VAL_AVAIL_B4,           // 0xB4
   KEY_VAL_AVAIL_B5,           // 0xB5
   BOSE_DVR,                   // 0xB6
   BOSE_TELETEXT,              // 0xB7
   KEY_VAL_AVAIL_B8,           // 0xB8
   KEY_VAL_AVAIL_B9,           // 0xB9
   KEY_VAL_AVAIL_BA,           // 0xBA
   KEY_VAL_AVAIL_BB,           // 0xBB
   KEY_VAL_AVAIL_BC,           // 0xBC
   KEY_VAL_AVAIL_BD,           // 0xBD
   KEY_VAL_AVAIL_BE,           // 0xBE
   KEY_VAL_AVAIL_BF,           // 0xBF

   BOSE_UP_ARROW,              // 0xC0
   BOSE_I_TV,                  // 0xC1
   KEY_VAL_AVAIL_C2,           // 0xC2
   KEY_VAL_AVAIL_C3,           // 0xC3
   KEY_VAL_AVAIL_C4,           // 0xC4
   BOSE_RED,                   // 0xC5
   BOSE_GREEN,                 // 0xC6
   BOSE_YELLOW,                // 0xC7
   BOSE_BLUE,                  // 0xC8
   KEY_VAL_AVAIL_C9,           // 0xC9
   KEY_VAL_AVAIL_CA,           // 0xCA
   KEY_VAL_AVAIL_CB,           // 0xCB
   BOSE_ASSERT_OFF,            // 0xCC
   BOSE_ASSERT_UNMUTE,         // 0xCD
   KEY_VAL_AVAIL_CE,           // 0xCE
   KEY_VAL_AVAIL_CF,           // 0xCF

   BOSE_EPG,                   // 0xD0
   KEY_VAL_AVAIL_D1,           // 0xD1
   KEY_VAL_AVAIL_D2,           // 0xD2
   KEY_VAL_AVAIL_D3,           // 0xD3
   KEY_VAL_AVAIL_D4,           // 0xD4
   KEY_VAL_AVAIL_D5,           // 0xD5
   KEY_VAL_AVAIL_D6,           // 0xD6
   KEY_VAL_AVAIL_D7,           // 0xD7
   KEY_VAL_AVAIL_D8,           // 0xD8
   KEY_VAL_AVAIL_D9,           // 0xD9
   KEY_VAL_AVAIL_DA,           // 0xDA
   KEY_VAL_AVAIL_DB,           // 0xDB
   BOSE_ASPECT_RATIO,          // 0xDC
   KEY_VAL_AVAIL_DD,           // 0xDD
   KEY_VAL_AVAIL_DE,           // 0xDE
   KEY_VAL_AVAIL_DF,           // 0xDF

   BOSE_ENTER,                 // 0xE0
   KEY_VAL_AVAIL_E1,           // 0xE1
   KEY_VAL_AVAIL_E2,           // 0xE2
   KEY_VAL_AVAIL_E3,           // 0xE3
   KEY_VAL_AVAIL_E4,           // 0xE4
   KEY_VAL_AVAIL_E5,           // 0xE5
   KEY_VAL_AVAIL_E6,           // 0xE6
   KEY_VAL_AVAIL_E7,           // 0xE7
   KEY_VAL_AVAIL_E8,           // 0xE8
   KEY_VAL_AVAIL_E9,           // 0xE9
   KEY_VAL_AVAIL_EA,           // 0xEA
   KEY_VAL_AVAIL_EB,           // 0xEB
   KEY_VAL_AVAIL_EC,           // 0xEC
   KEY_VAL_AVAIL_ED,           // 0xED
   KEY_VAL_AVAIL_EE,           // 0xEE
   KEY_VAL_AVAIL_EF,           // 0xEF

   BOSE_SPECIAL_FEATURE_3,     // 0xF0
   BOSE_RFREMOTE_BACK_TRIGGER, // 0xF1
   BOSE_RFREMOTE_REPAIR,       // 0xF2
   BOSE_RFREMOTE_PTS,          // 0xF3
   BOSE_RFREMOTE_FD,           // 0xF4
   KEY_VAL_AVAIL_F5,           // 0xF5
   KEY_VAL_AVAIL_F6,           // 0xF6
   BOSE_SPECIAL_FEATURE_4,     // 0xF7
   KEY_VAL_AVAIL_F8,           // 0xF8
   KEY_VAL_AVAIL_F9,           // 0xF9
   KEY_VAL_AVAIL_FA,           // 0xFA
   KEY_VAL_AVAIL_FB,           // 0xFB
   KEY_VAL_AVAIL_FC,           // 0xFC
   KEY_VAL_AVAIL_FD,           // 0xFD
   BOSE_ASSERT_MUTE,           // 0xFE
   BOSE_INVALID_KEY,           // 0xFF
   NUM_BOSE_KEYS = BOSE_INVALID_KEY,
} KEY_VALUE;

#endif //RIVIERALPM_KEYVALUES_H
