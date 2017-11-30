/*
    File    :   diag_cec.h
    Author  :   Frank Mazzarella
    Created :   07/17/15
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Support for the CEC


===============================================================================
*/

#ifndef __DIAG_CEC_H__
#define __DIAG_CEC_H__

void CECCtrl_RST_L_GpioInit( void );
void CECCtrl_RST_L_GpioInitAsOutput( void );
void CECCtrl_RST_L_GpioInitAsInput( void );
void CECCtrl_RST_L_SetState( FunctionalState state );
uint8_t CECCtrl_RST_L_ToggleState(void);

void CECCtrl_INT_GpioInit( void );

void CEC_I2C_GpioInit( void );
uint8_t read_CECCtrl_INT_GpioPin( void );
#endif /* __DIAG_CEC_H__ */


