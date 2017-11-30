/**
  ******************************************************************************
  * @file    UEISerial.h
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   UEI serial layer.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the UEI:
  *           + Serial drivers
  *           + IRQ functionality
 @verbatim
 ===============================================================================
 **/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UEISERIAL_H
#define __UEISERIAL_H

#include "project.h"
#include "UEIDefinitions.h"

typedef struct {
  uint16_t ReceivedByteIdx;
  uint8_t UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_SIZE];
  uint8_t UEIByteExpected;
} UEISerial_Globals;

extern void UEI_InitSerial(void);
extern void UEI_Serial_SendCommandArray(uint8_t*b, uint16_t count);
extern void UEI_Serial_ProcessRxdByte (uint8_t NextChar);
extern void UEI_Serial_ReceiveData(uint16_t rxSize, uint8_t* rxBuf, uint8_t* rxBufLen);
extern void UEI_IRQHandler(void);

extern   UEISerial_Globals g_Serial;


#define SET_UEI_DATA_RECEPTION_EXPECTATION(x) g_Serial.UEIByteExpected=x;
#define GET_UEI_DATA_RECEPTION_EXPECTATION() g_Serial.UEIByteExpected

#endif /* __UEISERIAL_H */
