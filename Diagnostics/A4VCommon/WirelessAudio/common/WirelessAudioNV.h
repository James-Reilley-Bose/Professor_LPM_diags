//
// WirelessAudioNV.h
//

#ifndef WIRELESS_AUDIO_NV_H
#define WIRELESS_AUDIO_NV_H

#include "WirelessAudioData.h"

typedef struct 
{
    uint8_t  flags;
    uint8_t  PDDCSetting;
    uint8_t  T500Setting;
    uint16_t band24Mask;
    uint16_t band52Mask;
    uint16_t band58Mask;
} WA_NVParams_t;

#define WA_BAND_NONE_BIT_MASK 0
#define WA_BAND_24_BIT_MASK (1 << 2)
#define WA_BAND_52_BIT_MASK (1 << 1)
#define WA_BAND_58_BIT_MASK (1 << 0)
#define WA_BAND_24_52_BITMASK (WA_BAND_24_BIT_MASK | WA_BAND_52_BIT_MASK)
#define WA_BAND_24_58_BITMASK (WA_BAND_24_BIT_MASK | WA_BAND_58_BIT_MASK)
#define WA_BAND_52_58_BITMASK (WA_BAND_52_BIT_MASK | WA_BAND_58_BIT_MASK)
#define WA_BAND_TRIBAND_BITMASK (WA_BAND_24_BIT_MASK | WA_BAND_52_BIT_MASK | WA_BAND_58_BIT_MASK)

#define WA_BAND_24_LOWER_LIMIT 24
#define WA_BAND_24_UPPER_LIMIT 25
#define WA_BAND_52_LOWER_LIMIT 51
#define WA_BAND_52_UPPER_LIMIT 52
#define WA_BAND_58_LOWER_LIMIT 57
#define WA_BAND_58_UPPER_LIMIT 58

#define WANV_FLAGS_PDDC (1 << 7)
#define WANV_FLAGS_T500 (1 << 6)
#define WANV_FLAGS_LOCALE (1 << 5)

#endif // WIRELESS_AUDIO_NV_H
