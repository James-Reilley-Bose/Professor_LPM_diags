/*
  File  : edidapi.h
  Title : EDID API
  Author  : Joe Dzado
  Created : 05/08/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        API for dealing with EDIDs that conform to the CEA-861-F and HDMI 2.0a specs.
===============================================================================
*/
#ifndef EDID_API_H
#define EDID_API_H

#include "ediddefs.h"
#include "project.h"

#define NUM_CEA_TAGS 20
#define MAX_EDID_BLOCK_DATA 10
#define SINK_MUTEX_WAIT 100

#define SHIFT_AUDIO_BLOCK_SIZE 5
#define SHIFT_SHORT_AUDIO_BYTE_1 3

#define MAX_AUDIO_DESCRIPTORS 10
#define SPKR_ALLOCATION_TAG 0x83, 0x00, 0x00, 0x00
#define NO_OPTIONS 0


// Converts raw frequency/block-size data for the codec into the value the audio data block expects
// (divided by 8)
#define RATE_KHZ_TO_AUDIO_DESCRIPTOR(a) a/8

typedef struct
{
    uint8_t tID;
    uint8_t tExtendedID;
    uint32_t tIEEERegID;
    uint8_t tLen;
    uint16_t tStartIndex;
} tag_t;

typedef struct
{
    uint8_t* edidData;
    uint8_t numBlocks;
    tag_t blockTags[MAX_EDID_BLOCK_DATA];
    tag_t cea861Tags[NUM_CEA_TAGS];
    uint8_t numCEA861Tags;
    uint16_t cecPhyiscalAddress;
    uint16_t cecPhyAddrOffset;
} edid_t;

typedef struct
{
    uint8_t padding1: 1;
    uint8_t audioFormatCode: 4;
    uint8_t maxNumChannels: 3;

    uint8_t padding2: 1;
    uint8_t supportedRates: 7;
    uint8_t options; //Padding before options depends on the id.
} ShortAudioDescriptor_t;

typedef struct
{
    uint8_t tagType: 3;
    uint8_t numShortAudioDescriptors: 5;
    ShortAudioDescriptor_t audioDescriptors[MAX_AUDIO_DESCRIPTORS];
} audioDataBlock_t;

BOOL EDID_ApplyUpstream(edid_t* edid);
BOOL EDID_Compare(const edid_t* a, const edid_t* b);
BOOL EDID_HaveDuplicateTags(const edid_t* edidA, uint8_t tagIndex, const edid_t* edidB);
BOOL EDID_IsTagHFVendorBlock(const edid_t* edidA, uint8_t tagIndex);
BOOL EDID_IsValid(const edid_t* edid);
BOOL EDID_SetCurrentSinkEdid(uint8_t* newTVEdid, uint8_t numBlocks);
edid_t* EDID_GetA4VEdid(void);
edid_t* EDID_GetCurrentModifiedEdid(void);
BOOL EDID_GetCurrentSinkEdid(edid_t* edid);
void EDID_Init(void);
void EDID_InitializeEDIDStruct(edid_t* edidDst, const uint8_t* rawEDIDData, uint8_t numBlocks);
void EDID_PrintTagDataToLog(edid_t* edid);
void EDID_PrintToLog(edid_t* edid);
void EDID_PrintToTAP(edid_t* edid);
void EDID_Update(edid_t* edid);
void EDID_Destroy(edid_t* toDestroy);
void EDID_DestroySinkEDID(void);
BOOL EDID_AddAudioCodec(uint8_t formatType, uint8_t maxChannels, uint8_t supportedRates, uint8_t options);
void EDID_SetSpkrAllocation(uint8_t spkrAllocation[CEA_SPEAKER_ALLOCATION_SIZE]);
void EDID_BuildRefEDID(edid_t* ref, uint8_t* data, size_t dataSize);
void EDID_PrepRefEDID(void);
audioDataBlock_t EDID_GetSupportedAudioFormats(void);

#endif  // EDID_API_H