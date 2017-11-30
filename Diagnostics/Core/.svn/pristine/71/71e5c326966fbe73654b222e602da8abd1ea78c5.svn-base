////////////////////////////////////////////////////////////////////////////////
/// @file            cc2500_platform.h
/// @brief           This file specifies the platform specific interfacing
///                   of the cc2500 driver, this is the Bardeen LPM version.
/// @author          dx1007255
/// @date            Creation Date: Thu March 24 2015
///
/// Copyright 2015 Bose Corporation
/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifndef CC2500_PLATFORM_H
#define CC2500_PLATFORM_H

#include "project.h"

#define CHIPCON_SPI_DEFAULT_TIMEOUT_MS          100

typedef void (*chipconFuncptr)(int);

void ChipconSPI_Initialize();

uint16_t ChipconSpi_Communicate(const unsigned DataLength,
                                const uint8_t* pDataToSend,
                                uint8_t* pDataReceived,
                                void (*pCallBackFunction)(int));


uint16_t ChipconSpi_Communicate_Blocking(const unsigned DataLength,
        const uint8_t* pDataToSend,
        uint8_t* pDataReceived);


void ChipconSpi_CommunicateComplete();

// pin configuration functions
////////////////////////////////////////////////////////////////////////////////
/// @brief Set up the GDO2 input pin a a GPIO input
///
/// @param
/// @return
////////////////////////////////////////////////////////////////////////////////
static inline void  CC2500_ConfigureGDO2AsInput(void)
{
    ConfigureGpioPin(CHIPCON_MSG_INT_GPIO_BANK,
                     CHIPCON_MSG_INT_GPIO_PIN,
                     GPIO_MODE_IN_FLOATING,
                     0 );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Configure the GDO2 line to trigger an interrupt on the falling edge.
///
/// @param
/// @return
////////////////////////////////////////////////////////////////////////////////
static inline void  CC2500_ConfigureGDO2IntFallingEdge(void)
{
    /*
    *   In stm32f2xx_exti.c, there is :
    *          In order to use an I/O pin as an external interrupt source, follow
    *          steps below:
    *            1- Configure the I/O in input mode using GPIO_Init()
    *            2- Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig()
    *            3- Select the mode(interrupt, event) and configure the trigger
    *               selection (Rising, falling or both) using EXTI_Init()
    *            4- Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init()
    */

    // step 1,   ConfigureGpioPin() calls GPIO_Init()
    ConfigureGpioPin(CHIPCON_MSG_INT_GPIO_BANK,
                     CHIPCON_MSG_INT_GPIO_PIN,
                     GPIO_MODE_IN_FLOATING,
                     0 );
    //step 2
    SYSCFG_EXTILineConfig (CHIPCON_MSG_INT_EXTI_PORT_SOURCE, CHIPCON_MSG_INT_LINE_NUM);

    // step 3.
    EXTI_InitTypeDef    EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Line    = CHIPCON_MSG_INT_GPIO_PIN_MASK;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    // step 4
    // Register the ISR
    Interrupt_RegisterISR(CHIPCON_MSG_INTERRUPT_CHANNEL, CHIPCON_EXTI_HANDLER);
}

// the functions used to turn on or off the GDO0 interrupt from firing
////////////////////////////////////////////////////////////////////////////////
/// @brief Enable the GDO2 interrupt to fire
///   may be called from both task and ISR context.
///
/// @param
/// @return
////////////////////////////////////////////////////////////////////////////////
static inline void  CC2500_EnableGDO2Interrupt(void)
{
    EXTI_INTERRUPT_ENABLE(CHIPCON_MSG_INT_LINE_NUM);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief disable the GDO2 interrupt from firing
///   will be called from both task and ISR context.
///
/// @param
/// @return
////////////////////////////////////////////////////////////////////////////////
static inline void  CC2500_DisableGDO2Interrupt(void)
{
    EXTI_INTERRUPT_DISABLE(CHIPCON_MSG_INT_LINE_NUM);
}

// system timer interface
typedef measure_time_t CC2500_SystemTime_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief get the number of micro seconds the system has been running
///  this can reutrn any free running counter of micro seconds in your system
///
/// @param
/// @return the numer of micro seconds representing "now".
////////////////////////////////////////////////////////////////////////////////

static inline CC2500_SystemTime_t CC2500_GetSystemTime_uS(void)
{
    return GET_MEASURE_TIME_NOW();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief return the amount of micro seconds that have passed since the time
///   given in the parameter.
///
/// @param a_start - the time in micro seconds you wish to measure from
/// @return the amount of micro seconds that have passed since a_start.
////////////////////////////////////////////////////////////////////////////////
static inline CC2500_SystemTime_t CC2500_GetSystemTimeSince_uS(
    const CC2500_SystemTime_t a_start)
{
    return MEASURE_TIME_SINCE(a_start);
}

// spi interfacing for the CC2500

////////////////////////////////////////////////////////////////////////////////
/// @brief Write bytes using the spi bus to the cc2500
///
/// @param a_pBuff - the buffer of bytes.
/// @param a_len - then number of bytes contained in a_pBuff.
/// @param a_doneCB - the function to call when the transfer completes.
/// @return
////////////////////////////////////////////////////////////////////////////////
static inline void CC2500_SPIWrite(
    const uint8_t* a_pBuff,
    const uint8_t a_len,
    void (*a_doneCB)(int))
{
    ChipconSpi_Communicate( a_len, a_pBuff, (uint8_t*)0, a_doneCB);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Write bytes using the spi bus to the cc2500 and reads back the
///   bytes from the cc2500
///
/// @param a_pBuff - the buffer of bytes.
/// @param a_pReply - the buffer to store the reply butes in.
/// @param a_len - then number of bytes contained in a_pBuff.
/// @param a_doneCB - the function to call when the transfer completes.
/// @return
////////////////////////////////////////////////////////////////////////////////
static inline void CC2500_SPIWriteAndRead(
    const uint8_t* a_pBuff,
    uint8_t* a_pReply,
    const uint8_t a_len,
    void (*a_doneCB)(int))
{
    ChipconSpi_Communicate(
        a_len,
        a_pBuff,
        a_pReply,
        a_doneCB);
}

#endif // CC2500_PLATFORM_H
