/**
  ******************************************************************************
  * @file    Diag_button_test.c
  *           + Diagnostic test for button
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f2xx.h"
#include "stm32f2xx_gpio.h"
#include "Diag_button_test.h"

#if (PRODUCT_HARDWARE_VARIANT == 0)
SCRIBE_DECL(diag);

/****************ISR routines start****************/
int gbl_btn_det[] = {0,0,0,0};
//button  gpio
//     1    B2
//     2    B9
//     3    D2
//     4    E10
static void ISR_button_Detect(void)
{
  uint32_t i,pr;
  uint32_t intmsk = 0;
  
    for (i = 0; i < 3; i++)
    {

        switch (i)
        {
            case 0:
                intmsk = EXTI_IMR_MR2;
                break;
            case 1:
                intmsk = EXTI_IMR_MR9;
                break;
            case 2:
                intmsk = EXTI_IMR_MR10;
                break;
    }
    
    pr = EXTI->PR;
    
        if (pr & intmsk)
        {
            EXTI->IMR &= ~intmsk;    // disable int
            if (i == 0)                //which is it? B2 or D2
            {
                if (!(GPIOB->IDR & intmsk))
                {
                    gbl_btn_det[0] = 1;     // set B2 flag
                }
                if (!(GPIOD->IDR & intmsk))
                {
                    gbl_btn_det[2] = 1;  // set D2 flag
                }
            }
            else
            {
                if (i == 1)
                {
                    gbl_btn_det[1] = 1;  // set the D9 flag
                }
                else
                {
                    gbl_btn_det[3] = 1;  // set the E10 flag
                }
            }
            EXTI->PR = intmsk;       // clear the interrupt
            EXTI->IMR |= intmsk;     // enable int
        }
    }
}

//enable or disable all 4 button interrupts
uint32_t enable_buttons_int_detect(int enable)
{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s %d", __FUNCTION__, enable);
    Interrupt_RegisterISR(EXTI2_IRQn, ISR_button_Detect);
    Interrupt_RegisterISR(EXTI9_5_IRQn, ISR_button_Detect);
    Interrupt_RegisterISR(EXTI15_10_IRQn, ISR_button_Detect);

    Interrupt_Enable(EXTI2_IRQn, 0, 0);     //id, priority, subPriority
    Interrupt_Enable(EXTI9_5_IRQn, 0, 0);
    Interrupt_Enable(EXTI15_10_IRQn, 0, 0);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource10);

    gbl_btn_det[0] = 0;  // INT B9
    gbl_btn_det[1] = 0;  // INT E10
    gbl_btn_det[2] = 0;  // INT B2
    gbl_btn_det[3] = 0;  // INT D2

    if (enable)
    {
        EXTI->IMR  |= EXTI_IMR_MR2;        // enable B2 and D2
        EXTI->IMR  |= EXTI_IMR_MR9;        // enable B9
        EXTI->IMR  |= EXTI_IMR_MR10;       // enable E10
    }
    else
    {
        EXTI->IMR  &= ~EXTI_IMR_MR2;       // disable B2 and D2
        EXTI->IMR  &= ~EXTI_IMR_MR9;       // disable B9
        EXTI->IMR  &= ~EXTI_IMR_MR10;      // disable E10
    }

    EXTI->FTSR |= EXTI_IMR_MR2;          // falling edge trigger
    EXTI->FTSR |= EXTI_IMR_MR9;          // falling edge trigger
    EXTI->FTSR |= EXTI_IMR_MR10;         // falling edge trigger

    return SUCCESS;
}

/****************ISR routines end****************/


//return a bit mask for the button pressed
int d_check_button_press(uint32_t btnmask)
{
    char bmsg[30];
    //button 1  - B2
    //button 2  - B9
    //button 3  - D2
    //button 4  - E10

    sprintf(bmsg, "press button ");
    if (btnmask & 1)
    {
        if (GPIOB->IDR & 0x0004)
        {
            strcat(bmsg, "1 ");
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Button 1 pressed");
            return 1;
        }
    }

    if (btnmask & 2)
    {
        if (GPIOB->IDR & 0x0200)
        {
            strcat(bmsg, "2 ");
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Button 2 pressed");
            return 2;
        }
    }

    if (btnmask & 4)
    {

        if (GPIOD->IDR & 0x0004)
        {
            strcat(bmsg, "3 ");
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Button 3 pressed");
            return 4;
        }
    }

    if (btnmask & 8)
    {
        if (GPIOE->IDR & 0x0400)
        {
            strcat(bmsg, "4 ");
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Button 4 pressed");
            return 8;
        }
    }
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", bmsg);

    return 0;
}
#endif
