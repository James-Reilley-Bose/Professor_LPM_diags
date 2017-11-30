#include "project.h"
#include "RemoteDefs.h"
#include "RemoteUpdate.h"
#include "RemoteSPI.h"
#include "RemoteDriver.h"
#include "RemoteUtility.h" // todo: get rid of if still dont need
#include "UpdateVariant.h"
#include "UpdateManagerBlob.h"
#include "buffermanager.h"
#include "HexParse.h"
//#include "nvram.h"


static BOOL GetImageAddr(uint32_t* addr);
static BOOL DoProgramming(uint32_t imageAddr);

static BOOL Remote_Update_SendACK(void);
static BOOL Remote_Update_SendNACK(void);
static BOOL Remote_Update_ReadSomeSPI(uint8_t* buf, uint16_t len);
static BOOL WaitForByte(uint8_t* response);
static BOOL WaitForPacket(uint8_t* response, uint8_t expectedLen);
static uint8_t GenerateChecksum(const uint8_t* buf, uint8_t len);
static BOOL SendTxPkt(void);

Remote_Update_Status_t status = REMOTE_UPDATE_STATUS_NOT_UPDATING;
uint8_t rxByte = 0;
// todo: what is the max size of response? just have a buffer of that size
Remote_Update_SPIPacket_t rxPkt;
// todo: does txPkt need to have max size data section?
Remote_Update_SPIPacket_t txPkt;
BOOL Remote_Update_readingSPI = FALSE;
SCRIBE_DECL(remote);
// todo: save space by packing 8 bool per int?
uint8_t sectorHasBeenErased[32] = {0};
//TimerHandle_t Remote_Update_SPIReadTimer = NULL;



Remote_Update_Status_t Remote_Update_Status(void)
{
    return status;
}


Remote_Update_Error_t Remote_Update_Start()
{
    uint32_t imageAddr;
    if(! GetImageAddr(&imageAddr))
    {
        return REMOTE_UPDATE_ERROR_INVALID_BLOB;
    }
    
    if(! DoProgramming(imageAddr))
    {
        return REMOTE_UPDATE_ERROR_FLASH_ERROR;
    }
    
    REMOTE_LOG("Update completed successfully!");
    return REMOTE_UPDATE_ERROR_SUCCESS;
}


static BOOL GetImageAddr(uint32_t* addr)
{
    // Get blob header
    UpdateBlobHeader blobHeader;
    if(!ReadBlobHeaderFromFlash(&blobHeader, BLOB_ADDRESS))
    {
        REMOTE_LOG("ERROR: failure while reading blob header from flash");
        return FALSE;
    }
    // get manifest entry
    ManifestEntry me;
    if(!GetManifestEntryByName(&blobHeader, BLOB_ADDRESS, &me, "remote.hex"))
    {
        REMOTE_LOG("ERROR: failure while reading manifest entry from flash");
        return FALSE;
    }
    *addr = sizeof(UpdateBlobHeader) +
            blobHeader.NumberOfImages * sizeof(ManifestEntry) +
            me.ImageOffset;
    return TRUE;
}


static BOOL DoProgramming(uint32_t imageAddr)
{
    uint8_t i;
    HexParser_t parser;
    parser.addr = imageAddr;
    HexRecord_t record;
    uint32_t address;
    // If this value is non-zero, it will be used for the 2 MSB in each
    //  address. Often this is set mid-programming to reach higher addresses
    uint16_t extendedLinearAddr = 0;
    
    // Erase flash
    if(!Remote_Update_BankErase())
    {
        REMOTE_LOG("Error while erasing flash. Aborting");
        return FALSE;
    }
    
    HexParse_Error_t parseError = HexParse_NextRecord(&parser, &record);
    while(parseError == HEX_PARSE_NO_ERROR)
    {
        if(record.type == HEX_PARSE_RECORD_TYPE_EXTENDED_LINEAR_ADDR)
        {
            // MSB to LSB
            extendedLinearAddr = record.data[1] | record.data[0] << 8;
        }
        
        else if(record.type == HEX_PARSE_RECORD_TYPE_DATA)
        {
            // Add on ExtendedLinearAddress if using
            address = record.addr;
            if(extendedLinearAddr != 0)
            {
                address |= extendedLinearAddr << 16;
            }
            
            REMOTE_LOG("Sending packet for 0x%08X\n", address);
            i = 0;
            do
            {
                i++;
                if(i > REMOTE_UPDATE_NUMBER_OF_WRITE_TRIES)
                {
                    REMOTE_LOG("Unable to send packet. Aborting update");
                    return FALSE;
                }
                Remote_Update_Download(address, record.lenData);
                Remote_Update_SendData(record.data, record.lenData);
                Remote_Update_GetStatus((uint8_t*)&rxPkt);
            } while(rxPkt.data[0] != REMOTE_UPDATE_STATUS_SUCCESS);
            
            if(rxPkt.data[0] != REMOTE_UPDATE_STATUS_SUCCESS)
            {
                REMOTE_LOG("Data send resulted in an error");
                return FALSE;
            }
        }
        
        parseError = HexParse_NextRecord(&parser, &record);
    }
    if(parseError == HEX_PARSE_ERROR_INVALID_CHECKSUM)
    {
        REMOTE_LOG("Invalid checksum");
        return FALSE;
    }
    if(parseError == HEX_PARSE_ERROR_INVALID_HEX_FILE)
    {
        REMOTE_LOG("Hex file is invalid");
        return FALSE;
    }
    return TRUE;
}


BOOL Remote_Update_GetStatus(uint8_t* response)
{
    txPkt.pktSize = 3;
    txPkt.checksum = REMOTE_UPDATE_COMMAND_GET_STATUS;
    txPkt.data[0] = REMOTE_UPDATE_COMMAND_GET_STATUS;
    BOOL gotAck = SendTxPkt();
    if(gotAck)
    {
        BOOL gotResponse = WaitForPacket(response, 3);
        if(gotResponse)
        {
            Remote_Update_SendACK();
            return TRUE;
        }
        else
        {
            Remote_Update_SendNACK();
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}


/*
 * Prepares for a SendData command. Download should be followed by a
 *  GetStatus to make sure the address and size are valid
*/
BOOL Remote_Update_Download(uint32_t address, uint32_t size)
{
    txPkt.pktSize = 11;
    txPkt.data[0] = REMOTE_UPDATE_COMMAND_DOWNLOAD;
    txPkt.data[1] = address >> 24;
    txPkt.data[2] = address >> 16;
    txPkt.data[3] = address >> 8;
    txPkt.data[4] = address;
    txPkt.data[5] = size >> 24;
    txPkt.data[6] = size >> 16;
    txPkt.data[7] = size >> 8;
    txPkt.data[8] = size;
    txPkt.checksum = GenerateChecksum(txPkt.data, 9);
    return SendTxPkt();
}
/*
 * Send data to be programmed into flash. Should be followed by a
 *  GetStatus pkt.
*/
BOOL Remote_Update_SendData(void* data, uint8_t len)
{
    if(len > REMOTE_UPDATE_PACKET_DATA_LEN)
    {
        return FALSE;
    }
    txPkt.pktSize = 3 + len;
    txPkt.data[0] = REMOTE_UPDATE_COMMAND_SEND_DATA;
    memcpy(&txPkt.data[1], data, len);
    txPkt.checksum = GenerateChecksum(txPkt.data, txPkt.pktSize-2);
    return SendTxPkt();
}

/*
BOOL Remote_Update_EraseSector(uint32_t address)
{
    txPkt.pktSize = 7;
    txPkt.data[0] = REMOTE_UPDATE_COMMAND_SECTOR_ERASE;
    txPkt.data[1] = address >> 24;
    txPkt.data[2] = address >> 16;
    txPkt.data[3] = address >> 8;
    txPkt.data[4] = address;
    txPkt.checksum = generateChecksum(txPkt.data, 5);
    return sendTxPkt();
}
*/
BOOL Remote_Update_BankErase()
{
    txPkt.pktSize = 3;
    txPkt.checksum = REMOTE_UPDATE_COMMAND_BANK_ERASE;
    txPkt.data[0] = REMOTE_UPDATE_COMMAND_BANK_ERASE;
    return SendTxPkt();
}

BOOL Remote_Update_Ping(void)
{
    txPkt.pktSize = 3;
    txPkt.checksum = REMOTE_UPDATE_COMMAND_PING;
    txPkt.data[0] = REMOTE_UPDATE_COMMAND_PING;
    BOOL gotAck = SendTxPkt();
    if(gotAck)
    {
        REMOTE_LOG("Ping received ACK byte");
        return TRUE;
    }
    return FALSE;
}


/*
 * Generates a checksum from a buffer. Adds together each byte in the
 *  buffer, and truncates the result to one byte. Returns the truncated
 *  result.
*/
static uint8_t GenerateChecksum(const uint8_t* buf, uint8_t len)
{
    uint8_t i; // max data[] size is 253 bytes
    uint8_t total = 0; // truncated to 1 byte
    for(i=0; i < len; i++)
    {
        total += buf[i];
    }
    return total;
}


/*
 * Wait for a ACK or NACK byte from bl. Will return false after
 *  REMOTE_UPDATE_SPI_TIMEOUT_MS
*/
static BOOL WaitForByte(uint8_t* response)
{
    uint32_t currentTime = GET_SYSTEM_UPTIME_MS();
    uint32_t endTime = currentTime + REMOTE_UPDATE_SPI_TIMEOUT_MS;
    while(currentTime < endTime)
    {
        if(Remote_Update_ReadSomeSPI(response, 1))
        {
            if(*response != REMOTE_UPDATE_BYTE_ACK &&
               *response != REMOTE_UPDATE_BYTE_NACK)
            {
                return FALSE;
            }
            return TRUE;
        }
        vTaskDelay(TIMER_MSEC_TO_TICKS(10)); // todo: remove?
        currentTime = GET_SYSTEM_UPTIME_MS();
    }
    return FALSE;
}

/*
 * Wait for a packet of expectedLen length
*/
static BOOL WaitForPacket(uint8_t* response, uint8_t expectedLen)
{
    uint32_t currentTime = GET_SYSTEM_UPTIME_MS();
    uint32_t endTime = currentTime + REMOTE_UPDATE_SPI_TIMEOUT_MS;
    while(currentTime < endTime)
    {
        if(Remote_Update_ReadSomeSPI(response, expectedLen))
        {
            if(response[0] != expectedLen)
            {
                return FALSE;
            }
            uint8_t checksum = GenerateChecksum(&response[2], response[0]-2);
            if(response[1] != checksum)
            {
                return FALSE;
            }
            return TRUE;
        }
        vTaskDelay(TIMER_MSEC_TO_TICKS(10)); // todo: remove?
        currentTime = GET_SYSTEM_UPTIME_MS();
    }
    return FALSE;
}


/*
 * Checks REMOTE_UPDATE_SPI_READ_SIZE data from SPI. If nonzero data is found,
 *  the next *len* bytes will be read from SPI into buf
 * Returns TRUE if data is in buf without error. FALSE otherwise
*/
static BOOL Remote_Update_ReadSomeSPI(uint8_t* buf, uint16_t len)
{
    BOOL validRead;
    uint8_t bytesOfDataRead = 0;
    for(; bytesOfDataRead < REMOTE_UPDATE_SPI_READ_SIZE; bytesOfDataRead++)
    {
        validRead = Remote_SPI_Read(buf, 1);
        //REMOTE_LOG("read one byte: 0x%x", buf[0]);
        if(! validRead)
            return FALSE;
        if(buf[0] != 0)
        {
            if(len > 1)
                validRead = Remote_SPI_Read(&buf[1], len-1);
            return validRead;
        }
    }
    return FALSE;
}


static BOOL SendTxPkt(void)
{
    BOOL validWrite = Remote_SPI_Write(&txPkt, txPkt.pktSize);
    if(!validWrite)
    {
        REMOTE_LOG("ERROR: invalid SPI write");
        return FALSE;
    };
    BOOL gotResponse = WaitForByte(&rxByte);
    if(gotResponse)
    {
        if(rxByte == REMOTE_UPDATE_BYTE_ACK)
        {
            return TRUE;
        }
        else if(rxByte == REMOTE_UPDATE_BYTE_NACK)
        {
            REMOTE_LOG("ERROR: Packet send got NACK response");
            return FALSE;
        }
        else
        {
            REMOTE_LOG("ERROR: Packet send got invalid response");
            return FALSE;
        }
    }
    else
    {
        REMOTE_LOG("ERROR: Packet send got no response");
        return FALSE;
    }
}


static BOOL Remote_Update_SendACK(void)
{
    uint8_t data = REMOTE_UPDATE_BYTE_ACK;
    return Remote_SPI_Write((void*)data, 1);
}

static BOOL Remote_Update_SendNACK(void)
{
    uint8_t data = REMOTE_UPDATE_BYTE_NACK;
    return Remote_SPI_Write((void*)data, 1);
}
