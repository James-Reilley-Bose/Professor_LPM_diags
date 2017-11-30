/*
  File  : edidapi.c
  Title : EDID API
  Author  : Joe Dzado
  Created : 05/08/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        API for dealing with EDIDs that conform to the CEA-861-F and HDMI 2.0a specs.
===============================================================================
*/
#include "project.h"
#include "edidapi.h"
#include "ediddefs.h"
#include "rottenlog.h"
#include "buffermanager.h"
#include <string.h>
#include "edidparse.h"
#include "edidmodify.h"
#include "A4VEDID.h"
#include "etap.h"
#include "BMutex.h"

SCRIBE(log_edid, ROTTEN_LOGLEVEL_NORMAL);

static edid_t edidCurrentSink = {.edidData = NULL};
static edid_t edidModified = {.edidData = NULL};
static edid_t edidA4V = {.edidData = NULL};
static audioDataBlock_t supportAudioFormats = {.tagType = CEA_TAG_AUDIO_DATA_BLOCK};
static uint8_t supportedSpeakerAllocation[CEA_SPEAKER_ALLOCATION_SIZE] = {SPKR_ALLOCATION_TAG};

static SemaphoreHandle_t currentSinkMutex;

/*
===============================================================================
@func EDID_Init
@brief initalizes edidCurrentSink and edidModified
@param void
@return void
===============================================================================
*/
void EDID_Init(void)
{
    currentSinkMutex = xSemaphoreCreateMutex();
}

/*
===============================================================================
@func EDID_InitializeEDIDStruct
@brief Initializes an edid struct
@param edidDst edid destintation
@param rawEDIDData raw edid data
@param numBlocks number of edid blocks
@return void
===============================================================================
*/
void EDID_InitializeEDIDStruct(edid_t* edidDst, const uint8_t* rawEDIDData, uint8_t numBlocks)
{
    debug_assert(NULL != edidDst);
    // if a buffer has already been taken, free it
    if (edidDst->edidData != NULL)
    {
        BufferManagerFreeBuffer(edidDst->edidData);
    }
    // clear the edid struct
    memset(edidDst, 0, sizeof(edid_t));

    // set the new values
    edidDst->numBlocks = numBlocks;
    edidDst->edidData = BufferManagerGetBuffer(edidDst->numBlocks * EDID_BLOCK_SIZE);
    debug_assert(edidDst->edidData);

    if (NULL != rawEDIDData)
    {
        // Data to use, fill and parse.
        memcpy(edidDst->edidData, rawEDIDData, edidDst->numBlocks * EDID_BLOCK_SIZE);
        EDID_Parse(edidDst);
    }
    else
    {
        // No data, fill with 0s, don't parse.
        memset(edidDst->edidData, 0, edidDst->numBlocks * EDID_BLOCK_SIZE);
    }
}

void EDID_Destroy(edid_t* toDestroy)
{
    BufferManagerFreeBuffer(toDestroy->edidData);
}

/*
===============================================================================
@func EDID_SetCurrentSinkEdid
@brief Set the current sink EDID.
@param newTVEdid raw data for an edid
@param numBlocks number of blocks in the edid
@return BOOL true if new EDID was different. false if it matches the current sink EDID or EDID is invalid.
===============================================================================
*/
BOOL EDID_SetCurrentSinkEdid(uint8_t* newTVEdid, uint8_t numBlocks)
{
    debug_assert(newTVEdid != NULL);
    edid_t edidSinkCandidate = {.edidData = NULL};

    // Initialize temporary EDID, new sink candidate.
    // Allocates a buffer for edidSinkCandidate
    EDID_InitializeEDIDStruct(&edidSinkCandidate, newTVEdid, numBlocks);

    if (FALSE == EDID_IsValid(&edidSinkCandidate))
    {
        LOG(log_edid, ROTTEN_LOGLEVEL_NORMAL, "Received an invalid edid!");
        // Frees the buffer allocated for edidSinkCandidate.
        EDID_Destroy(&edidSinkCandidate);
        return FALSE;
    }

    if (TRUE == EDID_Compare(&edidCurrentSink, &edidSinkCandidate))
    {
        LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "Received an edid that was the same as previously, ignoring it...");
        // Frees the buffer allocated for edidSinkCandidate.
        EDID_Destroy(&edidSinkCandidate);
        return FALSE;
    }

    if (BMutex_Take(currentSinkMutex, SINK_MUTEX_WAIT) != pdFALSE)
    {
        // Deep copy data and number of blocks into current sink EDID.
        EDID_InitializeEDIDStruct(&edidCurrentSink, edidSinkCandidate.edidData, edidSinkCandidate.numBlocks);
        BMutex_Give(currentSinkMutex);
    }
    else
    {
        LOG(log_edid, ROTTEN_LOGLEVEL_NORMAL, "Cannot get mutex to set new sink EDID.");
        EDID_Destroy(&edidSinkCandidate);
        return FALSE;
    }

    EDID_Destroy(&edidSinkCandidate);
    return TRUE;
}

/*
===============================================================================
@func EDID_GetCurrentSinkEdid
@brief Get a pointer to the current sink EDID.
@return pointer to current sink edid
===============================================================================
*/
BOOL EDID_GetCurrentSinkEdid(edid_t* edid)
{
    if (edidCurrentSink.edidData != NULL)
    {
        if (BMutex_Take(currentSinkMutex, SINK_MUTEX_WAIT) != pdFALSE)
        {
            EDID_InitializeEDIDStruct(edid, edidCurrentSink.edidData, edidCurrentSink.numBlocks);
            BMutex_Give(currentSinkMutex);
            return TRUE;
        }
        else
        {
            LOG(log_edid, ROTTEN_LOGLEVEL_NORMAL, "Cannot get mutex to get new sink EDID. ");
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

/*
===============================================================================
@func EDID_GetCurrentModifiedEdid
@brief Get a pointer to the current modified EDID.
@return pointer to current modified edid
===============================================================================
*/
edid_t* EDID_GetCurrentModifiedEdid(void)
{
    return &edidModified;
}

/*
===============================================================================
@func EDID_GetA4VEdid
@brief Get a pointer to the A4V EDID.
@return
===============================================================================
*/
edid_t* EDID_GetA4VEdid(void)
{
    if (NULL == edidA4V.edidData)
    {
        EDID_PrepRefEDID();
    }
    debug_assert(edidA4V.edidData != NULL);
    return &edidA4V;
}

/*
===============================================================================
@func EDID_IsValid
@brief Check to see if the edid is valid according to the CEA spec
@param edid edid to check
@return TRUE if valid FALSE if invalid
===============================================================================
*/
BOOL EDID_IsValid(const edid_t* edid)
{
    if (edid == NULL)
    {
        return FALSE;
    }

    if (edid->edidData == NULL)
    {
        return FALSE;
    }

    // Check for EDID HEADER 00FFFFFFFFFFFF00
    uint8_t EDID_HEADER[] = {EDID_HEADER_BLOCK};
    for (uint8_t i = 0; i < EDID_HEADER_LEN; ++i)
    {
        if (FALSE == (EDID_HEADER[i] == edid->edidData[i]))
        {
            return FALSE;
        }
    }

    // check to see if each block's checksum is valid
    for (uint8_t i = 0; i < edid->numBlocks; i++)
    {
        uint8_t sum = 0;
        for (uint8_t j = 0; j < edid->blockTags[i].tLen; j++)
        {
            sum += edid->edidData[edid->blockTags[i].tStartIndex + j];
        }
        if (0 != sum)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*
===============================================================================
@func EDID_Compare
@brief compare two EDIDs
@param a first edid
@param b second edid
@return TRUE if same FALSE if different
===============================================================================
*/
BOOL EDID_Compare(const edid_t* a, const edid_t* b)
{
    if (a->numBlocks != b->numBlocks)
    {
        return FALSE;
    }
    for (uint16_t i = 0; i < a->numBlocks * EDID_BLOCK_SIZE; i++)
    {
        if (a->edidData[i] != b->edidData[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*
===============================================================================
@func EDID_ApplyUpstream
@brief Send the desired EDID upstream to the source devices.
@param edid edid to apply
@return TRUE if it applied properly to the device FALSE otherwise
===============================================================================
*/
BOOL EDID_ApplyUpstream(edid_t* edid)
{
    return TRUE;
}

/*
===============================================================================
@func EDID_Update
@brief reparses an edid and recalculates the checksum
@param edid edid to update
@return void
===============================================================================
*/
void EDID_Update(edid_t* edid)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    EDID_Parse(edid);
    EDID_ModifyCheckSum(edid);
}

/*
===============================================================================
@func EDID_HaveDuplicateTags
@brief checks to see if the edidA's cea861Tag[tagIndex] is in edidB's cea861Tag[]
@param edidA edid that has a cea861Tag
@param tagIndex tag index of edidA
@param edidB edid to check for cea861Tag
@return TRUE if tag exists in edidB, FALSE otherwise
===============================================================================
*/
BOOL EDID_HaveDuplicateTags(const edid_t* edidA, uint8_t tagIndex, const edid_t* edidB)
{
    for (uint8_t i = 0; i < edidB->numCEA861Tags; i++)
    {
        switch (edidB->cea861Tags[i].tID)
        {
            case CEA_TAG_VENDOR_SPECIFIC_DATA_BLOCK:
                if (edidB->cea861Tags[i].tIEEERegID == edidA->cea861Tags[tagIndex].tIEEERegID)
                {
                    return TRUE;
                }
                break;
            case CEA_TAG_EXTENDED_TAG_BLOCK:
                if (edidB->cea861Tags[i].tExtendedID == edidA->cea861Tags[tagIndex].tExtendedID)
                {
                    return TRUE;
                }
                break;
            default:
                if (edidB->cea861Tags[i].tID == edidA->cea861Tags[tagIndex].tID)
                {
                    return TRUE;
                }
                break;
        }
    }
    return FALSE;
}

/*
===============================================================================
@func EDID_IsTagHFVendorBlock
@brief Checks to see if a particular tag in an EDID is the HF Vendor Specific tag.
@param const edid_t* edidA, uint8_t tagIndex
@return BOOL, TRUE if this is the HFVS tag
===============================================================================
*/
BOOL EDID_IsTagHFVendorBlock(const edid_t* edidA, uint8_t tagIndex)
{
    return (CEA_TAG_VENDOR_SPECIFIC_DATA_BLOCK == edidA->cea861Tags[tagIndex].tID &&
        CEA_TAG_VENDOR_SPECIFIC_HDMI_FORUM == edidA->cea861Tags[tagIndex].tIEEERegID);
}

/*
===============================================================================
@func EDID_PrintToLog
@brief Prints EDID XML format to log
@param edid edid to print
@return void
===============================================================================
*/
void EDID_PrintToLog(edid_t* edid)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=");
    LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "\"no\"?><DATAOBJ>");
    LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "<HEADER TYPE=\"DID\" VERSION=\"1.0\"/><DATA>");
    for (uint8_t i = 0; i < edid->numBlocks; ++i)
    {
        LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "<BLOCK%d>", i);
        uint16_t blockOffset = EDID_BLOCK_SIZE * i;
        for (uint8_t j = 0; j < EDID_BLOCK_SIZE; j += 8)
        {
            LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "%02X%02X%02X%02X%02X%02X%02X%02X", edid->edidData[blockOffset + j + 0], edid->edidData[blockOffset + j + 1],
                edid->edidData[blockOffset + j + 2], edid->edidData[blockOffset + j + 3], edid->edidData[blockOffset + j + 4], edid->edidData[blockOffset + j + 5],
                edid->edidData[blockOffset + j + 6], edid->edidData[blockOffset + j + 7]);
        }
        LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "</BLOCK%d>", i);
    }
    LOG(log_edid, ROTTEN_LOGLEVEL_VERBOSE, "</DATA></DATAOBJ>");
}

/*
===============================================================================
@func EDID_PrintToTAP
@brief Prints EDID XML format to tap
@param edid edid to print
@return void
===============================================================================
*/
void EDID_PrintToTAP(edid_t* edid)
{
    TAP_Printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=" );
    TAP_Printf("\"no\"?><DATAOBJ>");
    TAP_Printf( "<HEADER TYPE=\"DID\" VERSION=\"1.0\"/><DATA>" );
    for ( uint8_t i = 0; i < edid->numBlocks; ++i )
    {
        TAP_Printf("<BLOCK%d>", i );
        uint16_t blockOffset = EDID_BLOCK_SIZE * i;
        for ( uint8_t j = 0; j < EDID_BLOCK_SIZE; j += 8 )
        {
            TAP_Printf("%02X%02X%02X%02X%02X%02X%02X%02X",
                       edid->edidData[ blockOffset + j + 0 ], edid->edidData[ blockOffset + j + 1 ], edid->edidData[ blockOffset + j + 2 ], edid->edidData[ blockOffset + j + 3 ],
                       edid->edidData[ blockOffset + j + 4 ], edid->edidData[ blockOffset + j + 5 ], edid->edidData[ blockOffset + j + 6 ], edid->edidData[ blockOffset + j + 7 ]);
        }
        TAP_Printf("</BLOCK%d>", i );
    }
    TAP_Printf("</DATA></DATAOBJ>" );

}

/*
===============================================================================
@func EDID_PrintTagDataToLog
@brief Pretty prints the edid tag data
@param edid to print
@return void
===============================================================================
*/
void EDID_PrintTagDataToLog(edid_t* edid)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    LOG(log_edid, ROTTEN_LOGLEVEL_INSANE, "Number of EDID Blocks: %d", edid->numBlocks);
    for (uint8_t i = 0; i < edid->numBlocks; ++i)
    {
        LOG(log_edid, ROTTEN_LOGLEVEL_INSANE, "--ID: %d Index: %d Len: %d", edid->blockTags[i].tID, edid->blockTags[i].tStartIndex, edid->blockTags[i].tLen);
    }
    LOG(log_edid, ROTTEN_LOGLEVEL_INSANE, "Number of CEA 861 Block tags: %d", edid->numCEA861Tags);
    for (uint8_t i = 0; i < edid->numCEA861Tags; ++i)
    {
        switch (edid->cea861Tags[i].tID)
        {
            case CEA_TAG_VENDOR_SPECIFIC_DATA_BLOCK:
                LOG(log_edid, ROTTEN_LOGLEVEL_INSANE, "--ID: %d IEEERegID: %06X, Index: %d Len: %d", edid->cea861Tags[i].tID, edid->cea861Tags[i].tIEEERegID,
                    edid->cea861Tags[i].tStartIndex, edid->cea861Tags[i].tLen);
                break;
            case CEA_TAG_EXTENDED_TAG_BLOCK:
                LOG(log_edid, ROTTEN_LOGLEVEL_INSANE, "--ID: %d ExtendedID: %d, Index: %d Len: %d", edid->cea861Tags[i].tID, edid->cea861Tags[i].tExtendedID,
                    edid->cea861Tags[i].tStartIndex, edid->cea861Tags[i].tLen);
                break;
            default:
                LOG(log_edid, ROTTEN_LOGLEVEL_INSANE, "--ID: %d Index: %d Len: %d", edid->cea861Tags[i].tID, edid->cea861Tags[i].tStartIndex, edid->cea861Tags[i].tLen);
                break;
        }
    }
}

/*
===============================================================================
@func EDID_DestroySinkEDID
@brief Start over with a new sink EDID.
===============================================================================
*/
void EDID_DestroySinkEDID(void)
{
    if (edidCurrentSink.edidData)
    {
        EDID_Destroy(&edidCurrentSink);
    }
    memset(&edidCurrentSink, 0, sizeof(edid_t));
}

/*
===============================================================================
@func EDID_AddAudioCodec
@brief Save off audio codecs for the EDID
===============================================================================
*/
BOOL EDID_AddAudioCodec(uint8_t formatType, uint8_t maxChannels, uint8_t supportedRates, uint8_t options)
{
    debug_assert(formatType <= AUDIO_ID_MAX);
    debug_assert(maxChannels <= CHANNELS_MAX);

    if (supportAudioFormats.numShortAudioDescriptors >= MAX_AUDIO_DESCRIPTORS)
    {
        return FALSE;
    }
    supportAudioFormats.audioDescriptors[supportAudioFormats.numShortAudioDescriptors].audioFormatCode = formatType;
    supportAudioFormats.audioDescriptors[supportAudioFormats.numShortAudioDescriptors].maxNumChannels = maxChannels;
    supportAudioFormats.audioDescriptors[supportAudioFormats.numShortAudioDescriptors].supportedRates = supportedRates;
    supportAudioFormats.audioDescriptors[supportAudioFormats.numShortAudioDescriptors].options = options;
    supportAudioFormats.numShortAudioDescriptors++;
    return TRUE;
}

/*
===============================================================================
@func EDID_SetSpkrAllocation
@brief Save off the speaker allocation for the EDID
===============================================================================
*/
void EDID_SetSpkrAllocation(uint8_t spkrAllocation[CEA_SPEAKER_ALLOCATION_SIZE])
{
    supportedSpeakerAllocation[0] = SPKR_ALLOCATION_TAG; // tag goes in first nibble. Second nibble is size, which is fixed in this case.
    supportedSpeakerAllocation[1] = spkrAllocation [1];
    supportedSpeakerAllocation[2] = spkrAllocation [2];
    supportedSpeakerAllocation[3] = spkrAllocation [3];
}

/*
===============================================================================
@func EDID_BuildRefEDID
@brief Take our audio block and speaker allocation block and add those to the
  reference EDID.
===============================================================================
*/
void EDID_BuildRefEDID(edid_t* ref, uint8_t* data, size_t dataSize)
{
    // Bring buffer to life
    uint8_t* refEDIDdata = BufferManagerGetBuffer(dataSize);
    debug_assert(refEDIDdata);

    // Initialize with 0s
    memset(refEDIDdata, 0, (dataSize));

    // Block writing index
    uint16_t writeIndex = 0;

    // Copy block 0 wholesale
    memcpy(&refEDIDdata[writeIndex], &data[writeIndex], EDID_BLOCK_SIZE);
    writeIndex += EDID_BLOCK_SIZE;

    // Copy block 1 header (4 bytes)
    memcpy(&refEDIDdata[writeIndex], &data[writeIndex], EDID_CEA861_BLOCK_HEADER_SIZE);
    writeIndex += EDID_CEA861_BLOCK_HEADER_SIZE;

    // Add the size of the audio data block
    refEDIDdata[writeIndex++] = (CEA_TAG_AUDIO_DATA_BLOCK << SHIFT_AUDIO_BLOCK_SIZE) | (supportAudioFormats.numShortAudioDescriptors * CEA_SHORT_AUDIO_DESCRIPTOR_SIZE);

    // Copy in audio descriptors
    for (uint8_t i = 0; i < supportAudioFormats.numShortAudioDescriptors; ++i)
    {
        refEDIDdata[writeIndex++] = (supportAudioFormats.audioDescriptors[i].audioFormatCode << SHIFT_SHORT_AUDIO_BYTE_1 ) | (supportAudioFormats.audioDescriptors[i].maxNumChannels);
        refEDIDdata[writeIndex++] = (supportAudioFormats.audioDescriptors[i].supportedRates & 0x7F); // First bit is zero.
        refEDIDdata[writeIndex++] = supportAudioFormats.audioDescriptors[i].options;
    }

    // Add speaker allocation
    memcpy(&refEDIDdata[writeIndex], &supportedSpeakerAllocation, CEA_SPEAKER_ALLOCATION_SIZE);
    writeIndex += sizeof(CEA_SPEAKER_ALLOCATION_SIZE);

    // Update DTD reserved byte
    refEDIDdata[EDID_BLOCK_SIZE + END_OF_RESERVED_DATA_BLOCK] = writeIndex - EDID_BLOCK_SIZE;
    EDID_InitializeEDIDStruct(ref, refEDIDdata, A4VEDID_DATA_BLOCKS);

    // Update checksum
    EDID_ModifyCheckSum(ref);

    // Kill buffer
    BufferManagerFreeBuffer(refEDIDdata);
}

/*
===============================================================================
@func EDID_PrepRefEDID
@brief Build the ref EDID and parse it.
===============================================================================
*/
void EDID_PrepRefEDID(void)
{
    // Build reference EDID
    EDID_BuildRefEDID(&edidA4V, (uint8_t[])A4VEDID_DATA, A4VEDID_DATA_BLOCKS * EDID_BLOCK_SIZE);
    // Parse a4v edid
    EDID_Parse(&edidA4V);
}

/*
===============================================================================
@func EDID_GetSupportedAudioFormats
@brief Get a pointer to the current modified EDID.
@return pointer to current modified edid
===============================================================================
*/
audioDataBlock_t EDID_GetSupportedAudioFormats(void)
{
  return supportAudioFormats;
}
