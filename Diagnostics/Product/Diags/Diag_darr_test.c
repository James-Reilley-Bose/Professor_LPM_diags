/**
  ******************************************************************************
  * @file    Diag_darr_test.h
  *           + Diagnostic test for DARRS
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stm32f2xx_gpio.h"
#include "DiagsDispatcher.h"
#include "Diag_amp_test.h"


void darr_init(void)
{
    GPIO_TypeDef* GPIOx = PROF_LLR_RST_L_BANK;
    GPIO_InitTypeDef ioinit;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    ioinit.GPIO_Mode =  GPIO_Mode_OUT;

    ioinit.GPIO_Pin = 0x1 << PROF_LLR_RST_L_PIN;
    GPIO_Init(GPIOx, &ioinit);

    GPIO_WriteBit(GPIOx, ioinit.GPIO_Pin, Bit_SET); //Take LLR out of reset
}
