/**
  ******************************************************************************
  * @file    Diag_snap_test.c
  *           + Diagnostic test for snap
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stm32f2xx_gpio.h"
#include "Diag_snap_test.h"
#include "DiagsDispatcher.h"
#include "PowerRiviera.h"
#include "DiagTask.h"
#include "PowerTask.h"
#include "SpiIpcTask.h"

#define KYPD_PWR_SHUTDOWN_MS_DELAY 8000

SCRIBE_DECL(diag);

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
#if (PRODUCT_HARDWARE_VARIANT == 0)
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, RIVIERA_MSM_RESOUT_BANK,        RIVIERA_MSM_RESOUT_PIN,     "RIV_8017_LPM_MSM_RESOUT_L"}, // B6
#endif
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_MOSI_GPIO_BANK,            SPI2_MOSI_GPIO_PIN,         "SPI2_MOSI"},                 // C3
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_MISO_GPIO_BANK,            SPI2_MISO_GPIO_PIN,         "SPI2_MISO"},                 // C2
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_CLK_GPIO_BANK,             SPI2_CLK_GPIO_PIN,          "SPI2_CLK"},                  // B13
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, SPI2_CS_GPIO_BANK,              SPI2_CS_GPIO_PIN,           "SPI2_CS"},                   // B12
{GPIO_Mode_OUT, GPIO_OType_PP, 0, 0, RIVIERA_FORCE_USB_BOOT_BANK,    RIVIERA_FORCE_USB_BOOT_PIN, "FORCE_USB_BOOT"}             // E1
};

//Riviera PON_RESET
int gbl_pon_reset_int_det = 0;
#if 0
static void ISR_PON_RESET_Detect(void)
{
  uint32_t pr;
  uint32_t intmsk = 0;  

  pr = EXTI->PR;
  intmsk = EXTI_IMR_MR8;

  if(pr & intmsk)
  {  
    EXTI->IMR &= ~intmsk;      // disable int
    if(!(GPIOE->IDR & intmsk))
    {
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
  
  if(enable)
  {
    EXTI->IMR  |= EXTI_IMR_MR8;
  }
  else
  {
    EXTI->IMR  &= ~EXTI_IMR_MR8;
    EXTI->PR = EXTI_IMR_MR8;     // clear any leftover int
  }
  
  EXTI->FTSR |= EXTI_IMR_MR8;          // falling edge trigger
  return SUCCESS;
}

#else

//use ps_hold D15  !!!!!!!!!!!!!!!!!! because PON_RESET is not connected!!!
static void ISR_PON_RESET_Detect(void)
{
  uint32_t pr;
  uint32_t intmsk = 0;  

  pr = EXTI->PR;
  intmsk = EXTI_IMR_MR15;

  if(pr & intmsk)
  {
    EXTI->IMR &= ~intmsk;      // disable int
    if(!(GPIOD->IDR & intmsk))
    {
      gbl_pon_reset_int_det = 1;

      GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1<<RIVIERA_KYPD_POWER_PIN), (RIVIERA_KYPD_POWER_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET); //INV

      GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1<<RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_RESET:Bit_SET); //INV


      EXTI->PR = intmsk;       // clear the interrupt
      EXTI->IMR |= intmsk;     // enable int
    }
  }
}

//enable or disable pon_reset interrupt
uint32_t pon_reset_int_detect(int enable)
{
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PS_HOLD %s %s", __FUNCTION__, (enable)?"enable":"disabled");
  Interrupt_RegisterISR(EXTI15_10_IRQn, ISR_PON_RESET_Detect);
  
  Interrupt_Enable(EXTI15_10_IRQn, 0, 0);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource15);

  gbl_pon_reset_int_det = 0;
  
  if(enable)
  {
    EXTI->IMR  |= EXTI_IMR_MR15;
  }
  else
  {
    EXTI->IMR  &= ~EXTI_IMR_MR15;
    EXTI->PR = EXTI_IMR_MR15;     // clear any leftover int
  }
  
//  EXTI->FTSR |= EXTI_IMR_MR15;          // falling edge trigger
  EXTI->RTSR |= EXTI_IMR_MR15;          // falling edge trigger
  return SUCCESS;
}

#endif

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
uint32_t check_pon_int_state(void)
{
  if(gbl_pon_reset_int_det)
  {
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
  if(pon_int_cnt> (200 * DIAG_CONSOLE_SCAN_RATE_MSEC))
  {
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PON_RESET_N check int timed out %d seconds  %d  poni=%d\r\n\n", 
          (KYPD_PWR_SHUTDOWN_MS_DELAY/1000)+(pon_int_cnt/10), pon_int_cnt, gbl_pon_reset_int_det);
          pon_int_cnt = 0;

          GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1 << RIVIERA_KYPD_POWER_PIN), (RIVIERA_KYPD_POWER_ACTIVE_STATE==GPIO_LOW)?Bit_SET:Bit_RESET); //INV

          GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1 << RIVIERA_PMIC_RESIN_PIN), (RIVIERA_PMIC_RESIN_ACTIVE_STATE==GPIO_LOW)?Bit_RESET:Bit_SET); //INV

          return TRUE;  //done, timed out!!!!
  }
  return FALSE;  // keep checking
}

void snap_on(void)
{

  PowerAPI_SetPowerState(POWER_STATE_FULL_POWER);
  while (PowerAPI_GetCurrentPowerState() != POWER_STATE_FULL_POWER);

//    PowerRiviera_Enable(TRUE);
}

int snap_force_reboot(int phase)
{
  if(phase == 0)
  {
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (0x1 << RIVIERA_PMIC_RESIN_PIN), Bit_RESET);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Riviera Reboot start");
    Delay_ms(10);
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (0x1 << RIVIERA_PMIC_RESIN_PIN), Bit_SET);
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (0x1 << RIVIERA_KYPD_POWER_PIN), Bit_RESET);
    return 0;
  }
  else
  {
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Riviera Reboot done");
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (0x1 << RIVIERA_KYPD_POWER_PIN), Bit_SET);
    return 1;
  }
}

void snap_show(void)
{
  uint32_t i, mode, idr, odr, pupdr;
  GPIO_TypeDef *GPIOx;
  char fs[64], ss[64];

  LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"LPM SNAP interface pins");
  for(i=0;i<(sizeof(snapdefs)/sizeof(SNAP_ENTRY));i++)
  {
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

    if(mode==1)
    {
      sprintf(ss, "odr[%d][%d]         ", odr, (snapdefs[i].inv)?(odr)?0:1:odr);
    }
    else
    {
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

void snap_off(void)
{
  
  PowerAPI_SetPowerState(POWER_STATE_LOW_POWER);
  while (PowerAPI_GetCurrentPowerState() != POWER_STATE_LOW_POWER);
  
  #if 0
  uint32_t arg[]={11,0};
  if(bb_check_pon_reset_state())
  {
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

uint32_t cmd_snap(int argc, char *argv[])
{
  uint32_t pon=0;
  uint32_t arg[]={0};

  if(argc>0)
  {
    pon = (!strncmp(argv[0], "init", 2))?0:
      (!strncmp(argv[0], "on", 2))?1:
        (!strncmp(argv[0], "reboot", 2))?2:
          (!strncmp(argv[0], "show", 2))?3:
            (!strncmp(argv[0], "qfil", 2))?4:
             (!strncmp(argv[0], "fboot", 2))?5:
               (!strncmp(argv[0], "off", 2))?6:
              (!strncmp(argv[0], "help", 2))?9:10;

        if(pon != 9)
        {
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"SNAP %s", (pon == 0)?"init":
                       (pon == 1)?"on":
                         (pon == 2)?"off":
                           (pon == 3)?"show":
                             (pon == 4)?"QFIL":
                               (pon == 5)?"fboot":
                                 (pon == 6)?"off":"unknown"); 
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\r");
        }        
        
        switch(pon)
        {
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
        
  }
  else
  {
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", SNAP_HELP); 
    return ERROR;
  }
  return SUCCESS;
}
