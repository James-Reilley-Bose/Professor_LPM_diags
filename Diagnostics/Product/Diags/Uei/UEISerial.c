/**
  ******************************************************************************
  * @file    UEISerial.c
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   UEI serial layer.
  *          This file provides firmware functions to manage the following
  *          functionalities of the UEI:
  *           + Transmits data via UART
  *           + IRQ functionality to provide UART receive
 @verbatim
 ===============================================================================
 **/

#include "UEIDefinitions.h"
#include "UEITask.h"
#include "UEISerial.h"
#include "BoseUSART_API.h"
#include "SystemAnalysis.h"

SCRIBE_DECL(uei_serial);

/* Local functions */
static void SendSingleChar(const char c);
static void ProcessRxByte(uint8_t c);
static inline void PacketHandler(BU_Id_t buart);
void UEI_IRXmitComplete(void);

/* Global varilables */
UEISerial_Globals g_Serial;

/* Global functions */
/*
===============================================================================
@fn UEI_InitService
@brief Init for service
===============================================================================
*/
void UEI_InitSerial(void)
{
  g_Serial.ReceivedByteIdx = 0;
  memset(g_Serial.UEIInterruptRXBuffer, 0, sizeof(g_Serial.UEIInterruptRXBuffer));
  SET_UEI_DATA_RECEPTION_EXPECTATION(UEI_NO);
}

/*
===============================================================================
@fn Function: UEI_Serial_SendCommandArray()
@brief Sends a character array
===============================================================================
*/
void UEI_Serial_SendCommandArray(uint8_t *b, uint16_t count)
{
  /* Initialize the receive buffer to "UEI_NO_RESPONSE" and reset the length,
   * and bytes received.
   */
  g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_CODE_POS] = UEI_NO_RESPONSE;
  g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_DATA_LENGTH_POS] = 0;
  g_Serial.ReceivedByteIdx  = 0;

  while(count--)
  {
    SendSingleChar(*b++);
  }
}

/*
===============================================================================
@fn Function: UEI_IRQHandler()
@brief IRQ handler for UART receive
===============================================================================
*/
void UEI_IRQHandler(void)
{
  INTERRUPT_RUN_LATENCY_BEGIN(UEI_UART_ISR);
  PacketHandler(UEI_BUART);
  INTERRUPT_RUN_LATENCY_END(UEI_UART_ISR,0);
}

/* Local functions */
/*
===============================================================================
@fn Function: SendSingleChar()
@brief Sends a single character
===============================================================================
*/
static void SendSingleChar(const char c)
{
   while( !BUSART_TRANSMIT_COMPLETE(UEI_BUART) ) {
            if(!InInterrupt()) {
            }
   }
   BUSART_SendData(UEI_BUART,c);
}

/*
===============================================================================
@fn Function: PacketHandler()
@brief Check for overun error and if a byte is received.
===============================================================================
*/
static inline void PacketHandler(BU_Id_t buart)
{
  static portBASE_TYPE xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;

  if (BUSART_OVERRUN_ERROR(buart))
  {
     LOG(uei_serial,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t uei serial BUSAART overrun error");
  }

  if (BUSART_BYTE_RECEIVED(buart))
  {
      ProcessRxByte(BUSART_ReceiveData(buart));
  }
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

/*
===============================================================================
@fn Function: ProcessRxByte()
@brief Process the received byte
===============================================================================
*/
static void ProcessRxByte(uint8_t c)
{
   if (GET_UEI_DATA_RECEPTION_EXPECTATION() == UEI_NO)
   {
     LOG(uei_serial,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t Error: Received unexpected bytes from UEI Blaster!!");
     return;
   }

   /* Put everything into a receive buffer as long as there is enough space */
   if (g_Serial.ReceivedByteIdx < UEI_RECEIVE_BUFFER_SIZE - 1)
   {
     g_Serial.UEIInterruptRXBuffer[g_Serial.ReceivedByteIdx++] = c;

     /* Get the data based on the length info, expect no more byte to arrive and give the semaphore for processing */
     if ( g_Serial.ReceivedByteIdx == (g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_DATA_LENGTH_POS]+1))
     {
       SET_UEI_DATA_RECEPTION_EXPECTATION(UEI_NO);
       UEI_IRXmitComplete(); //can't be transmitting IR and receiving bytes simultaneously
       GiveUEISemaphore();
     }
   }
   else
   {
      LOG(uei_serial,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t Error: more bytes than the buffer can receive!!");
     return;
   }

}




