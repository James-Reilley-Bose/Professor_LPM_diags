#include "edidapi.h"
#include "edidmodify.h"
#include <string.h>

static void EDID_ModifyBaseEDIDBlock(edid_t* newEDID, const edid_t* sinkEDID, const edid_t* referenceEDID);
static void EDID_ModifyCEA861Block(edid_t* newEDID, const edid_t* sinkEDID, const edid_t* referenceEDID);
static void EDID_ModifyHDMIForumTag(edid_t* edid);

/*
===============================================================================
@func EDID_Modify
@brief modifies an edid based off a sinkEDID and referenceEDID
@param newEDID the newly created modified edid
@param sinkEDID the sink edid
@param referenceEDID the reference edid
@return void
===============================================================================
*/
void EDID_Modify(edid_t* newEDID, const edid_t* sinkEDID, const edid_t* referenceEDID)
{
    debug_assert(newEDID != NULL);
    debug_assert(newEDID->edidData != NULL);
    debug_assert(sinkEDID != NULL);
    debug_assert(sinkEDID->edidData != NULL);
    debug_assert(referenceEDID != NULL);
    debug_assert(referenceEDID->edidData != NULL);

    // set all data to 0s
    memset(newEDID->edidData, 0, newEDID->numBlocks * EDID_BLOCK_SIZE);

    // Base EDID Block:
    // newEDID will have
    // everything from sinkEDID
    // except:
    //  - vendor info from referenceEDID
    //  - dtds that are in both sinkEDID and referenceEDID will be copied from referenceEDID
    EDID_ModifyBaseEDIDBlock(newEDID, sinkEDID, referenceEDID);

    // CEA861 EDID Block:
    // newEDID will have
    // everything from sinkEDID
    // except:
    //  - datablocks that are in both sinkEDID and referenceEDID will be copied from referenceEDID
    EDID_ModifyCEA861Block(newEDID, sinkEDID, referenceEDID);

    // updates the edid with our new changes
    EDID_Update(newEDID);

    // After the new EDID is parsed, scrub out things this HDMI chip can't do.
    EDID_ModifyHDMIForumTag(newEDID);

    // Update again  
    EDID_Update(newEDID);

    // we should always have a valid edid
    debug_assert(EDID_IsValid(newEDID) == TRUE);
}

/*
===============================================================================
@func EDID_ModifyBaseEDIDBlock
@brief modifies the base edid block based off a sinkEDID and referenceEDID
@param newEDID the newly created modified edid
@param sinkEDID the sink edid
@param referenceEDID the reference edid
@return void
===============================================================================
*/
static void EDID_ModifyBaseEDIDBlock(edid_t* newEDID, const edid_t* sinkEDID, const edid_t* referenceEDID)
{
    debug_assert(newEDID != NULL);
    debug_assert(newEDID->edidData != NULL);
    debug_assert(sinkEDID != NULL);
    debug_assert(sinkEDID->edidData != NULL);
    debug_assert(referenceEDID != NULL);
    debug_assert(referenceEDID->edidData != NULL);

    // copy over first block verbaidum.
    memcpy(newEDID->edidData, sinkEDID->edidData, EDID_BLOCK_SIZE);

    // copy in vendor info from referenceEDID
    memcpy(&newEDID->edidData[EDID_BASE_BLOCK_VENDOR_OFFSET], &referenceEDID->edidData[EDID_BASE_BLOCK_VENDOR_OFFSET], EDID_BASE_BLOCK_VENDOR_SIZE);

    // check to see if both sinkEDID and referenceEDID have the same DTDs
    // if they do, override it with referenceEDID's DTD
    uint8_t DTDOffset[] = EDID_BASE_BLOCK_DTD_OFFSET;
    for (uint8_t i = 0; i < EDID_BASE_BLOCK_DTD_COUNT; i++)
    {
        for (uint8_t j = 0; j < EDID_BASE_BLOCK_DTD_COUNT; j++)
        {
            if (memcmp(&(newEDID->edidData[DTDOffset[i]]), &(referenceEDID->edidData[DTDOffset[j]]), EDID_BASE_BLOCK_DTD_HEADER_SIZE) == 0)
            {
                memcpy(&(newEDID->edidData[DTDOffset[i]]), &(referenceEDID->edidData[DTDOffset[j]]), EDID_BASE_BLOCK_DTD_SIZE);
            }
        }
    }
}

/*
===============================================================================
@func EDID_ModifyCEA861Block
@brief modifies the cea861 edid block based off a sinkEDID and referenceEDID
@param newEDID the newly created modified edid
@param sinkEDID the sink edid
@param referenceEDID the reference edid
@return void
===============================================================================
*/
static void EDID_ModifyCEA861Block(edid_t* newEDID, const edid_t* sinkEDID, const edid_t* referenceEDID)
{
    debug_assert(newEDID != NULL);
    debug_assert(newEDID->edidData != NULL);
    debug_assert(sinkEDID != NULL);
    debug_assert(sinkEDID->edidData != NULL);
    debug_assert(referenceEDID != NULL);
    debug_assert(referenceEDID->edidData != NULL);

    // copy header infomation from sinkEDID
    memcpy(&newEDID->edidData[EDID_CEA861_BLOCK_DEFAULT_OFFSET], &sinkEDID->edidData[EDID_CEA861_BLOCK_DEFAULT_OFFSET], EDID_CEA861_BLOCK_HEADER_SIZE);

    //  An index from the start of the EDIDs 0th byte.
    uint16_t edidDataIndex = EDID_CEA861_BLOCK_DEFAULT_OFFSET + EDID_CEA861_BLOCK_DATA_BLOCK_OFFSET;
    // Keeping track of the size of this current block.
    uint16_t totalBlockSize = EDID_CEA861_BLOCK_DATA_BLOCK_OFFSET;

    // copy all data tags from referenceEDID to newEDID
    for (uint8_t i = 0; i < referenceEDID->numCEA861Tags; i++)
    {
        if ((totalBlockSize + referenceEDID->cea861Tags[i].tLen) < (EDID_BLOCK_SIZE - 1))
        {
            memcpy(&newEDID->edidData[edidDataIndex], &referenceEDID->edidData[referenceEDID->cea861Tags[i].tStartIndex], referenceEDID->cea861Tags[i].tLen);
            edidDataIndex += referenceEDID->cea861Tags[i].tLen;
            totalBlockSize += referenceEDID->cea861Tags[i].tLen;
        }
    }

    // copy all the data tags from sinkEDID to newEDID
    // except:
    //  - if there's a duplicate tag in sinkEDID and referenceEDID, copy from referenceEDID
    for (uint8_t i = 0; i < sinkEDID->numCEA861Tags; i++)
    {
        // the data tag doesn't exist in referenceEDID, so copy from sinkEDID
        if ((EDID_HaveDuplicateTags(sinkEDID, i, referenceEDID) == FALSE) && ((totalBlockSize + sinkEDID->cea861Tags[i].tLen) < (EDID_BLOCK_SIZE - 1)))
        {
            memcpy(&newEDID->edidData[edidDataIndex], &sinkEDID->edidData[sinkEDID->cea861Tags[i].tStartIndex], sinkEDID->cea861Tags[i].tLen);
            edidDataIndex += sinkEDID->cea861Tags[i].tLen;
            totalBlockSize += sinkEDID->cea861Tags[i].tLen;
        }
    }

    debug_assert(totalBlockSize < EDID_BLOCK_SIZE - 1);

    // update DTD Offset
    newEDID->edidData[EDID_CEA861_BLOCK_DEFAULT_OFFSET + EDID_CEA861_BLOCK_DTD_OFFSET_BYTE_OFFSET] = totalBlockSize;

    // TODO copy over DTDs
    // last 4 bites = total number of native DTDs
    uint8_t DTDNativeCount = newEDID->edidData[EDID_CEA861_BLOCK_DEFAULT_OFFSET + EDID_CEA861_BLOCK_DTD_NATIVE_COUNT_OFFSET];
    newEDID->edidData[EDID_CEA861_BLOCK_DEFAULT_OFFSET + EDID_CEA861_BLOCK_DTD_NATIVE_COUNT_OFFSET] = (DTDNativeCount & 0xF0) | (0x00);
}

/*
===============================================================================
@func EDID_ModifyHDMIForumTag
@brief Given an EDID, scrub out HF VSB capabilites we can't do with this HDMI chip
@param edid_t* edid
@return void
===============================================================================
*/
static void EDID_ModifyHDMIForumTag(edid_t* edid)
{
    // Modify the HFVSB data blocks RR_Enable bit since the FW can't pass the CTS test.
    for (uint8_t i = 0; i < edid->numCEA861Tags; i++)
    {
        if (EDID_IsTagHFVendorBlock(edid, i))
        {
            // Pick off the RR_Enabled bit from byte 6, 6th bit
            edid->edidData[(edid->cea861Tags[i].tStartIndex) + RR_ENABLED_OFFSET] &= ~RR_ENABLED_CLEAR_MASK;
        }
    } 
}

/*
===============================================================================
@func EDID_ModifyCEA861Block
@brief update the checksum for each edid block
@param edid edid to modify
@return void
===============================================================================
*/
void EDID_ModifyCheckSum(edid_t* edid)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    // check to see if each block's checksum is valid
    for (uint8_t i = 0; i < edid->numBlocks; i++)
    {
        uint8_t sum = 0;
        for (uint8_t j = 0; j < edid->blockTags[i].tLen - 1; j++)
        {
            sum += edid->edidData[edid->blockTags[i].tStartIndex + j];
        }
        edid->edidData[(i + 1) * EDID_BLOCK_SIZE - 1] = (0x100 - sum) % 0x100;
    }
}

/*
===============================================================================
@func EDID_ModifyCECAddress
@brief updates the cec address an in edid
@param edid edid to modify
@param sourceAddress address to set
@return void
===============================================================================
*/

void EDID_ModifyCECAddress(edid_t* edid, uint16_t sourceAddress)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    edid->edidData[edid->cecPhyAddrOffset] = (sourceAddress & 0xFF00) >> 8;
    edid->edidData[edid->cecPhyAddrOffset + 1] = (sourceAddress & 0x00FF);

    EDID_ModifyCheckSum(edid);
}
