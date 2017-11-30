
////////////////////////////////////////////////////////////////////////////////
/// @file          RivieraLPM_CompressedKeys.c
/// @brief         converts compressed keys to RivieraLPM_KeyValues
/// @author        Ryan Mitchell
/// Copyright      2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "project.h"
#include "RivieraLPM_CompressedKeys.h"

KEY_VALUE IR_ConvertCompressedKey(uint8_t compressedVal)
{
  COMPRESSED_WAVE_KEY keyVal = (COMPRESSED_WAVE_KEY)compressedVal;

  switch(keyVal)
  {
    case COMP_POWER:
      return BOSE_ON_OFF;

    case COMP_SOURCE:
      return BOSE_BLUETOOTH_SOURCE;

    case COMP_PRESET1:
      return BOSE_NUMBER_1;

    case COMP_PRESET2:
      return BOSE_NUMBER_2;

    case COMP_PRESET3:
      return BOSE_NUMBER_3;

    case COMP_PRESET4:
      return BOSE_NUMBER_4;

    case COMP_PRESET5:
      return BOSE_NUMBER_5;

    case COMP_PRESET6:
      return BOSE_NUMBER_6;

    case COMP_VOLUME_UP:
      return BOSE_VOLUME_UP;

    case COMP_VOLUME_DOWN:
      return BOSE_VOLUME_DOWN;

    case COMP_PLAY_PAUSE:
      return BOSE_PLAY;

    case COMP_SKIP_BACK:
      return BOSE_QUICK_REPLAY;

    case COMP_SKIP_FORWARD:
      return BOSE_QUICK_SKIP;

    case COMP_MUTE:
      return BOSE_MUTE;

    case COMP_CONNECT:
      return KEY_VAL_AVAIL_11;

    case COMP_ACTION:
      return KEY_VAL_AVAIL_12;

    case COMP_TV:
      return BOSE_TV_SOURCE;

    case COMP_SOUNDTOUCH:
      return BOSE_SOUND_TOUCH_SOURCE;

    case COMP_THUMB_DOWN:
      return BOSE_PAGE_THUMBS_DOWN;

    case COMP_THUMB_UP:
      return BOSE_PAGE_THUMBS_UP;

    //Key value is not compatable
    default:
      return BOSE_INVALID_KEY;
  }

}
