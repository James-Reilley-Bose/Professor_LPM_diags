
/**
  ******************************************************************************
  * @file    Diag_led_test.h
  *           + Diagnostic test for LED
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_LED_TEST_H
#define DIAG_LED_TEST_H

#define LED_HELP1 "led <x>,<on,off,t>  - set led state"
#define LED_HELP2 "\t    <x> = 1,2,3,4, 5-all"
#define LED_HELP LED_HELP1 LED_HELP2

typedef struct
{
    GPIO_TypeDef* bank;
    uint32_t pin;
} LEDGPIO;

uint32_t dled(int argc, char *argv[]);
void led_ctrl(int led_cmd, int mode);


#endif

/* ======== END OF FILE ======== */
