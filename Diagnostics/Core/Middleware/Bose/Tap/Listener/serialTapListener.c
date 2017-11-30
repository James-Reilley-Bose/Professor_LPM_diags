#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "project.h"
#include "etap.h"
#include "TapParserAPI.h"
#include "serialTapListener.h"
#include "TapListenerAPI.h"
#include "SystemAnalysis.h"
#include "ascii.h"
#include "BoseUSART_API.h"
#include "rottenlog.h"

SCRIBE_DECL(tap_events);
SCRIBE_DECL(system_events);

void Listener_TAPPrintChar(const char c);
void Listener_PrintString(const char* s);
ManagedTask* ManagedTapParserTask = NULL;
static inline uint8_t Listener_Handler(BU_Id_t buart);
static SemaphoreHandle_t tapMutex;
#define TAP_MUTEX_WAIT TIMER_MSEC_TO_TICKS(100)

static const BU_InitTypeDef buartInit =
{
    TAP_BAUD,  // See config
    WL_8BITS,  // 8 bit
    SB_1,      // 1 stop bit
    P_NONE,    // No parity
    M_RX_TX,   // RX and TX
    FC_NONE    // No Flow Control
};

//Global buffer to hold comand that is entered
static char TAPCommandBuffer[TAP_NUM_LINES_BUFFERED][TAP_COMMAND_BUFFER_SIZE];

/*******************************************************************************
Function: Init_Listener_UART()

Returns:  N/A

Description:
Initialize the TAP uart.

Called by:  SerialTapListenerModule_Init()

********************************************************************************/
void Init_Listener_UART()
{
    // Setup interupt on the UART
    BUSART_PlatformBindInterruptHandler(TAP_BUART, Listener_IRQHandler, TRUE);

    // GPIO Init
    ConfigureGpioPin (TAP_UART_RX_GPIO_BANK, TAP_RX_PIN, GPIO_MODE_AF_IN_FLOATING, TAP_UART_GPIO_AF); //changed to GPIO_MODE_AF_IN_FLOATING
    ConfigureGpioPin (TAP_UART_TX_GPIO_BANK, TAP_TX_PIN, GPIO_MODE_AF_OUT_PP, TAP_UART_GPIO_AF);

    // Configure the UART for tap
    BUSART_DeInit(TAP_BUART);
    BUSART_Init(TAP_BUART, &buartInit);

    BUSART_ITConfig(TAP_BUART, IRQ_RXNE, PS_ENABLE);
    BUSART_ITConfig(TAP_BUART, IRQ_TXE, PS_DISABLE);
    BUSART_Cmd(TAP_BUART, PS_ENABLE);

    // Create Mutex
    tapMutex = xSemaphoreCreateMutex();
    debug_assert(tapMutex);
    xSemaphoreGive(tapMutex);

    // Enable Local Echo in TeraTerm
    Listener_PrintString("\x1b[12l");
}


/*******************************************************************************
Function: Listener_IRQHandler()

Returns:  N/A

Description: Calls listener handler

Called by:  UART5's interupt handler

********************************************************************************/
void Listener_IRQHandler(void)
{
    INTERRUPT_RUN_LATENCY_BEGIN(TAP_UART_ISR);
    Listener_Handler(TAP_BUART);
    INTERRUPT_RUN_LATENCY_END(TAP_UART_ISR, 0);
}

/*******************************************************************************
Function: Listener_IRQHandler()

Returns:  N/A

Description: Checks for overrun uart and if not posts message to listener to
process rxd byte.

Called by:  Listener_IRQHandler()

********************************************************************************/
static inline uint8_t Listener_Handler(BU_Id_t buart)
{
    uint8_t nextByte = 0;
    static portBASE_TYPE xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = pdFALSE;

    if (BUSART_OVERRUN_ERROR(buart))
    {
        /* The overrun error is cleared by a software sequence (read to USART_SR followed
         * by read to the USART_DR register). The BUSART_OVERRUN_ERROR macro already
         * indirectly reads the USART_SR register, so read the USART_DR register
         * to clear the error, ignore the return value
         */

        uint16_t temp = BUSART_ReceiveData(buart);
        xHigherPriorityTaskWoken = TapListenerPostOverrunError();
    }

    if (BUSART_BYTE_RECEIVED(buart))
    {
        Listener_ProcessRxdByte(BUSART_ReceiveData(buart));
    }
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

    return nextByte;
}

/*******************************************************************************
Function:   Listener_ProcessRxdByte()

Returns:    N/A

Description:
This is the TAP command parser. It is meant to be run as a task in
an operating system. Note that it can use a lot of processor time when
executing TAP commands, so it really should be part of a multitasked system.

Globals:    UartBuild[ETAP_SOFTUART].SoftReceiveBuffer

Locals:     TAPCommandBufferIndex
TAPCommandBuffer[]

Calls:      FIFOB_IsNotEmpty()
FIFOB_Get()

Called by:  Operating System.

********************************************************************************
*/
void Listener_ProcessRxdByte (uint8_t NextChar)
{
    static uint16_t bufferIndex = 0;
    static uint8_t bufferNum = 0;

    switch (NextChar)
    {
        case ASCII_CARRIAGE_RETURN:
            {
                if (bufferIndex != 0) // if this completes a command line
                {
                    /* Null terminate the line */
                    TAPCommandBuffer[bufferNum][bufferIndex] = '\0';

                    /* Send off the line to be processed */
                    TapParserPostTapCommand((uint32_t)TAPCommandBuffer[bufferNum], TapListener_PrintPrompt);

                    /* Setup to use next buffer */
                    bufferNum = (bufferNum + 1) % TAP_NUM_LINES_BUFFERED;
                    bufferIndex = 0;
                }
                else // empty line, just print out a new prompt
                {
                    TapListener_PrintPrompt(NULL, 0);
                }
            }
            break;
        case ASCII_LINE_FEED: // ignore these! we don't want double prompts.
            break;
        case ASCII_BACKSPACE: // delete last character
            {
                if (bufferIndex > 0)
                {
                    TAPCommandBuffer[bufferNum][--bufferIndex] = 0;
                }
            }
            break;
        case ASCII_SPACE:
            {
                if (bufferIndex == 0) //ignore leading spaces
                {
                    break;
                }
            }
        // else fall-through
        default: // insert character into buffer if not full
            {
                if (bufferIndex < TAP_COMMAND_BUFFER_SIZE - 1)
                {
                    TAPCommandBuffer[bufferNum][bufferIndex] = NextChar;
                    bufferIndex++;
                }
                else
                {
                    LOG(tap_events, ROTTEN_LOGLEVEL_NORMAL, "TAP command buffer filled. Dropped character (%c).", NextChar);
                }
            }
            break;
    }
}

/*******************************************************************************
Function:   Listener_TAPPrintChar()
Returns:    N/A
Description:    output a character to the internal TAP
Globals:
Locals:
Calls:
Called by:  any routine that wishes to print a character to the serial port.
********************************************************************************/
void Listener_TAPPrintChar(const char c)
{
    while ( !BUSART_TRANSMIT_COMPLETE(TAP_BUART) )
    {
        if (!InInterrupt())
        {
        }
    }
    BUSART_SendData(TAP_BUART, c);
}

/*******************************************************************************
Function:   Listener_PrintString()
Returns:    N/A
Description: Outputs a string to tap (no mutex)
Globals:
Locals:
Calls:
Called by:any routine that wishes to print a string to the serial port.
*******************************************************************************/
void Listener_PutString(const char* s)
{
    debug_assert(s);
    while (s && *s)
    {
        char newchar = *s;
        char nextchar = *(s + 1);
        switch (newchar)
        {
            case '\r':
            case '\n':
                {
                    if (((newchar == '\r') && (nextchar == '\n')) || ((newchar == '\n') && (nextchar == '\r')))
                    {
                        s++;
                    }
                    char* newline = TAP_NEW_LINE;
                    while (*newline)
                    {
                        Listener_TAPPrintChar(*newline++);
                    }
                    break;
                }
            default:
                Listener_TAPPrintChar(newchar);
                break;
        }
        s++;
    }
}

/*******************************************************************************
Function:   Listener_PrintString()
Returns:    N/A
Description: Outputs a string to tap (with a mutex)
Globals:
Locals:
Calls:
Called by:  any routine that wishes to print a string to the serial port.
*******************************************************************************/
void Listener_PrintString(const char* s)
{
    // Don't print from an ISR
    debug_assert(!InInterrupt());
    debug_assert(s);

    if (xSemaphoreTake(tapMutex, TAP_MUTEX_WAIT) == pdTRUE)
    {
        Listener_PutString(s);
        xSemaphoreGive(tapMutex);
    }
}

#ifdef BARDEEN
/*******************************************************************************
Function:   Listener_LowPowerStandby()
Returns:    N/A
Description:   To meet power requirements, we need to disable the TX pin, which
               feeds an N-MOSFET. It has a pulldown, so we can let it float. Since
               we are going between input and Alt func, we only need to change the
               MODER register. All else stays the same.
Globals:
Locals:
Calls:
Called by:  power recipe for Bardeen
*******************************************************************************/
void Listener_EnableLowPowerStandby(BOOL enableStby)
{
    GPIO_TypeDef* gpio = (GPIO_TypeDef*)TAP_UART_TX_GPIO_BANK;
    uint32_t mode = (enableStby == TRUE) ? GPIO_Mode_IN : GPIO_Mode_AF;

    portENTER_CRITICAL();
    uint32_t regVal = gpio->MODER;

    gpio->MODER = (regVal & ~(GPIO_MODER_MODER0 << (TAP_TX_PIN * 2))) | (mode << (TAP_TX_PIN * 2));
    portEXIT_CRITICAL();

}
#endif // BARDEEN

/* ****************************************************
 * Function Name: TapGetChar
 *
 * Parameters:  None
 *
 * Description: gets a single character.
 *              The return value is a 16-bit quantity
 *              due to driver requirements.
 *
 * Returns:  a single character if detected, else
 *           no character.
 *
 * *****************************************************/
uint16_t TapGetChar(void)
{
    uint16_t ch = TAP_NO_CHAR;
    if (USART_GetFlagStatus(TAP_UART, USART_FLAG_RXNE))
    {
        ch = USART_ReceiveData(TAP_UART);
    }
    return (ch);
}
