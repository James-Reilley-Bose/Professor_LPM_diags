#ifndef HEX_PARSE_H
#define HEX_PARSE_H

/*
 * This is a simple parser for Intel Hex files living in nvram
 * Because Intel Hex stores data big-endian and LPM prefers little, may
 *  have to do some byte swapping for the data field. Address is swapped
 *  automatically.
 *
 * 1. Create a HexParser_t and HexRecord_t
 * 2. set addr of HexParser
 * 3. call NextRecord until an error is returned or you are done.
*/

#define HEX_PARSE_FLIP_ADDRESS_BYTES
#define HEX_PARSE_RECORD_MAX_DATA_LEN 64
#define HEX_PARSE_RECORD_TYPE_DATA 0
#define HEX_PARSE_RECORD_TYPE_EOF 1
#define HEX_PARSE_RECORD_TYPE_EXTENDED_SEG_ADDR 2
#define HEX_PARSE_RECORD_TYPE_START_SEG_ADDR 3
#define HEX_PARSE_RECORD_TYPE_EXTENDED_LINEAR_ADDR 4
#define HEX_PARSE_RECORD_TYPE_START_LINEAR_ADDR 5

typedef enum
{
    HEX_PARSE_NO_ERROR = 0,
    HEX_PARSE_ERROR_INVALID_CHECKSUM,
    HEX_PARSE_ERROR_EOF,
    HEX_PARSE_ERROR_INVALID_HEX_FILE,
} HexParse_Error_t;

/*
 * This structure is packed so data can be copied directly into
 *  it rather than field-by-field. Otherwise padding bytes cause it to
 *  not line up
*/
typedef __packed struct
{
    uint8_t lenData;
    uint16_t addr;
    uint8_t type;
    uint8_t data[HEX_PARSE_RECORD_MAX_DATA_LEN];
    uint8_t checksum;
} HexRecord_t;

typedef struct
{
    uint32_t addr;
    uint8_t hexbuf[HEX_PARSE_RECORD_MAX_DATA_LEN*2];
} HexParser_t;

HexParse_Error_t HexParse_NextRecord(HexParser_t* hp, HexRecord_t* rec);
void HexParse_SwapEndian(uint8_t* buf, uint16_t len);

#endif