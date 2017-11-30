/*!
* @file        BoseUSART_API.h
* @version     $Id:
* @author      ja70076
* @brief       Bose USART Abstraction Layer
*
* Copyright 2014 Bose Corporation.
*/


/* This is the Bose API for all things USART.  The generic calls and types enumerated here will
   map to hardware resources for a given platform.  That mapping occurs in BoseUSART_API.c
   This file will be extended as generic USART capability is required */


#ifndef HEADER_BOSE_USART
#define HEADER_BOSE_USART


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "project.h"
#include "interrupt.h"


// USART port names -
typedef enum
{
    BUSART1 = 0,
    BUSART2,
    BUSART3,
    BUSART4,
    BUSART5,
    BUSART6
} BU_Id_t;


// available USART Baud Rates -
typedef enum
{
    BR_9600 = 0,
    BR_19200,
    BR_57600,
    BR_115200,
    BR_921600,
    BR_230400
} BU_Baudrate_t;


// Word Length selections - 8 or 9 bits for now
typedef enum
{
    WL_8BITS = 0,
    WL_9BITS
} BU_Wordlength_t;


// Number of Stop Bits
typedef enum
{
    SB_0_5 = 0,
    SB_1,
    SB_1_5,
    SB_2
} BU_Stopbits_t;


// Parity Selections
typedef enum
{
    P_NONE = 0,
    P_EVEN,
    P_ODD
} BU_Parity_t;


// enable receive, transmit, or both
typedef enum
{
    M_RX = 0,
    M_TX,
    M_RX_TX
} BU_RxTxMode_t;


// HW flow control
typedef enum
{
    FC_NONE = 0,
    FC_RTS,
    FC_CTS,
    FC_RTS_CTS
} BU_FlowControl_t;


// peripheral State options
typedef enum
{
    PS_DISABLE = 0,
    PS_ENABLE
} BU_State_t;


// Interrupt Source options
typedef enum
{
    IRQ_CTS = 0, //CTS change interrupt
    IRQ_TXE,   //Transmit Data Register empty interrupt
    IRQ_TC,    //Transmission complete interrupt
    IRQ_RXNE,  //Receive Data register not empty interrupt
    IRQ_PE,    //Parity Error interrupt
    IRQ_ERR    //Error interrupt(Frame error, noise error, overrun error)
} BU_IrqSource_t;


// USART Flags
typedef enum
{
    FL_CTS = 0,
    FL_TXE,
    FL_TC,
    FL_RXNE,
    FL_IDLE,
    FL_ORE,
    FL_NE,
    FL_FE,
    FL_PE
} BU_Flags_t;

// DMA Transfer Type Options
typedef enum
{
    DMA_TX = 0,
    DMA_RX,
    DMA_TX_RX
} BU_DmaReq_t;


typedef unsigned char bool;

#define BUSART_OVERRUN_ERROR(USARTx) (BUSART_GetFlagStatus(USARTx, FL_ORE) == BU_FLAG_IS_SET)
#define BUSART_BYTE_RECEIVED(USARTx) (BUSART_GetFlagStatus(USARTx, FL_RXNE) == BU_FLAG_IS_SET)
#define BUSART_TRANSMIT_COMPLETE(USARTx) (BUSART_GetFlagStatus(USARTx, FL_TXE) == BU_FLAG_IS_SET)



// Bose USART function return values
#define BU_FLAG_IS_SET             1
#define BU_FLAG_IS_RESET           0

#define BU_SUCCESS                 0
#define BU_ERROR_BAD_USART_ID     -1
#define BU_ERROR_BAD_BAUD_RATE    -2
#define BU_ERROR_BAD_WORD_LENGTH  -3
#define BU_ERROR_BAD_STOP_BITS    -4
#define BU_ERROR_BAD_PARITY       -5
#define BU_ERROR_BAD_RX_TX_MODE   -6
#define BU_ERROR_BAD_FLOW_CONTROL -7
#define BU_ERROR_BAD_COMMAND      -8
#define BU_ERROR_BAD_FLAG         -9
#define BU_ERROR_BAD_STATE        -10
#define BU_ERROR_BAD_IRQ_SOURCE   -11
#define BU_ERROR_BAD_DMA_REQ     -12


typedef struct
{
    BU_Baudrate_t    BaudRate;            /*!< This member configures the USART communication baud rate. */
    BU_Wordlength_t  WordLength;          /*!< Specifies the number of data bits transmitted or received in a frame. */
    BU_Stopbits_t    StopBits;            /*!< Specifies the number of stop bits transmitted. */
    BU_Parity_t      Parity;              /*!< Specifies the parity mode. */
    BU_RxTxMode_t    RxTxMode;            /*!< Specifies wether the Receive or Transmit mode is enabled or disabled. */
    BU_FlowControl_t HardwareFlowControl; /*!< Specifies wether the hardware flow control mode is enabled or disabled. */
} BU_InitTypeDef;

// IRQ Handler Dispatch
void BUSART_IRQ(BU_Id_t USARTx);

// API calls
void BUSART_PlatformInitUSART();
int32_t BUSART_PlatformBindInterruptHandler(BU_Id_t USARTx, INTERRUPT_FNCT_PTR handler, bool enable);
int32_t BUSART_Init(BU_Id_t USARTx, const BU_InitTypeDef* USART_InitStruct);
int32_t BUSART_DeInit(BU_Id_t USARTx);
int32_t BUSART_Cmd(BU_Id_t USARTx, BU_State_t NewState);
int32_t BUSART_SendData(BU_Id_t USARTx, uint16_t Data);
uint16_t BUSART_ReceiveData(BU_Id_t USARTx);
int32_t BUSART_ITConfig(BU_Id_t USARTx, BU_IrqSource_t USART_IT, BU_State_t NewState);
int32_t BUSART_GetFlagStatus(BU_Id_t USARTx, BU_Flags_t USART_FLAG);
int32_t BUSART_ClearFlag(BU_Id_t USARTx, BU_Flags_t USART_FLAG);
int32_t BUSART_GetITStatus(BU_Id_t USARTx, BU_IrqSource_t USART_IT);
int32_t BUSART_ClearITPendingBit(BU_Id_t USARTx, BU_IrqSource_t USART_IT);
int32_t BUSART_DMACmd(BU_Id_t USARTx, BU_DmaReq_t DMAReqType, BU_State_t NewState);

#endif //HEADER_BOSE_USART


