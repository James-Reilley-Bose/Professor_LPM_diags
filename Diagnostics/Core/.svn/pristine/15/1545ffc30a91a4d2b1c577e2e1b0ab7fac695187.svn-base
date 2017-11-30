#ifndef UNIFY_SOURCE_LIST_H
#define UNIFY_SOURCE_LIST_H

#include "IpcProtocolLpm.h"
#include "sourceInterface.h"

#define INVALID_SOURCE_LIST_INDEX 0xFF
#define UEI_KEY_RECORD_SIZE 8
#define MAX_NUM_MORE_KEYS  64

typedef uint8_t UnifySourceIndex;

#pragma pack(1)

#define UNIFY_SRC_LIST_RESERVED_LENGTH 2
typedef struct av_source
{
  char ueiKeyRecordID[UEI_KEY_RECORD_SIZE];
  uint32_t moreButtonsBitMap;
  SOURCE_ID sourceID;
  BOOL      displayInList;
  uint8_t   sourceType;
  uint8_t   sourceTypeSeqNum;
  uint8_t   contentType;
  uint8_t   AVSyncDelay;
  uint8_t   bassLevel;
  uint8_t   trebleLevel;
  uint8_t   audioMode;
  uint32_t  UEItoBoseKeyBitMap;
  uint32_t  moreButtonsBitMap2;
  uint8_t   audioInput;
  uint8_t   audioTrack;
  uint8_t   reserved[UNIFY_SRC_LIST_RESERVED_LENGTH];
}UnifySourceStruct;

typedef struct uei_code_more_buttons_t
{
  uint16_t uei_codeID;
  uint64_t moreButtonsMap;
  uint32_t UEItoBoseKeyBitMap;
}codeKeyStruct;

typedef struct uei_key_map_t
{
  uint8_t key;
  uint64_t bitMask;
}keyMapStruct;
//extern const keyMapStruct moreKeyTable[];

#pragma pack()

uint8_t UI_GetTotalNumSources(void);
uint8_t UITask_GetMaxNumOfUnifySrcs(void);
UnifySourceStruct*  UITask_GetUnifySourceStruct(UnifySourceIndex index);
//const char* UITask_GetUnifySrcName(UnifySourceIndex index);
const char* UITask_GetUnifySrcName(SOURCE_ID id);
UnifySourceIndex UI_GetUnifySourceIndex(SOURCE_ID id);
const UnifySourceStruct* UI_GetUnifyStructForSource(SOURCE_ID id);
const UnifySourceStruct* UI_GetNextUnifyStruct(SOURCE_ID id);
void UITask_SetSourceList(UnifySourceStruct* newlist, uint8_t numSources);
uint8_t UITask_GetNumOfDefaultUnifySrcs(void);
const UnifySourceStruct* UI_GetDefaultSourceList(void);
void UnifySetCurrentAudioProfile(SOURCE_ID src);
const keyMapStruct* UI_GetMoreButtonTableEnrty(uint8_t index);
void UI_SetShowInList(SOURCE_ID src, BOOL show);
void UI_BulkSrcListHandler(GENERIC_MSG_t* msg);
DSPSource_t UnifyGetAudioMuxSetting(SOURCE_ID src);
uint8_t UI_GetNumUnifySources(void);
uint8_t UI_GetNumInternalSources(void);

#endif //UNIFY_SOURCE_LIST_H
