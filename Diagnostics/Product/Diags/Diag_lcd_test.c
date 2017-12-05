/**
  ******************************************************************************
  * @file    Diag_lcd_test.c
  *           + Diagnostic test for LCD
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f2xx_gpio.h"
#include "Diag_lcd_test.h"

#if (PRODUCT_HARDWARE_VARIANT == 0)
SCRIBE_DECL(diag);

float pwm_percent=0.0;

//Configure the LCD BACKLIGHT pin for PWM
void config_TIM1_pwm_ch3(int enable, uint16_t* frq_pwm)
{
    uint16_t ccr3;
    float arrflt = 0.0, clkpsc = 0.0;

    clkpsc =  (CLKPCS / LCD_BCKLIGHT_PSC) * 2;
    arrflt = (uint16_t)(clkpsc / frq_pwm[0]);
    ccr3 = (uint16_t)(arrflt * (pwm_percent / 100));

    if (enable)
    {
        LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "\r\nenable tim1ch3\r\n\n");

        TIM1->CCMR1 = 0x0068;   //OC1M=6, OC1E
        TIM1->CCMR2 = 0x0068;   //OC3M=6, OC3E
        TIM1->CCER  = 0x0101;   //CC3E  enable channel 3
        TIM1->PSC   = (uint16_t)LCD_BCKLIGHT_PSC - 1;
        TIM1->ARR   = (uint16_t)arrflt;      //Freq
        TIM1->CCR1  = ccr3;     //PWM
        TIM1->CCR3  = ccr3;     //PWM
        TIM1->CR1   = 0x81;     //auto preload, en
        TIM1->BDTR  = 0x8000;   //MOE
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\ndisable tim1ch3\r\n\n");
        TIM1->CR1   = 1;     //ARPE off
        TIM1->CR1   = 0;     //disable
    }
}

void lcd_backlight(uint16_t* lbarg)
{
    int enable = 1;
    GPIO_InitTypeDef gp;
    LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "\r\n%s frq[%d] pwm[%d]\r\n\n",
        __FUNCTION__, lbarg[0], lbarg[1]);

    gp.GPIO_Mode  = GPIO_Mode_AF;
    gp.GPIO_OType = GPIO_OType_PP;
    gp.GPIO_Speed = GPIO_Speed_50MHz;
    gp.GPIO_Pin   = 0x1 << LCD_BACKLIGHT_PIN;

    GPIO_Init(LCD_BACKLIGHT_BANK, &gp);
    GPIO_PinAFConfig(LCD_BACKLIGHT_BANK, LCD_BACKLIGHT_PIN, GPIO_AF_TIM1);

    if (lbarg[0] == 0)
    {
        enable = 0;
    }
    config_TIM1_pwm_ch3(enable, lbarg);
}

void set_led4_af(void)
{
    GPIO_InitTypeDef ioinit;

    ioinit.GPIO_Mode = GPIO_Mode_AF;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    ioinit.GPIO_Pin = 0x1 << LED4_GPIO_PIN;
    GPIO_Init(LED4_GPIO_BANK, &ioinit);

    GPIO_PinAFConfig(LED4_GPIO_BANK, LED4_GPIO_PIN, GPIO_AF_TIM1);
}

// this bit of code interperates a string with a decimal point
// and converts it to an integer and fractional part
// to allow user input of a percentage with two characters of precision
// to the right of the decimal point
void strtoflt(char* inptstr, uint32_t*  arg)
{
    float prcnt = 0;
    prcnt = strtof(inptstr, NULL);
    pwm_percent = prcnt; //update global
    arg[1] = (uint32_t)prcnt;  //integer part
    arg[2] = ((uint32_t)(prcnt * 10000) - ((uint32)prcnt) * 10000); //fractional part
}

float get_pwm_percent(void)
{
    return pwm_percent;
}

void set_pwm_percent(char* inptstr)
{
    pwm_percent = strtof(inptstr, NULL);
}
#endif