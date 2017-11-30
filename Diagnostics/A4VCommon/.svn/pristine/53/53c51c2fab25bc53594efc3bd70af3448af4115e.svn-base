#include "project.h"
#include "RemoteDefs.h"
#include "RemoteTx.h"
#include "RemoteSPI.h"


static BOOL Remote_Tx_SendPacket(uint16_t opcode, const void* data, uint16_t len);


SCRIBE_DECL(remote);
//Remote_SPIPacket_t pkt;
uint8_t Remote_numPacketsSent = 0;
uint8_t Remote_numPacketsReceived = 0;


static BOOL Remote_Tx_SendPacket(uint16_t opcode, const void* data, uint16_t len)
{
    if(len > sizeof(pkt.data))
    {
        REMOTE_LOG("Remote_Tx attempt to send packet with invalid len");
        return FALSE;
    }
    pkt.opcode = opcode;
    pkt.txSeqNum = Remote_numPacketsSent++;
    pkt.rxSeqNum = Remote_numPacketsReceived;
    if(data)
        memcpy(pkt.data, data, len);
    
    // pad buffer with 0
    memset(pkt.data+len, 0, sizeof(pkt.data)-len);
    
    pkt.xor = Remote_SPI_ComputeXOR(&pkt);
    
    return Remote_SPI_Write(&pkt, sizeof(Remote_SPIPacket_t));
}


/*
BOOL Remote_Tx_SendLED(uint8_t id, uint8_t op, uint32_t timeOnMs, uint32_t timeOffMs)
{
    
    Remote_TxPktData_LED_t ledMsg = {
            .ledId = id,
            .ledOp = op,
            .timeOnMs = timeOnMs,
            .timeOffMs = timeOffMs };
    
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_LED,
                                (void*)&ledMsg,
                                sizeof(Remote_TxPktData_LED_t));
}
BOOL Remote_Tx_SendEcho(Remote_TxPktData_Echo_t data)
{
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_LED,
                                (void*)&data,
                                sizeof(Remote_TxPktData_Echo_t));
}
*/


BOOL Remote_Tx_SendGetVersion(void)
{
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_GET_VERSION, NULL, 0);
}


BOOL Remote_Tx_SendGetStatus(void)
{
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_GET_STATUS, NULL, 0);
}

BOOL Remote_Tx_SendReset(void)
{
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_RESET, NULL, 0);
}


BOOL Remote_Tx_SendReadChar(Remote_Data_t dataType, uint16_t arg)
{
    Remote_TxPktData_ReadChar_t data = {
        .dataType = dataType,
        .arg = arg
    };
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_READ_CHAR,
                               (void*)&data,
                               sizeof(Remote_TxPktData_ReadChar_t));
}



BOOL Remote_Tx_SendWriteChar(Remote_Data_t dataType, uint16_t arg, 
                        const uint8_t* data, uint8_t len)
{
    Remote_TxPktData_WriteChar_t pktData;
    if(len > sizeof(pktData.data))
        len = sizeof(pktData.data);
    pktData.dataType = dataType;
    pktData.arg = arg;
    if(data)
        memcpy(pktData.data, data, len);
    pktData.len = len;
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_WRITE_CHAR,
                               (void*)&pktData,
                               sizeof(Remote_TxPktData_WriteChar_t));
}


BOOL Remote_Tx_SendStartScan(Remote_TxPktData_StartScan_t timeout)
{
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_START_SCAN,
                                (void*)&timeout,
                                sizeof(Remote_TxPktData_StartScan_t));
}


BOOL Remote_Tx_SendClearPairing(void)
{
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_CLEAR_PAIRING, NULL, 0);
}


BOOL Remote_Tx_SendPrepForUpdate(void)
{
    return Remote_Tx_SendPacket(TOREMOTE_OPCODE_PREP_FOR_UPDATE, NULL, 0);
}
