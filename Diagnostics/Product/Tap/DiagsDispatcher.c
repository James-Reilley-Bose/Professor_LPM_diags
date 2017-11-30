#include "project.h"
#include "LoggerTask.h"
#include "etap.h"
//#include "globalParameterList.h"
#include "buffermanager.h"
#include <stdlib.h>
#include <string.h>
#include "versionlib.h"
#include "stm32f2xx.h"
#include "stm32f2xx_gpio.h"
#include "DiagsDispatcher.h"
#include "TaskDefs.h"
#include "i2cMaster_API.h"
#include "stdio.h"
#include "Diag_i2c_test.h"
#include <math.h>
#include  "BoseUSART_API.h"
#include "serialPSOCListener.h"
#include "PowerRiviera.h"
#include "psocbootloader.h"
#include "SpiIpcTask.h"
#include "DiagTask.h"
#include "PowerTask.h"
#include "KeyData.h"
#include "product_config.h"

SCRIBE_DECL(diag);

uint32_t diag_help_cmd(int argc, char* argv[]);
static int dled_init = 1;

static int psoc_interrupt_enabled = 1;

#include "TaskDefs.h"
extern ManagedTask* ManagedDiagHandlerTask;
//#define DiagHandlerPostMsg(_theMsg, _p1, _p2, _p3) QueueManagerPostCallbackMsg(&ManagedDiagHandlerTask->Queue, _theMsg, (uint32_t []){_p1,_p2, _p3},NOP_CALLBACK, NO_BLOCK_TIME);

#define DiagHandlerPostMsg(_theMsg, _p1) QueueManagerPostCallbackMsg(&ManagedDiagHandlerTask->Queue, _theMsg, _p1, NOP_CALLBACK, NO_BLOCK_TIME);

static char fs[64], ss[64];

/****************ISR routines start****************/
int gbl_btn_det[] = {0, 0, 0, 0};
//button  gpio
//     1    B2
//     2    B9
//     3    D2
//     4    E10
static void ISR_button_Detect(void)
{
    uint32_t i, pr;
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


//Riviera PON_RESET
int gbl_pon_reset_int_det = 0;
#if 0
static void ISR_PON_RESET_Detect(void){
  uint32_t pr;
  uint32_t intmsk = 0;  

  pr = EXTI->PR;
  intmsk = EXTI_IMR_MR8;

  if(pr & intmsk){  
    EXTI->IMR &= ~intmsk;      // disable int
    if(!(GPIOE->IDR & intmsk)){
      gbl_pon_reset_int_det = 1;

      GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1<<RIVIERA_KYPD_POWER_PIN), (RIVIERA_KYPD_POWER_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET); //INV

      GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1<<RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_RESET:Bit_SET); //INV


      EXTI->PR = intmsk;       // clear the interrupt
      EXTI->IMR |= intmsk;     // enable int
    }
  }
}

//enable or disable pon_reset interrupt
uint32_t pon_reset_int_detect(int enable){
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s %s", __FUNCTION__, (enable)?"enable":"disabled");
  Interrupt_RegisterISR(EXTI9_5_IRQn, ISR_PON_RESET_Detect);
  
  Interrupt_Enable(EXTI9_5_IRQn, 0, 0);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource8);

  gbl_pon_reset_int_det = 0;
  
   if(enable){
    EXTI->IMR  |= EXTI_IMR_MR8;
  }else{
    EXTI->IMR  &= ~EXTI_IMR_MR8;
    EXTI->PR = EXTI_IMR_MR8;     // clear any leftover int
  }
  
  EXTI->FTSR |= EXTI_IMR_MR8;          // falling edge trigger
  return SUCCESS;
}

#else

//use ps_hold D15  !!!!!!!!!!!!!!!!!! because PON_RESET is not connected!!!
static void ISR_PON_RESET_Detect(void){
  uint32_t pr;
  uint32_t intmsk = 0;  

  pr = EXTI->PR;
  intmsk = EXTI_IMR_MR15;

  if(pr & intmsk){  
    EXTI->IMR &= ~intmsk;      // disable int
    if(!(GPIOD->IDR & intmsk)){
      gbl_pon_reset_int_det = 1;

      GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1<<RIVIERA_KYPD_POWER_PIN), (RIVIERA_KYPD_POWER_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET); //INV

      GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1<<RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_RESET:Bit_SET); //INV


      EXTI->PR = intmsk;       // clear the interrupt
      EXTI->IMR |= intmsk;     // enable int
    }
  }
}

//enable or disable pon_reset interrupt
uint32_t pon_reset_int_detect(int enable){
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PS_HOLD %s %s", __FUNCTION__, (enable)?"enable":"disabled");
  Interrupt_RegisterISR(EXTI15_10_IRQn, ISR_PON_RESET_Detect);
  
  Interrupt_Enable(EXTI15_10_IRQn, 0, 0);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource15);

  gbl_pon_reset_int_det = 0;
  
   if(enable){
    EXTI->IMR  |= EXTI_IMR_MR15;
  }else{
    EXTI->IMR  &= ~EXTI_IMR_MR15;
    EXTI->PR = EXTI_IMR_MR15;     // clear any leftover int
  }
  
//  EXTI->FTSR |= EXTI_IMR_MR15;          // falling edge trigger
  EXTI->RTSR |= EXTI_IMR_MR15;          // falling edge trigger
  return SUCCESS;
}

#endif

/****************ISR routines end****************/


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
  uint32_t i=0, pin, pinval, moder, idr, odr, pup;
  GPIO_TypeDef *GPIOx;

  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PIN: mode  level       pull signal name");
  
  for(i=0;i<sizeof(lpmpindefs)/sizeof(LPM_PIN_DEFS);i++){
    
    memset(fs, 0, sizeof(fs));
    
    switch(lpmpindefs[i].name[1]){
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

    if(pinval == 0){
      sprintf(ss, "      id=%d ", (idr>>pin)&1);
    }else if(pinval == 1){
      sprintf(ss, "od=%d       ", (odr>>pin)&1);
    }else{
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

TAPCommand(TAP_dlpmplist)
{
    bb_show_all_gpio_pins();
}


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

uint32_t dgpio_get(int argc, char *argv[]){  
  static char gpioBank = NULL; 
  static GPIO_TypeDef *GPIOx;  
  uint32_t fldval, i;
  static GPIO_TypeDef gpio_sv;
  static char temp1[8], temp[80];
  
  if (argc >0){
    gpioBank = (TAP_ToLowerCase(argv[0][0]));
  }
  
  if(gpioBank){
    
    switch(gpioBank){
    case 'a': GPIOx = GPIOA;break;
    case 'b': GPIOx = GPIOB;break;
    case 'c': GPIOx = GPIOC;break;
    case 'd': GPIOx = GPIOD;break;
    case 'e': GPIOx = GPIOE;break;
    case 'f': GPIOx = GPIOF;break;
    case 'g': GPIOx = GPIOG;break;
    case 'h': GPIOx = GPIOH;break;
    case 'i': GPIOx = GPIOI;break;
    default:
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"invalid gpio bank! <a,b,c,d,e,f,g,h,i>");
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
    if (argc >2){
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Bank: %c", gpioBank);
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%18s: [15][14][13][12][11][10][ 9][ 8][ 7][ 6][ 5][ 4][ 3][ 2][ 1][ 0]", "gpio pin vals");
      
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%10s:%08x", "MODER",  gpio_sv.MODER);
      sprintf(temp, "%18s  ","");
      for(i=0;i<32;i+=2){
        sprintf(temp1, " %2x ", (gpio_sv.MODER>>(30-i))&3);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%18s  ","");
      for(i=0;i<32;i+=2){
        fldval = ((gpio_sv.MODER>>(30-i))&3);
        sprintf(temp1, "| %s ", 
                (fldval==1)?"o":
                  (fldval==2)?"a":
                    (fldval==3)?"a":"i");
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      
      sprintf(temp, "%18s  ","");
      for(i=0;i<32;i+=2){
        fldval = ((gpio_sv.MODER>>(30-i))&3);
        sprintf(temp1, "| %s ", 
                (fldval==1)?"u":
                  (fldval==2)?"l":
                    (fldval==3)?"n":"n");
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%18s  ","");
      for(i=0;i<32;i+=2){
        fldval = ((gpio_sv.MODER>>(30-i))&3);
        sprintf(temp1, "| %s ",   
                (fldval==1)?"t":
                  (fldval==2)?"t":
                    (fldval==3)?"a":" ");
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%11s:%08x", "OTYPER", gpio_sv.OTYPER);
      for(i=0;i<16;i+=1){
        fldval = (gpio_sv.OTYPER>>(15-i))&1;          //1 bits per field
        sprintf(temp1, " %2x ", fldval);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%11s:%08x", "OSPEEDR", gpio_sv.OSPEEDR);
      for(i=0;i<32;i+=2){
        fldval = (gpio_sv.OSPEEDR>>(30-i))&3;       //2 bits per field
        sprintf(temp1, " %2x ", fldval);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%11s:%08x", "PUPDR", gpio_sv.PUPDR);
      for(i=0;i<32;i+=2){
        fldval = (gpio_sv.PUPDR>>(30-i))&3;           //2 bits per field
        sprintf(temp1, " %2x ", fldval);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%11s:%08x", "IDR", gpio_sv.IDR);
      for(i=0;i<16;i+=1){
        fldval = (gpio_sv.IDR>>(15-i))&1;           //1 bits per field
        sprintf(temp1, " %2x ", fldval);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%11s:%08x", "ODR", gpio_sv.ODR);
      for(i=0;i<16;i+=1){
        fldval = (gpio_sv.ODR>>(15-i))&1;           //1 bits per field
        sprintf(temp1, " %2x ", fldval);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%11s:%08x%32s", "AFRL", gpio_sv.AFR[0], "");
      for(i=0;i<32;i+=4){
        fldval = (gpio_sv.AFR[0]>>(28-i))&0xf;         //4 bits per field
        sprintf(temp1, " %2x ", fldval);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      sprintf(temp, "%11s:%08x", "AFRH", gpio_sv.AFR[1]);
      for(i=0;i<32;i+=4){
        fldval = (gpio_sv.AFR[1]>>(28-i))&0xf;         //4 bits per field
        sprintf(temp1, " %2x ", fldval);
        strcat(temp, temp1);
      }
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", temp);
      
      
      
    }else{
      //execute directly
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"use dt gpio,<bank> command");
    }
  }else{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"select a Bank <a,b,c,d,e,f,g,h,i>\n");
  }
  return ERROR;
}

TAPCommand(TAP_dgpio_get)
{
    dgpio_get(CommandLine->numArgs, CommandLine->args);
}


// void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
// void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
// uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
// uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
// GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
//typedef enum
//{
//  Bit_RESET = 0,
//  Bit_SET
//}BitAction;

//  GPIO_Mode_IN   = 0x00,
//  GPIO_Mode_OUT  = 0x01,
//  GPIO_Mode_AF   = 0x02,
//  GPIO_Mode_AN   = 0x03

uint32_t dgpio_set(int argc, char *argv[]){
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s\n", __FUNCTION__);
  char gpioBank = NULL;
  GPIO_TypeDef *GPIOx;
  GPIO_InitTypeDef ioinit;
  BitAction lvl = Bit_RESET;
  int shft=0;
  uint16_t GPIO_PinMask = 0;
  
  if (argc > 6){
    gpioBank = (TAP_ToLowerCase(argv[0][0])); //bank
    if (argc >1){
      shft = atoi(argv[1]);                   //pin
      GPIO_PinMask = 0x1<<shft;
      if (argc >2){                           //mode
        if(!strncmp(argv[2], "out", 2)){
          ioinit.GPIO_Mode = GPIO_Mode_OUT;
        }else if(!strncmp(argv[2], "af", 2)){
          ioinit.GPIO_Mode = GPIO_Mode_AF;
        }else if(!strncmp(argv[2], "an", 2)){
          ioinit.GPIO_Mode = GPIO_Mode_AN;
        }else{
          ioinit.GPIO_Mode = GPIO_Mode_IN;
        }
      }
    }
    
    switch(gpioBank){
    case 'a': GPIOx = GPIOA;break;
    case 'b': GPIOx = GPIOB;break;
    case 'c': GPIOx = GPIOC;break;
    case 'd': GPIOx = GPIOD;break;
    case 'e': GPIOx = GPIOE;break;
    case 'f': GPIOx = GPIOF;break;
    case 'g': GPIOx = GPIOG;break;
    case 'h': GPIOx = GPIOH;break;
    case 'i': GPIOx = GPIOI;break;
    default:
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"invalid gpio bank!\n");
      return ERROR;
    }
    
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"bank mode %08x\n",GPIOx->MODER);
    
    //pullup none, up, down
    ioinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    if(!strncmp(argv[3], "up", 2)){
      ioinit.GPIO_PuPd = GPIO_PuPd_UP;
    }else{
      if(!strncmp(argv[3], "down", 2)){
        ioinit.GPIO_PuPd = GPIO_PuPd_DOWN;
      }
    }    
    
    //type = pp,od
    ioinit.GPIO_OType = GPIO_OType_PP;
    if(!strncmp(argv[4], "od", 2)){
      ioinit.GPIO_OType = GPIO_OType_OD;;
    }
    
    //speed = low,med,fast,high
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    if(!strncmp(argv[5], "med", 2)){
      ioinit.GPIO_Speed = GPIO_Speed_25MHz;
    }else{
      if(!strncmp(argv[5], "fast", 2)){
        ioinit.GPIO_Speed = GPIO_Speed_50MHz;
      }else{
        if(!strncmp(argv[5], "high", 2)){
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
    if(!strncmp(argv[6], "t", 1)){
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"toggle\n");
      GPIO_ToggleBits(GPIOx, GPIO_PinMask);
    }else{
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

TAPCommand(TAP_dgpio_set)
{
    dgpio_set(CommandLine->numArgs, CommandLine->args);
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

TAPCommand(TAP_dled)
{
    dled(CommandLine->numArgs, CommandLine->args);
}


/* get_set_clk_speed_cmd(mode)
    mode  - result
       0  - show current clk speeds, terse(default)
       1  - show current clk speeds, verbose
       2  - set clk speed 96MHz
       3  - set clk speed 64MHz
       4  - set clk speed 48MHz
       5  - set clk speed 32MHz
       6  - set clk speed 16MHz
       7  - help
*/
uint32_t get_set_clk_speed_cmd(CLKSPD_GET_SET mode)
{
    uint8_t result;
    uint32_t pllcfg, cfgr, rcccr, m, n, p, ahbpre = 1, ahbpre1 = 1, ahbpre2 = 1, sysclk, plli2s;
    uint32_t pllsource = 0, pllm = 8, plln = 192, pllp = 4, pllq = 4;
    uint32_t pre1, pre2, temp, verb = 1;
    uint32_t FLASH_Latency = FLASH_Latency_0;
    //  uint32_t TIM2_Prescaler = 0;
    RCC_ClocksTypeDef RCC_Clocks;

    // get current clock config
    pllcfg = RCC->PLLCFGR;

    RCC->CR &= 0xfffeffff; //turn off HSE enable
    rcccr = RCC->CR;
    cfgr = RCC->CFGR;
    plli2s = RCC->PLLI2SCFGR;

    switch (mode)
    {

        case CLKSPD_HELP:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tdi clk   - show current\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clkv  - show current verbose\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clkh  - show help\n");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk96 - set to 96MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk64 - set to 64MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk48 - set to 48MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk32 - set to 32MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk24 - set to 24MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk16 - set to 16MHz\n");
            return ERROR;
            break;

        case CLKSPD_RD:  // read current
            verb = 0;
        case CLKSPD_RD_V:  // read current
            result = RCC_GetSYSCLKSource();
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tsystem_clk = %s\n",
                       (result == 0) ? "HSI" : \
                       (result == 4) ? "HSE" : \
                       (result == 8) ? "PLL" : "unknown");
            RCC_GetClocksFreq(&RCC_Clocks);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tRCC_Clocks\n");

            m = pllcfg & RCC_PLLCFGR_PLLM;
            n = (pllcfg >> 6) & 0x1ff;
            p = (pllcfg >> 16) & 0x3;
            pllp = (p + 1) * 2;

            switch ((cfgr >> 4) & 0xf)
            {
                case 0x0:
                    ahbpre = 1;
                    break;
                case 0x8:
                    ahbpre = 2;
                    break;
                case 0x9:
                    ahbpre = 4;
                    break;
                case 0xa:
                    ahbpre = 8;
                    break;
                case 0xb:
                    ahbpre = 16;
                    break;
                case 0xc:
                    ahbpre = 64;
                    break;
                case 0xd:
                    ahbpre = 128;
                    break;
                case 0xe:
                    ahbpre = 256;
                    break;
                case 0xf:
                    ahbpre = 512;
                    break;
                default:
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Error invalid ahbpre value:!\n");
                    break;
            }

            temp = (cfgr >> 10) & 0x7;
            switch (temp)
            {
                case 0:
                    ahbpre1 = 1;
                    break;
                case 4:
                    ahbpre1 = 2;
                    break;
                case 5:
                    ahbpre1 = 4;
                    break;
                case 6:
                    ahbpre1 = 8;
                    break;
                case 7:
                    ahbpre1 = 16;
                    break;
                default:
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Error invalid ahbpre1 value:!\n");
                    break;
            }

            temp = (cfgr >> 13);
            switch (temp)
            {
                case 0:
                    ahbpre2 = 1;
                    break;
                case 4:
                    ahbpre2 = 2;
                    break;
                case 5:
                    ahbpre2 = 4;
                    break;
                case 6:
                    ahbpre2 = 8;
                    break;
                case 7:
                    ahbpre2 = 16;
                    break;
                default:
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Error invalid ahbpre2 value:!\n");
                    break;
            }


            if (verb)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%08x\n", "CR", rcccr);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "PLLI2S",
                       ((rcccr >> 26) & 1) ? "on" : "off",
                       ((rcccr >> 27) & 1) ? "locked" : "unlocked");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "PLLON",
                       ((rcccr >> 24) & 1) ? "on" : "off",
                       ((rcccr >> 25) & 1) ? "locked" : "unlocked");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "HSEON",
                       ((rcccr >> 16) & 1) ? "on" : "off",
                       ((rcccr >> 17) & 1) ? "rdy" : "notrdy");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "HSION",
                       (rcccr & 1) ? "on" : "off",
                       ((rcccr >> 1) & 1) ? "rdy" : "notrdy");

            //    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "sysclk",
            //              ((cfgr & 0xc)==0)?"HSI":
            //                (((cfgr & 0xc)==0x4)?"HSE":
            //                  (((cfgr & 0xc)==0x8)?"PLL":"invalid")));

            if (verb)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = 0x%08x\n", "PLLCFGR", pllcfg);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "m", m, m);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "n", n, n);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "p", pllp, pllp);

                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = %08x\n", "CFGR", cfgr);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "ahbpre",  ahbpre, ahbpre);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "ahb1pre", ahbpre1, ahbpre1);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "ahb2pre", ahbpre2, ahbpre2);

                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = 0x%08x\n", "PLLI2SCLK", plli2s);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "plli2s_n", (plli2s >> 6) & 0x1ff, (plli2s >> 6) & 0x1ff);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "plli2s_r", (plli2s >> 28) & 0x7, (plli2s >> 28) & 0x7);
            }
            if ((rcccr >> 24) & 1) // PLLON
            {
                sysclk = (HSI_VALUE * n) / (m * pllp);
            }
            else
            {
                sysclk = HSI_VALUE;
            }

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = %d\n", "SYSCLK_Freq", sysclk);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "HCLK_Freq", sysclk / ahbpre);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "PCLK1_Freq", sysclk / ahbpre1);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "PCLK2_Freq", sysclk / ahbpre2);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "I2SCLK_Freq",
                       (HSI_VALUE / m) * (((plli2s >> 6) & 0x1ff) / ((plli2s >> 28) & 0x7))); // (HSI/m)*(n/r)
            return SUCCESS;
            break;

        case CLKSPD_96: // set the new speed 96MHz
            pllsource = 0;
            pllm = 10;
            plln = 240;
            pllp = 4;
            pre1 = RCC_HCLK_Div4;
            pre2 = RCC_HCLK_Div4;
            FLASH_Latency = FLASH_Latency_3;
            //    TIM2_Prescaler = TIM_PRE_48MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(210, 7);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_64: // set the new speed 64MHz
            pllsource = 0;
            pllm = 12;
            plln = 192;
            pllp = 4;
            pre1 = RCC_HCLK_Div4;
            pre2 = RCC_HCLK_Div4;
            RCC_PLLI2SCmd(DISABLE);
            FLASH_Latency = FLASH_Latency_2;
            //    TIM2_Prescaler = TIM_PRE_32MHZ;
            RCC_PLLI2SConfig(216, 6);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_48: // set the new speed 48MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllp = 4;
            pre1 = RCC_HCLK_Div2;
            pre2 = RCC_HCLK_Div2;
            FLASH_Latency = FLASH_Latency_1;
            //    TIM2_Prescaler = TIM_PRE_48MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(216, 6);
            SPI2->I2SPR = 0x6;
            break;
        case CLKSPD_32: // set the new speed 32MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllp = 6;
            pllq = 4;
            pre1 = RCC_HCLK_Div2;
            pre2 = RCC_HCLK_Div2;
            FLASH_Latency = FLASH_Latency_1;
            //    TIM2_Prescaler = TIM_PRE_32MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(192, 4);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_24: // set the new speed 24MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllp = 8;
            pllq = 4;
            pre1 = RCC_HCLK_Div1;
            pre2 = RCC_HCLK_Div1;
            FLASH_Latency = FLASH_Latency_1;
            //    TIM2_Prescaler = TIM_PRE_24MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(192, 4);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_16: // set the new speed 16MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllq = 4;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(192, 4);
            RCC_PCLK1Config(RCC_HCLK_Div1);
            RCC_PCLK2Config(RCC_HCLK_Div1);
            FLASH_Latency = FLASH_Latency_0;
            //    TIM2_Prescaler = TIM_PRE_16MHZ;

            //select HSI as sysclk source
            temp = RCC->CFGR & ~RCC_CFGR_SW;
            RCC->CFGR = temp;                   // select HSI as sysclk
            RCC_PLLCmd(DISABLE);
            RCC_PLLConfig(pllsource, pllm, plln, pllp, pllq);
            FLASH_SetLatency(FLASH_Latency);
            if ((rcccr >> 26) & 1) // if it was enabled turn it back on
            {
                RCC_PLLI2SCmd(ENABLE);
            }
            //    TIM2_Prescaler = TIM_PRE_16MHZ;

            //    TIM2_Init(TIM2_Prescaler);
            //    TapUartInit();
            return SUCCESS;
            break;
        default:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tERROR: invalid Frequency, use 16, 24, 32, 64 or 96\n");
            return ERROR;
            break;
    }


    //select HSI as sysclk source
    temp = RCC->CFGR & ~RCC_CFGR_SW;
    RCC->CFGR = temp;                   // select HSI as sysclk

    RCC_PLLCmd(DISABLE);
    RCC_PCLK1Config(pre1);
    RCC_PCLK2Config(pre2);
    RCC_PLLConfig(pllsource, pllm, plln, pllp, pllq);
    if ((rcccr >> 26) & 1)                  // if it was enabled turn it back on
    {
        RCC_PLLI2SCmd(ENABLE);                // enable PLLI2S clock
        while (!(RCC->CR & RCC_CR_PLLI2SRDY)) {} //wiat for it
    }


    RCC_PLLCmd(ENABLE);                 // enable PLL clocks
    while (!(RCC->CR & RCC_CR_PLLRDY)) {} //wiat for it


    FLASH_SetLatency(FLASH_Latency);
    //select PLL as sysclk source
    temp = RCC->CFGR | RCC_CFGR_SW_1;      // select PLL as sysclk
    RCC->CFGR = temp;
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {} //wiat for it

    //  TIM2_Init(TIM2_Prescaler);
    //  TapUartInit();

    return SUCCESS;
}

TAPCommand(TAP_dget_clk_speed_cmd)
{
    CLKSPD_GET_SET mode = CLKSPD_RD;
    get_set_clk_speed_cmd(mode);
}

uint32_t clkspd(int argc, char* argv[])
{
    CLKSPD_GET_SET mode = CLKSPD_RD;

    if (argc > 1 )
    {
        if ((!strncmp(argv[1], "help", 1)) || (!strncmp(argv[1], "?", 1)))
        {
            mode = CLKSPD_HELP;
        }
    }
    else if (!strncmp(argv[0], "clkv", 4))
    {
        mode = CLKSPD_RD_V;
    }
    else if (!strncmp(argv[0], "clkh", 4))
    {
        mode = CLKSPD_HELP;
    }
    else if (!strncmp(argv[0], "clk96", 6))
    {
        mode = CLKSPD_96;
    }
    else if (!strncmp(argv[0], "clk48", 6))
    {
        mode = CLKSPD_48;
    }
    else if (!strncmp(argv[0], "clk64", 6))
    {
        mode = CLKSPD_64;
    }
    else if (!strncmp(argv[0], "clk32", 6))
    {
        mode = CLKSPD_32;
    }
    else if (!strncmp(argv[0], "clk24", 6))
    {
        mode = CLKSPD_24;
    }
    else if (!strncmp(argv[0], "clk16", 6))
    {
        mode = CLKSPD_16;
    }


    switch (mode)
    {
        case CLKSPD_96:
        case CLKSPD_64:
        case CLKSPD_48:
        case CLKSPD_32:
        case CLKSPD_24:
        case CLKSPD_16:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"set %s\n", __FUNCTION__);
            break;
        case CLKSPD_RD:
        case CLKSPD_RD_V:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"get %s\n", __FUNCTION__);
            break;
        case CLKSPD_HELP:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s help\n", __FUNCTION__);
            break;
        default:
            return ERROR;
            break;
    }

    return get_set_clk_speed_cmd(mode);
}


#if 0
void set_clk_speed(int argc, char* argv[])
{
    //   CLKSPD_GET_SET mode = CLKSPD_RD;
    uint32_t temp;

    if (argc > 1)
    {
        if ((strncmp(argv[1], "he", 2)) == 0)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tclkspd     - show current\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  1  - show current verbose\n");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  96 - set to 96MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  64 - set to 64MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  48 - set to 48MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  32 - set to 32MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  24 - set to 24MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  16 - set to 16MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  mco2 SYSCLK \n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  mco2 PLLI2S\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tclkspd  mco2 PLL\n");
            return;
        }

        if ((strncmp(argv[1], "mco2", 2)) == 0)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tset  GPIOC pin 9 altfunc mode to output MCO2\n");
            ConfigureGpioPin(I2C3_SDA_GPIO_BANK,
                             I2C3_SDA_GPIO_PIN,
                             GPIO_MODE_AF_OUT_PP,
                             0);
            if (argc > 2)
            {
                temp = RCC->CFGR;
                if ((strncmp(argv[2], "sysclk", 1)) == 0)
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tMCO2 HSI selected /4 \n");
                    RCC->CFGR = (temp & 0x0fffffff) | 0x30000000; //HSI, prescaller=4
                }
                if ((strncmp(argv[2], "plli2s", 4)) == 0)
                {
                    RCC->CFGR = (temp & 0x0fffffff) | 0x70000000; //PLL, prescaller=4
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tMCO2 PLLI2S selected /4 \n");
                }
                else
                {
                    if ((strncmp(argv[2], "pll", 3)) == 0)
                    {
                        RCC->CFGR = (temp & 0x0fffffff) | 0xf0000000; //PLL, prescaller=4
                        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tMCO2 PLL selected /4 \n");
                    }
                }
            }
            else
            {
                temp = RCC->CFGR;
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tMCO2 %s selected /4 \n",
                           ((temp & 0xc0000000) == 0) ? "sysclk" :
                           (((temp & 0xc0000000) == 0x40000000) ? "PLLI2S" :
                            (((temp & 0xc0000000) == 0xc0000000) ? "PLL" : "HSE")));

            }

            return;
        }


        if ((strcmp(argv[1], "1")) == 0)
        {
            mode = CLKSPD_RD_V;   //show speeds verbose
        }
        if ((strcmp(argv[1], "96")) == 0)
        {
            mode = CLKSPD_96;
        }
        if ((strcmp(argv[1], "64")) == 0)
        {
            mode = CLKSPD_64;
        }
        if ((strcmp(argv[1], "48")) == 0)
        {
            mode = CLKSPD_48;
        }
        if ((strcmp(argv[1], "32")) == 0)
        {
            mode = CLKSPD_32;
        }
        if ((strcmp(argv[1], "24")) == 0)
        {
            mode = CLKSPD_24;
        }
        if ((strcmp(argv[1], "16")) == 0)
        {
            mode = CLKSPD_16;
        }
    }
    return;
}
#endif

static DiagsCmdHist cmdhist[DIAG_CMD_HIST_MAX];
static int dicmdhistindx = 0;

uint32_t cmd_hist(int argc, char* argv[])
{
    int i, j, diarg;

    j = dicmdhistindx;

    for (i = 0; i < DIAG_CMD_HIST_MAX; i++)
    {
        if (cmdhist[j].args[0][0]) //first arg NULL?
        {
            diarg = 0;
            while ((cmdhist[j].args[diarg][0]) &&
                    (diarg < DIAG_CMD_ARGS_MAX))
            {
                if (diarg == 0)
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%d: di ", j);
                }
                else
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,",");
                }
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", cmdhist[j].args[diarg++]);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
        }
        j = (j) ? j - 1 : DIAG_CMD_HIST_MAX - 1; //next most recent cmd
    }
    return SUCCESS;
}

#define I2CPINGDELAY 64  //Ms

char dev_list[256];
uint32_t cmd_i2cscan(int argc, char* argv[])
{
    uint32_t bus = 0;
    uint8_t addr;
    int log = 0;
    // I2C_InitTypeDef I2C_InitStruct;

    if (argc > 0)
    {
        bus = atoi(argv[0]);
        if (argc > 1)
        {
            log = 1;
        }
    }

    if ((bus > 0) && (bus < 4))
    {
        if (!log)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Scan i2c bus %d\r\n", bus);
        }
        else
        {
            //      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Scan i2c bus %d\r\n", bus);
        }

#if 1
        I2C_Master_Config_t myI2cConfig;
        myI2cConfig.busNumber = bus;
        myI2cConfig.masterClockFrequency = 400000;
        myI2cConfig.disableDMA = 1;

        I2C_API_ERROR_CODE i2cError = I2C_API_NO_ERROR;
        I2C_BUS_HANDLE_TYPE myI2cHandle = i2cMaster_Init(&myI2cConfig, &i2cError);
#endif



        //    I2C_InitStruct.I2C_Ack = 1;
        //    I2C_InitStruct.I2C_ClockSpeed = 400000;
        //    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
        //    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;

        switch (bus)
        {
            case 1:
                //      I2C_InitStruct.I2C_AcknowledgedAddress = 0x20;
                //      I2C_InitStruct.I2C_OwnAddress1 = 0x20;
                //      I2C_Init(I2C1, &I2C_InitStruct);
                I2C1->OAR1 = 0x4020;
                I2C1->OAR2 = 0x21;
                break;
            case 2:
                //      I2C_InitStruct.I2C_AcknowledgedAddress = 0x40;
                //      I2C_InitStruct.I2C_OwnAddress1 = 0x40;
                //      I2C_Init(I2C2, &I2C_InitStruct);
                I2C2->OAR1 = 0x4040;
                I2C2->OAR2 = 0x41;
                break;
            case 3:
                //      I2C_InitStruct.I2C_AcknowledgedAddress = 0x60;
                //      I2C_InitStruct.I2C_OwnAddress1 = 0x60;
                //      I2C_Init(I2C3, &I2C_InitStruct);
                I2C3->OAR1 = 0x4060;
                I2C3->OAR2 = 0x61;
                break;
        }



#if 1
        char buffer[40];
        int devfound = 0;
        sprintf(buffer, "bus %d: ", bus);
        strcpy(dev_list, buffer);
        for (addr = 0; addr < 254; addr += 2)
        {
            if (i2cMaster_Ping(myI2cHandle, addr) != FALSE)
            {
                sprintf(buffer, "%02x ", addr);
                strcat(dev_list, buffer);
                devfound++;
            }
            //without a delay subsequent pings may fail to discover a device
            //      Delay_ms(I2CPINGDELAY);
        }

        sprintf(buffer, " - %d devices found\n\r", devfound);
        strcat(dev_list, buffer);

        if (!log)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\r\n%s\n\r", dev_list);
        }
        else
        {
            sprintf(buffer, "\r\n");
            strcat(dev_list, buffer);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n%s\n\r", dev_list);
        }


#else

        for (addr = 0; addr < 254; addr += 2)
        {
            if (((addr % 32) == 0) || (addr == 0))
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\r\n%02x:", addr);
            }
            if (i2cMaster_Ping(myI2cHandle, addr) == FALSE)
            {
                TAP_PrintString(" .");
            }
            else
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%02x", addr);
            }
            //without a delay subsequent pings may fail to discover a device
            Delay_ms(I2CPINGDELAY);
        }
#endif


    }
    else
    {
        if (!log)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Invalid arg, %s", I2CSCAN_HELP);
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Invalid arg, %s", I2CSCAN_HELP);
        }
        return ERROR;
    }
    return SUCCESS;
}

TAPCommand(TAP_di2cscan)
{
    cmd_i2cscan(CommandLine->numArgs, CommandLine->args);
}

typedef struct
{
    GPIO_TypeDef* bank;
    uint32_t pin;
} LEDGPIO;

void led_gpio_config(void)
{
    uint32_t i;
    GPIO_TypeDef* GPIOx;
    GPIO_InitTypeDef ioinit;


    LEDGPIO led_init[] =
    {
        {LED1_GPIO_BANK,    LED1_GPIO_PIN},
//        {LED2_GPIO_BANK,    LED2_GPIO_PIN},
//        {LED4_GPIO_BANK,    LED4_GPIO_PIN},
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


#if 0
APQ PMIC Power - up Sequence).  These include, with relevant pin in ():
    1.  RIV_SPI2_MISO_3P3                (PC2)
    2.  RIV_SPI2_SCK_3P3                 (PB13)
    3.  RIVIERA_CTS_GPIO_PD6_3P3_JUMP_SEL (PD6)
    4.  RIVIERA_RTS_GPIO_3P3             (PA11)
    5.  RIVIERA_FORCE_USB_BOOT_3P3           (PE1)
    6.  RIVIERA_MSM_RESOUT_3P3    (PB6)
7.  RIV_CODEC_RST_N                  (PE15).  Note:
    this is a dual purpose output and requires J1602 jumper to go to APQ8017.
    8.  RIVIERA_PON_1_1                        (PE11)
    5)  Initialize the following LPM outputs high
    1.  RIVIERA_KYPD_POWER_N_3P3               (PB15)
    2.  RIVIERA_CBL_POWER_N_3P3                (PB14)
    3.  RIV_PMIC_RESET_N_3P3             (PE7)
    6)  Set high to enable translator     PA12

    APQ PMIC Power - up Sequence
    7)  LPM turns APQ8017 PMIC on by asserting RIVIERA_KYPD_POWER_N_3P3 (PB15, active low) for at least 2 sec.  LPM should leave RIVIERA_PON_1_1_3P3 (PE11, active high) low.
    8)  LPM detects rising edge on RIVIERA_PON_RESET_N_3P3 (PE6, for APQ8017 PMIC power sequence completed)
        9)  LPM detects rising edge on RIV_PS_HOLD _3P3 (PD15, for APQ8017 coming out of reset).
            10) LPM releases assertion of RIVIERA_KYPD_POWER_N_3P3 after rising edge on RIV_PS_HOLD _3P3 detected.
            11) Reconfigure formerly low LPM outputs 1 – 7 above for normal operation (e.g., as SPI2 or INT).

                APQ PMIC Power - down Sequence
12) To avoid damaging the APQ8017, all translator outputs to the APQ8017 must be held low during power down of the PMIC on the SNAP Board. These outputs are :
                1.  RIV_SPI2_MISO_3P3 (PC2)
                2.  RIV_SPI2_SCK_3P3 (PB13)
                3.  RIVIERA_CTS_GPIO_PD6_3P3_JUMP_SEL (PD6)
                4.  RIVIERA_RTS_GPIO_3P3 (PA11)
                5.  RIVIERA_FORCE_USB_BOOT_3P3 (PE1)
                6.  RIVIERA_MSM_RESOUT_3P3 (PB6)
                7.  RIV_CODEC_RST_N (PE15).
                13) Assert RIVIERA_KYPD_POWER_N_3P3 (PB15, active low) for TBD seconds (much longer than 2 sec).
#endif

                    // PM8937/PM8940 Power Management IC Device Specification
                    // PON_RESET_N – DO Power-on reset control to modem IC
                    //                  - HIGH: modem IC released from reset after successful PMIC power on
                    //                  - LOW: modem IC is reset when PMIC is reset or during shutdown
                    // PS_HOLD     – DI Power-supply hold control input – keeps PMIC on when HIGH,
                    //                  or initiates a reset or power down when asserted LOW
                    // RESIN_N     – DI PMIC reset input; initiates a stage 2 or stage 3 reset if held LOW
                    // CBL_PWR_N   – DI Cable power-on; internal pull-up to dVdd; initiates power on when grounded
                    // KYPD_PWR_N  – DI Internal pull-up to dVdd; Dual function:
                    //                 1) Keypad power-on; initiates power on when grounded
                    //                 2) Can be configured as stage 2 or stage 3 reset if held LOW longer

typedef struct {
  GPIOMode_TypeDef io;        // direction, in/out
  GPIOOType_TypeDef type;     //def(PP)/1=open drain
  uint32_t level;             // output value
  uint32_t inv;               // output value is inverted, compared to BB
  GPIO_TypeDef *bank;
  uint32_t pin;
  char *name;
 }SNAP_ENTRY;

SNAP_ENTRY snapdefs[] =
{
{GPIO_Mode_IN,  GPIO_OType_PP, 0, 1, RIVIERA_PON_RESET_BANK,         RIVIERA_PON_RESET_PIN,      "RIVIERA_PON_RESET_N"},       //E8
{GPIO_Mode_IN,  GPIO_OType_PP, 0, 1, RIVIERA_PS_HOLD_BANK,           RIVIERA_PS_HOLD_PIN,        "RIV_PS_HOLD"},               //D15 PMIC input, set OD so we can drive it
{GPIO_Mode_IN,  GPIO_OType_PP, 0, 0, RIVIERA_RESET_REQ_BANK,         RIVIERA_RESET_REQ_PIN,      "RIV_8017_LPM_RST"},          //D11
{GPIO_Mode_IN,  GPIO_OType_PP, 0, 0, RIVIERA_CTS_GPIO_BANK,          RIVIERA_CTS_GPIO_PIN,       "RIV_SPARE_GPIO2"},           //D6

{GPIO_Mode_OUT, GPIO_OType_PP, 1, 0, RIVIERA_PON_1_BANK,             RIVIERA_PON_1_PIN,          "RIVIERA_PON_1_1"},           // E11
{GPIO_Mode_OUT, GPIO_OType_PP, 1, 1, RIVIERA_PMIC_RESIN_BANK,        RIVIERA_PMIC_RESIN_PIN,     "RIVIERA_PMIC_RESIN_N"},      // E7  PMIC input
{GPIO_Mode_OUT, GPIO_OType_PP, 1, 0, RIVIERA_CBL_POWER_BANK,         RIVIERA_CBL_POWER_PIN,      "RIVIERA_CBL_POWER_N"},       // B14 PMIC input
{GPIO_Mode_OUT, GPIO_OType_PP, 1, 1, RIVIERA_KYPD_POWER_BANK,        RIVIERA_KYPD_POWER_PIN,     "RIVIERA_KYPD_POWER"},        // B15 PMIC input
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, RIVIERA_TRANSLATOR_EN_BANK,     RIVIERA_TRANSLATOR_EN_PIN,  "RIV_TRANS_ENA"},             // A12
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, RIVIERA_RTS_GPIO_BANK,          RIVIERA_RTS_GPIO_PIN ,      "RIV_LPM_8017_IRQ"},          // A11
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_MOSI_GPIO_BANK,            SPI2_MOSI_GPIO_PIN,         "SPI2_MOSI"},                 // C3
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_MISO_GPIO_BANK,            SPI2_MISO_GPIO_PIN,         "SPI2_MISO"},                 // C2
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_CLK_GPIO_BANK,             SPI2_CLK_GPIO_PIN,          "SPI2_CLK"},                  // B13
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_CS_GPIO_BANK,              SPI2_CS_GPIO_PIN,           "SPI2_CS"},                   // B12
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, RIVIERA_FORCE_USB_BOOT_BANK,    RIVIERA_FORCE_USB_BOOT_PIN, "FORCE_USB_BOOT"}             // E1
};

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

void nfc_init(void)
{
}

void ledbar_init(void)
{
    // configure the PSOC reset pin
    GPIO_InitTypeDef ioinit;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_DOWN;
    ioinit.GPIO_Mode =  GPIO_Mode_OUT;

    ioinit.GPIO_Pin = 0x1 << PSOC_RESET_GPIO_PIN;
    GPIO_Init(PSOC_RESET_GPIO_BANK, &ioinit);

    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, ioinit.GPIO_Pin, Bit_SET); //out of reset
}





static float pwm_percent = 0.0;
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


//set SPI2 pins to outputs w/pulldowns to protect APQ when it's off
void spi2_disable(void)
{
    GPIO_TypeDef* GPIOx;
    GPIO_InitTypeDef ioinit;
    ioinit.GPIO_Mode = GPIO_Mode_OUT;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_DOWN;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;

    GPIOx = SPI2_MOSI_GPIO_BANK;               //C
    ioinit.GPIO_Pin = 0x1 << SPI2_MOSI_GPIO_PIN; //3
    GPIO_Init(GPIOx, &ioinit);
    GPIO_WriteBit(GPIOx, ioinit.GPIO_Pin, Bit_RESET);

    ioinit.GPIO_Pin = 0x1 << SPI2_MISO_GPIO_PIN; //2
    GPIO_Init(GPIOx, &ioinit);
    GPIO_WriteBit(GPIOx, ioinit.GPIO_Pin, Bit_RESET);

    GPIOx = SPI2_CLK_GPIO_BANK;               //B
    ioinit.GPIO_Pin = 0x1 << SPI2_CLK_GPIO_PIN; //13
    GPIO_Init(GPIOx, &ioinit);
    GPIO_WriteBit(GPIOx, ioinit.GPIO_Pin, Bit_RESET);

    ioinit.GPIO_Pin = 0x1 << SPI2_CS_GPIO_PIN; //12
    GPIO_Init(GPIOx, &ioinit);
    GPIO_WriteBit(GPIOx, ioinit.GPIO_Pin, Bit_RESET);
}

void snap_on(void){

  PowerAPI_SetPowerState(POWER_STATE_FULL_POWER);
  while (PowerAPI_GetCurrentPowerState() != POWER_STATE_FULL_POWER);

//    PowerRiviera_Enable(TRUE);
}

int snap_force_reboot(int phase){
  if(phase == 0){
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (0x1 << RIVIERA_PMIC_RESIN_PIN), Bit_RESET);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Riviera Reboot start");
    Delay_ms(10);
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (0x1 << RIVIERA_PMIC_RESIN_PIN), Bit_SET);
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (0x1 << RIVIERA_KYPD_POWER_PIN), Bit_RESET);
    return 0;
  }else{  
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Riviera Reboot done");
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (0x1 << RIVIERA_KYPD_POWER_PIN), Bit_SET);
    return 1;
  }
}

void snap_show(void)
{
  uint32_t i, mode, idr, odr, pupdr;
  GPIO_TypeDef *GPIOx;

  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"LPM SNAP interface pins");
  for(i=0;i<(sizeof(snapdefs)/sizeof(SNAP_ENTRY));i++){
    GPIOx = snapdefs[i].bank;

    mode = (GPIOx->MODER>>(snapdefs[i].pin * 2))&0x3;
    idr = (GPIOx->IDR>>(snapdefs[i].pin))&0x1;
    odr = (GPIOx->ODR>>(snapdefs[i].pin))&0x1;
    pupdr = (GPIOx->PUPDR>>(snapdefs[i].pin * 2))&0x3;
    
    sprintf(fs, "%s%-2d",
               (GPIOx == GPIOA)?"A":(GPIOx == GPIOB)?"B":
                 (GPIOx == GPIOC)?"C":(GPIOx == GPIOD)?"D":
                   (GPIOx == GPIOE)?"E":(GPIOx == GPIOF)?"F":
                     (GPIOx == GPIOG)?"G":(GPIOx == GPIOH)?"H":
                       (GPIOx == GPIOH)?"H":(GPIOx == GPIOI)?"I":"unknown bank",
                       snapdefs[i].pin);

    sprintf(ss, "   %3s  ",(mode==0)?"in":(mode==1)?"out":(mode==2)?"af":"an");
    strcat(fs, ss);

    if(mode==1){
      sprintf(ss, "odr[%d][%d]         ", odr, (snapdefs[i].inv)?(odr)?0:1:odr);
    }else{
      sprintf(ss, "        idr[%d][%d] ", idr, (snapdefs[i].inv)?(idr)?0:1:idr);
    }
    strcat(fs, ss);
    
    sprintf(ss, "%-5s : %s %s", (pupdr==0)?"nop":(pupdr==1)?"p-up":(pupdr==2)?"p-dwn":"res",
               snapdefs[i].name, (snapdefs[i].inv)?"(inverted)":"");
    strcat(fs, ss);

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", fs);
  }
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
}


#define KYPD_PWR_SHUTDOWN_MS_DELAY 8000

void snap_off(void){
  
  PowerAPI_SetPowerState(POWER_STATE_LOW_POWER);
  while (PowerAPI_GetCurrentPowerState() != POWER_STATE_LOW_POWER);
  
  #if 0
  uint32_t arg[]={11,0};
  if(bb_check_pon_reset_state()){
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\rAPQ power already off");
    return;
  }

  PowerRiviera_Enable(FALSE);

  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\rshutdown APQ power");
  spi2_disable();

  pon_reset_int_detect(TRUE);
 
  GPIO_WriteBit(RIVIERA_PON_1_BANK, (1<<RIVIERA_PON_1_PIN), (RIVIERA_PON_1_ACTIVE_STATE==GPIO_HIGH)?Bit_RESET:Bit_SET);
   
  DiagHandlerPostMsg(DIAG_MESSAGE_PON_RESET_N, arg); // start poll task

#ifndef RIVIERA_RESIN_ONLY_SHUTDOWN
  GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1<<RIVIERA_KYPD_POWER_PIN), (RIVIERA_KYPD_POWER_ACTIVE_STATE==GPIO_LOW)?Bit_RESET:Bit_SET); //INV

#endif
  GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1<<RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET); //INV

//  PowerRiviera_Enable(FALSE);

  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\rwaiting for PMIC to shutdown power");
#endif
}



uint32_t cmd_snap(int argc, char *argv[]){
  uint32_t pon=0;
  uint32_t arg[]={0};

  if(argc>0){
    pon = (!strncmp(argv[0], "init", 2))?0:
      (!strncmp(argv[0], "on", 2))?1:
        (!strncmp(argv[0], "reboot", 2))?2:
          (!strncmp(argv[0], "show", 2))?3:
            (!strncmp(argv[0], "qfil", 2))?4:
             (!strncmp(argv[0], "fboot", 2))?5:
               (!strncmp(argv[0], "off", 2))?6:
              (!strncmp(argv[0], "help", 2))?9:10;

        if(pon != 9){
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"SNAP %s", (pon == 0)?"init":
                       (pon == 1)?"on":
                         (pon == 2)?"off":
                           (pon == 3)?"show":
                             (pon == 4)?"QFIL":
                               (pon == 5)?"fboot":
                                 (pon == 6)?"off":"unknown"); 
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\r");
        }        
        
        switch(pon){
        case 0: //init
   
          ConfigureGpioPin(RIVIERA_PON_1_BANK, RIVIERA_PON_1_PIN, GPIO_MODE_OUT_PP, 0);
          GPIO_WriteBit(RIVIERA_PON_1_BANK, (1<<RIVIERA_PON_1_PIN), Bit_RESET);

          break;
        case 1: //on 
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"KYPD start APQ!");
          snap_on();
          break;
          
        case 2: //reboot
          DiagHandlerPostMsg(DIAG_MESSAGE_SNAP_REBOOT, arg); // start poll task
          break;
          
        case 3: //show
          snap_show();
          break;
          
        case 4: //QFIL 
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\rconnect the USB cable and run QFIL\n\r");
          snap_off();
          GPIO_WriteBit(RIVIERA_FORCE_USB_BOOT_BANK, (0x1 << RIVIERA_FORCE_USB_BOOT_PIN), Bit_SET);      // E1 hi, RIVIERA_FORCE_USB_BOOT_PIN
          snap_on();
          break;

        case 5: //FastBoot 
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\rconnect the USB cable and run FastBoot\n\r");
          GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (0x1 << RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET);  //FastBoot
          GPIO_WriteBit(RIVIERA_PON_1_BANK, (0x1 << RIVIERA_PON_1_PIN), (RIVIERA_PON_1_ACTIVE_STATE==GPIO_HIGH)?Bit_SET:Bit_RESET);      //FastBoot
 //         snap_on();
          break;

        case 6: //off 
          SpiIpcPostMsg(SpiIpc_Msg_ID_DeInitSpi, 0, 0);
          GPIO_WriteBit(RIVIERA_FORCE_USB_BOOT_BANK, (0x1 << RIVIERA_FORCE_USB_BOOT_PIN), Bit_RESET);      // clear RIVIERA_FORCE_USB_BOOT_PIN
          
          snap_off();
          break;

        default:
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", SNAP_HELP);
          break;
        }  
        
  }else{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", SNAP_HELP); 
    return ERROR;
  }
  return SUCCESS;
}

TAPCommand(TAP_dsnap)
{
    cmd_snap(CommandLine->numArgs, CommandLine->args);
}

static uint32_t poff_cnt = 0;
uint32_t bb_check_pon_reset_state(void)
{
    //  Delay_ms(10);
    if (Bit_SET == GPIO_ReadInputDataBit(RIVIERA_PON_RESET_BANK, (uint16_t)(0x1 << RIVIERA_PON_RESET_PIN)))
    {
       if (Bit_SET == GPIO_ReadInputDataBit(RIVIERA_PS_HOLD_BANK, (uint16_t)(0x1 << RIVIERA_PS_HOLD_PIN)))
       {
    
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PON_RESET_N low, SNAP power is off, took %d seconds\r\n\n",
            (KYPD_PWR_SHUTDOWN_MS_DELAY / 1000) + (poff_cnt / 10));
        poff_cnt = 0;
        return 1;
       }
    }
    poff_cnt++;
    return 0;
}

static uint32_t pon_int_cnt = 0;
uint32_t check_pon_int_state(void){
  if(gbl_pon_reset_int_det){
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PON_RESET_N got int  cnt %d pon[%d]\r\n\n", 
        pon_int_cnt, gbl_pon_reset_int_det);
#ifndef RIVIERA_RESIN_ONLY_SHUTDOWN
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1 << RIVIERA_KYPD_POWER_PIN), (RIVIERA_KYPD_POWER_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET); //INV
#endif
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1 << RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_RESET:Bit_SET);  //INV

    pon_reset_int_detect(FALSE);
    gbl_pon_reset_int_det = 0;
    pon_int_cnt = 0;
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "APQ power is off\r\n");
    return TRUE;   //done success
  }

  pon_int_cnt++;
  if(pon_int_cnt> (200 * DIAG_CONSOLE_SCAN_RATE_MSEC)){
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PON_RESET_N check int timed out %d seconds  %d  poni=%d\r\n\n", 
          (KYPD_PWR_SHUTDOWN_MS_DELAY/1000)+(pon_int_cnt/10), pon_int_cnt, gbl_pon_reset_int_det);
          pon_int_cnt = 0;

          GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1 << RIVIERA_KYPD_POWER_PIN), (RIVIERA_KYPD_POWER_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET); //INV

          GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1 << RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_RESET:Bit_SET); //INV

          return TRUE;  //done, timed out!!!!
  }
  return FALSE;  // keep checking
}


void ConfigureADC1(void)
{
    ADC_InitTypeDef ADC_InitStruct;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;

    /*Enable ADC1 clock so we can talk to it.*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // Reset to power-on defaults.
    ADC_DeInit();

    // settings from void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
    ADC_InitStruct.ADC_Resolution             = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode           = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode     = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_ExternalTrigConv       = ADC_ExternalTrigConv_T1_CC1; // is this == none?
    ADC_InitStruct.ADC_DataAlign              = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfConversion        = 1;

    ADC_Init(ADC1, &ADC_InitStruct);

    // default values for ADC_CommonStructInit()
    ADC_CommonInitStruct.ADC_Mode             = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_Prescaler        = ADC_Prescaler_Div2;
    ADC_CommonInitStruct.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

    ADC_CommonInit(&ADC_CommonInitStruct);
    ADC_Cmd(ADC1, ENABLE);
}




void config_adc(void)
{
    ConfigureGpioPin(ADC_VSYS_SENSE1_BANK, ADC_VSYS_SENSE1_PIN , GPIO_MODE_IN_ANALOG, NO_ALT_FUNC); //A4 29 ADC12_IN4
    ConfigureGpioPin(ADC_THERM1_BANK, ADC_THERM1_PIN, GPIO_MODE_IN_ANALOG, NO_ALT_FUNC);            //B0 35 ADC12_IN8
    ConfigureGpioPin(ADC_THERM2_BANK, ADC_THERM2_PIN, GPIO_MODE_IN_ANALOG, NO_ALT_FUNC);            //B1 36 ADC12_IN9
#if (PRODUCT_HARDWARE_VARIANT == 0)
    ConfigureGpioPin(ADC_VSYS_SENSE2_BANK,  ADC_VSYS_SENSE2_PIN, GPIO_MODE_IN_ANALOG, NO_ALT_FUNC); //C0 15 ADC
#endif
}


static float ncp21xv103j03ra_temp_converter(uint16_t conv_value, int verb)
{
    float fVoltage, tres, hires_lt, lowres_ht, tempc;
    int i;
    float voltPerDiv = (VOLTAGE_MAX / DIVISIONS_MAX);
    double num, intpart, decpart;

    TCONVERT_T tconvert[] =
    {
        // TVC
        { -40, 328.996 * THERM_NOMIAL_OHMS },  // -40
        { -35, 237.387 * THERM_NOMIAL_OHMS },  // -35
        { -30, 173.185 * THERM_NOMIAL_OHMS },  // -30
        { -25, 127.773 * THERM_NOMIAL_OHMS },  // -25
        { -20, 95.327  * THERM_NOMIAL_OHMS },  // -20
        { -15, 71.746  * THERM_NOMIAL_OHMS },  // -15
        { -10, 54.564  * THERM_NOMIAL_OHMS },  // -10
        { -5 , 41.813  * THERM_NOMIAL_OHMS },  // -5
        {0  , 32.330  * THERM_NOMIAL_OHMS },   //  0
        {5  , 25.194  * THERM_NOMIAL_OHMS },   //  5
        {10 , 19.785  * THERM_NOMIAL_OHMS },   // 10
        {15 , 15.651  * THERM_NOMIAL_OHMS },   // 15
        {20 , 12.468  * THERM_NOMIAL_OHMS },  // 20
        {25 , 10.000  * THERM_NOMIAL_OHMS },  // 25
        {30 , 8.072   * THERM_NOMIAL_OHMS },  // 30
        {35 , 6.556   * THERM_NOMIAL_OHMS },  // 35
        {40 , 5.356   * THERM_NOMIAL_OHMS },  // 40
        {45 , 4.401   * THERM_NOMIAL_OHMS },  // 45
        {50 , 3.635   * THERM_NOMIAL_OHMS },  // 50
        {55 , 3.019   * THERM_NOMIAL_OHMS },  // 55
        {60 , 2.521   * THERM_NOMIAL_OHMS },  // 60
        {65 , 2.115   * THERM_NOMIAL_OHMS },  // 65
        {70 , 1.781   * THERM_NOMIAL_OHMS },  // 70
        {75 , 1.509   * THERM_NOMIAL_OHMS },  // 75
        {80 , 1.284   * THERM_NOMIAL_OHMS }, // 80
        {85 , 1.097   * THERM_NOMIAL_OHMS }, // 85
        {90 , 0.941   * THERM_NOMIAL_OHMS }, // 90
        {95 , 0.810   * THERM_NOMIAL_OHMS }, // 95
        {100, 0.701   * THERM_NOMIAL_OHMS }, // 100
        {105, 0.608   * THERM_NOMIAL_OHMS }, // 105
        {110, 0.530   * THERM_NOMIAL_OHMS }, // 110
        {115, 0.463   * THERM_NOMIAL_OHMS }, // 115
        {120, 0.406   * THERM_NOMIAL_OHMS }, // 120
        {125, 0.358   * THERM_NOMIAL_OHMS }, // 125
        {130, 0 }                            // 130 or more
    };
    // refernce resistor voltage drop
    fVoltage = (voltPerDiv * conv_value);
    // calculate the Thermistor resistance
    tres = fVoltage / ((VOLTAGE_MAX - fVoltage) / THERM_SERIES_OHMS);

    num = fVoltage;
    intpart = (int)num;
    decpart = num - intpart;

    if (verb)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"[%04d] %d.%03dV  ",
                   conv_value,
                   (int)intpart,
                   (int)(decpart * N3_DECIMAL_POINTS_PRECISION));
    }

    num = tres;
    intpart = (int)num;
    decpart = num - intpart;

    for (i = 0; i < (sizeof(tconvert) / sizeof(TCONVERT_T)) - 1; i++)
    {
        hires_lt = tconvert[i].tscale;
        lowres_ht = tconvert[i + 1].tscale;
        if ((tres >= lowres_ht) && (tres <= hires_lt))
        {
            tempc = tconvert[i].cval + (5 * ((hires_lt - tres) / (hires_lt - lowres_ht)));
            return tempc;
        }
    }
    return -1;
}

#if 0
static float ertj1vt302j_temp_converter(uint16_t conv_value, int verb)
{
    float fVoltage, tres, hires_lt, lowres_ht, tempc;
    int i;
    float voltPerDiv = (VOLTAGE_MAX / DIVISIONS_MAX);
    double num, intpart, decpart;

    TCONVERT_T tconvert[] =
    {
        // TVC
        { -40, 63.3    * THERM_NOMIAL_OHMS },  // -40
        { -35, 42.92   * THERM_NOMIAL_OHMS },  // -35
        { -30, 29.5    * THERM_NOMIAL_OHMS },  // -30
        { -25, 20.53   * THERM_NOMIAL_OHMS },  // -25
        { -20, 14.46   * THERM_NOMIAL_OHMS },  // -20
        { -15, 10.30   * THERM_NOMIAL_OHMS },  // -15
        { -10, 7.407   * THERM_NOMIAL_OHMS },  // -10
        { -5 , 5.388   * THERM_NOMIAL_OHMS },  // -5
        {0  , 3.966   * THERM_NOMIAL_OHMS },   //  0
        {5  , 2.953   * THERM_NOMIAL_OHMS },   //  5
        {10 , 2.221   * THERM_NOMIAL_OHMS },   // 10
        {15 , 1.687   * THERM_NOMIAL_OHMS },   // 15
        {20 , 1.293   * THERM_NOMIAL_OHMS },  // 20
        {25 , 1       * THERM_NOMIAL_OHMS },  // 25
        {30 , 0.7799  * THERM_NOMIAL_OHMS },  // 30
        {35 , 0.6131  * THERM_NOMIAL_OHMS },  // 35
        {40 , 0.4856  * THERM_NOMIAL_OHMS },  // 40
        {45 , 0.3874  * THERM_NOMIAL_OHMS },  // 45
        {50 , 0.3111  * THERM_NOMIAL_OHMS },  // 50
        {55 , 0.2513  * THERM_NOMIAL_OHMS },  // 55
        {60 , 0.2042  * THERM_NOMIAL_OHMS },  // 60
        {65 , 0.1670  * THERM_NOMIAL_OHMS },  // 65
        {70 , 0.1377  * THERM_NOMIAL_OHMS },  // 70
        {75 , 0.1144  * THERM_NOMIAL_OHMS },  // 75
        {80 , 0.0956  * THERM_NOMIAL_OHMS }, // 80
        {85 , 0.08033 * THERM_NOMIAL_OHMS }, // 85
        {90 , 0.06782 * THERM_NOMIAL_OHMS }, // 90
        {95 , 0.05753 * THERM_NOMIAL_OHMS }, // 95
        {100, 0.04903 * THERM_NOMIAL_OHMS }, // 100
        {105, 0.04198 * THERM_NOMIAL_OHMS }, // 105
        {110, 0.03609 * THERM_NOMIAL_OHMS }, // 110
        {115, 0.03117 * THERM_NOMIAL_OHMS }, // 115
        {120, 0.02702 * THERM_NOMIAL_OHMS }, // 120
        {125, 0.02351 * THERM_NOMIAL_OHMS }, // 125
        {130, 0 }                            // 130 or more
    };

    // refernce resistor voltage drop
    fVoltage = (voltPerDiv * conv_value);
    // calculate the Thermistor resistance
    tres = fVoltage / ((VOLTAGE_MAX - fVoltage) / THERM_SERIES_OHMS);

    num = fVoltage;
    intpart = (int)num;
    decpart = num - intpart;

    if (verb)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"[%04d] %d.%03dV  ",
                   conv_value,
                   (int)intpart,
                   (int)(decpart * N3_DECIMAL_POINTS_PRECISION));
    }

    num = tres;
    intpart = (int)num;
    decpart = num - intpart;

    for (i = 0; i < (sizeof(tconvert) / sizeof(TCONVERT_T)) - 1; i++)
    {
        hires_lt = tconvert[i].tscale;
        lowres_ht = tconvert[i + 1].tscale;
        if ((tres >= lowres_ht) && (tres <= hires_lt))
        {
            tempc = tconvert[i].cval + (5 * ((hires_lt - tres) / (hires_lt - lowres_ht)));
            return tempc;
        }
    }
    return -1;
}
#endif


ADC_CHANNELS_T adcch[] =
{
    {"SENSE1",  SENSE1_CH},
    {"THERM1",  THERM1_CH},
    {"THERM2",  THERM2_CH},
    {"SENSE2",  SENSE2_CH},
    {"STM16_CH",   STM16_CH}
};

uint32_t cmd_adc(int index, int lastindex, uint32_t repeat, int verb, int delay)
{
    uint16_t conv_value, cnt;
    int i = 0;
    int ch_strt = 0, ch_end = sizeof(adcch) / sizeof(ADC_CHANNELS_T);
    FlagStatus flgstats;
    uint32_t rptcnt, rtrnstatus = SUCCESS;
    uint8_t ADC_Channel;
    float tempc;
    float fVoltage;
    double num, intpart, decpart;
    float voltPerDiv = (VOLTAGE_MAX / DIVISIONS_MAX);
    char adcstr1[80];

    num = voltPerDiv;
    intpart = (int)num;
    decpart = num - intpart;

    if (verb > 2)
    {
        LOG(diag, ROTTEN_LOGLEVEL_VERBOSE," conversion Mv/per %d.%05d\n",
                   (int)intpart,
                   (int)(decpart * 100000000));
    }
    if (index <= lastindex)
    {
        if ((index < ch_end) && (lastindex < ch_end))
        {
            ch_strt = index;
            ch_end = lastindex;
        }
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"invalid index\n\r");
        return ERROR;
    }

    ADC_Cmd(ADC1, ENABLE);

    Delay_ms(200);

    rptcnt = 0;
    do
    {
        if (rptcnt)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
        }
        for (i = ch_strt; i <= ch_end; i++)
        {
            ADC_Channel = adcch[i].channel;

            ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_56Cycles);

            ADC_SoftwareStartConv(ADC1);

            memset(fs, 0, sizeof(fs));
            
            cnt = 0;
            do
            {
                flgstats =  ADC_GetFlagStatus(ADC1, ADC_FLAG_STRT);
            }
            while ((flgstats == 0) && (cnt++ < 100));

            Delay_ms(200);

            conv_value = ADC_GetConversionValue(ADC1);

            fVoltage = voltPerDiv * conv_value;

            num = fVoltage;
            intpart = (int)num;
            decpart = num - intpart;

            if (verb > 1)
            {
                sprintf(ss,"[%d] %12s ch[%2d] raw[%04d] ",
                           i, adcch[i].adcname, ADC_Channel, conv_value);
                strcat(fs, ss);
                
                sprintf(ss,"%d.%02dV  ", (int)intpart,
                           (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                strcat(fs, ss);
            }
            else
            {
                if (verb > 0)
                {
                    sprintf(ss,"\t%12s ch[%2d] %d.%02dV ",
                               adcch[i].adcname, ADC_Channel,
                               (int)intpart,
                               (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                    strcat(fs, ss);
                }
                else
                {
                    sprintf(ss, "\t%12s ch[%2d] %d.%02dV ",
                            adcch[i].adcname, ADC_Channel,
                            (int)intpart,
                            (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                    strcat(fs, ss);
                 }
            }

            switch (ADC_Channel)
            {
                case SENSE1_CH:
                case SENSE2_CH:
                    num = fVoltage * 10;  // full scale 2.4V * 10.00 = 24.00V
                    intpart = (int)num;
                    decpart = num - intpart;
                    if (verb > 0)
                    {
                        sprintf(ss, " %02d.%02dV",
                                   (int)intpart,
                                   (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                        strcat(fs, ss);
                    }
                    else
                    {
                        sprintf(ss,  "%s %02d.%02dV ", adcstr1,
                            (int)intpart,
                            (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                        strcat(fs, ss);
                    }

                    break;

                case THERM1_CH:
                case THERM2_CH:
                    tempc = ncp21xv103j03ra_temp_converter(conv_value, 0);
                    num = tempc;
                    intpart = (int)num;
                    decpart = num - intpart;
                    if (verb > 0)
                    {
                        sprintf(ss,  " %d.%02dC",
                                   (int)intpart,
                                   abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                     strcat(fs, ss);
                   }
                    else
                    {
                        sprintf(ss,   "%s %d.%02dC", adcstr1,
                            (int)intpart,
                            abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                     strcat(fs, ss);
                     }
                    break;

                case STM16_CH:
                    tempc = (( fVoltage - V25) / AVG_SLOPE) + TEMP_C_AT_76MV;
                    num = tempc;
                    intpart = (int)num;
                    decpart = num - intpart;
                    if (verb > 0)
                    {
                        sprintf(ss,   " %d.%02dC",
                                   (int)intpart,
                                   abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                      strcat(fs, ss);
                    }
                    else
                    {
                        sprintf(ss,    "%s %d.%02dC", adcstr1,
                            (int)intpart,
                            abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                      strcat(fs, ss);
                    }

                    break;

                default:
                    break;
            }

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", fs);

            ADC_ClearFlag(ADC1, ADC_FLAG_STRT);
        }
        if (delay)
        {
            Delay_ms(delay);
        }
        i = 0;
    }
    while (rptcnt++ < repeat);

    return rtrnstatus;
}

static int dacd_init = 1;
void dadc_config(void)
{
    ConfigureADC1();
    ADC->CCR |= ADC_CCR_TSVREFE;
    dacd_init = 0;
    config_adc();

}

TAPCommand(TAP_dadc)
{
    int index = 0, lastindex = 4, repeat = 0, verb = 3, delay = 0;
    if (dacd_init)
    {
        dadc_config();
    }

    if (CommandLine->numArgs > 0)
    {
        verb = atoi(CommandLine->args[0]);
    }

    cmd_adc(index, lastindex, repeat, verb, delay);
}

#define DEVICE_ID1 0x1fff7a10
#define DEVICE_ID2 0x1fff7a14
#define DEVICE_ID3 0x1fff7a18
#define FLASH_SIZE 0x1fff7a22

TAPCommand(TAP_optbyter)
{

    //uint32_t optbyter(int argc, char *argv[]){
    uint32_t ob_bor = OB_BOR_OFF, seclock = 0, slb;
    uint8_t get_bor = 0;
    uint16_t sectormask = 0;
    FLASH_Status status;
    FunctionalState state = ENABLE;

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tUnique DEVICE_ID1,2,3 = %08x ", *(int*)DEVICE_ID1);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%08x", *(int*)DEVICE_ID2);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%08x", *(int*)DEVICE_ID3);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_SIZE = %04x ", (*(int*)FLASH_SIZE) & 0xffff);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t= %d Bytes", ((*(int*)FLASH_SIZE) & 0xffff) * 0x400);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tCPUID = %04x", SCB->CPUID);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t Implementer =  %02x", ((SCB->CPUID) >> 24) & 0xff);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t Variant     =   %01x", ((SCB->CPUID) >> 20) & 0xf);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t PartNo      = %03x", ((SCB->CPUID) >> 4) & 0xfff);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t Revision    =   %01x", (SCB->CPUID) & 0xf);


    status = FLASH_GetStatus();
    if (FLASH_COMPLETE == status)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH ready!");
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH status FAIL! %x", status);
    }

    // current sector lock and BOR
    seclock = FLASH_OB_GetWRP();
    get_bor = FLASH_OB_GetBOR();
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tcurrent BOR = %x seclock = %03x", get_bor, seclock);

    if (CommandLine->numArgs > 0)
    {
        if (0 == strncmp(CommandLine->args[0], "help", 2))
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt <arg1> <arg2>");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt def  - set factory defaults, unlock all, BOR off");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt bor off");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t        l1");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t        l2");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t        l3");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt secl  <0 to 11>  - lock a flach sector");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt secu  <0 to 11>  - unlock a flach sector");

            return;
        }

        if (0 == strncmp(CommandLine->args[0], "def", 2))
        {
            FLASH_OB_Unlock();
            FLASH_OB_BORConfig(OB_BOR_OFF);
            FLASH_OB_WRPConfig(0xfff, DISABLE); //unprotect all sectors
            status = FLASH_OB_Launch();
            if (FLASH_COMPLETE == status)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR Success!");
            }
            else
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR FAIL! %x", status);
            }
            FLASH_OB_Lock();
        }

        if (0 == strncmp(CommandLine->args[0], "bor", 2))
        {
            if (CommandLine->numArgs > 2)
            {
                if (0 == strncmp(CommandLine->args[1], "l1", 2))
                {
                    ob_bor = OB_BOR_LEVEL1;
                }
                else
                {
                    if (0 == strncmp(CommandLine->args[1], "l2", 2))
                    {
                        ob_bor = OB_BOR_LEVEL2;
                    }
                    else
                    {
                        if (0 == strncmp(CommandLine->args[1], "l3", 2))
                        {
                            ob_bor = OB_BOR_LEVEL3;
                        }
                    }
                }
            }

            FLASH_OB_Unlock();
            FLASH_OB_BORConfig(ob_bor);
            // keep current sector locks
            FLASH_OB_WRPConfig(seclock, DISABLE);

            status = FLASH_OB_Launch();
            if (FLASH_COMPLETE != status)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR FAILED! %x", status);
            }

            FLASH_OB_Lock();
        }



        if ((0 == strncmp(CommandLine->args[0], "secl", 4)) || (0 == strncmp(CommandLine->args[0], "secu", 4)))
        {
            if (0 == strncmp(CommandLine->args[0], "secu", 4))
            {
                state = DISABLE;
            }
            if (CommandLine->numArgs > 2)
            {
                slb = atoi(CommandLine->args[1]);

                if (state == ENABLE)
                {
                    sectormask = ~seclock | (0x1 << slb);
                }
                else
                {
                    sectormask = (0x1 << slb);
                }

                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tsector %slock %x mask %03x", (state == ENABLE) ? "" : "un", seclock, sectormask);

                FLASH_OB_Unlock();

                FLASH_OB_BORConfig(get_bor);
                FLASH_OB_WRPConfig(sectormask, state);

                status = FLASH_OB_Launch();
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR ");
                if (FLASH_COMPLETE == status)
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Success!");
                }
                else
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"FAILED! %x", status);
                }
                FLASH_OB_Lock();
            }
        }
        get_bor = FLASH_OB_GetBOR();
        seclock = FLASH_OB_GetWRP();
    }

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%s = (0x%02x) Level_", "BOR", get_bor);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s",
               (get_bor == OB_BOR_LEVEL1) ? "1 (2.19-2.29V)" :
               (get_bor == OB_BOR_LEVEL2) ? "2 (2.50-2.59V)" :
               (get_bor == OB_BOR_LEVEL3) ? "3 (2.83-2.92V)" :
               (get_bor == OB_BOR_OFF) ? "OFF  (1.88-1.72V)" : "invalid");

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%10s = [%03x]", "sector wr prot", seclock);
    for (int i = 0; i < 12; i++)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tsector %2d %slocked", i, ((seclock >> i) & 0x1) ? "un" : "");
    }
    return;
}


TAPCommand(TAP_Diags_Display)
{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dadc, dadc",       ADC_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dclk, dclkget",    CLK_SPD_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dgpiog, dgpioget", IOD_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dgpios, dgpioset", IOS_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "di2c, di2cscan",   I2CSCAN_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dt, dtask",        DIAGPRESS_HELP);
//    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dl, dled",         LED_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dlpm, dlpmlist",   LPM_PINLIST_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dopt, dopt",       LPM_OPT_BYTES_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "ds, dsnap",        SNAP_HELP);
    DiagHandlerPostMsg(DIAG_MESSAGE_ID_HELP, (uint32_t*)0);
}

void set_led4_af(void)
{
#if 0
  GPIO_InitTypeDef ioinit;

    ioinit.GPIO_Mode = GPIO_Mode_AF;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    ioinit.GPIO_Pin = 0x1 << LED4_GPIO_PIN;
    GPIO_Init(LED4_GPIO_BANK, &ioinit);

    GPIO_PinAFConfig(LED4_GPIO_BANK, LED4_GPIO_PIN, GPIO_AF_TIM1);
#endif
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

static uint8_t saved_psoc_walking_led_cmd[] = {
  LEDsSetOne_cmd_size,
  LEDsSetOne_cmd,
  0,              //LED # 0-23
  0,              //intesity MSb 11:8
  0,              //intesity LSb 7:0
  TRUE};

static int psoc_walk_speed = 0;
int psoc_send_walking_led_cmd(void){  
  saved_psoc_walking_led_cmd[3] = psoc_cmd[3];
  saved_psoc_walking_led_cmd[4] = psoc_cmd[4];  
  
  if(psoc_walk_speed == 0){
    psoc_walk_speed = 10 * psoc_cmd[5];
    
    psoc_cmd[2] = saved_psoc_walking_led_cmd[2]; //get LED number 
    
    if(saved_psoc_walking_led_cmd[5]){
      Listener_Psoc_PutString(psoc_cmd);
      saved_psoc_walking_led_cmd[5] = 0;
      return TRUE;
    }else{
      psoc_cmd[3] = 0;  //LED off
      psoc_cmd[4] = 0;
      Listener_Psoc_PutString(psoc_cmd);
      psoc_cmd[3] = saved_psoc_walking_led_cmd[3];  //restore LED intensity
      psoc_cmd[4] = saved_psoc_walking_led_cmd[4];
      saved_psoc_walking_led_cmd[2] = (saved_psoc_walking_led_cmd[2] < 24)?(saved_psoc_walking_led_cmd[2] +1):0;
      saved_psoc_walking_led_cmd[5] = 1;
      return TRUE; 
    }
  }else{
      psoc_walk_speed --;
      return TRUE; 
  }
}

int psoc_send_cmd(void){
  if(psoc_cmd[1] == LEDsSetOne_cmd){
    if(psoc_cmd[5]){
      return psoc_send_walking_led_cmd();
    }
  }
  Listener_Psoc_PutString(psoc_cmd);
  return FALSE;
}

// Drive psoc reset pin low for 100Ms then pull high
void psoc_reset(void)
{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc reset!");
    PSoCInt_en_dis(0);  //stop psoc int detector
    Delay_ms(500);
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN, Bit_RESET);
    Delay_ms(500);
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN, Bit_SET);
}
typedef enum {
  ALL_WHITE_ON,
  ALL_RED_ON,
  ALL_GREEN_ON,
  ALL_BLUE_ON,
}led_color_suites;

static int set_led_intensity_all(led_color_suites color, uint16_t intensity){
  int led_mask = 0;

  switch(color){
  case ALL_WHITE_ON:
    led_mask = 0x00fffc00;  //white 10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23
    break;
  case ALL_RED_ON:
    led_mask = 0x00000300;  //red   8,9
    break;
  case ALL_GREEN_ON:
    led_mask = 0x000000f0;  //green 4,5,6,7
    break;
  case ALL_BLUE_ON:
    led_mask = 0x0000000f;   //blue  0,1,2,3
    break;
  default:
    led_mask = 0;
    break;
  }

  for(int i=0;i<48;i+=2){
    if((led_mask >> i/2)&0x1){
      psoc_cmd[i+3] = (intensity & 0xff);       // bits 7:0
      psoc_cmd[i+4] = ((intensity>>8) & 0x0f);  // bits 12:8 
    }else{
      psoc_cmd[i+3] = 0;
      psoc_cmd[i+4] = 0;
    }
  }
  return led_mask;
}

#define INTESITY_STEP 64
#define DEFAULT_LED_INTENSITY 0x0fff
static KEY_DATA_t lkey;
static int led_intensity = DEFAULT_LED_INTENSITY;
static uint32_t  arg[] = {1, 0, 0, 0, 0, 0};
extern int diags_cts_int_cnt;

void set_LB_led_intensity(void){
  
  psoc_cmd[0] = LEDsSetAll_cmd_size;
  psoc_cmd[1] = LEDsSetAll_cmd; 
  psoc_cmd[2] = 0;
  
  for(int i=3;i<53;i+=2)
  { 
    if(psoc_cmd[i] || psoc_cmd[i+1])            //change intesity for currently lit leds
    {
      psoc_cmd[i]   = led_intensity & 0xff;     //intensity 7:0
      psoc_cmd[i+1] = (led_intensity>>8) & 0xf; //intensity 11:8
      
      if(!led_intensity){                       // this is to prevent a led from turning off
        psoc_cmd[i]   =  1;                     // because of a zero intensity
      }
    }
  }
}

static uint32_t diags_opt_ltbar_control_delay_count = 50, diags_opt_ltbar_control_gain = 1;
void diags_opt_ltbar_control(void){
  static uint32_t lux = 0;
  static uint32_t diags_opt_ltbar_control_delay_counter = 0;
  
  if (ERROR == psoc_i2c_init())
  {
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc i2c int Failed");
    return;
  }

  if(diags_opt_ltbar_control_delay_counter++ > diags_opt_ltbar_control_delay_count){
    diags_opt_ltbar_control_delay_counter = 0;
    if(SUCCESS == opt3001_rd_lux(&lux)){
      //set led intesity based on LUX
      led_intensity = lux * diags_opt_ltbar_control_gain;
    }else{
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "OPT LUX read Failed");
    }
    set_LB_led_intensity(); 
    DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
  }
}

void diags_ltbar_control(void){
  static int last_diags_cts_int_cnt=0;
  static led_color_suites led_set_state = ALL_WHITE_ON;
  int mask;
  psoc_cmd[0] = LEDsSetOne_cmd_size;
  psoc_cmd[1] = LEDsSetOne_cmd; 
  psoc_cmd[5] = 0;
 
  if (ERROR == psoc_i2c_init())
  {
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc i2c int Failed");
    return;
  }

  switch(lkey.value)
  {
  case BOSE_ON_OFF:
    break;
    
  case BOSE_VOLUME_UP:  
  case BOSE_VOLUME_DOWN:

    switch(lkey.value)
    { 
    case BOSE_VOLUME_UP:
      led_intensity += 
        (led_intensity<32)?1:
        (led_intensity<64)?2:
        (led_intensity<128)?4:
        (led_intensity<256)?32:
        (led_intensity<512)?64:128;
        if(led_intensity >4095){  //wrap back to 1
          led_intensity = 1;
        }
        
      //      led_intensity = (led_intensity < (4095 - INTESITY_STEP))? led_intensity + INTESITY_STEP:0;
      break;
    case BOSE_VOLUME_DOWN:
      led_intensity -= 
        (led_intensity<32)?1:
        (led_intensity<64)?2:
        (led_intensity<128)?4:
        (led_intensity<256)?32:
        (led_intensity<512)?64:128;
        if(led_intensity < 0){  //wrap back to 4095
          led_intensity = 4095;
        }
//      led_intensity = (led_intensity > INTESITY_STEP)? led_intensity - INTESITY_STEP:4095;
      break;
    }
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"LED intensity = %d", led_intensity);
    set_LB_led_intensity();
    break;
    
  case BOSE_NUMBER_1: // led walk
    psoc_cmd[2] = (psoc_cmd[2] < 23)?psoc_cmd[2]+1:0; //led walk
    psoc_cmd[3] = led_intensity & 0xff;
    psoc_cmd[4] = (led_intensity>>8) & 0xf;
    break;
    
  case BOSE_NUMBER_2:  // off current led
    psoc_cmd[3] = 0;
    psoc_cmd[4] = 0;
    break;
    
  case BOSE_NUMBER_3: // all on
    psoc_cmd[0] = LEDsSetAll_cmd_size;
    psoc_cmd[1] = LEDsSetAll_cmd; 
    psoc_cmd[2] = 0;
    
    for(int i=3;i<53;i+=2)
    { 
      psoc_cmd[i]   = led_intensity & 0xff;     //intensity 7:0
      psoc_cmd[i+1] = (led_intensity>>8) & 0xf; //intensity 11:8
    }
    psoc_interrupt_enabled = 0;
    break;
    
  case BOSE_NUMBER_4:
    // if the CTS line from the APQ is not changing, the APQ is dead
    // GREEN is good
    // RED is dead
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"diags_cts_int_cnt = %d", diags_cts_int_cnt);
    if(diags_cts_int_cnt > last_diags_cts_int_cnt)
    {
      last_diags_cts_int_cnt = diags_cts_int_cnt;
      psoc_cmd[2] = 5; // green led
    }else{
      psoc_cmd[2] = 9; // red leds
    }
    break;
    
  case BOSE_NUMBER_5:
    led_intensity = DEFAULT_LED_INTENSITY;
    psoc_reset();
    psoc_interrupt_enabled = 1;
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc reset");
    return;
    break;
    
  case BOSE_NUMBER_6:
    psoc_cmd[0] = LEDsClearAll_cmd_size;
    psoc_cmd[1] = LEDsClearAll_cmd; 
    break;

  case BOSE_PLAY:

    psoc_cmd[0] = LEDsSetAll_cmd_size;
    psoc_cmd[1] = LEDsSetAll_cmd; 

    switch(led_set_state){
    case ALL_WHITE_ON:
      mask = set_led_intensity_all(led_set_state, led_intensity);
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR ALL_WHITE_ON %08x", mask);
      led_set_state = ALL_RED_ON;
      break;

    case ALL_RED_ON:
      mask = set_led_intensity_all(led_set_state, led_intensity);
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR ALL_RED_ON %08x", mask);
      led_set_state = ALL_GREEN_ON;
      break;
      
    case ALL_GREEN_ON:
      mask = set_led_intensity_all(led_set_state, led_intensity);
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR ALL_GREEN_ON %08x", mask);
      led_set_state = ALL_BLUE_ON;
      break;
      
    case ALL_BLUE_ON:
      mask = set_led_intensity_all(led_set_state, led_intensity);
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR ALL_BLUE_ON %08x", mask);
      led_set_state = ALL_WHITE_ON;
      break;
      
    default:
      psoc_cmd[0] = LEDsClearAll_cmd_size;
      psoc_cmd[1] = LEDsClearAll_cmd; 
      led_set_state = ALL_WHITE_ON;
      break;
    }
 
    psoc_cmd[2] = 0;
    break; //BOSE_PLAY
  

  case BOSE_QUICK_SKIP:
    switch(led_set_state){
    case ALL_WHITE_ON:
      led_set_state = ALL_RED_ON;
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR NEXT PLAY LED STATE ALL_RED_ON");
      break;
    case ALL_RED_ON:
      led_set_state = ALL_GREEN_ON;
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR NEXT PLAY LED STATE ALL_GREEN_ON");
      break;
    case ALL_GREEN_ON:
      led_set_state = ALL_BLUE_ON;
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR NEXT PLAY LED STATE ALL_BLUE_ON");
      break;
    case ALL_BLUE_ON:
      led_set_state = ALL_WHITE_ON;
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IR NEXT PLAY LED STATE ALL_WHITE_ON");
      break;
    }
    return;
    break;  

  default:
    return;
    break;
  }
   
  DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);  
  
  diags_ltbar_control_save_key_flag = 0;
}

int diags_ltbar_control_save_key_flag = 0;
void diags_ltbar_control_save_key(KEY_DATA_t key){

  if(!diags_ltbar_control_save_key_flag){
    if(key.state == LPM_KEY_STATE_PRESSED)
    {
    diags_ltbar_control_save_key_flag = 1;
    lkey.group = key.group;
    lkey.ph_timestamp = key.ph_timestamp;
    lkey.producer = key.producer;
    lkey.value = key.value;
    lkey.state = key.state;
    lkey.state_before_release = key.state_before_release;
    lkey.timestamp = key.timestamp;
    }
  }
}


//int psoc_send_cmd(void)
//{
//    Listener_Psoc_PutString(psoc_cmd);
//    return TRUE;
//}


// The only easy way to do this is by using line by line tap commands
// There is no capability of doing tapload in the diags apps
// The PSoC cyacd files are ascii, the first line is just a hex number, the remainder start with : and are hex #s
// Start bootload by doing psoc load
// Then send psoc load xxxx (where xxxx is the next line from the cyacd file)
// Then send psoc load done

#ifdef PSOCLOAD_IS_WORKING
// TODO - THIS HAS TO COME OUT; IT'S EATING ALL THE RAM!
//static char psocBootloaderInputBuffer[255][300]; // Read in from tap input
static char psocBootloaderInputBuffer[1][1]; // Read in from tap input
static char* psocBootloaderBuffer[255]; // How the bootloader expects it
static uint8_t psocBufferLineIndex = 0;
static void HandlePSoCLoad(CommandLine_t* CommandLine)
{
    if (CommandLine->numArgs == 2)
    {
        // Start the bootloader
        for (uint8_t i = 0; i < 255; i++)
        {
            memset(psocBootloaderInputBuffer[i], 0, sizeof(psocBootloaderInputBuffer[i]));
        }
        psocBufferLineIndex = 0;
    }
    else
    {
        if (!strncmp(CommandLine->args[2], "done", 2))
        {
            // The bootloader code expects an array of strings, which the input buffer is NOT.
            for (uint8_t i = 0; i < psocBufferLineIndex; i++)
            {
                psocBootloaderBuffer[i] = psocBootloaderInputBuffer[i];
            }
            psoc_reset();
            Delay_ms(100);
            int ret = PSoCBootload((const char**)psocBootloaderBuffer, psocBufferLineIndex);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PSoC Bootloading done, status %x", ret);
            psoc_reset();
        }
        else
        {
            if (psocBufferLineIndex >= (uint8_t)sizeof(psocBootloaderBuffer))
            {
                TAP_PrintString("Too many lines for psoc bootloader, can only handle 255 right now");
                return;
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PSoC Bootload copied line %d, %.8s", psocBufferLineIndex, CommandLine->args[2]);
            strcpy(psocBootloaderInputBuffer[psocBufferLineIndex++], CommandLine->args[2]);
        }
    }
}
#endif //PSOCLOAD_IS_WORKING


void psoc_enable_int(void)
{
    uint32_t  arg[] = {1, 0, 0, 0, 0, 0};
    if(psoc_interrupt_enabled){
    if (GPIO_ReadOutputDataBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN))
    {
        PSoCInt_en_dis(1);  //start psoc int detector
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc in reset!, int not enabled");
    }
    }
}

static uint8_t isxnum(uint8_t character)
{
  if((character >= 'a') && (character <= 'f')){return character-'a' + 10;}
  if((character >= 'A') && (character <= 'F')){return character-'A' + 10;}
  if((character >= '0') && (character <= '9')){return character-'0';}
  return 0;
}


static void psoc_cmds(CommandLine_t *CommandLine, uint32_t  *arg){
  static int psoc_interrupt_enabled = 1;

  if (ERROR == psoc_i2c_init())
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc i2c int Failed");
            return;
        }

  
        if(psoc_interrupt_enabled)
        { 
          if (i2cMaster_Ping(psocI2cHandle, PSOC_I2C3_ADDR) == FALSE)
          {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PSOC not found I2C3 dev[%02x]!",
                PSOC_I2C3_ADDR);
            return;
          }

          PSoCInt_en_dis(1);  //start psoc int detector 
          psoc_interrupt_enabled = 0;

          if (i2cMaster_Ping(psocI2cHandle, PSOC_I2C3_ADDR) == FALSE)
          {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PSOC not found I2C3 dev[%02x]!",
                PSOC_I2C3_ADDR);
            return;
          }
        }
  
        if (CommandLine->numArgs > 1)
        {
            if (!(strncmp(CommandLine->args[1],      "reset",  3)))
            {
              psoc_reset();
              psoc_interrupt_enabled = 1;
              LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc reset");
            }
            else if (!(strncmp(CommandLine->args[1], "intoff", 4)))
            {
              psoc_interrupt_enabled = 0;
              PSoCInt_en_dis(0);  //stop psoc int detector 
            }
            else if (!(strncmp(CommandLine->args[1], "int",    3)))
            {
              psoc_interrupt_enabled = 1;
              PSoCInt_en_dis(1);  //start psoc int detector 
            }
            else if (!(strncmp(CommandLine->args[1], "all",    3)))
            {
              psoc_cmd[0] = LEDsSetAll_cmd_size;
              psoc_cmd[1] = LEDsSetAll_cmd; 
              psoc_cmd[2] = 0x00;  //unused 
              int intensity = 0;
              if(CommandLine->numArgs > 2){  
               intensity = (atoi(CommandLine->args[2]));
               for(int i=0;i<48;i+=2){
                 psoc_cmd[i+3] = intensity & 0xff;

                 psoc_cmd[i+4] = (intensity>>8)&0xf;
               }
              }else{              
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc led pattern, missing arg");
              }
               LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc all led intesity = %d", (intensity & 0xfff)); 
               DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
             }
            else if (!(strncmp(CommandLine->args[1], "oneled", 1)))
            {
               psoc_cmd[0] = LEDsSetOne_cmd_size;
               psoc_cmd[1] = LEDsSetOne_cmd; 
               psoc_cmd[2] = 0; 
               psoc_cmd[3] = 0x0f;
               psoc_cmd[4] = 0x00;
               psoc_cmd[5] = 0;
               if(CommandLine->numArgs > 2)
               {
                 psoc_cmd[2] = atoi(CommandLine->args[2]);
                 if(CommandLine->numArgs > 3)
                 {
                   psoc_cmd[3] = (atoi(CommandLine->args[3])) & 0xff;
                   psoc_cmd[4] = ((atoi(CommandLine->args[3]))>>8)&0xf;
                 }
                 if(CommandLine->numArgs > 4)
                 {
                    psoc_cmd[5] = 1;
                 }
               }
               LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc led cmd  %d cmd[%d] led[%d] intensity[%02x %02x] ", 
                   psoc_cmd[0],psoc_cmd[1],psoc_cmd[2],psoc_cmd[3],psoc_cmd[4]);
               DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
            }
            else if (!(strncmp(CommandLine->args[1], "clear",  2)))
            {
               LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "clear leds");
               psoc_cmd[0] = LEDsClearAll_cmd_size;
               psoc_cmd[1] = LEDsClearAll_cmd;
               DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
            }
            else if (!(strncmp(CommandLine->args[1], "version",1)))
            {
              psoc_cmd[0] = GetVersion_cmd_size;
              psoc_cmd[1] = GetVersion_cmd;
              LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc version ");
                DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
            } 

            // start loading an animation that contains #patterns
            // autostart=0/1 to queue/start immediately when finished loading
            // loop=0/1 to play once or loop infinitely until stopped
            // lpm will be in animation load mode until the last pattern is sent at which time it will autostart if specified.
            else if (!(strncmp(CommandLine->args[1], "als",   3)))
            {
              psoc_cmd[0] = ANI_LOAD_ST_cmd_size;
              psoc_cmd[1] = ANI_LOAD_ST_cmd; 
              psoc_cmd[2] = atoi(CommandLine->args[2]); //pattern 1-8
              psoc_cmd[3] = (atoi(CommandLine->args[3])&1); //autostart
              psoc_cmd[4] = (atoi(CommandLine->args[4])&1); //loop
              
              DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
            }
            // load one of the patterns for the currently loading animation
            // duration (decimal) is in ms, intensities are as in "lb ls"
            else if (!(strncmp(CommandLine->args[1], "alp",   3)))
            {
              psoc_cmd[0] = ANI_LOAD_cmd_size;
              psoc_cmd[1] = ANI_LOAD_cmd; 
              psoc_cmd[2] = atoi(CommandLine->args[2])&0xff;      //duration lsb
              psoc_cmd[3] = (atoi(CommandLine->args[2])>>8)&0xff; //duration msb

              if(strlen(CommandLine->args[3])>95)
              {
                //              .        1         2         3        4         5         6         7         8         9      
                //dt ps,alp,100,0000f00000000000f00000000000000000000000f000000000000000f000000000000000f000000000000000f0000000
                //dt ps,alp,100,0123456789a0123456789b0123456789c0123456789d01234 
                //dt ps,alp,100,ff0fee0edd0dcc0cbb0baa0a990988087707660655054404330322021101ff0fee0edd0dcc0cbb0baa0a990988087707
                //dt ps,als,1,1,0
#if 0
dt ps,als,8,1,1
dt ps,alp,40,000000000000000000000000000000000000ff0f0000000000000000000000000000000000000000000000000000ff0f
dt ps,alp,40,000000000000000000000000000000000000ff0fff0f00000000000000000000000000000000000000000000ff0fff0f
dt ps,alp,40,0000000000000000000000000000000000000000ff0fff0f000000000000000000000000000000000000ff0fff0f0000
dt ps,alp,40,00000000000000000000000000000000000000000000ff0fff0f0000000000000000000000000000ff0fff0f00000000
dt ps,alp,40,000000000000000000000000000000000000000000000000ff0fff0f00000000000000000000ff0fff0f000000000000
dt ps,alp,40,0000000000000000000000000000000000000000000000000000ff0fff0f000000000000ff0fff0f0000000000000000
dt ps,alp,40,00000000000000000000000000000000000000000000000000000000ff0fff0f0000ff0fff0f00000000000000000000
dt ps,alp,40,0000000000000000000000000000000000000000000000000000000000000000ff0f0000000000000000000000000000

lb als,8,1,1
lb alp,40,000000000000000000000000000000000000ff0f0000000000000000000000000000000000000000000000000000ff0f
lb alp,40,000000000000000000000000000000000000ff0fff0f00000000000000000000000000000000000000000000ff0fff0f
lb alp,40,0000000000000000000000000000000000000000ff0fff0f000000000000000000000000000000000000ff0fff0f0000
lb alp,40,00000000000000000000000000000000000000000000ff0fff0f0000000000000000000000000000ff0fff0f00000000
lb alp,40,000000000000000000000000000000000000000000000000ff0fff0f00000000000000000000ff0fff0f000000000000
lb alp,40,0000000000000000000000000000000000000000000000000000ff0fff0f000000000000ff0fff0f0000000000000000
lb alp,40,00000000000000000000000000000000000000000000000000000000ff0fff0f0000ff0fff0f00000000000000000000
lb alp,40,0000000000000000000000000000000000000000000000000000000000000000ff0f0000000000000000000000000000
#endif       
                
                for(int i=0;i<48;i++){
                 psoc_cmd[i+4] = isxnum(CommandLine->args[3][i*2])<<4;   //LSB
                 psoc_cmd[i+4] |= isxnum(CommandLine->args[3][(i*2)+1]); //MSB
                }
                DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
              }else{
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "ani string is to short %d",
                    strlen(CommandLine->args[3]));
                return;
              }
            }
            // stop animation at the end of the current cycle
            else if (!(strncmp(CommandLine->args[1], "ase",   3)))
            {
              psoc_cmd[0] = ANI_STOP_E_cmd_size;
              psoc_cmd[1] = ANI_STOP_E_cmd; 
              DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
            }
            //stop animation immediately
            else if (!(strncmp(CommandLine->args[1], "asi",   3)))
            {
              psoc_cmd[0] = ANI_STOP_I_cmd_size;
              psoc_cmd[1] = ANI_STOP_I_cmd; 
              DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
            }
            // start animation
            // will start an animation that has been loaded with autostart=0
            // or restart (at the beginning) an animation that has been stopped
            else if (!(strncmp(CommandLine->args[1], "as",   3)))
            {
              psoc_cmd[0] = ANI_START_cmd_size;
              psoc_cmd[1] = ANI_START_cmd; 
              DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
            }

            else if (!(strncmp(CommandLine->args[1], "read",   3)))
            {
             DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_READ_CMD, arg);
            }
            else if (!(strncmp(CommandLine->args[1], "load",   2)))
            {
             LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PSOC load not working! (new method in progress)"); 
#ifdef PSOCLOAD_IS_WORKING
                HandlePSoCLoad(CommandLine);
#endif //PSOCLOAD_IS_WORKING
            }
            else
            {
              goto get_ps_help;
            }
        }
        else
        {
        get_ps_help:
          TAP_PrintString(PSOC_HELP);
          TAP_PrintString(PSOC_HELP1);
          TAP_PrintString(PSOC_HELP2);
          TAP_PrintString(PSOC_HELP3);
          TAP_PrintString(PSOC_HELP4);
          TAP_PrintString(PSOC_HELP5);
        }

}

TAPCommand(TAP_DiagPress)
{
    BOOL valid = FALSE; // valid flag for parsing various arguments

//    memset(psoc_cmd, 0, sizeof(psoc_cmd));  //clear psoc cmd buffer

    /* Set the globals to some defaults... */
    uint32_t PressTime = GET_SYSTEM_UPTIME_MS();
    uint32_t  arg[] = {1, 0, 0, 0, 0, 0};

    if (CommandLine->numArgs > 0)
    {
        arg[0] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 1, &valid);
        if (CommandLine->numArgs > 1)
        {
            arg[1] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 2, &valid);
            if (CommandLine->numArgs > 2)
            {
                arg[2] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 3, &valid);
                if (CommandLine->numArgs > 3)
                {
                    arg[3] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 4, &valid);
                    if (CommandLine->numArgs > 4)
                    {
                        arg[4] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 5, &valid);
                    }
                }
            }
        }
    }

    if (!(strcmp(CommandLine->args[0], "help")))
    {
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_HELP, arg);
    }
    else if (!(strcmp(CommandLine->args[0], "adc")))
    {
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_ADC, arg);
    }
    else if (!(strcmp(CommandLine->args[0], "i2c")))
    {
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_I2C, arg);
    }
    else if (!(strcmp(CommandLine->args[0], "gpio")))
    {
        arg[0] = CommandLine->args[1][0];
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_GPIO, arg);
    }
    else if (!(strcmp(CommandLine->args[0], "show")))
    {
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_SHOW, arg);
    }
#if 0
    else if (!(strncmp(CommandLine->args[0], "lcdbacklight", 4)))
    {
        if (CommandLine->numArgs > 2)
        {
            strtoflt(CommandLine->args[2], arg);
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "no PWM value found!");
        }

        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PWM = %02d.%04d%", arg[1], arg[2]);

        DiagHandlerPostMsg(DIAG_MESSAGE_ID_LCD_BACKLIGHT, arg);

    }
    else if (!(strncmp(CommandLine->args[0], "bopt", 4)))
    {
        set_led4_af();
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_LCD_BACKLIGHT_OPT, arg);


    }
#endif
    else if (!(strncmp(CommandLine->args[0], "lopt", 2)))
    { 
      if (CommandLine->numArgs > 1)
      {
        diags_opt_ltbar_control_delay_count = TAP_DecimalArgToInt(CommandLine, 1, &valid);
        if (CommandLine->numArgs > 2)
        {
          diags_opt_ltbar_control_gain = TAP_DecimalArgToInt(CommandLine, 2, &valid);
        }
      }
      
      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "LOPT delay= %d, gain = %d",
          diags_opt_ltbar_control_delay_count, diags_opt_ltbar_control_gain);
      DiagHandlerPostMsg(DIAG_MESSAGE_ID_LED_OPT, arg);
        
    }
    else if (!(strncmp(CommandLine->args[0], "psoc", 2)))
    {
        psoc_cmds(CommandLine, arg);
            
    }
    else if (!(strncmp(CommandLine->args[0], "opt3001", 3)))
    {
        if (CommandLine->numArgs > 1)
        {
            if (!(strncmp(CommandLine->args[1], "init", 1)))
            {
                arg[0] = OPT3001_INIT;
            }
            else if (!(strncmp(CommandLine->args[1], "wr", 1)))
            {
                arg[0] = OPT3001_WR;
                //get the reg# and value in HEX
                if (CommandLine->numArgs > 1)
                {
                    arg[1] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 2, &valid);
                    if (CommandLine->numArgs > 2)
                    {
                        arg[2] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 3, &valid);
                    }
                }
            }
            else if (!(strncmp(CommandLine->args[1], "rd", 1)))
            {
                arg[0] = OPT3001_RD;
                //get the reg# and value in HEX
                if (CommandLine->numArgs > 1)
                {
                    arg[1] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 2, &valid);
                    if (CommandLine->numArgs > 2)
                    {
                        arg[2] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 3, &valid);
                    }
                }
            }
            else if (!(strncmp(CommandLine->args[1], "lux", 1)))
            {
                arg[0] = OPT3001_LUX_RD;
                //get the delay and repeat value in DEC
                if (CommandLine->numArgs > 1)
                {
                    arg[1] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 2, &valid);
                    if (CommandLine->numArgs > 2)
                    {
                        arg[2] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 3, &valid);
                    }
                }
            }
            else if (!(strncmp(CommandLine->args[1], "bus", 1)))
            {
                arg[0] = OPT3001_I2C_BUS;
                //get the delay and repeat value in DEC
                if (CommandLine->numArgs > 1)
                {
                    arg[1] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 2, &valid);
                }
            }
            else
            {
                TAP_PrintString("dt help for help\n\r");
                return;
            }
        }
        else
        {
            TAP_PrintString("dt help for help\n\r");
            return;
        }

        DiagHandlerPostMsg(DIAG_MESSAGE_ID_OPT, arg);
    }
    else
    {
        TAP_PrintString("dt help for help\n\r");
    }
    return;
}
