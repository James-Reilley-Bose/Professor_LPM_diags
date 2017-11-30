
////////////////////////////////////////////////////////////////////////////////
/// @file          DiagRivieraLPM_Values.h
/// @brief         define diag msg values
////////////////////////////////////////////////////////////////////////////////
#ifndef _A4V_DIAGVALUES_H_
#define _A4V_DIAGVALUES_H_

//#define GENERATE_ENUM(ENUM) ENUM,

typedef enum
{
    DIAG_STATE_PRESSED,
    DIAG_STATE_RELEASED,
    DIAG_STATE_SINGLE_PRESS,
    DIAG_STATE_SHORT_PRESS_AND_HOLD,
    DIAG_STATE_PRESS_AND_HOLD,
    DIAG_STATE_VENTI_PRESS_AND_HOLD,
    DIAG_STATE_LONG_PRESS_AND_HOLD,
    DIAG_STATE_XL_PRESS_AND_HOLD,
    DIAG_STATE_INVALID,
    NUM_DIAG_STATES = DIAG_STATE_INVALID,
} DIAG_STATE;

/* Diag Key Producers */
#define FOREACH_DIAG_PRODUCER(DIAG_PRODUCER)\
    DIAG_PRODUCER(DIAG_PRODUCER_CONSOLE)\
    DIAG_PRODUCER(DIAG_PRODUCER_RF)\
    DIAG_PRODUCER(DIAG_PRODUCER_IR)\
    DIAG_PRODUCER(DIAG_PRODUCER_TAP)\
    DIAG_PRODUCER(DIAG_PRODUCER_IPC)\
    DIAG_PRODUCER(DIAG_PRODUCER_CEC)\
\
    DIAG_PRODUCER(DIAG_PRODUCER_INVALID)
#define NUM_DIAG_PRODUCERS DIAG_PRODUCER_INVALID
typedef enum
{
    FOREACH_DIAG_PRODUCER (GENERATE_ENUM)
} DIAG_PRODUCER;
extern const char* DiagsProducerNames[];

/* Diag Key Groups */
#define FOREACH_DIAG_GROUP(DIAG_GROUP)\
    DIAG_GROUP(DIAG_GROUP_SOURCE)\
    DIAG_GROUP(DIAG_GROUP_AUDIO)\
    DIAG_GROUP(DIAG_GROUP_TRANSPORT_CONTROL)\
    DIAG_GROUP(DIAG_GROUP_MENU_NAVIGATION)\
    DIAG_GROUP(DIAG_GROUP_SPECIAL)\
    DIAG_GROUP(DIAG_GROUP_NUMERIC)\
    DIAG_GROUP(DIAG_GROUP_COLOR)\
   DIAG_GROUP(DIAG_GROUP_SOFTDIAG)\
\
    DIAG_GROUP(DIAG_GROUP_INVALID)
#define DIAG_GROUP_START DIAG_GROUP_SOURCE  /* First element of above list! */
#define NUM_DIAG_GROUPS DIAG_GROUP_INVALID
typedef enum
{
    FOREACH_DIAG_GROUP (GENERATE_ENUM)
} DIAG_GROUP;
extern const char* DiagGroupNames[];

typedef enum
{
    DIAG_SINGLE_PRESS,
    DIAG_CONTINUOUS_PRESS,
    DIAG_PRESS_AND_HOLD,
    DIAG_PRESS_INVALID,
} DIAG_PRESS_TYPE;

typedef struct
{
    DIAG_GROUP group;
    DIAG_PRESS_TYPE press_type;
    const char* name;
} DIAG_GROUP_TABLE_ELEMENT;
extern const DIAG_GROUP_TABLE_ELEMENT Diag_Info[];

typedef enum
{
    DIAG_VAL_AVAIL_00,           // 0x00
    DIAG_BOSE_MUTE,                  // 0x01
    DIAG_BOSE_VOLUME_DOWN,           // 0x02
    DIAG_BOSE_VOLUME_UP,             // 0x03
    DIAG_VAL_AVAIL_04,           // 0x04
    DIAG_VAL_AVAIL_05,           // 0x05
    DIAG_BOSE_GAME_SOURCE,           // 0x06
    DIAG_BOSE_SPECIAL_FEATURE_2,     // 0x07
    DIAG_VAL_AVAIL_08,           // 0x08
    DIAG_VAL_AVAIL_09,           // 0x09
    DIAG_VAL_AVAIL_0A,           // 0x0A
    DIAG_VAL_AVAIL_0B,           // 0x0B
    DIAG_BOSE_SPECIAL_FEATURE_1,     // 0x0C
    DIAG_BOSE_AUX_SOURCE,            // 0x0D
    DIAG_BOSE_TV_SOURCE,             // 0x0E
    DIAG_BOSE_321_AUX_SOURCE,        // 0x0F

    DIAG_BOSE_SETUP,                 // 0x10
    DIAG_VAL_AVAIL_11,           // 0x11
    DIAG_VAL_AVAIL_12,           // 0x12
    DIAG_VAL_AVAIL_13,           // 0x13
    DIAG_BOSE_LAST_CHANNEL,          // 0x14
    DIAG_BOSE_CBL_SAT_SOURCE,        // 0x15
    DIAG_VAL_AVAIL_16,           // 0x16
    DIAG_VAL_AVAIL_17,           // 0x17
    DIAG_BOSE_CHANNEL_PRESET_DOWN,   // 0x18
    DIAG_BOSE_CHANNEL_PRESET_UP,     // 0x19
    DIAG_BOSE_STOP,                  // 0x1A
    DIAG_VAL_AVAIL_1B,           // 0x1B
    DIAG_VAL_AVAIL_1C,           // 0x1C
    DIAG_VAL_AVAIL_1D,           // 0x1D
    DIAG_VAL_AVAIL_1E,           // 0x1E
    DIAG_VAL_AVAIL_1F,           // 0x1F

    DIAG_BOSE_DOWN_ARROW,            // 0x20
    DIAG_BOSE_SHUFFLE,               // 0x21
    DIAG_BOSE_SOURCE_CYCLE,          // 0x22
    DIAG_BOSE_MORE_BUTTONS,          // 0x23
    DIAG_BOSE_TV_POWER,              // 0x24
    DIAG_VAL_AVAIL_25,           // 0x25
    DIAG_VAL_AVAIL_26,           // 0x26
    DIAG_VAL_AVAIL_27,           // 0x27
    DIAG_VAL_AVAIL_28,           // 0x28
    DIAG_VAL_AVAIL_29,           // 0x29
    DIAG_VAL_AVAIL_2A,           // 0x2A
    DIAG_VAL_AVAIL_2B,           // 0x2B
    DIAG_BOSE_SOURCE_POWER,          // 0x2C
    DIAG_VAL_AVAIL_2D,           // 0x2D
    DIAG_VAL_AVAIL_2E,           // 0x2E
    DIAG_VAL_AVAIL_2F,           // 0x2F

    DIAG_BOSE_EXIT,                  // 0x30
    DIAG_BOSE_MENU_EXIT,             // 0x31
    DIAG_VAL_AVAIL_32,           // 0x32
    DIAG_VAL_AVAIL_33,           // 0x33
    DIAG_VAL_AVAIL_34,           // 0x34
    DIAG_VAL_AVAIL_35,           // 0x35
    DIAG_VAL_AVAIL_36,           // 0x36
    DIAG_VAL_AVAIL_37,           // 0x37
    DIAG_VAL_AVAIL_38,           // 0x38
    DIAG_VAL_AVAIL_39,           // 0x39
    DIAG_VAL_AVAIL_3A,           // 0x3A
    DIAG_VAL_AVAIL_3B,           // 0x3B
    DIAG_BOSE_TV_INPUT,              // 0x3C
    DIAG_VAL_AVAIL_3D,           // 0x3D
    DIAG_VAL_AVAIL_3E,           // 0x3E
    DIAG_VAL_AVAIL_3F,           // 0x3F

    DIAG_BOSE_NUMBER_0,              // 0x40
    DIAG_BOSE_NUMBER_1,              // 0x41
    DIAG_BOSE_NUMBER_2,              // 0x42
    DIAG_BOSE_NUMBER_3,              // 0x43
    DIAG_BOSE_NUMBER_4,              // 0x44
    DIAG_BOSE_NUMBER_5,              // 0x45
    DIAG_BOSE_NUMBER_6,              // 0x46
    DIAG_BOSE_NUMBER_7,              // 0x47
    DIAG_BOSE_NUMBER_8,              // 0x48
    DIAG_BOSE_NUMBER_9,              // 0x49
    DIAG_VAL_AVAIL_4A,           // 0x4A
    DIAG_VAL_AVAIL_4B,           // 0x4B
    DIAG_BOSE_ON_OFF,                // 0x4C
    DIAG_VAL_AVAIL_4D,           // 0x4D
    DIAG_VAL_AVAIL_4E,           // 0x4E
    DIAG_VAL_AVAIL_4F,           // 0x4F

    DIAG_VAL_AVAIL_50,           // 0x50
    DIAG_VAL_AVAIL_51,           // 0x51
    DIAG_VAL_AVAIL_52,           // 0x52
    DIAG_BOSE_BD_DVD_SOURCE,         // 0x53
    DIAG_VAL_AVAIL_54,           // 0x54
    DIAG_BOSE_PLAY,                  // 0x55
    DIAG_BOSE_PAUSE,                 // 0x56
    DIAG_BOSE_FAST_REVERSE,          // 0x57
    DIAG_BOSE_FAST_FORWARD,          // 0x58
    DIAG_BOSE_QUICK_REPLAY,          // 0x59
    DIAG_BOSE_QUICK_SKIP,            // 0x5A
    DIAG_VAL_AVAIL_5B,           // 0x5B
    DIAG_BOSE_DASH,                  // 0x5C
    DIAG_BOSE_AUTO_WAKE_TOGGLE,      // 0x5D
    DIAG_VAL_AVAIL_5E,           // 0x5E
    DIAG_VAL_AVAIL_5F,           // 0x5F

    DIAG_BOSE_RIGHT_ARROW,           // 0x60
    DIAG_VAL_AVAIL_61,           // 0x61
    DIAG_VAL_AVAIL_62,           // 0x62
    DIAG_VAL_AVAIL_63,           // 0x63
    DIAG_BOSE_INFO,                  // 0x64
    DIAG_VAL_AVAIL_65,           // 0x65
    DIAG_VAL_AVAIL_66,           // 0x66
    DIAG_VAL_AVAIL_67,           // 0x67
    DIAG_BOSE_PAGE_DOWN,             // 0x68
    DIAG_BOSE_PAGE_UP,               // 0x69
    DIAG_BOSE_PAGE_THUMBS_DOWN,      // 0x6A
    DIAG_BOSE_PAGE_THUMBS_UP,        // 0x6B
    DIAG_VAL_AVAIL_6C,           // 0x6C
    DIAG_VAL_AVAIL_6D,           // 0x6D
    DIAG_VAL_AVAIL_6E,           // 0x6E
    DIAG_VAL_AVAIL_6F,           // 0x6F

    DIAG_HDMI_1,                     // 0x70
    DIAG_HDMI_2,                     // 0x71
    DIAG_HDMI_3,                     // 0x72
    DIAG_HDMI_4,                     // 0x73
    DIAG_HDMI_5,                     // 0x74
    DIAG_HDMI_FRONT,                 // 0x75
    DIAG_VAL_AVAIL_76,           // 0x76
    DIAG_VAL_AVAIL_77,           // 0x77
    DIAG_VAL_AVAIL_78,           // 0x78
    DIAG_VAL_AVAIL_79,           // 0x79
    DIAG_VAL_AVAIL_7A,           // 0x7A
    DIAG_VAL_AVAIL_7B,           // 0x7B
    DIAG_VAL_AVAIL_7C,           // 0x7C
    DIAG_VAL_AVAIL_7D,           // 0x7D
    DIAG_VAL_AVAIL_7E,           // 0x7E
    DIAG_VAL_AVAIL_7F,           // 0x7F

    DIAG_BOSE_SYSTEM,                // 0x80
    DIAG_BOSE_BLUETOOTH_SOURCE,      // 0x81
    DIAG_BOSE_BASS_ADJUST,           // 0x82
    DIAG_BOSE_DIALOG_ADJUST,         // 0x83
    DIAG_BOSE_MORE_SOFTDIAG_LIVE,     // 0x84
    DIAG_BOSE_MORE_SOFTDIAG_FAVORITE, // 0x85
    DIAG_BOSE_MORE_SOFTDIAG_FORMAT,   // 0x86
    DIAG_BOSE_MORE_SOFTDIAG_SAP,      // 0x87
    DIAG_BOSE_MORE_SOFTDIAG_PIP,      // 0x88
    DIAG_BOSE_MORE_SOFTDIAG_PIP_OFF,  // 0x89
    DIAG_BOSE_MORE_SOFTDIAG_PANDORA,  // 0x8A
    DIAG_BOSE_MORE_SOFTDIAG_YOUTUBE,  // 0x8B
    DIAG_BOSE_ASSERT_ON,             // 0x8C
    DIAG_BOSE_MORE_SOFTDIAG_ROKU,     // 0x8D
    DIAG_BOSE_MORE_SOFTDIAG_YAHOO,    // 0x8E
    DIAG_BOSE_MORE_SOFTDIAG_SPOTIFY,  // 0x8F

    DIAG_BOSE_DVD_MENU,              // 0x90
    DIAG_BOSE_MORE_SOFTDIAG_WEB_INTERNET,  // 0x91
    DIAG_BOSE_MORE_SOFTDIAG_WIDGETS_APPS,  // 0x92
    DIAG_BOSE_MORE_SOFTDIAG_PICTURE_MODE,  // 0x93
    DIAG_BOSE_MORE_SOFTDIAG_SOUND_MODE,    // 0x94
    DIAG_BOSE_MORE_SOFTDIAG_CLEAR,         // 0x95
    DIAG_BOSE_MORE_SOFTDIAG_PWR_ON,        // 0x96
    DIAG_BOSE_MORE_SOFTDIAG_SUBTITLE,      // 0x97
    DIAG_BOSE_MORE_SOFTDIAG_SLOW,          // 0x98
    DIAG_BOSE_MORE_SOFTDIAG_PWR_OFF,       // 0x99
    DIAG_BOSE_MORE_SOFTDIAG_DELIMITER,     // 0x9A
    DIAG_BOSE_MORE_SOFTDIAG_LAST_CHANNEL,  // 0x9B
    DIAG_BOSE_RECORD,                // 0x9C
    DIAG_BOSE_MORE_SOFTDIAG_BACK,          // 0x9D
    DIAG_BOSE_MORE_SOFTDIAG_CHANNEL_LIST,  // 0x9E
    DIAG_BOSE_MORE_SOFTDIAG_EJECT,         // 0x9F

    DIAG_BOSE_LEFT_ARROW,            // 0xA0
    DIAG_BOSE_MORE_SOFTDIAG_QUICK_SKIP,    // 0xA1
    DIAG_BOSE_MORE_SOFTDIAG_RECORDINGS,    // 0xA2
    DIAG_BOSE_MORE_SOFTDIAG_NETFLIX,       // 0xA3
    DIAG_BOSE_MORE_SOFTDIAG_AMAZON,        // 0xA4
    DIAG_BOSE_MORE_SOFTDIAG_3DVIDEO,       // 0xA5
    DIAG_BOSE_MORE_SOFTDIAG_STANDBY,       // 0xA6
    DIAG_BOSE_MORE_SOFTDIAG_HULU,          // 0xA7
    DIAG_BOSE_MORE_SOFTDIAG_OPTIONS,       // 0xA8
    DIAG_BOSE_MORE_SOFTDIAG_TOOLS,         // 0xA9
    DIAG_BOSE_MORE_SOFTDIAG_PWRTOGGLE,     // 0xAA
    DIAG_BOSE_MORE_SOFTDIAG_POPUPMENU,     // 0xAB
    DIAG_BOSE_MORE_SOFTDIAG_TITLEMENU,     // 0xAC
    DIAG_BOSE_MORE_SOFTDIAG_HELP,          // 0xAD
    DIAG_VAL_AVAIL_AE,           // 0xAE
    DIAG_BOSE_INTERNET,              // 0xAF

    DIAG_VAL_AVAIL_B0,           // 0xB0
    DIAG_VAL_AVAIL_B1,           // 0xB1
    DIAG_BOSE_CLOSED_CAPTION,        // 0xB2
    DIAG_BOSE_SOUND_TOUCH_SOURCE,    // 0xB3
    DIAG_VAL_AVAIL_B4,           // 0xB4
    DIAG_VAL_AVAIL_B5,           // 0xB5
    DIAG_BOSE_DVR,                   // 0xB6
    DIAG_BOSE_TELETEXT,              // 0xB7
    DIAG_VAL_AVAIL_B8,           // 0xB8
    DIAG_VAL_AVAIL_B9,           // 0xB9
    DIAG_VAL_AVAIL_BA,           // 0xBA
    DIAG_VAL_AVAIL_BB,           // 0xBB
    DIAG_VAL_AVAIL_BC,           // 0xBC
    DIAG_VAL_AVAIL_BD,           // 0xBD
    DIAG_VAL_AVAIL_BE,           // 0xBE
    DIAG_VAL_AVAIL_BF,           // 0xBF

    DIAG_BOSE_UP_ARROW,              // 0xC0
    DIAG_BOSE_I_TV,                  // 0xC1
    DIAG_VAL_AVAIL_C2,           // 0xC2
    DIAG_VAL_AVAIL_C3,           // 0xC3
    DIAG_VAL_AVAIL_C4,           // 0xC4
    DIAG_BOSE_RED,                   // 0xC5
    DIAG_BOSE_GREEN,                 // 0xC6
    DIAG_BOSE_YELLOW,                // 0xC7
    DIAG_BOSE_BLUE,                  // 0xC8
    DIAG_VAL_AVAIL_C9,           // 0xC9
    DIAG_VAL_AVAIL_CA,           // 0xCA
    DIAG_VAL_AVAIL_CB,           // 0xCB
    DIAG_BOSE_ASSERT_OFF,            // 0xCC
    DIAG_BOSE_ASSERT_UNMUTE,         // 0xCD
    DIAG_VAL_AVAIL_CE,           // 0xCE
    DIAG_VAL_AVAIL_CF,           // 0xCF

    DIAG_BOSE_EPG,                   // 0xD0
    DIAG_VAL_AVAIL_D1,           // 0xD1
    DIAG_VAL_AVAIL_D2,           // 0xD2
    DIAG_VAL_AVAIL_D3,           // 0xD3
    DIAG_VAL_AVAIL_D4,           // 0xD4
    DIAG_VAL_AVAIL_D5,           // 0xD5
    DIAG_VAL_AVAIL_D6,           // 0xD6
    DIAG_VAL_AVAIL_D7,           // 0xD7
    DIAG_VAL_AVAIL_D8,           // 0xD8
    DIAG_VAL_AVAIL_D9,           // 0xD9
    DIAG_VAL_AVAIL_DA,           // 0xDA
    DIAG_VAL_AVAIL_DB,           // 0xDB
    DIAG_BOSE_ASPECT_RATIO,          // 0xDC
    DIAG_VAL_AVAIL_DD,           // 0xDD
    DIAG_VAL_AVAIL_DE,           // 0xDE
    DIAG_VAL_AVAIL_DF,           // 0xDF

    DIAG_BOSE_ENTER,                 // 0xE0
    DIAG_VAL_AVAIL_E1,           // 0xE1
    DIAG_VAL_AVAIL_E2,           // 0xE2
    DIAG_VAL_AVAIL_E3,           // 0xE3
    DIAG_VAL_AVAIL_E4,           // 0xE4
    DIAG_VAL_AVAIL_E5,           // 0xE5
    DIAG_VAL_AVAIL_E6,           // 0xE6
    DIAG_VAL_AVAIL_E7,           // 0xE7
    DIAG_VAL_AVAIL_E8,           // 0xE8
    DIAG_VAL_AVAIL_E9,           // 0xE9
    DIAG_VAL_AVAIL_EA,           // 0xEA
    DIAG_VAL_AVAIL_EB,           // 0xEB
    DIAG_VAL_AVAIL_EC,           // 0xEC
    DIAG_VAL_AVAIL_ED,           // 0xED
    DIAG_VAL_AVAIL_EE,           // 0xEE
    DIAG_VAL_AVAIL_EF,           // 0xEF

    DIAG_BOSE_SPECIAL_FEATURE_3,     // 0xF0
    DIAG_BOSE_RFREMOTE_BACK_TRIGGER, // 0xF1
    DIAG_BOSE_RFREMOTE_REPAIR,       // 0xF2
    DIAG_BOSE_RFREMOTE_PTS,          // 0xF3
    DIAG_BOSE_RFREMOTE_FD,           // 0xF4
    DIAG_VAL_AVAIL_F5,           // 0xF5
    DIAG_VAL_AVAIL_F6,           // 0xF6
    DIAG_BOSE_SPECIAL_FEATURE_4,     // 0xF7
    DIAG_VAL_AVAIL_F8,           // 0xF8
    DIAG_VAL_AVAIL_F9,           // 0xF9
    DIAG_VAL_AVAIL_FA,           // 0xFA
    DIAG_VAL_AVAIL_FB,           // 0xFB
    DIAG_VAL_AVAIL_FC,           // 0xFC
    DIAG_VAL_AVAIL_FD,           // 0xFD
    DIAG_BOSE_ASSERT_MUTE,           // 0xFE
    DIAG_BOSE_INVALID_DIAG,           // 0xFF
    NUM_DIAG_BOSE_DIAGS = DIAG_BOSE_INVALID_DIAG,
} DIAG_VALUE;

#endif //_A4V_DIAGVALUES_H_
