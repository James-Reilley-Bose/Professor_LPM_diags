#ifndef REMOTE_UPDATE_H
#define REMOTE_UPDATE_H


// todo: If am using these in one file (RemoteUpdate.c) can move most or all
//  of them to there
#define REMOTE_UPDATE_PACKET_DATA_LEN 256-2
#define REMOTE_UPDATE_BYTE_ACK 0xcc
#define REMOTE_UPDATE_BYTE_NACK 0x33

//#define REMOTE_UPDATE_MAX_BL_REPSONSE_SIZE
#define REMOTE_UPDATE_SPI_READ_SIZE 32
// default time to block while waiting for some response data
#define REMOTE_UPDATE_SPI_TIMEOUT_MS 120
#define REMOTE_UPDATE_SPI_TIMEOUT_TICKS TIMER_MSEC_TO_TICKS(REMOTE_UPDATE_SPI_TIMEOUT_MS)
#define REMOTE_UPDATE_NUMBER_OF_WRITE_TRIES 3

// Flash memory is divided into 4K sectors
//#define SectorOfAddr(a) (a / 4096)

typedef enum
{
    REMOTE_UPDATE_ERROR_SUCCESS,
    // Update was aborted because of an invalid blob. Unable to find header
    //  or remote manifest entry
    REMOTE_UPDATE_ERROR_INVALID_BLOB,
    // Update abandoned while flashing. 2640 is now in an invalid state
    REMOTE_UPDATE_ERROR_FLASH_ERROR,
} Remote_Update_Error_t;

typedef enum
{
    REMOTE_UPDATE_STATUS_NOT_UPDATING,
    REMOTE_UPDATE_STATUS_UPDATING,
    REMOTE_UPDATE_STATUS_COMPLETED_SUCCESS,
    REMOTE_UPDATE_STATUS_COMPLETED_FAIL,
} Remote_Update_Status_t;

typedef struct
{
    uint8_t pktSize;
    uint8_t checksum;
    uint8_t data[REMOTE_UPDATE_PACKET_DATA_LEN];
} Remote_Update_SPIPacket_t;


// Flag is TRUE if remote is reading SPI, waiting for some response.
extern BOOL Remote_Update_readingSPI;
extern TimerHandle_t Remote_Update_SPIReadTimer;



/*
 * =========================================================
 * Codes SENT TO bl
 * =========================================================
*/
/*
 * Single byte commands sent to the bl
*/
#define REMOTE_UPDATE_COMMAND_PING 0x20
#define REMOTE_UPDATE_COMMAND_DOWNLOAD 0x21
#define REMOTE_UPDATE_COMMAND_GET_STATUS 0x23
#define REMOTE_UPDATE_COMMAND_SEND_DATA 0x24
#define REMOTE_UPDATE_COMMAND_RESET 0x25
#define REMOTE_UPDATE_COMMAND_SECTOR_ERASE 0x26
#define REMOTE_UPDATE_COMMAND_CRC32 0x27
#define REMOTE_UPDATE_COMMAND_GET_CHIP_ID 0x28
#define REMOTE_UPDATE_COMMAND_MEMORY_READ 0x2a
#define REMOTE_UPDATE_COMMAND_MEMORY_WRITE 0x2b
#define REMOTE_UPDATE_COMMAND_BANK_ERASE 0x2c
#define REMOTE_UPDATE_COMMAND_SET_CCFG 0x2d


/*
 * =========================================================
 * Codes RECEIVED FROM bl
 * =========================================================
*/
/*
 * A single byte returned from bl in response to CMD_GET_STATUS
*/
#define REMOTE_UPDATE_STATUS_SUCCESS 0x40
#define REMOTE_UPDATE_STATUS_UNKNOWN_CMD 0x41
#define REMOTE_UPDATE_STATUS_INVALID_CMD 0x42
#define REMOTE_UPDATE_STATUS_INVALID_ADR 0x43
#define REMOTE_UPDATE_STATUS_FLASH_FAIL 0x44

Remote_Update_Status_t Remote_Update_Status(void);
Remote_Update_Error_t Remote_Update_Start(void);
BOOL Remote_Update_Download(uint32_t address, uint32_t size);
BOOL Remote_Update_SendData(void* data, uint8_t len);
//BOOL Remote_Update_EraseSector(uint32_t address);
BOOL Remote_Update_BankErase(void);
BOOL Remote_Update_GetStatus(uint8_t* response);
BOOL Remote_Update_Ping(void);

#endif