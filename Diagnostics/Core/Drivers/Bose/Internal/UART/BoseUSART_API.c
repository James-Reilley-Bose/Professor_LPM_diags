/*!
* @file        BoseUSART_API.c
* @version     $Id:
* @author      ja70076
* @brief       Bose USART Abstraction layer
*
* Copyright 2014 Bose Corporation.
*/


#include "BoseUSART_API.h"
#include "rottenlog.h"


SCRIBE_DECL(sys_events);



/* This maps the Bose generic USART API to available STMF2xxx series USART/UART peripherals
   This mapping will be used in Bardeen

   NOTE that the enumerated generic types (from BoseUSART_API.h) are used to index into the mapped type arrays defined below.
        this greatly simplifies the mapping process.

*/

#include "stm32f2xx_usart.h"


// The mapped type arrays - these contain STMF2xxx values

USART_TypeDef* STMF2_USARTS[] =
{
    USART1,
    USART2,
    USART3,
    UART4,
    UART5,
    USART6,
};

uint32_t STMF2_Baudrate[] =
{
    9600,
    19200,
    57600,
    115200,
    921600,
    230400
};

uint16_t STMF2_WordLength[] =
{
    USART_WordLength_8b,
    USART_WordLength_9b
};

uint16_t STMF2_StopBits[] =
{
    USART_StopBits_0_5,
    USART_StopBits_1,
    USART_StopBits_1_5,
    USART_StopBits_2
};

uint16_t STMF2_Parity[] =
{
    USART_Parity_No,
    USART_Parity_Even,
    USART_Parity_Odd
};

uint16_t STMF2_RxTxMode[] =
{
    USART_Mode_Rx,
    USART_Mode_Tx,
    (USART_Mode_Rx | USART_Mode_Tx)
};

uint16_t STMF2_FlowControl[] =
{
    USART_HardwareFlowControl_None,
    USART_HardwareFlowControl_RTS,
    USART_HardwareFlowControl_CTS,
    USART_HardwareFlowControl_RTS_CTS
};

FunctionalState STMF2_States[] =
{
    DISABLE,
    ENABLE
};


uint16_t STMF2_InterruptSources[] =
{
    USART_IT_CTS,
    USART_IT_TXE,
    USART_IT_TC,
    USART_IT_RXNE,
    USART_IT_PE,
    USART_IT_ERR
};


uint16_t STMF2_Flags[] =
{
    USART_FLAG_CTS,
    USART_FLAG_TXE,
    USART_FLAG_TC,
    USART_FLAG_RXNE,
    USART_FLAG_IDLE,
    USART_FLAG_ORE,
    USART_FLAG_NE,
    USART_FLAG_FE,
    USART_FLAG_PE
};

IRQn_Type STMF2_USART_INTERRUPTS[] =
{
    USART1_IRQn,
    USART2_IRQn,
    USART3_IRQn,
    UART4_IRQn,
    UART5_IRQn,
    USART6_IRQn,
};

uint16_t STMF2_DmaReqs[] =
{
    USART_DMAReq_Tx,
    USART_DMAReq_Rx,
    (USART_DMAReq_Tx | USART_DMAReq_Rx)
};

// bounds check the index
#define BAD_TYPE_MAP(value, table)((uint16_t)value > sizeof(table)/sizeof(table[0]))



/**
  * @brief API call to encapsulate Platform related 1 time USART init behavior.
  * @param  None yet - add as necessary.
  * @retval None yet - add as necessary.
  */
void BUSART_PlatformInitUSART()
{
    RCC_APB1PeriphClockCmd(TAP_UART_RCC_PERIPH, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
}


/**
  * @brief API call to bind and eable or disable a USART interrupt service routine.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  INTERRUPT_FNCT_PTR: pointer to the ISR.
  * @param  enable: Enable or Disable the interrupt.
  * @retval None yet - add as necessary.
  */
int32_t BUSART_PlatformBindInterruptHandler(BU_Id_t USARTx, INTERRUPT_FNCT_PTR handler, bool enable)
{
    IRQn_Type theInterrupt;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USART_INTERRUPTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theInterrupt = STMF2_USART_INTERRUPTS[(uint16_t)USARTx];

    // always disable
    Interrupt_Disable(theInterrupt);

    // maybe bind and enable
    if (enable != FALSE)
    {
        // Setup interrupt on the UART
        Interrupt_RegisterISR(theInterrupt, handler);
        Interrupt_Enable(theInterrupt, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
    }
    return (BU_SUCCESS);

}


/**
  * @brief  Initializes the USARTx peripheral according to the specified
  *         parameters in the USART_InitStruct .
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that contains
  *         the configuration information for the specified USART peripheral.
  * @retval BU_SUCCESS or BU_ERROR_BAD_USART_ID
  */
int32_t BUSART_Init(BU_Id_t USARTx, const BU_InitTypeDef* USART_InitStruct)
{
    USART_TypeDef* theUsart;
    USART_InitTypeDef theConfig;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(USART_InitStruct->BaudRate, STMF2_Baudrate))
    {
        return (BU_ERROR_BAD_BAUD_RATE);
    }
    theConfig.USART_BaudRate = STMF2_Baudrate[(uint16_t)USART_InitStruct->BaudRate];

    if (BAD_TYPE_MAP(USART_InitStruct->WordLength, STMF2_WordLength))
    {
        return (BU_ERROR_BAD_WORD_LENGTH);
    }
    theConfig.USART_WordLength = STMF2_WordLength[(uint16_t)USART_InitStruct->WordLength];

    if (BAD_TYPE_MAP(USART_InitStruct->StopBits, STMF2_StopBits))
    {
        return (BU_ERROR_BAD_STOP_BITS);
    }
    theConfig.USART_StopBits = STMF2_StopBits[(uint16_t)USART_InitStruct->StopBits];

    if (BAD_TYPE_MAP(USART_InitStruct->Parity, STMF2_Parity))
    {
        return (BU_ERROR_BAD_PARITY);
    }
    theConfig.USART_Parity = STMF2_Parity[(uint16_t)USART_InitStruct->Parity];

    if (BAD_TYPE_MAP(USART_InitStruct->RxTxMode, STMF2_RxTxMode))
    {
        return (BU_ERROR_BAD_RX_TX_MODE);
    }
    theConfig.USART_Mode = STMF2_RxTxMode[(uint16_t)USART_InitStruct->RxTxMode];

    if (BAD_TYPE_MAP(USART_InitStruct->HardwareFlowControl, STMF2_FlowControl))
    {
        return (BU_ERROR_BAD_FLOW_CONTROL);
    }
    theConfig.USART_HardwareFlowControl = STMF2_FlowControl[(uint16_t)USART_InitStruct->HardwareFlowControl];

    // call into the STM peripheral library
    USART_Init(theUsart, &theConfig);

    // USART_Init does not return anything - guess it succeeded.
    return (BU_SUCCESS);
}


/**
  * @brief  Deinitializes the USARTx peripheral registers to their default reset values.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @retval None
  */
int32_t BUSART_DeInit(BU_Id_t USARTx)
{
    USART_TypeDef* theUsart;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    // call into the STM peripheral library
    USART_DeInit(theUsart);

    // USART_Cmd does not return anything - guess it succeeded.
    return (BU_SUCCESS);
}


/**
  * @brief  Enables or disables the specified USART peripheral.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  NewState: new state of the USARTx peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval BU_SUCCESS or a BU_ERROR_xxx response
  */
int32_t BUSART_Cmd(BU_Id_t USARTx, BU_State_t NewState)
{
    USART_TypeDef* theUsart;
    FunctionalState theState;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(NewState, STMF2_States))
    {
        return (BU_ERROR_BAD_STATE);
    }
    theState = STMF2_States[(uint16_t)NewState];

    // call into the STM peripheral library
    USART_Cmd(theUsart, theState);

    // USART_Cmd does not return anything - guess it succeeded.
    return (BU_SUCCESS);
}



/**
  * @brief  Transmits single data through the USARTx peripheral.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
int32_t BUSART_SendData(BU_Id_t USARTx, uint16_t Data)
{
    USART_TypeDef* theUsart;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    // call into the STM peripheral library
    USART_SendData(theUsart, Data);

    // USART_SendData does not return anything - guess it succeeded.
    return (BU_SUCCESS);
}



/**
  * @brief  Returns the most recent received data by the USARTx peripheral.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @retval The received data.
  */
uint16_t BUSART_ReceiveData(BU_Id_t USARTx)
{
    USART_TypeDef* theUsart;


    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    // call into the STM peripheral library
    return (USART_ReceiveData(theUsart));

}


/**
  * @brief  Enables or disables the specified USART interrupts.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  USART_IT: specifies the USART interrupt sources to be enabled or disabled.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt
  *            @arg USART_IT_TXE:  Transmit Data Register empty interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_PE:   Parity Error interrupt
  *            @arg USART_IT_ERR:  Error interrupt(Frame error, noise error, overrun error)
  * @param  NewState: new state of the specified USARTx interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
int32_t BUSART_ITConfig(BU_Id_t USARTx, BU_IrqSource_t USART_IT, BU_State_t NewState)
{
    USART_TypeDef* theUsart;
    uint16_t theIrq;
    FunctionalState theState;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(USART_IT, STMF2_InterruptSources))
    {
        return (BU_ERROR_BAD_IRQ_SOURCE);
    }
    theIrq = STMF2_InterruptSources[(uint16_t) USART_IT];

    if (BAD_TYPE_MAP(NewState, STMF2_States))
    {
        return (BU_ERROR_BAD_STATE);
    }
    theState = STMF2_States[(uint16_t)NewState];

    // call into the STM peripheral library
    USART_ITConfig(theUsart, theIrq, theState);

    // USART_Cmd does not return anything - guess it succeeded.
    return (BU_SUCCESS);

}


/**
  * @brief  Checks whether the specified USART flag is set or not.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  USART_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg USART_FLAG_CTS:  CTS Change flag (not available for UART4 and UART5)
  *            @arg USART_FLAG_LBD:  LIN Break detection flag
  *            @arg USART_FLAG_TXE:  Transmit data register empty flag
  *            @arg USART_FLAG_TC:   Transmission Complete flag
  *            @arg USART_FLAG_RXNE: Receive data register not empty flag
  *            @arg USART_FLAG_IDLE: Idle Line detection flag
  *            @arg USART_FLAG_ORE:  OverRun Error flag
  *            @arg USART_FLAG_NE:   Noise Error flag
  *            @arg USART_FLAG_FE:   Framing Error flag
  *            @arg USART_FLAG_PE:   Parity Error flag
  * @retval An Error or The new state of USART_FLAG (SET or RESET).
  */
int32_t BUSART_GetFlagStatus(BU_Id_t USARTx, BU_Flags_t USART_FLAG)
{
    USART_TypeDef* theUsart;
    uint16_t theFlag;
    FlagStatus status;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(USART_FLAG, STMF2_Flags))
    {
        return (BU_ERROR_BAD_FLAG);
    }
    theFlag = STMF2_Flags[(uint16_t) USART_FLAG];

    // call into the STM peripheral library
    status = USART_GetFlagStatus(theUsart, theFlag);

    // Map FlagStatus and return.
    return ((status == SET) ? BU_FLAG_IS_SET : BU_FLAG_IS_RESET);

}


/**
  * @brief  Clears the USARTx's pending flags.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  USART_FLAG: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg USART_FLAG_CTS:  CTS Change flag (not available for UART4 and UART5).
  *            @arg USART_FLAG_LBD:  LIN Break detection flag.
  *            @arg USART_FLAG_TC:   Transmission Complete flag.
  *            @arg USART_FLAG_RXNE: Receive data register not empty flag.
  *
  * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (OverRun
  *          error) and IDLE (Idle line detected) flags are cleared by software
  *          sequence: a read operation to USART_SR register (USART_GetFlagStatus())
  *          followed by a read operation to USART_DR register (USART_ReceiveData()).
  * @note   RXNE flag can be also cleared by a read to the USART_DR register
  *          (USART_ReceiveData()).
  * @note   TC flag can be also cleared by software sequence: a read operation to
  *          USART_SR register (USART_GetFlagStatus()) followed by a write operation
  *          to USART_DR register (USART_SendData()).
  * @note   TXE flag is cleared only by a write to the USART_DR register
  *          (USART_SendData()).
  *
  * @retval None
  */
int32_t BUSART_ClearFlag(BU_Id_t USARTx, BU_Flags_t USART_FLAG)
{
    USART_TypeDef* theUsart;
    uint16_t theFlag;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(USART_FLAG, STMF2_Flags))
    {
        return (BU_ERROR_BAD_FLAG);
    }
    theFlag = STMF2_Flags[(uint16_t) USART_FLAG];

    // call into the STM peripheral library
    USART_ClearFlag(theUsart, theFlag);

    // USART_ClearFlag does not return anything - guess it succeeded.
    return (BU_SUCCESS);

}


/**
  * @brief  Checks whether the specified USART interrupt has occurred or not.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  USART_IT: specifies the USART interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt (not available for UART4 and UART5)
  *            @arg USART_IT_LBD:  LIN Break detection interrupt
  *            @arg USART_IT_TXE:  Transmit Data Register empty interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_ORE:  OverRun Error interrupt
  *            @arg USART_IT_NE:   Noise Error interrupt
  *            @arg USART_IT_FE:   Framing Error interrupt
  *            @arg USART_IT_PE:   Parity Error interrupt
  * @retval The new state of USART_IT (SET or RESET).
  */
int32_t BUSART_GetITStatus(BU_Id_t USARTx, BU_IrqSource_t USART_IT)
{
    USART_TypeDef* theUsart;
    uint16_t theIrq;
    ITStatus status;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(USART_IT, STMF2_InterruptSources))
    {
        return (BU_ERROR_BAD_IRQ_SOURCE);
    }
    theIrq = STMF2_InterruptSources[(uint16_t) USART_IT];

    // call into the STM peripheral library
    status = USART_GetITStatus(theUsart, theIrq);

    // Map FlagStatus and return.
    return ((status == SET) ? BU_FLAG_IS_SET : BU_FLAG_IS_RESET);

}


/**
  * @brief  Clears the USARTx's interrupt pending bits.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  USART_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt (not available for UART4 and UART5)
  *            @arg USART_IT_LBD:  LIN Break detection interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt.
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt.
  *
  * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (OverRun
  *          error) and IDLE (Idle line detected) pending bits are cleared by
  *          software sequence: a read operation to USART_SR register
  *          (USART_GetITStatus()) followed by a read operation to USART_DR register
  *          (USART_ReceiveData()).
  * @note   RXNE pending bit can be also cleared by a read to the USART_DR register
  *          (USART_ReceiveData()).
  * @note   TC pending bit can be also cleared by software sequence: a read
  *          operation to USART_SR register (USART_GetITStatus()) followed by a write
  *          operation to USART_DR register (USART_SendData()).
  * @note   TXE pending bit is cleared only by a write to the USART_DR register
  *          (USART_SendData()).
  *
  * @retval None
  */
int32_t BUSART_ClearITPendingBit(BU_Id_t USARTx, BU_IrqSource_t USART_IT)
{
    USART_TypeDef* theUsart;
    uint16_t theIrq;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(USART_IT, STMF2_InterruptSources))
    {
        return (BU_ERROR_BAD_IRQ_SOURCE);
    }
    theIrq = STMF2_InterruptSources[(uint16_t) USART_IT];

    // call into the STM peripheral library
    USART_ClearITPendingBit(theUsart, theIrq);

    // USART_ClearITPendingBit does not return anything - guess it succeeded.
    return (BU_SUCCESS);

}


/**
  * @brief  Enables or disables the USART's DMA interface.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5 or 6 to select the USART or
  *         UART peripheral.
  * @param  USART_DMAReq: specifies the DMA request.
  *          This parameter can be any combination of the following values:
  *            @arg USART_DMAReq_Tx: USART DMA transmit request
  *            @arg USART_DMAReq_Rx: USART DMA receive request
  * @param  NewState: new state of the DMA Request sources.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
int32_t BUSART_DMACmd(BU_Id_t USARTx, BU_DmaReq_t DMAReqType, BU_State_t NewState)
{
    USART_TypeDef* theUsart;
    uint16_t theDmaType;
    FunctionalState theState;

    // validate and map types...
    if (BAD_TYPE_MAP(USARTx, STMF2_USARTS))
    {
        return (BU_ERROR_BAD_USART_ID);
    }
    theUsart = STMF2_USARTS[(uint16_t) USARTx];

    if (BAD_TYPE_MAP(DMAReqType, STMF2_DmaReqs))
    {
        return (BU_ERROR_BAD_DMA_REQ);
    }
    theDmaType = STMF2_DmaReqs[(uint16_t) DMAReqType];

    if (BAD_TYPE_MAP(NewState, STMF2_States))
    {
        return (BU_ERROR_BAD_STATE);
    }
    theState = STMF2_States[(uint16_t)NewState];

    // call into the STM peripheral library
    USART_DMACmd(theUsart, theDmaType, theState);

    // USART_DMACmd does not return anything - guess it succeeded.
    return (BU_SUCCESS);

}






