#ifndef REMOTE_TX_H
#define REMOTE_TX_H

#include "RemoteDefs.h"



BOOL Remote_Tx_SendPacket(Remote_OpCode_t opcode, const void* data, uint16_t len);

//BOOL Remote_Tx_SendLED(uint8_t id, uint8_t op, uint32_t timeOnMs, uint32_t timeOffMs);
//BOOL Remote_Tx_SendEcho(Remote_TxPktData_Echo_t echo_msg);
BOOL Remote_Tx_SendGetVersion(void);
BOOL Remote_Tx_SendGetStatus(void);
BOOL Remote_Tx_SendReset(void);
BOOL Remote_Tx_SendReadChar(Remote_Data_t dataType, uint16_t arg);
BOOL Remote_Tx_SendWriteChar(Remote_Data_t dataType,
                        uint16_t arg,
                        const uint8_t* data, 
                        uint8_t len);
BOOL Remote_Tx_SendStartScan(Remote_TxPktData_StartScan_t timeout);
BOOL Remote_Tx_SendClearPairing(void);
BOOL Remote_Tx_SendPrepForUpdate(void);


#endif