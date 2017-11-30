#ifndef REMOTE_DEFS_H
#define REMOTE_DEFS_H

#include "project.h"

/*
 * Some shared definitions, enums, and structures for remote
 *
 *
*/

#define USE_REMOTE_LOG
#define USE_REMOTE_NAMES

//for debugging purposes. This should be removed for final product. Any
// logging that should be in the final product use LOG(...) directly.
#ifdef USE_REMOTE_LOG
#define REMOTE_LOG(...) LOG(remote, ROTTEN_LOGLEVEL_NORMAL, __VA_ARGS__)
#else
#define REMOTE_LOG(...)
#endif
     
#define REMOTESPI_GROUP_MASK                0xff00
#define REMOTESPI_GROUP_TOREMOTE            0x0100
#define REMOTESPI_GROUP_FROMREMOTE          0x0200

/*
 * Opcodes for header of SPI packets
*/
typedef uint16_t Remote_OpCode_t;
#define TOREMOTE_OPCODE_RESET               (REMOTESPI_GROUP_TOREMOTE + 1)
#define TOREMOTE_OPCODE_LED                 (REMOTESPI_GROUP_TOREMOTE + 2)
#define TOREMOTE_OPCODE_START_SCAN          (REMOTESPI_GROUP_TOREMOTE + 3)
#define TOREMOTE_OPCODE_START_XFERTEST      (REMOTESPI_GROUP_TOREMOTE + 4)
#define TOREMOTE_OPCODE_XFERTEST_PKT        (REMOTESPI_GROUP_TOREMOTE + 5)
#define TOREMOTE_OPCODE_PREP_FOR_UPDATE     (REMOTESPI_GROUP_TOREMOTE + 6)
#define TOREMOTE_OPCODE_READ_CHAR           (REMOTESPI_GROUP_TOREMOTE + 7)
#define TOREMOTE_OPCODE_WRITE_CHAR          (REMOTESPI_GROUP_TOREMOTE + 8)
#define TOREMOTE_OPCODE_GET_VERSION         (REMOTESPI_GROUP_TOREMOTE + 9)
#define TOREMOTE_OPCODE_GET_STATUS          (REMOTESPI_GROUP_TOREMOTE + 10)
#define TOREMOTE_OPCODE_CLEAR_PAIRING       (REMOTESPI_GROUP_TOREMOTE + 11)
#define TOREMOTE_OPCODE_ECHO                (REMOTESPI_GROUP_TOREMOTE + 12)
#define TOREMOTE_OPCODE_UPDATE_LINK         (REMOTESPI_GROUP_TOREMOTE + 13)
#define FROMREMOTE_OPCODE_XFERTEST_RDY      (REMOTESPI_GROUP_FROMREMOTE + 1)
#define FROMREMOTE_OPCODE_XFERTEST_PKT      (REMOTESPI_GROUP_FROMREMOTE + 2)
#define FROMREMOTE_OPCODE_OPCOMPLETE        (REMOTESPI_GROUP_FROMREMOTE + 3)
#define FROMREMOTE_OPCODE_READ_CHAR         (REMOTESPI_GROUP_FROMREMOTE + 4)
#define FROMREMOTE_OPCODE_NOTIFY            (REMOTESPI_GROUP_FROMREMOTE + 5)
#define FROMREMOTE_OPCODE_VERSION           (REMOTESPI_GROUP_FROMREMOTE + 6)
#define FROMREMOTE_OPCODE_STATUS            (REMOTESPI_GROUP_FROMREMOTE + 7)
#define FROMREMOTE_OPCODE_ECHO              (REMOTESPI_GROUP_FROMREMOTE + 8)
#define FROMREMOTE_OPCODE_LOGMSG            (REMOTESPI_GROUP_FROMREMOTE + 9)
#define FROMREMOTE_OPCODE_GET_LOG_LEVEL     (REMOTESPI_GROUP_FROMREMOTE + 10)
#define FROMREMOTE_OPCODE_HEAP_FAIL         (REMOTESPI_GROUP_FROMREMOTE + 11)

#define REMOTE_PACKET_DATA_LEN (64-8)  /* Length of data field in spi packet */
#define REMOTE_VERSION_STR_LEN 20
#define REMOTE_OP_COMPLETE_SUCCESS 0
#define REMOTE_OP_COMPLETE_FAILURE 1
// TODO: this needs tuned to max MTU len
// NOTE: related to SPIMSG_DATA_LEN above
#define REMOTE_MAX_DATA_LEN  40
#define REMOTE_LONG_TIMEOUT_MS 5000
#define REMOTE_LONG_TIMEOUT_TICKS pdMS_TO_TICKS(REMOTE_LONG_TIMEOUT_MS)


extern uint8_t Remote_numPacketsSent;
extern uint8_t Remote_numPacketsReceived;

// Packet type sent to and received from CC2640
typedef struct
{
    uint16_t opcode;
    uint8_t txSeqNum;
    uint8_t rxSeqNum; 
    uint8_t data[REMOTE_PACKET_DATA_LEN];
    uint32_t xor;
} Remote_SPIPacket_t;

/*
 * Used to identify type of data when read/writing charactaristics, and
 *  in notifications
*/
typedef enum
{
    REMOTE_DATA_UNKNOWN,
    REMOTE_DATA_HID_KEY,
    REMOTE_DATA_HID_CC,
    REMOTE_DATA_HID_UAPI_IN,
    REMOTE_DATA_HID_UAPI_OUT,
    REMOTE_DATA_BATT_LEVEL,
    REMOTE_DATA_HANDLE,
    REMOTE_DATA_FEEDBACK,
    REMOTE_DATA_TESTHIDLED,
} Remote_Data_t;


/*
 * Holds a semaphore and information while caller is blocking, waiting for a
 *  SPI response.
*/
typedef struct
{
    SemaphoreHandle_t semaphore;
    void* data;
} Remote_BlockingInfo_t;
extern Remote_BlockingInfo_t Remote_BlockingInfo_getStatus;
extern Remote_BlockingInfo_t Remote_BlockingInfo_opComplete;


/*
 * Used in response to READ_CHAR and NOTIFY
*/
typedef struct
{
    Remote_Data_t dataType;       // BLEDataType
    uint16_t arg;           // generic arg (e.g. handle)
    uint8_t status;
    uint8_t len;
    uint8_t data[REMOTE_MAX_DATA_LEN];
} Remote_PktData_Blob_t;



typedef struct
{
    uint16_t minConnInterval;
    uint16_t maxConnInterval;
    uint16_t slaveLatency;
    uint16_t timeout;
} Remote_ConnectionParams_t;



/*
 * ==================================================
 * Types for Remote status
 * ==================================================
*/
typedef enum 
{
    REMOTE_HIDTYPE_NONE,
    REMOTE_HIDTYPE_UEI,
    REMOTE_HIDTYPE_TI,
} Remote_HID_t;

typedef enum
{
    REMOTE_STATE_IDLE,
    REMOTE_STATE_WAIT_INIT_DONE,    // waiting for INIT_DONE from BLE stack
    REMOTE_STATE_LINKING,           // re-establishing link
    REMOTE_STATE_SCAN_WAIT_HOST,    // waiting for host to tell us to start scan
    REMOTE_STATE_SCAN_SCANNING,     // scanning
    REMOTE_STATE_BONDED,            // paired/bonded
    REMOTE_STATE_RUN,               // normal run mode
    REMOTE_STATE_ERR
} Remote_BLEState_t;

typedef enum
{
    REMOTE_NORESULT,
    REMOTE_FOUND,
    REMOTE_NOTFOUND
} Remote_ScanResult_t;

typedef enum
{
    REMOTE_GAPSTATE_IDLE,
    REMOTE_GAPSTATE_SCANNING,           // scanning, nothing found yet
    REMOTE_GAPSTATE_FOUND1,             // scanning, found at least one
    REMOTE_GAPSTATE_LINK_START,         // done scanning, establishing link
    REMOTE_GAPSTATE_LINK_ESTABLISHED,   // wait pairing
    REMOTE_GAPSTATE_LINK_PAIRING,       // wait paired
    REMOTE_GAPSTATE_LINK_BONDED,        // paired/bonded
    REMOTE_GAPSTATE_TERM_LINK,          // 'forced' termination of the link
} Remote_GapState_t;

typedef enum
{
    REMOTE_GAPSCAN_NORESULT,
    REMOTE_GAPSCAN_FOUND,
    REMOTE_GAPSCAN_STARTERR,
    REMOTE_GAPSCAN_TIMEOUT,
    REMOTE_GAPSCAN_LINKERR,
    REMOTE_GAPSCAN_BONDERR,
} Remote_GapScanResult_t;

typedef enum
{
    REMOTE_GATTSTATE_IDLE,

    REMOTE_GATTSTATE_DISC_SVCS,       // discover services
    REMOTE_GATTSTATEE_DISC_CHARS,      // discover characteristics
    REMOTE_GATTSTATE_DISC_DESCS,      // discover descriptors
    REMOTE_GATTSTATE_DISC_HIDCHARS,   // identify HID chars
    REMOTE_GATTSTATE_DISC_NOTIF,      // enable notifs

    REMOTE_GATTSTATE_RUN,             // normal run mode

    REMOTE_GATTSTATE_READ_CHAR,       // readChar active
    REMOTE_GATTSTATE_WRITE_CHAR,      // readChar active
} Remote_GattState_t;

typedef enum
{
    REMOTE_GATTRESULT_NORESULT,
    REMOTE_GATTRESULT_DONE,
    REMOTE_GATTRESULT_ERR,
    REMOTE_GATTRESULT_SVCSTARTERR,
    REMOTE_GATTRESULT_SVCCHKERR,
    REMOTE_GATTRESULT_SVCCBERR,
    REMOTE_GATTRESULT_CHARSTARTERR,
    REMOTE_GATTRESULT_CHARCHKERR,
    REMOTE_GATTRESULT_CHARCBERR,
    REMOTE_GATTRESULT_DESCSTARTERR,
    REMOTE_GATTRESULT_DESCCHKERR,
    REMOTE_GATTRESULT_DESCCBERR,
    REMOTE_GATTRESULT_RREFSTARTERR,
    REMOTE_GATTRESULT_RREFCHKERR,
    REMOTE_GATTRESULT_RREFCBERR,
    REMOTE_GATTRESULT_NOTIFSTARTERR,
    REMOTE_GATTRESULT_NOTIFCBERR,
} Remote_GattDiscoverResult_t;



/*
 * ===========================================================
 * Definitions for rx
 * ===========================================================
*/
/*
typedef struct  
{
    Remote_HID_t type;
    Remote_BLEState_t state;
    Remote_ScanResult_t scanResult;
    Remote_GapState_t gapState;
    Remote_GapScanResult_t gapScanResult;
    Remote_GattState_t gattState;
    Remote_GattDiscoverResult_t gattDiscoverResult;
    Remote_ConnectionParams_t connectionParams;
} Remote_RxPktData_Status_t;
*/
typedef struct  
{
    uint8_t type;
    uint8_t state;
    uint8_t scanResult;
    uint8_t gapState;
    uint8_t gapScanResult;
    uint8_t gattState;
    uint8_t gattDiscoverResult;
    Remote_ConnectionParams_t connectionParams;
} Remote_RxPktData_Status_t;

typedef struct
{
    Remote_OpCode_t opCode;
    // REMOTE_OP_COMPLETE_SUCCESS or REMOTE_OP_COMPLETE_FAILURE
    uint8_t status;
} Remote_RxPktData_OpComplete_t;

// Used for ReadChar response packets, as well as Notify packets
typedef struct
{
    uint8_t dataType;
    uint16_t arg;
    uint8_t status;
    uint8_t len;
    uint8_t data[REMOTE_MAX_DATA_LEN];
} Remote_RxPktData_DataBlob_t;

typedef uint32_t Remote_RxPktData_Echo_t;



/*
 * ===========================================================
 * Definitions for tx
 * ===========================================================
*/
typedef struct
{
    uint8_t ledId;          // LEDId enum
    uint8_t ledOp;          // LEDOp enum
    uint32_t timeOnMs;      // for flash
    uint32_t timeOffMs;     // for flash
} Remote_TxPktData_LED_t;

typedef struct
{
    Remote_Data_t dataType;
    uint16_t arg;
    uint8_t data[REMOTE_MAX_DATA_LEN];
    uint8_t len;
} Remote_TxPktData_WriteChar_t;

typedef struct
{
    Remote_Data_t dataType;
    uint16_t arg;
} Remote_TxPktData_ReadChar_t;

// value to echo back
typedef uint32_t Remote_TxPktData_Echo_t;

// timeout for scan
typedef uint32_t Remote_TxPktData_StartScan_t;




#endif





