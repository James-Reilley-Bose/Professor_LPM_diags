
#include "project.h"
#include "RemoteRx.h"
#include "RemoteDefs.h"
#include "RemoteNames.h"
#include "RemoteUtility.h"
#include "RemoteDriver.h"
#include "etap.h"
#include "BMutex.h"


static void Remote_Rx_ReceiveGetVersion(Remote_SPIPacket_t* pkt);
static void Remote_Rx_ReceiveGetStatus(Remote_SPIPacket_t* pkt);
static void Remote_Rx_ReceiveOpComplete(Remote_SPIPacket_t* pkt);
static void Remote_Rx_ReceiveReadChar(Remote_SPIPacket_t* pkt);
static void Remote_Rx_ReceiveNotify(Remote_SPIPacket_t* pkt);
static void Remote_Rx_ReceiveLogMsg(Remote_SPIPacket_t* pkt);
static void Remote_Rx_ReceiveHeapFail(Remote_SPIPacket_t* pkt);


SCRIBE_DECL(remote);
char Remote_versionString[REMOTE_VERSION_STR_LEN+1] = {0};


void Remote_Rx_ParsePacket(Remote_SPIPacket_t* pkt)
{
    switch(pkt->opcode)
    {
        case FROMREMOTE_OPCODE_XFERTEST_RDY:
            break;
        case FROMREMOTE_OPCODE_XFERTEST_PKT:
            break;
        case FROMREMOTE_OPCODE_OPCOMPLETE:
            Remote_Rx_ReceiveOpComplete(pkt);
            break;
        case FROMREMOTE_OPCODE_READ_CHAR:
            Remote_Rx_ReceiveReadChar(pkt);
            break;
        case FROMREMOTE_OPCODE_NOTIFY:
            Remote_Rx_ReceiveNotify(pkt);
            break;
        case FROMREMOTE_OPCODE_VERSION:
            Remote_Rx_ReceiveGetVersion(pkt);
            break;
        case FROMREMOTE_OPCODE_STATUS:
            Remote_Rx_ReceiveGetStatus(pkt);
            break;
        case FROMREMOTE_OPCODE_LOGMSG:
            Remote_Rx_ReceiveLogMsg(pkt);
            break;
        case FROMREMOTE_OPCODE_HEAP_FAIL:
            Remote_Rx_ReceiveHeapFail(pkt);
            break;
        default:
            REMOTE_LOG("Rx: Unhandled opcode in packet: op=0x%x", pkt->opcode);
    }
}


static void Remote_Rx_ReceiveGetStatus(Remote_SPIPacket_t* pkt)
{
    // Check if something is blocking for this message
    if(BMutex_Take(Remote_BlockingInfo_getStatus.semaphore, 0) == pdFALSE)
    {
        memcpy(Remote_BlockingInfo_getStatus.data, pkt->data, sizeof(Remote_RxPktData_Status_t));
        BMutex_Give(Remote_BlockingInfo_getStatus.semaphore);
    }
    else
    {
        BMutex_Give(Remote_BlockingInfo_getStatus.semaphore);
    }
}


static void Remote_Rx_ReceiveGetVersion(Remote_SPIPacket_t* pkt)
{
    memcpy(Remote_versionString, pkt->data, REMOTE_VERSION_STR_LEN);
    Remote_versionString[REMOTE_VERSION_STR_LEN] = '\0'; // Just in case
    //REMOTE_LOG("Updated version string: \"%s\"", Remote_versionString);
}


static void Remote_Rx_ReceiveOpComplete(Remote_SPIPacket_t* pkt)
{
    Remote_RxPktData_OpComplete_t* pktData = (Remote_RxPktData_OpComplete_t*)pkt->data;
    REMOTE_LOG("Rx: Got op complete, op=%x status=%d", pktData->opCode, pktData->status);
    if(BMutex_Take(Remote_BlockingInfo_opComplete.semaphore, 0) == pdFALSE)
    {
        switch(pktData->opCode)
        {
            case TOREMOTE_OPCODE_START_SCAN:
                //REMOTE_LOG("Got StartScan");
                *((uint8_t*)Remote_BlockingInfo_opComplete.data) = pktData->status;
                BMutex_Give(Remote_BlockingInfo_opComplete.semaphore);
                break;
            case TOREMOTE_OPCODE_CLEAR_PAIRING:
                //REMOTE_LOG("Got ClearPairing");
                *((uint8_t*)Remote_BlockingInfo_opComplete.data) = pktData->status;
                BMutex_Give(Remote_BlockingInfo_opComplete.semaphore);
                break;
        }
    }
    else
    {
        BMutex_Give(Remote_BlockingInfo_opComplete.semaphore);
    }
}

static void Remote_Rx_ReceiveReadChar(Remote_SPIPacket_t* pkt)
{
    Remote_RxPktData_DataBlob_t* dataBlob = (Remote_RxPktData_DataBlob_t*)pkt->data;
    REMOTE_LOG("Rx_ReceiveReadChar: dataType=%d arg=%x status=%d len=%d",
               dataBlob->dataType, dataBlob->arg, dataBlob->status, dataBlob->len);
}


static void Remote_Rx_ReceiveNotify(Remote_SPIPacket_t* pkt)
{
    Remote_RxPktData_DataBlob_t* dataBlob = (Remote_RxPktData_DataBlob_t*)pkt->data;
    REMOTE_LOG("Rx_ReceiveNotify: dataType=0x%x arg=0x%x status=0x%x len=%d",
               dataBlob->dataType, dataBlob->arg, dataBlob->status, dataBlob->len);
    switch(dataBlob->dataType)
    {
        case REMOTE_DATA_HID_KEY:
            printData(dataBlob->data, dataBlob->len);
            break;
        case REMOTE_DATA_HID_CC:
            break;
        case REMOTE_DATA_HID_UAPI_IN:
            break;
        case REMOTE_DATA_BATT_LEVEL:
            break;
    }
}


static void Remote_Rx_ReceiveLogMsg(Remote_SPIPacket_t* pkt)
{
    
    REMOTE_LOG("%s", pkt->data);
}


static void Remote_Rx_ReceiveHeapFail(Remote_SPIPacket_t* pkt)
{
    REMOTE_LOG("got HeapFail message!!");
    Remote_ClearWaitingTasks();
    Remote_HardReset();
}
