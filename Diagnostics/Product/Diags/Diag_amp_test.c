/**
  ******************************************************************************
  * @file    Diag_amp_test.h
  *           + Diagnostic test for AMP
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stm32f2xx_gpio.h"
#include "DiagsDispatcher.h"
#include "Diag_amp_test.h"

#if (PRODUCT_HARDWARE_VARIANT == 0)
void amp_init(void)
{
    GPIO_TypeDef* GPIOx = AMP_FAULT_BANK;
    GPIO_InitTypeDef ioinit;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    ioinit.GPIO_Mode =  GPIO_Mode_IN;

    ioinit.GPIO_Pin = 0x1 << AMP_FAULT_PIN;
    GPIO_Init(GPIOx, &ioinit);

    ioinit.GPIO_Pin = 0x1 << AMP_OFFSET_FAULT_PIN;
    GPIO_Init(GPIOx, &ioinit);

}
#endif
