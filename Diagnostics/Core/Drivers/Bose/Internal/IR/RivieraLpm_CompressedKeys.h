
////////////////////////////////////////////////////////////////////////////////
/// @file          RivieraLPM_CompressedKeys.h
/// @brief         define compressed key values
/// @author        Ryan Mitchell
/// Copyright      2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#ifndef RIVIERA_LPM_COMPRESSED_KEYS_H
#define RIVIERA_LPM_COMPRESSED_KEYS_H
#include "RivieraLPM_KeyValues.h"

KEY_VALUE IR_ConvertCompressedKey(uint8_t compressedVal);

typedef enum
{
  COMP_POWER            = 0x32,
  COMP_SOURCE           = 0xF0, //Source BT/Aux
  COMP_PRESET1          = 0xE0,
  COMP_PRESET2          = 0x10,
  COMP_PRESET3          = 0x90,
  COMP_PRESET4          = 0x50,
  COMP_PRESET5          = 0xD0,
  COMP_PRESET6          = 0x30,
  COMP_VOLUME_UP        = 0xC0,
  COMP_VOLUME_DOWN      = 0x40,
  COMP_PLAY_PAUSE       = 0xD8,
  COMP_SKIP_BACK        = 0x18,
  COMP_SKIP_FORWARD     = 0x98,
  COMP_MUTE             = 0x80,
  //TODO - SoundTouch not in red on the ProfessorSimpleRemoteIRKeyProposal excel sheet. So might not be needed?
  COMP_SOUNDTOUCH       = 0xA0,
  COMP_CONNECT          = 0x20,
  COMP_ACTION           = 0x21,
  COMP_TV               = 0xF1,
  COMP_THUMB_DOWN       = 0x25,
  COMP_THUMB_UP         = 0x55,
} COMPRESSED_WAVE_KEY;

#endif //RIVIERA_LPM_COMPRESSED_KEYS_H
