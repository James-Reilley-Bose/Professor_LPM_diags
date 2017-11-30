/*
    File    :   diag_cec.c
    Author  :   Frank Mazzarella
    Created :   07/17/15
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Support for the CEC


===============================================================================
*/

#include "configuresystem.h"
#include "stm32f2xx_gpio.h"
#include "etap.h"
#include "TapListenerAPI.h"
#include "diag_cec.h"


void CECCtrl_RST_L_GpioInit( void )
{
    ConfigureGpioPin( CEC_CTRL_RST_L_GPIO_BANK,
                      CEC_CTRL_RST_L_GPIO_PIN,
                      GPIO_MODE_OUT_OD,
                      NO_ALT_FUNC );
}
void CECCtrl_RST_L_GpioInitAsOutput( void )
{
    ConfigureGpioPin( CEC_CTRL_RST_L_GPIO_BANK,
                      CEC_CTRL_RST_L_GPIO_PIN,
                      GPIO_MODE_OUT_PP,
                      NO_ALT_FUNC );
}
void CECCtrl_RST_L_GpioInitAsInput( void )
{
    ConfigureGpioPin( CEC_CTRL_RST_L_GPIO_BANK,
                      CEC_CTRL_RST_L_GPIO_PIN,
                      GPIO_MODE_IN_FLOATING,
                      NO_ALT_FUNC );
}
void CECCtrl_RST_L_SetState( FunctionalState state )
{
    if (state == ENABLE)
    {

        GPIO_WriteBit( CEC_CTRL_RST_L_GPIO_BANK,
                       CEC_CTRL_RST_L_GPIO_PIN_MASK,
                       Bit_SET );
    }
    else
    {
        GPIO_WriteBit( CEC_CTRL_RST_L_GPIO_BANK,
                       CEC_CTRL_RST_L_GPIO_PIN_MASK,
                       Bit_RESET );
    }
}
uint8_t CECCtrl_RST_L_ToggleState(void)
{
    GPIO_ToggleBits( CEC_CTRL_RST_L_GPIO_BANK,
                     CEC_CTRL_RST_L_GPIO_PIN_MASK );

    // wait for bit to settle
    Delay_ms(GPIO_READ_DELAY_MS);

    return ( GPIO_ReadInputDataBit( CEC_CTRL_RST_L_GPIO_BANK,
                                    CEC_CTRL_RST_L_GPIO_PIN_MASK ));
}
//-------------------------------------------------------------------
// Function:  void CECCtrl_INT_GpioInit( void )
//
// Description:
//
//
//--------------------------------------------------------------------
void CECCtrl_INT_GpioInit( void )
{
    ConfigureGpioPin( CEC_CTRL_INT_GPIO_BANK,
                      CEC_CTRL_INT_GPIO_PIN,
                      GPIO_MODE_IN_PULLDN,
                      NO_ALT_FUNC );
}
void CEC_I2C_GpioInit( void )
{
    ConfigureGpioPin( CEC_SCL_GPIO_BANK,
                      CEC_SCL_GPIO_PIN,
                      GPIO_MODE_AF_OUT_OD_2MHZ,
                      GPIO_AF_I2C3 );

    ConfigureGpioPin( CEC_SDA_GPIO_BANK,
                      CEC_SDA_GPIO_PIN,
                      GPIO_MODE_AF_OUT_OD_2MHZ,
                      GPIO_AF_I2C3 );

    /* drive SCL+SDA high to "off" state */
    GPIO_WriteBit( CEC_SCL_GPIO_BANK,
                   1 << CEC_SCL_GPIO_PIN,
                   Bit_SET );

    GPIO_WriteBit( CEC_SDA_GPIO_BANK,
                   1 << CEC_SDA_GPIO_PIN,
                   Bit_SET );

}
uint8_t read_CECCtrl_INT_GpioPin( void )
{
    return ( GPIO_ReadInputDataBit( CEC_CTRL_INT_GPIO_BANK,
                                    CEC_CTRL_INT_GPIO_PIN_MASK ));
}

