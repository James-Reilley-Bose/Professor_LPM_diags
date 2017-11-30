////////////////////////////////////////////////////////////////////////////////
/// @file          KeyData.h
/// @brief         Defines a key data structure.
/// @author        Dillon Johnson
/// Copyright      2015 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#ifndef _KEYDATA_H_
#define _KEYDATA_H_

#include "platform_settings.h"
#include "RivieraLPM_KeyValues.h"

typedef struct
{
    uint32_t timestamp;
    uint32_t ph_timestamp;
    KEY_VALUE value;
    LPM_KEY_STATE state;
    KEY_GROUP group;
    KEY_PRODUCER producer;
    LPM_KEY_STATE state_before_release;
} KEY_DATA_t;

#endif // _KEYDATA_H_
