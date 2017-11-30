#ifndef UEI_KEY_INDEX_H
#define UEI_KEY_INDEX_H

#include "KeyData.h"

typedef enum
{
  UEI_KEY_PRESS,
  UEI_KEY_RELEASE
} UEI_KEY_STATE;

typedef enum
{
    UEI_KEY_PWRTOGGLE                = 0x01,
    UEI_KEY_PWRON                    = 0x02,
    UEI_KEY_PWROFF                   = 0x03,
    UEI_KEY_CHUP                     = 0x04,
    UEI_KEY_CHDN                     = 0x05,
    UEI_KEY_VOLUP                    = 0x06,
    UEI_KEY_VOLDN                    = 0x07,
    UEI_KEY_MUTE                     = 0x08,
    UEI_KEY_DIG1                     = 0x09,
    UEI_KEY_DIG2                     = 0x0A,
    UEI_KEY_DIG3                     = 0x0B,
    UEI_KEY_DIG4                     = 0x0C,
    UEI_KEY_DIG5                     = 0x0D,
    UEI_KEY_DIG6                     = 0x0E,
    UEI_KEY_DIG7                     = 0x0F,
    UEI_KEY_DIG8                     = 0x10,
    UEI_KEY_DIG9                     = 0x11,
    UEI_KEY_DIG0                     = 0x12,
    UEI_KEY_ENTER                    = 0x13,
    UEI_KEY_HUNDRED                  = 0x14,
    UEI_KEY_LASTCH                   = 0x15,
    UEI_KEY_INPUT                    = 0x16,
    UEI_KEY_EXTERNAL                 = 0x17,
    UEI_KEY_PLAY                     = 0x18,
    UEI_KEY_STOP                     = 0x19,
    UEI_KEY_PAUSE                    = 0x1A,
    UEI_KEY_REWIND                   = 0x1B,
    UEI_KEY_FORWARD                  = 0x1C,
    UEI_KEY_RECORD                   = 0x1D,
    UEI_KEY_SKIPFWD                  = 0x1E,
    UEI_KEY_SKIPREV                  = 0x1F,
    UEI_KEY_LIVE                     = 0x20,
    UEI_KEY_MENU                     = 0x21,
    UEI_KEY_SETUPMENU                = 0x22,
    UEI_KEY_GUIDE                    = 0x23,
    UEI_KEY_EXIT                     = 0x24,
    UEI_KEY_BACK                     = 0x25,
    UEI_KEY_UP                       = 0x26,
    UEI_KEY_DOWN                     = 0x27,
    UEI_KEY_LEFT                     = 0x28,
    UEI_KEY_RIGHT                    = 0x29,
    UEI_KEY_OK                       = 0x2A,
    UEI_KEY_PGUP                     = 0x2B,
    UEI_KEY_PGDN                     = 0x2C,
    UEI_KEY_FAVORITE                 = 0x2D,
    UEI_KEY_INFO                     = 0x2E,
    UEI_KEY_FORMAT                   = 0x2F,
    UEI_KEY_SAP                      = 0x30,
    UEI_KEY_SURROUND                 = 0x31,
    UEI_KEY_SLOW                     = 0x32,
    UEI_KEY_DELIMITER                = 0x33,
    UEI_KEY_RANDOM                   = 0x34,
    UEI_KEY_PIP                      = 0x35,
    UEI_KEY_PIPOFF                   = 0x36,
    UEI_KEY_PIPFRZ                   = 0x37,
    UEI_KEY_PIPSWAP                  = 0x38,
    UEI_KEY_PIPMOVE                  = 0x39,
    UEI_KEY_PIPINPUT                 = 0x3A,
    UEI_KEY_PIPCHUP                  = 0x3B,
    UEI_KEY_PIPCHDN                  = 0x3C,
    UEI_KEY_PIPSPLIT                 = 0x3D,
    UEI_KEY_DISCREETINPUT1           = 0x3E,
    UEI_KEY_DISCREETINPUT2           = 0x3F,
    UEI_KEY_DISCREETINPUT3           = 0x40,
    UEI_KEY_DISCREETINPUT4           = 0x41,
    UEI_KEY_DISCREETINPUT5           = 0x42,
    UEI_KEY_DISCREETINPUT6           = 0x43,
    UEI_KEY_DISCREETINPUT7           = 0x44,
    UEI_KEY_DISCREETINPUT8           = 0x45,
    UEI_KEY_DISCREETINPUT9           = 0x46,
    UEI_KEY_DISCREETINPUT10          = 0x47,
    UEI_KEY_CHANNELLIST              = 0x48,
    UEI_KEY_HDMI1                    = 0x49,
    UEI_KEY_HDMI2                    = 0x4A,
    UEI_KEY_HDMI3                    = 0x4B,
    UEI_KEY_HDMI4                    = 0x4C,
    UEI_KEY_RED                      = 0x4D,
    UEI_KEY_GREEN                    = 0x4E,
    UEI_KEY_YELLOW                   = 0x4F,
    UEI_KEY_BLUE                     = 0x50,
    UEI_KEY_HDMI5                    = 0x51,
    UEI_KEY_A                        = 0x52,
    UEI_KEY_B                        = 0x53,
    UEI_KEY_C                        = 0x54,
    UEI_KEY_D                        = 0x55,
    UEI_KEY_WEBINTERNET              = 0x56,
    UEI_KEY_WIDGETSAPPS              = 0x57,
    UEI_KEY_LINKDEVICE               = 0x58,
    UEI_KEY_3DVIDEO                  = 0x59,
    UEI_KEY_PICTUREMODE              = 0x5A,
    UEI_KEY_SOUNDMODE                = 0x5B,
    UEI_KEY_CLEAR                    = 0x5C,
    UEI_KEY_STANDBY                  = 0x5D,
    UEI_KEY_SLEEP                    = 0x5E,
    UEI_KEY_EJECT                    = 0x5F,
    UEI_KEY_PVRMENU                  = 0x60,
    UEI_KEY_POPUPMENU                = 0x61,
    UEI_KEY_TITLEMENU                = 0x62,
    UEI_KEY_QUICKSKIP                = 0x63,
    UEI_KEY_INSTANTRPLY              = 0x64,
    UEI_KEY_DAYUP                    = 0x65,
    UEI_KEY_DAYDN                    = 0x66,
    UEI_KEY_SUBTITLE                 = 0x67,
    UEI_KEY_RECORDSPD                = 0x68,
    UEI_KEY_RECORDINGS               = 0x69,
    UEI_KEY_TEXT                     = 0x6A,
    UEI_KEY_TEXTOFF                  = 0x6B,
    UEI_KEY_TEXTHOLD                 = 0x6C,
    UEI_KEY_TEXTMIX                  = 0x6D,
    UEI_KEY_TEXTEXP                  = 0x6E,
    UEI_KEY_TIME                     = 0x6F,
    UEI_KEY_TEXTUPDATE               = 0x70,
    UEI_KEY_TEXTINDX                 = 0x71,
    UEI_KEY_TEXTREVEAL               = 0x72,
    UEI_KEY_WHITE                    = 0x73,
    UEI_KEY_PURPLE                   = 0x74,
    UEI_KEY_TVDTV                    = 0x75,
    UEI_KEY_DTV                      = 0x76,
    UEI_KEY_ATV                      = 0x77,
    UEI_KEY_TENDIG                   = 0x78,
    UEI_KEY_HOME                     = 0x79,
    UEI_KEY_INTERACTIVE              = 0x7A,
    UEI_KEY_SERVICES                 = 0x7B,
    UEI_KEY_BLURAY                   = 0x7C,
    UEI_KEY_PC                       = 0x7D,
    UEI_KEY_COMPONENT1               = 0x7E,
    UEI_KEY_COMPONENT2               = 0x7F,
    UEI_KEY_COMPONENT3               = 0x80,
    UEI_KEY_DVI1                     = 0x81,
    UEI_KEY_DVI2                     = 0x82,
    UEI_KEY_GAME                     = 0x83,
    UEI_KEY_AUX2                     = 0x84,
    UEI_KEY_AUX3                     = 0x85,
    UEI_KEY_NETFLIX                  = 0x86,
    UEI_KEY_VUDU                     = 0x87,
    UEI_KEY_HELP                     = 0x88,
    UEI_KEY_RECORDSTOP               = 0x89,
    UEI_KEY_OPTION                   = 0x8A,
    UEI_KEY_TOOLS                    = 0x8B,
    UEI_KEY_SKYPE                    = 0x8C,
    UEI_KEY_PANDORA                  = 0x8D,
    UEI_KEY_YOUTUBE                  = 0x8E,
    UEI_KEY_AUDIODESCRIPTION         = 0x8F,
    UEI_KEY_SCREENSIZE               = 0x90,
    UEI_KEY_BLACKSCREEN              = 0x91,
    UEI_KEY_KEYSTONE                 = 0x92,
    UEI_KEY_KEYSTONEUP               = 0x93,
    UEI_KEY_KEYSTONEDOWN             = 0x94,
    UEI_KEY_ZOOM                     = 0x95,
    UEI_KEY_ZOOMIN                   = 0x96,
    UEI_KEY_ZOOMOUT                  = 0x97,
    UEI_KEY_FOCUS                    = 0x98,
    UEI_KEY_FOCUSUP                  = 0x99,
    UEI_KEY_FOCUSDOWN                = 0x9A,
    UEI_KEY_PICTUREFREEZE            = 0x9B,
    UEI_KEY_AUTOADJUST               = 0x9C,
    UEI_KEY_PLAY_PAUSE               = 0x9D,
    OEM_SPECIFIC_KEY_1               = 0x9E,
    OEM_SPECIFIC_KEY_2               = 0x9F,
    OEM_SPECIFIC_KEY_3               = 0xA0,
    UEI_KEY_AMAZON                   = 0xA1,
    UEI_KEY_IPOD                     = 0xA2,
    UEI_KEY_ROKU                     = 0xA3,
    UEI_KEY_YAHOO                    = 0xA4,
    UEI_KEY_HULU                     = 0xA5,
    UEI_KEY_SPOTIFY                  = 0xA6,
    UEI_KEY_RESERVED24               = 0xA7,
    UEI_KEY_RESERVED25               = 0xA8,
    UEI_KEY_RESERVED26               = 0xA9,
    UEI_KEY_RESERVED27               = 0xAA,
    UEI_KEY_RESERVED28               = 0xAB,
    UEI_KEY_RESERVED29               = 0xAC,
    UEI_KEY_RESERVED30               = 0xAD,
    UEI_KEY_RESERVED31               = 0xAE,
    UEI_KEY_RESERVED32               = 0xAF,
    UEI_KEY_SD                       = 0xB0,
    UEI_KEY_DIGIT11                  = 0xB1,
    UEI_KEY_DIGIT12                  = 0xB2,
    UEI_KEY_DATA                     = 0xB3,
    UEI_KEY_ANALOGTERREST            = 0xB4,
    UEI_KEY_DIGITALTERREST           = 0xB5,
    UEI_KEY_BROADCASTSAT             = 0xB6,
    UEI_KEY_COMSAT                   = 0xB7,
    UEI_KEY_TUNERSELECT              = 0xB8,
    UEI_KEY_ANALOG1                  = 0xB9,
    UEI_KEY_ANALOG2                  = 0xBA,
    UEI_KEY_ANALOG3                  = 0xBB,
    UEI_KEY_ANALOG4                  = 0xBC,
    UEI_KEY_ANALOG5                  = 0xBD,
    UEI_KEY_ANALOG6                  = 0xBE,
    UEI_KEY_ANALOG7                  = 0xBF,
    UEI_KEY_ANALOG8                  = 0xC0,
    UEI_KEY_ANALOG9                  = 0xC1,
    UEI_KEY_ANALOG10                 = 0xC2,
    UEI_KEY_ANALOG11                 = 0xC3,
    UEI_KEY_ANALOG12                 = 0xC4,
    UEI_KEY_DIGITAL1                 = 0xC5,
    UEI_KEY_DIGITAL2                 = 0xC6,
    UEI_KEY_DIGITAL3                 = 0xC7,
    UEI_KEY_DIGITAL4                 = 0xC8,
    UEI_KEY_DIGITAL5                 = 0xC9,
    UEI_KEY_DIGITAL6                 = 0xCA,
    UEI_KEY_DIGITAL7                 = 0xCB,
    UEI_KEY_DIGITAL8                 = 0xCC,
    UEI_KEY_DIGITAL9                 = 0xCD,
    UEI_KEY_DIGITAL10                = 0xCE,
    UEI_KEY_DIGITAL11                = 0xCF,
    UEI_KEY_DIGITAL12                = 0xD0,
    UEI_KEY_BROADCASTSAT1            = 0xD1,
    UEI_KEY_BROADCASTSAT2            = 0xD2,
    UEI_KEY_BROADCASTSAT3            = 0xD3,
    UEI_KEY_BROADCASTSAT4            = 0xD4,
    UEI_KEY_BROADCASTSAT5            = 0xD5,
    UEI_KEY_BROADCASTSAT6            = 0xD6,
    UEI_KEY_BROADCASTSAT7            = 0xD7,
    UEI_KEY_BROADCASTSAT8            = 0xD8,
    UEI_KEY_BROADCASTSAT9            = 0xD9,
    UEI_KEY_BROADCASTSAT10           = 0xDA,
    UEI_KEY_BROADCASTSAT11           = 0xDB,
    UEI_KEY_BROADCASTSAT12           = 0xDC,
    UEI_KEY_COMMSAT1                 = 0xDD,
    UEI_KEY_COMMSAT2                 = 0xDE,
    UEI_KEY_COMMSAT3                 = 0xDF,
    UEI_KEY_COMMSAT4                 = 0xE0,
    UEI_KEY_COMMSAT5                 = 0xE1,
    UEI_KEY_COMMSAT6                 = 0xE2,
    UEI_KEY_COMMSAT7                 = 0xE3,
    UEI_KEY_COMMSAT8                 = 0xE4,
    UEI_KEY_COMMSAT9                 = 0xE5,
    UEI_KEY_COMMSAT10                = 0xE6,
    UEI_KEY_COMMSAT11                = 0xE7,
    UEI_KEY_COMMSAT12                = 0xE8,
    UEI_KEY_PROGRAMINFO              = 0xE9,
    UEI_KEY_PROGRAMNAVIGATION        = 0xEA,
    UEI_KEY_VIEWRECSCHEDULES         = 0xEB,
    UEI_KEY_DELETE                   = 0xEC,
    UEI_KEY_ROTATERIGHT              = 0xED,
    UEI_KEY_ROTATELEFT               = 0xEE,
    UEI_KEY_ROTATECENTER             = 0xEF,
    UEI_KEY_SKYPE1                   = 0xF0,
    UEI_KEY_TIMESLIP                 = 0xF1,
    UEI_KEY_RADIO                    = 0xF2,
    UEI_KEY_COUNTERPROGRAM           = 0xF3,
    UEI_KEY_CHAPTERMARK              = 0xF4,
    UEI_KEY_AMBIGUOUS                = 0xFE,   // This is added to handle keys that have multiple meanings
    UEI_KEY_INVALID                  = 0xFF,
} UEI_KEY_CODE;



// UEI to Bose key special translation definitions.  These definitions
// are used dynamically to decide what UEI key to send from an ambiguous
// BOSE key (for example UEI OK key vs. UEI Enter key when Bose Enter 
// remote key is sent.  Max 32 special key definitions.

#define UEI_HAS_ENTER_KEY          (1<<0)
#define UEI_HAS_OK_KEY             (1<<1)
#define UEI_HAS_DVR_KEY            (1<<2)
#define UEI_HAS_RECORDINGS_KEY     (1<<3)
#define UEI_HAS_LEFT_KEY           (1<<4)
#define UEI_HAS_BACK_KEY           (1<<5)
#define UEI_HAS_CHANNELLIST_KEY    (1<<6)
#define UEI_HAS_QUICKSKIP_KEY      (1<<7)
#define UEI_HAS_SKIPFWD_KEY        (1<<8)
#define UEI_HAS_HOME_KEY           (1<<9)
#define UEI_HAS_TITLEMENU_KEY      (1<<10)
#define UEI_HAS_TEXT_KEY           (1<<11)
#define UEI_HAS_SAP_KEY            (1<<12)
#define UEI_HAS_SUBTITLE_KEY       (1<<13)
#define UEI_HAS_SERVICES_KEY       (1<<14)
#define UEI_HAS_MENU_KEY           (1<<15)
#define UEI_HAS_INSTANTRPLY_KEY    (1<<16)
#define UEI_HAS_SKIPREV_KEY        (1<<17)
#define UEI_HAS_WIDGETSAPPS_KEY    (1<<18)
#define UEI_HAS_WEBINTERNET_KEY    (1<<19)
#define UEI_HAS_LASTCH_KEY         (1<<20)
#define UEI_HAS_POPUPMENU_KEY      (1<<21)
#define UEI_HAS_LIVE_KEY           (1<<22)
#define UEI_HAS_DTV_KEY            (1<<23)
#define UEI_HAS_DISCRETE7_KEY      (1<<24)
#define UEI_HAS_DISCRETE_POWER     (1<<25)

// Used to determine whether TV/source should include the key as a more buttons item
#define LIVE_SOFTKEY_BIT           (1LL << 0LL)
#define FAVORITE_SOFTKEY_BIT       (1LL << 1LL)
#define FORMAT_SOFTKEY_BIT         (1LL << 2LL)
#define SAP_SOFTKEY_BIT            (1LL << 3LL)
#define PIP_SOFTKEY_BIT            (1LL << 4LL)
#define PIPOFF_SOFTKEY_BIT         (1LL << 5LL)
#define PANDORA_SOFTKEY_BIT        (1LL << 6LL)
#define YOUTUBE_SOFTKEY_BIT        (1LL << 7LL)
#define ROKU_SOFTKEY_BIT           (1LL << 8LL)
#define YAHOO_SOFTKEY_BIT          (1LL << 9LL)
#define SPOTIFY_SOFTKEY_BIT        (1LL << 10LL)
#define WEBINTERNET_SOFTKEY_BIT    (1LL << 11LL)
#define APPSWIDGETS_SOFTKEY_BIT    (1LL << 12LL)
#define PICTUREMODE_SOFTKEY_BIT    (1LL << 13LL)
#define SOUNDMODE_SOFTKEY_BIT      (1LL << 14LL)
#define CLEAR_SOFTKEY_BIT          (1LL << 15LL)
#define PWRON_SOFTKEY_BIT          (1LL << 16LL)
#define SUBTITLE_SOFTKEY_BIT       (1LL << 17LL)
#define SLOW_SOFTKEY_BIT           (1LL << 18LL)
#define PWROFF_SOFTKEY_BIT         (1LL << 19LL)
#define DELIMITER_SOFTKEY_BIT      (1LL << 20LL)
#define LASTCHANNEL_SOFTKEY_BIT    (1LL << 21LL)
#define BACK_SOFTKEY_BIT           (1LL << 22LL)
#define CHANNELLIST_SOFTKEY_BIT    (1LL << 23LL)
#define EJECT_SOFTKEY_BIT          (1LL << 24LL)
#define QUICKSKIP_SOFTKEY_BIT      (1LL << 25LL)
#define RECORDINGS_SOFTKEY_BIT     (1LL << 26LL)
#define NETFLIX_SOFTKEY_BIT        (1LL << 27LL)
#define AMAZON_SOFTKEY_BIT         (1LL << 28LL)
#define VIDEO3D_SOFTKEY_BIT        (1LL << 29LL)
#define STANDBY_SOFTKEY_BIT        (1LL << 30LL)
#define HULU_SOFTKEY_BIT           (1LL << 31LL)
#define OPTIONS_SOFTKEY_BIT        (1LL << 32LL)
#define TOOLS_SOFTKEY_BIT          (1LL << 33LL)
#define PWRTOGGLE_SOFTKEY_BIT      (1LL << 34LL)
#define POPUPMENU_SOFTKEY_BIT      (1LL << 35LL)
#define TITLEMENU_SOFTKEY_BIT      (1LL << 36LL)
#define HELP_SOFTKEY_BIT           (1LL << 37LL)
// These two aren't uei keys but need to live here so they do
#define BT_PAIRING_SOFTKEY_BIT     (1LL << 38LL)
#define BT_CLEAR_SOFTKEY_BIT       (1LL << 39LL)
#define DISCRETE_PWR_ON            (1LL << 40LL)
#define DISCRETE_PWR_OFF           (1LL << 41LL)




// This is so we can have non uei more buttons
#define BLUETOOTH_MORE_BUTTONS_2 ((uint32_t)((BT_PAIRING_SOFTKEY_BIT | BT_CLEAR_SOFTKEY_BIT) >> 32))
                                            

#endif
