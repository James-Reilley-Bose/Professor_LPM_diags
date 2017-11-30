#include "edidapi.h"
#include "edidparse.h"

/*
===============================================================================
@func EDID_Parse
@brief parses an edid
@param edid edid to parse, assumed to be valid
@return void
===============================================================================
*/
void EDID_Parse(edid_t* edid)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    // First we see what the block types are.
    EDID_ParseEDIDBlocks(edid);

    // Zero this out. We don't know what it could be set to.
    // Do this here (and not in EDID_ParseCEA861DataBlock )
    // because there could be more than one CEA 861 block
    // and the tags all get stored together.
    edid->numCEA861Tags = 0;

    for (uint8_t i = 0; i < edid->numBlocks; i++)
    {
        // We probably only care about this block type.
        // parse out the tag metadata so we can inspect for
        // important difference with our edid.
        if (EDID_CEA861_BLOCK == edid->blockTags[i].tID)
        {
            EDID_ParseCEA861DataBlock(edid, i);
        }
    }

    for (uint8_t i = 0; i < edid->numCEA861Tags; i++)
    {
        EDID_ParseCEA861Tag(edid, i);
    }
}

/*
===============================================================================
@func EDID_ParseEDIDBlocks
@brief pares the cea blocks of an edid
@param edid edid to parse
@return void
===============================================================================
*/
void EDID_ParseEDIDBlocks(edid_t* edid)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    for (uint8_t i = 0; i < edid->numBlocks; i++)
    {
        uint16_t blockindex = i * EDID_BLOCK_SIZE;

        // Block index so you can refernce this block by ID and index easily.
        edid->blockTags[i].tStartIndex = blockindex;

        // All blocks (Per CEA-861-F spec) are 128 Bytes (EDID_BLOCK_SIZE).
        edid->blockTags[i].tLen = EDID_BLOCK_SIZE;

        // The type is either a base block (we use 0 to identify) or the number give in the CEA-861-F spec.
        edid->blockTags[i].tID = (0 == i) ? EDID_BASE_BLOCK : edid->edidData[blockindex];
    }
}

/*
===============================================================================
@func EDID_ParseCEA861DataBlock
@brief parses the CEA861 tags from an edid block
@param edid edid to parse
@param blockIndex index of edid's block to parse
@return void
===============================================================================
*/
void EDID_ParseCEA861DataBlock(edid_t* edid, uint8_t blockIndex)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    // Start at the begining of the arbitray block.
    uint16_t tStartIndex = edid->blockTags[blockIndex].tStartIndex;
    uint8_t* edidData = edid->edidData;

    // The data that follows this point is reserved for DTDs.
    uint16_t tStopIndex = tStartIndex + edidData[tStartIndex + END_OF_RESERVED_DATA_BLOCK];

    uint8_t i = tStartIndex + EDID_CEA861_BLOCK_DATA_BLOCK_OFFSET;
    while (i < tStopIndex)
    {
        // The start of this tag is the start of the data plus our current index.
        // This way the block can be referenced directly from this index.
        edid->cea861Tags[edid->numCEA861Tags].tStartIndex = i;

        // The first 3 bits are for the tag's type.
        edid->cea861Tags[edid->numCEA861Tags].tID = (edidData[i] & 0xE0) >> 5;

        // The last 5 bits are for the payload size (additional bytes).
        edid->cea861Tags[edid->numCEA861Tags].tLen = (edidData[i] & 0x1F) + 1;

        // The next tag is after this payload, aka the length of this tag.
        i += edid->cea861Tags[edid->numCEA861Tags].tLen;

        // Add this tag to the total.
        edid->numCEA861Tags++;
    }
}

/*
===============================================================================
@func EDID_ParseCEA861Tag
@brief parses a CEA861 tag of an edid
@param edid edid to parse
@param tagIndex cea861 tag to parse in the edid
@return void
===============================================================================
*/
void EDID_ParseCEA861Tag(edid_t* edid, uint8_t tagIndex)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    switch (edid->cea861Tags[tagIndex].tID)
    {
        case CEA_TAG_VIDEO_DATA_BLOCK:
            EDID_ParseCEAVideoTag(edid, tagIndex);
            break;
        case CEA_TAG_EXTENDED_TAG_BLOCK:
            EDID_ParseCEAExtendedTag(edid, tagIndex);
            break;
        case CEA_TAG_VENDOR_SPECIFIC_DATA_BLOCK:
            EDID_ParseCEAVendorSpecificTag(edid, tagIndex);
            break;
    }
}

/*
===============================================================================
@func EDID_ParseCEAExtendedTag
@brief parses an extend tag in an edid
@param edid edid to parse
@param tagIndex tag to parse in the edid
@return void
===============================================================================
*/
void EDID_ParseCEAExtendedTag(edid_t* edid, uint8_t tagIndex)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    edid->cea861Tags[tagIndex].tExtendedID = edid->edidData[edid->cea861Tags[tagIndex].tStartIndex + 1];
    switch (edid->cea861Tags[tagIndex].tExtendedID)
    {
        // TODO
        default:
            break;
    }
}

/*
===============================================================================
@func EDID_ParseCEAVendorSpecificTag
@brief parses a vendor specific tag in an edid
@param edid edid to parse
@param tagIndex tag to parse in the edid
@return void
===============================================================================
*/
void EDID_ParseCEAVendorSpecificTag(edid_t* edid, uint8_t tagIndex)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    uint16_t startIndex = edid->cea861Tags[tagIndex].tStartIndex;
    edid->cea861Tags[tagIndex].tIEEERegID = (edid->edidData[startIndex + 3] << 16) | (edid->edidData[startIndex + 2] << 8) | (edid->edidData[startIndex + 1]);

    switch (edid->cea861Tags[tagIndex].tIEEERegID)
    {
        case CEA_TAG_VENDOR_SPECIFIC_HDMI_1_4B:
            EDID_ParseHDMI14BTag(edid, tagIndex);
            break;
        case CEA_TAG_VENDOR_SPECIFIC_HDMI_FORUM:
            EDID_ParseHDMIForumTag(edid, tagIndex);
            break;
    }
}

/*
===============================================================================
@func EDID_ParseHDMI14BTag
@brief
@param edid_t*
@param tagIndex tag to parse in the edid
@return void
===============================================================================
*/
void EDID_ParseHDMI14BTag(edid_t* edid, uint8_t tagIndex)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);

    uint16_t tStartIndex = edid->cea861Tags[tagIndex].tStartIndex;
    edid->cecPhyAddrOffset = tStartIndex + EDID_CEA861_BLOCK_HDMI14B_CEC_OFFSET;
    edid->cecPhyiscalAddress =
        (edid->edidData[tStartIndex + EDID_CEA861_BLOCK_HDMI14B_CEC_OFFSET] << 8) | edid->edidData[tStartIndex + EDID_CEA861_BLOCK_HDMI14B_CEC_OFFSET + 1];
    ;
}

/*
===============================================================================
@func EDID_ParseHDMIForumTag
@brief
@param edid_t*
@param tagIndex tag to parse in the edid
@return void
===============================================================================
*/
void EDID_ParseHDMIForumTag(edid_t* edid, uint8_t tagIndex)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);
}

/*
===============================================================================
@func EDID_ParseCEAVideoTag
@brief
@param edid_t* edid
@param tagIndex tag to parse in the edid
@return void
===============================================================================
*/
void EDID_ParseCEAVideoTag(edid_t* edid, uint8_t tagIndex)
{
    debug_assert(edid != NULL);
    debug_assert(edid->edidData != NULL);
}
