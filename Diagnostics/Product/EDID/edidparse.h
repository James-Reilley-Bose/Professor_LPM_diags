#ifndef EDIDPARSE_H
#define EDIDPARSE_H

void EDID_Parse(edid_t* edid);
void EDID_ParseCEA861DataBlock(edid_t* edid, uint8_t blockIndex);
void EDID_ParseCEA861Tag(edid_t* edid, uint8_t tagIndex);
void EDID_ParseCEAExtendedTag(edid_t* edid, uint8_t tagIndex);
void EDID_ParseCEAVendorSpecificTag(edid_t* edid, uint8_t tagIndex);
void EDID_ParseCEAVideoTag(edid_t* edid, uint8_t tagIndex);
void EDID_ParseEDIDBlocks(edid_t* edid);
void EDID_ParseHDMI14BTag(edid_t* edid, uint8_t tagIndex);
void EDID_ParseHDMIForumTag(edid_t* edid, uint8_t tagIndex);

#endif
