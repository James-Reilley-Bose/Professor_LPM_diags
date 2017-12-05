/**
  ******************************************************************************
  * @file    Diag_gpio_test.c
  *           + Diagnostic test for GPIO
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stdlib.h"
#include "Diag_gpio_test.h"

SCRIBE_DECL(diag);

LPM_PIN_DEFS lpmpindefs[] =
{
    { 23, "PA00", "GPIO_PA0_LED1",                 GPIO_Mode_OUT},
    { 24, "PA01", "RIV_8017_LPM_CTS",               GPIO_Mode_IN},
    { 25, "PA02", "ENET_PME_IRQ_L",                 GPIO_Mode_IN},
    { 26, "PA03", "LPM_TO_487_RST",                GPIO_Mode_OUT},
    { 29, "PA04", "ADC_VSYS_SENSE1",                GPIO_Mode_IN},
    { 30, "PA05", "UCFLASH_SPI1_SCK",              GPIO_Mode_OUT},
    { 31, "PA06", "UCFLASH_SPI1_MISO",              GPIO_Mode_IN},
    { 32, "PA07", "UCFLASH_SPI1_MOSI",             GPIO_Mode_OUT},
    { 67, "PA08", "I2C3_SCL",                       GPIO_Mode_AF},
    { 68, "PA09", "USART1_LPM_TAP_TX",              GPIO_Mode_AF},
    { 69, "PA10", "USART1_LPM_TAP_RX",              GPIO_Mode_AF},
    { 70, "PA11", "RIV_LPM_8017_IRQ",              GPIO_Mode_OUT},
    { 71, "PA12", "APQ_LVLSHFTR_EN",               GPIO_Mode_OUT},
    { 72, "PA13", "PA13_UC_TMS",                    GPIO_Mode_AF},
    { 76, "PA14", "PA14_UC_TCK",                    GPIO_Mode_AF},
    { 77, "PA15", "PA15_UC_TDI",                    GPIO_Mode_AF},
    { 00, "PS00", "",                               GPIO_Mode_IN},

    { 35, "PB00", "ADC_THERM1",                     GPIO_Mode_IN},
    { 36, "PB01", "ADC_THERM2",                     GPIO_Mode_IN},
    { 37, "PB02", "BOOT1",                          GPIO_Mode_IN},
    { 89, "PB03", "JTDO",                           GPIO_Mode_IN},
    { 90, "PB04", "NJRST",                          GPIO_Mode_IN},
    { 91, "PB05", "RF_RMT_SPI3_MOSI",               GPIO_Mode_AF},
    { 92, "PB06", "MIC_MUTE",                       GPIO_Mode_IN},
    { 93, "PB07", "I2C1_EARC_SDA",                  GPIO_Mode_AF},
    { 95, "PB08", "I2C1_EARC_SCL",                  GPIO_Mode_AF},
    { 96, "PB09", "BUTTON2",                        GPIO_Mode_IN},
    { 47, "PB10", "I2C2_LLR_SCL",                   GPIO_Mode_AF},
    { 48, "PB11", "I2C2_LLR_SDA",                   GPIO_Mode_AF},
    { 51, "PB12", "RIV_SPI2_CS",                    GPIO_Mode_IN},
    { 52, "PB13", "RIV_SPI2_SCK",                   GPIO_Mode_AF},
    { 53, "PB14", "RIV_CBL_PWR",                   GPIO_Mode_OUT},
    { 54, "PB15", "RIV_KYPD_PWR",                  GPIO_Mode_OUT},
    { 00, "PS00", "",                               GPIO_Mode_IN},

    { 15, "PC00", "487_RECOVERY_L",                 GPIO_Mode_IN},
    { 16, "PC01", "PSOC_RESET_L",                  GPIO_Mode_OUT},
    { 17, "PC02", "RIV_SPI2_MISO",                  GPIO_Mode_AF},
    { 18, "PC03", "RIV_SPI2_MOSI",                  GPIO_Mode_AF},
    { 33, "PC04", "UCFLASH_SPI1_CS_L",             GPIO_Mode_OUT},
    { 34, "PC05", "UCFLASH_SPI1_WP_L",             GPIO_Mode_OUT},
    { 63, "PC06", "USART6_LPM_TO_DSP1",             GPIO_Mode_AF},
    { 64, "PC07", "USART6_DSP1_TO_LPM",             GPIO_Mode_AF},
    { 65, "PC08", "CECCTRL_IRQ",                    GPIO_Mode_IN},
    { 66, "PC09", "I2C3_SDA",                       GPIO_Mode_AF},
    { 78, "PC10", "RF_RMT_SPI3_CLK",                GPIO_Mode_AF},
    { 79, "PC11", "RF_RMT_SPI3_MISO",               GPIO_Mode_AF},
    { 80, "PC12", "HDMI_EARC_RST_L",               GPIO_Mode_OUT},
    {  7, "PC13", "HDIM_5V_TVOUT_EN",              GPIO_Mode_OUT},
    {  8, "PC14", "BLE_RMT_RST_L",                 GPIO_Mode_OUT},
    {  9, "PC15", "PWR_HDMI_EN",                   GPIO_Mode_OUT},
    { 00, "PS00", "",                               GPIO_Mode_IN},

    { 81, "PD00", "LLR_RST_L",                     GPIO_Mode_OUT},
    { 82, "PD01", "RIV_SYSCLK_EN",                 GPIO_Mode_OUT},
    { 83, "PD02", "PWR_RIVIERA_EN",                GPIO_Mode_OUT},
    { 84, "PD03", "PSOC_IRQ",                       GPIO_Mode_IN},
    { 85, "PD04", "PFAIL_IRQ_L",                    GPIO_Mode_IN},
    { 86, "PD05", "MIC_DETECT",                     GPIO_Mode_IN},
    { 87, "PD06", "LLR_IRQ",                        GPIO_Mode_IN},
    { 88, "PD07", "CECCTRL_RST_L",                 GPIO_Mode_OUT},
    { 55, "PD08", "USART3_LPM_TO_IR",               GPIO_Mode_AF},
    { 56, "PD09", "USART3_IR_TO_LPM",               GPIO_Mode_AF},
    { 57, "PD10", "IR_RST_L",                      GPIO_Mode_OUT},
    { 58, "PD11", "RIV_8017_LPM_RST",               GPIO_Mode_IN},
    { 59, "PD12", "LPSTBY_L",                      GPIO_Mode_OUT},
    { 60, "PD13", "HDMI_EARC_IRQ",                  GPIO_Mode_IN},
    { 61, "PD14", "BLE_RMT_CS_L",                  GPIO_Mode_OUT},
    { 62, "PD15", "RIV_PS_HOLD",                    GPIO_Mode_IN},
    { 00, "PS00", "",                               GPIO_Mode_IN},

    { 97, "PE00", "DSP_LVLSHFTR_EN_L",              GPIO_Mode_IN},
    { 98, "PE01", "RIV_FORCE_USB_BOOT",            GPIO_Mode_OUT},
    {  1, "PE02", "UC_TRACE_CLK",                   GPIO_Mode_AF},
    {  2, "PE03", "UC_TRACE_D0",                    GPIO_Mode_AF},
    {  3, "PE04", "UC_TRACE_D1",                    GPIO_Mode_AF},
    {  4, "PE05", "UC_TRACE_D2",                    GPIO_Mode_AF},
    {  5, "PE06", "UC_TRACE_D3",                    GPIO_Mode_AF},
    { 38, "PE07", "RIV_PMIC_RESIN",                GPIO_Mode_OUT},
    { 39, "PE08", "RIVIERA_PON_RESET",              GPIO_Mode_IN},
    { 40, "PE09", "CONSOLE_DE_TX",                 GPIO_Mode_OUT},
    { 41, "PE10", "BASSBOX_DC_RX",                  GPIO_Mode_IN},
    { 42, "PE11", "RIVIERA_PON_1_1",               GPIO_Mode_OUT},
    { 43, "PE12", "PWR_EN_VAMP",                   GPIO_Mode_OUT},
    { 44, "PE13", "NC",                             GPIO_Mode_IN},
    { 45, "PE14", "IR_DATA_IN",                     GPIO_Mode_IN},
    { 46, "PE15", "NWSTBY_L",                      GPIO_Mode_OUT},
    { 00, "PS00", "",                              GPIO_Mode_OUT},
};

void bb_show_all_gpio_pins(void)
{
  char fs[64], ss[64];
  uint32_t i=0, pin, pinval, moder, idr, odr, pup;
  GPIO_TypeDef *GPIOx;

  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PIN: mode  level       pull signal name");
  
  for(i=0;i<sizeof(lpmpindefs)/sizeof(LPM_PIN_DEFS);i++)
  {
    
    memset(fs, 0, sizeof(fs));
    
    switch(lpmpindefs[i].name[1])
	{
    case 'A': GPIOx = GPIOA; break;
    case 'B': GPIOx = GPIOB; break;
    case 'C': GPIOx = GPIOC; break;
    case 'D': GPIOx = GPIOD; break;
    case 'E': GPIOx = GPIOE; break;
    case 'F': GPIOx = GPIOF; break;
    case 'G': GPIOx = GPIOG; break;
    case 'H': GPIOx = GPIOH; break;
    case 'I': GPIOx = GPIOI; break;
    default:
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
      continue;
      break;
    }

    moder = GPIOx->MODER;
    idr   = GPIOx->IDR;
    odr   = GPIOx->ODR;
    pup   = GPIOx->PUPDR;
    
    pin = atoi(&lpmpindefs[i].name[2]);
    pinval = moder>>(pin*2)&3;

    sprintf(fs, "%-3d: [%3s] ",
            lpmpindefs[i].lpm_pin, 
            (pinval == 0)?"IN":
              (pinval == 1)?"OUT":
                (pinval == 2)?"ALT":"ANA");

    if(pinval == 0)
	{
      sprintf(ss, "      id=%d ", (idr>>pin)&1);
    }
	else if(pinval == 1)
	{
      sprintf(ss, "od=%d       ", (odr>>pin)&1);
    }
	else
	{
      sprintf(ss, "           ");
    }
    strcat(fs, ss);

    sprintf(ss, " %3s ", 
                   (((pup >> (pin*2)) & 3) == 0)?"   ":
                     (((pup >> (pin*2)) & 3) == 1)?" up":
                       (((pup >> (pin*2)) & 3) == 2)?"dwn":"-");
    strcat(fs, ss);

    sprintf(ss, "%-5s %s",
               lpmpindefs[i].name, 
               lpmpindefs[i].desc);
    strcat(fs, ss);

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", fs);
    Delay_ms(5);  //for the task handler
  }

  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
}

uint32_t dgpio_get(int argc, char* argv[])
{
    static char gpioBank = NULL;
    static GPIO_TypeDef* GPIOx;

    uint32_t fldval, i;
    static GPIO_TypeDef gpio_sv;
    static char temp1[8], temp[80];

    if (argc > 0)
    {
        gpioBank = (TAP_ToLowerCase(argv[0][0]));
    }

    if (gpioBank)
    {

        switch (gpioBank)
        {
            case 'a':
                GPIOx = GPIOA;
                break;
            case 'b':
                GPIOx = GPIOB;
                break;
            case 'c':
                GPIOx = GPIOC;
                break;
            case 'd':
                GPIOx = GPIOD;
                break;
            case 'e':
                GPIOx = GPIOE;
                break;
            case 'f':
                GPIOx = GPIOF;
                break;
            case 'g':
                GPIOx = GPIOG;
                break;
            case 'h':
                GPIOx = GPIOH;
                break;
            case 'i':
                GPIOx = GPIOI;
                break;
            default:
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "invalid gpio bank! <a,b,c,d,e,f,g,h,i>\n");
                return ERROR;
        }


        gpio_sv.MODER   = GPIOx->MODER;
        gpio_sv.BSRRH   = GPIOx->BSRRH;
        gpio_sv.BSRRL   = GPIOx->BSRRL;
        gpio_sv.IDR     = GPIOx->IDR;
        gpio_sv.LCKR    = GPIOx->LCKR;
        gpio_sv.ODR     = GPIOx->ODR;
        gpio_sv.OSPEEDR = GPIOx->OSPEEDR;
        gpio_sv.OTYPER  = GPIOx->OTYPER;
        gpio_sv.PUPDR   = GPIOx->PUPDR;
        gpio_sv.AFR[0]  = GPIOx->AFR[0];
        gpio_sv.AFR[1]  = GPIOx->AFR[1];

        // execute as a task
        if (argc > 2)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Bank: %c\n", gpioBank);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%18s: [15][14][13][12][11][10][ 9][ 8][ 7][ 6][ 5][ 4][ 3][ 2][ 1][ 0]", "gpio pin vals");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%10s:%08x ", "MODER",  gpio_sv.MODER);
            sprintf(temp, "%18s  ", "");
            for (i = 0; i < 32; i += 2)
            {
                sprintf(temp1, " %2x ", (gpio_sv.MODER >> (30 - i)) & 3);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%18s  ", "");
            for (i = 0; i < 32; i += 2)
            {
                fldval = ((gpio_sv.MODER >> (30 - i)) & 3);
                sprintf(temp1, "| %s ",
                        (fldval == 1) ? "o" :
                        (fldval == 2) ? "a" :
                        (fldval == 3) ? "a" : "i");
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);


            sprintf(temp, "%18s  ", "");
            for (i = 0; i < 32; i += 2)
            {
                fldval = ((gpio_sv.MODER >> (30 - i)) & 3);
                sprintf(temp1, "| %s ",
                        (fldval == 1) ? "u" :
                        (fldval == 2) ? "l" :
                        (fldval == 3) ? "n" : "n");
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%18s  ", "");
            for (i = 0; i < 32; i += 2)
            {
                fldval = ((gpio_sv.MODER >> (30 - i)) & 3);
                sprintf(temp1, "| %s ",
                        (fldval == 1) ? "t" :
                        (fldval == 2) ? "t" :
                        (fldval == 3) ? "a" : " ");
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%11s:%08x", "OTYPER", gpio_sv.OTYPER);
            for (i = 0; i < 16; i += 1)
            {
                fldval = (gpio_sv.OTYPER >> (15 - i)) & 1;    //1 bits per field
                sprintf(temp1, " %2x ", fldval);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%11s:%08x", "OSPEEDR", gpio_sv.OSPEEDR);
            for (i = 0; i < 32; i += 2)
            {
                fldval = (gpio_sv.OSPEEDR >> (30 - i)) & 3; //2 bits per field
                sprintf(temp1, " %2x ", fldval);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%11s:%08x", "PUPDR", gpio_sv.PUPDR);
            for (i = 0; i < 32; i += 2)
            {
                fldval = (gpio_sv.PUPDR >> (30 - i)) & 3;     //2 bits per field
                sprintf(temp1, " %2x ", fldval);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%11s:%08x", "IDR", gpio_sv.IDR);
            for (i = 0; i < 16; i += 1)
            {
                fldval = (gpio_sv.IDR >> (15 - i)) & 1;     //1 bits per field
                sprintf(temp1, " %2x ", fldval);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%11s:%08x", "ODR", gpio_sv.ODR);
            for (i = 0; i < 16; i += 1)
            {
                fldval = (gpio_sv.ODR >> (15 - i)) & 1;     //1 bits per field
                sprintf(temp1, " %2x ", fldval);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%11s:%08x%32s", "AFRL", gpio_sv.AFR[0], "");
            for (i = 0; i < 32; i += 4)
            {
                fldval = (gpio_sv.AFR[0] >> (28 - i)) & 0xf;   //4 bits per field
                sprintf(temp1, " %2x ", fldval);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s", temp);

            sprintf(temp, "%11s:%08x", "AFRH", gpio_sv.AFR[1]);
            for (i = 0; i < 32; i += 4)
            {
                fldval = (gpio_sv.AFR[1] >> (28 - i)) & 0xf;   //4 bits per field
                sprintf(temp1, " %2x ", fldval);
                strcat(temp, temp1);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%s\r\n", temp);



        }
        else
        {
            //execute directly
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"use dt gpio,<bank> command");
        }
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"select a Bank <a,b,c,d,e,f,g,h,i>\n");
    }
    return ERROR;
}

uint32_t dgpio_set(int argc, char* argv[])
{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s\n", __FUNCTION__);
    char gpioBank = NULL;
    GPIO_TypeDef* GPIOx;
    GPIO_InitTypeDef ioinit;
    BitAction lvl = Bit_RESET;
    int shft = 0;
    uint16_t GPIO_PinMask = 0;

    if (argc > 6)
    {
        gpioBank = (TAP_ToLowerCase(argv[0][0]));	//bank
        if (argc > 1)
        {
            shft = atoi(argv[1]);					//pin
            GPIO_PinMask = 0x1 << shft;
            if (argc > 2)							//mode
            {
                if (!strncmp(argv[2], "out", 2))
                {
                    ioinit.GPIO_Mode = GPIO_Mode_OUT;
                }
                else if (!strncmp(argv[2], "af", 2))
                {
                    ioinit.GPIO_Mode = GPIO_Mode_AF;
                }
                else if (!strncmp(argv[2], "an", 2))
                {
                    ioinit.GPIO_Mode = GPIO_Mode_AN;
                }
                else
                {
                    ioinit.GPIO_Mode = GPIO_Mode_IN;
                }
            }
        }

        switch (gpioBank)
        {
            case 'a':
                GPIOx = GPIOA;
                break;
            case 'b':
                GPIOx = GPIOB;
                break;
            case 'c':
                GPIOx = GPIOC;
                break;
            case 'd':
                GPIOx = GPIOD;
                break;
            case 'e':
                GPIOx = GPIOE;
                break;
            case 'f':
                GPIOx = GPIOF;
                break;
            case 'g':
                GPIOx = GPIOG;
                break;
            case 'h':
                GPIOx = GPIOH;
                break;
            case 'i':
                GPIOx = GPIOI;
                break;
            default:
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"invalid gpio bank!\n");
                return ERROR;
        }

     LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"bank mode %08x\n",GPIOx->MODER);
    
    //pullup none, up, down
    ioinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    if(!strncmp(argv[3], "up", 2))
	{
      ioinit.GPIO_PuPd = GPIO_PuPd_UP;
    }
	else
	{
      if(!strncmp(argv[3], "down", 2))
	  {
        ioinit.GPIO_PuPd = GPIO_PuPd_DOWN;
      }
    }    
    
    //type = pp,od
    ioinit.GPIO_OType = GPIO_OType_PP;
    if(!strncmp(argv[4], "od", 2))
	{
      ioinit.GPIO_OType = GPIO_OType_OD;;
    }
    
    //speed = low,med,fast,high
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    if(!strncmp(argv[5], "med", 2))
	{
      ioinit.GPIO_Speed = GPIO_Speed_25MHz;
    }
	else
	{
      if(!strncmp(argv[5], "fast", 2))
	  {
        ioinit.GPIO_Speed = GPIO_Speed_50MHz;
      }
	  else
	  {
        if(!strncmp(argv[5], "high", 2))
		{
          ioinit.GPIO_Speed = GPIO_Speed_100MHz;
        }
      }
    }
    
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Do it!\n"); 
    ioinit.GPIO_Pin = GPIO_PinMask;
    GPIO_Init(GPIOx, &ioinit);
    
    
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"bank %c: pinmask:%08x mode:%08x %s %s %s:", 
               gpioBank, GPIO_PinMask, ioinit.GPIO_Mode,
               (ioinit.GPIO_PuPd == GPIO_PuPd_UP)?"pup":
                 (ioinit.GPIO_PuPd == GPIO_PuPd_DOWN)?"pdown":"none",
                 (ioinit.GPIO_OType == GPIO_OType_PP)?"pp":"od",
                 (ioinit.GPIO_Speed == GPIO_Speed_25MHz)?"med":
                   (ioinit.GPIO_Speed == GPIO_Speed_50MHz)?"fast":
                     (ioinit.GPIO_Speed == GPIO_Speed_100MHz)?"high":"low");
    
    //set pin value
    if(!strncmp(argv[6], "t", 1))
	{
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"toggle\n");
      GPIO_ToggleBits(GPIOx, GPIO_PinMask);
    }
	else
	{
      lvl = (!strncmp(argv[6], "hi", 2))?Bit_SET:Bit_RESET;
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%x\n", lvl);
      GPIO_WriteBit(GPIOx, GPIO_PinMask, lvl);
    }
    
    
    
    return SUCCESS;
  }
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", IOS_FULL_HELP);
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", IOS_FULL_HELP1);
  return ERROR;
}


