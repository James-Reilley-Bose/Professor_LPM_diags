/**
  ******************************************************************************
  * @file    Diag_led_test.c
  *           + Diagnostic test for LED
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stm32f2xx_gpio.h"
#include "Diag_led_test.h"

static int dled_init = 1;

SCRIBE_DECL(diag);

void led_gpio_config(void)
{
    uint32_t i;
    GPIO_TypeDef* GPIOx;
    GPIO_InitTypeDef ioinit;


    LEDGPIO led_init[] =
    {
        {LED1_GPIO_BANK,    LED1_GPIO_PIN},
    };


    ioinit.GPIO_Mode = GPIO_Mode_OUT;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;


    for (i = 0; i < (sizeof(led_init) / sizeof(LEDGPIO)); i++)
    {
        GPIOx = led_init[i].bank;
        ioinit.GPIO_Pin = 0x1 << led_init[i].pin;
        GPIO_Init(GPIOx, &ioinit);
    }
}

void led_ctrl(int led_cmd, int mode)
{
    GPIO_TypeDef* GPIOx = NULL;
    uint16_t GPIO_PinMask = 0;
    int led;

    if (dled_init)
    {
        led_gpio_config();
        dled_init = 0;
    }


    led = led_cmd;
    GPIOx = LED1_GPIO_BANK;
    GPIO_PinMask = 0x1 << LED1_GPIO_PIN;

    if (!(mode & 0x10))
    {
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"led %d ", led);
    }
    switch (mode & 0xf)
    {
    case 0:
      if (!(mode & 0x10))
      {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"off\n");
      }
      GPIO_WriteBit(GPIOx, GPIO_PinMask, Bit_RESET);
      break;
    case 1:
      if (!(mode & 0x10))
      {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"on\n");
      }
      GPIO_WriteBit(GPIOx, GPIO_PinMask, Bit_SET);
      break;
    case 2:
      if (!(mode & 0x10))
      {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"tgl\n");
      }
      GPIO_ToggleBits(GPIOx, GPIO_PinMask);
      break;
    }
}

uint32_t dled(int argc, char* argv[])
{
    int led = 0, mode = 0;
    //  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s\n", __FUNCTION__);

    if (dled_init)
    {
        led_gpio_config();
        dled_init = 0;
    }
    if (argc > 0)
    {
        led = (argv[0][0]) - '0';
        if (led > 0)
        {

            if (argc > 1)
            {
                if (!strncmp(argv[1], "on", 2))
                {
                    mode = 1;
                }
                else if (!strncmp(argv[1], "t", 1))
                {
                    mode = 2;
                }
                led_ctrl(led, mode);
                return SUCCESS;
            }
        }
    }
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"invalid arg\n\r");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", LED_HELP);
    return SUCCESS;
}