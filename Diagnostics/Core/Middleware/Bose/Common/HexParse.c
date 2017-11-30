#include "project.h"
#include "HexParse.h"
//#include "buffermanager.h"
#include "nvram.h"

static BOOL FromAscii(const uint8_t* inBuf, uint8_t* outBuf, uint8_t numCharacters);
static BOOL HexChecksumMatches(HexRecord_t* rec);


/*
 * Reads the next record and increments the working address.
 * 1c colon : 2c len : 4c address : 2c Record type : len*2c data : 2c checksum
*/
HexParse_Error_t HexParse_NextRecord(HexParser_t* hp, HexRecord_t* rec)
{
    // Read record header
    nvram_read(hp->addr+1, 8, hp->hexbuf); // addr + 1 to skip the colon
    if(!FromAscii(hp->hexbuf, (uint8*)rec, 8))
    {
        return HEX_PARSE_ERROR_INVALID_HEX_FILE;
    }
#ifdef HEX_PARSE_FLIP_ADDRESS_BYTES
    // Because intel hex is big endian and IAR uses little, have to swap
    //  bytes. The only 2-byte field is address
    rec->addr = __REV16(rec->addr);
#endif
    
    // Read data section
    nvram_read(hp->addr+9, rec->lenData * 2, hp->hexbuf);
    if(!FromAscii(hp->hexbuf, rec->data, rec->lenData * 2))
    {
        return HEX_PARSE_ERROR_INVALID_HEX_FILE;
    }
    
    // Read the checksum
    nvram_read(hp->addr+9 + rec->lenData*2, 2, hp->hexbuf);
    if(!FromAscii(hp->hexbuf, &rec->checksum, 2))
    {
        return HEX_PARSE_ERROR_INVALID_HEX_FILE;
    }
    // 12 bytes = Colon + header + checksum + newline
    hp->addr += 12 + rec->lenData*2;
    if(! HexChecksumMatches(rec))
    {
        return HEX_PARSE_ERROR_INVALID_CHECKSUM;
    }
    if(rec->type == HEX_PARSE_RECORD_TYPE_EOF)
    {
        return HEX_PARSE_ERROR_EOF;
    }
    return HEX_PARSE_NO_ERROR;
}

void HexParse_SwapEndian(uint8_t* buf, uint16_t len)
{
    uint8_t tmp;
    uint16_t i;
    for(i=0; i < len/2; i++)
    {
        tmp = buf[i];
        buf[i] = buf[len-i-1];
        buf[len-i-1] = tmp;
    }
}

/*
 * Converts ascii characters in the range '0'-'9' or 'A'-'F' to the actual
 *  values they represent. Places the converted values into an output buf.
 * Output buf must be at least half the size fo the input buf, as each 1-byte
 *  character in the inBuf represents a 4-bit hex number
 * 
 * Returns FALSE if the input string has an invalid character
*/
static BOOL FromAscii(const uint8_t* inBuf, uint8_t* outBuf, uint8_t numCharacters)
{
    uint8_t outx, inx;  // outBuf index, inBuf index
    for(outx=0,inx=0; outx < numCharacters/2; outx+=1, inx+=2)
    {
        // Hex character #1
        if(inBuf[inx] >= 'A' && inBuf[inx] <= 'F')
        {
            outBuf[outx] = (inBuf[inx] - 55) << 4;
        }
        else if(inBuf[inx] >= '0' && inBuf[inx] <= '9')
        {
            outBuf[outx] = (inBuf[inx] - '0') << 4;
        }
        else
        {
            return FALSE;
        }

        // Hex character #2
        if(inBuf[inx+1] >= 'A' && inBuf[inx+1] <= 'F')
        {
            outBuf[outx] |= inBuf[inx+1] - 55;
        }
        else if(inBuf[inx+1] >= '0' && inBuf[inx+1] <= '9')
        {
            outBuf[outx] |= inBuf[inx+1] - '0';
        }
        else
        {
            return FALSE;
        }
    }
    
    // odd length string, put last nibble in MSB places
    if(numCharacters % 2 != 0)
    {
        if(inBuf[numCharacters-1] >= 'A' && inBuf[numCharacters-1] <= 'F')
        {
            outBuf[numCharacters/2] = (inBuf[numCharacters-1] - 55) << 4;
        }
        else if(inBuf[numCharacters-1] >= '0' && inBuf[numCharacters-1] <= '9')
        {

            outBuf[numCharacters/2] = (inBuf[numCharacters-1] - '0') << 4;
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}


/*
 * Because the checksum is the 2's complement of the sum of the rest of the
 *  bytes, if everything transmitted correctly the sum of all bytes in packet
 *  will equal 0 (when truncated to 1b)
*/
static BOOL HexChecksumMatches(HexRecord_t* rec)
{
    uint8_t i, sum = 0;
    sum += rec->lenData;
    sum += rec->addr;
    sum += rec->addr >> 8;  // MSB of address
    sum += rec->type;
    for(i = 0; i < rec->lenData; i++)
    {
        sum += rec->data[i];
    }
    sum += rec->checksum;
    return sum == 0;
}
