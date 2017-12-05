/**
  ******************************************************************************
  * @file    Diag_psoc_test.c
  *           + Diagnostic test for PSOC
 @verbatim
 ===============================================================================
 **/

#include "stdio.h"
#include "Diag_psoc_test.h"
#include "serialPSOCListener.h"
#include "Diag_lcd_test.h"
#include "DiagsDispatcher.h"
#include "psocbootloader.h" 
#include "KeyData.h"

SCRIBE_DECL(diag);

static uint8_t saved_psoc_walking_led_cmd[] = 
{
  LEDsSetOne_cmd_size,
  LEDsSetOne_cmd,
  0,              //LED # 0-23
  0,              //intesity MSb 11:8
  0,              //intesity LSb 7:0
  TRUE
};
#define INTESITY_STEP 64
#define DEFAULT_LED_INTENSITY 0x0fff
static KEY_DATA_t lkey;
static int led_intensity = DEFAULT_LED_INTENSITY;
static uint32_t  arg[] = {1, 0, 0, 0, 0, 0};
static int psoc_walk_speed = 0;
int diags_rf_remote_key_press_flag = 0;
extern int diags_cts_int_cnt;

void ledbar_init(void)
{
    // configure the PSOC reset pin
#if 0
    GPIO_InitTypeDef ioinit;
    ioinit.GPIO_Speed = GPIO_Speed_2MHz;
    ioinit.GPIO_OType = GPIO_OType_PP;
    ioinit.GPIO_PuPd = GPIO_PuPd_DOWN;
    ioinit.GPIO_Mode =  GPIO_Mode_OUT;

    ioinit.GPIO_Pin = 0x1 << PSOC_RESET_GPIO_PIN;
    GPIO_Init(PSOC_RESET_GPIO_BANK, &ioinit);
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, ioinit.GPIO_Pin, Bit_SET); //out of reset
#else
    ConfigureGpioPin(PSOC_RESET_GPIO_BANK, PSOC_RESET_GPIO_PIN, GPIO_MODE_OUT_OD, 0);
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN, Bit_SET);
#endif 

    if (ERROR == psoc_i2c_init())
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc i2c int Failed");
        return;
    }
}

int psoc_send_walking_led_cmd(void)
{
  saved_psoc_walking_led_cmd[3] = psoc_cmd[3];
  saved_psoc_walking_led_cmd[4] = psoc_cmd[4];  
  
  if(psoc_walk_speed == 0){
    psoc_walk_speed = 10 * psoc_cmd[5];
    
    psoc_cmd[2] = saved_psoc_walking_led_cmd[2]; //get LED number 
    
    if(saved_psoc_walking_led_cmd[5])
    {
      Listener_Psoc_PutString(psoc_cmd);
      saved_psoc_walking_led_cmd[5] = 0;
      return TRUE;
    }
    else
    {
      psoc_cmd[3] = 0;  //LED off
      psoc_cmd[4] = 0;
      Listener_Psoc_PutString(psoc_cmd);
      psoc_cmd[3] = saved_psoc_walking_led_cmd[3];  //restore LED intensity
      psoc_cmd[4] = saved_psoc_walking_led_cmd[4];
      saved_psoc_walking_led_cmd[2] = (saved_psoc_walking_led_cmd[2] < 24)?(saved_psoc_walking_led_cmd[2] +1):0;
      saved_psoc_walking_led_cmd[5] = 1;
      return TRUE; 
    }
  }
  else
  {
      psoc_walk_speed --;
      return TRUE; 
  }
}

int psoc_send_cmd(void)
{
  if(psoc_cmd[5])
  {
    return psoc_send_walking_led_cmd();
  }
  Listener_Psoc_PutString(psoc_cmd);
  return FALSE;
}


void diag_rf_remote(uint8_t Remote_numPacketsReceived, Remote_SPIPacket_t *pkt)
{

  
  switch(pkt->data[7])
  {
  case 'U':
    lkey.value = BOSE_PLAY;
    break;
  case 'A':
    lkey.value = BOSE_NUMBER_1;
    break;
  case 'B':
    lkey.value = BOSE_NUMBER_2;
    break;
  case 'C':
    lkey.value = BOSE_NUMBER_3;
    break;
  case 'D':
    lkey.value = BOSE_NUMBER_4;
    break;
  case 'E':
    lkey.value = BOSE_NUMBER_5;
    break;
  case 'F':
    lkey.value = BOSE_NUMBER_6;
    break;
  case 2:
    lkey.value = BOSE_VOLUME_DOWN;
    break;
   case 3:
    lkey.value = BOSE_VOLUME_UP;
    break;
   case 0:   // end of key sequence
    return;
    break;
  default:
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "RF remote key code not used = 0x%02x\n\r", pkt->data[7]);
    return;
    break;
  }
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "RF remote key code = 0x%02x", pkt->data[7]);
  diags_rf_remote_key_press_flag = TRUE;
}
  
typedef enum {
  ALL_WHITE_ON,
  ALL_RED_ON,
  ALL_GREEN_ON,
  ALL_BLUE_ON,
}led_color_suites;

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

#ifdef OPT_LB_CONTROL
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
#endif //OPT_LB_CONTROL

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
    PSoCInt_en_dis(0);  //stop psoc int detector
    //psoc_interrupt_enabled = 0;
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
//    psoc_interrupt_enabled = 1;
    PSoCInt_en_dis(1);  //start psoc int detector
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
 
#ifdef THE_OLD_ONE
void diags_ltbar_control(void)
{
  static uint32_t  arg[] = {1, 0, 0, 0, 0, 0};
  static uint16_t led_intensity = INTESITY_STEP;
  static int last_diags_cts_int_cnt=0;
  psoc_cmd[0] = LEDsSetOne_cmd_size;
  psoc_cmd[1] = LEDsSetOne_cmd; 
  psoc_cmd[5] = 0;
#if 0
  if (ERROR == psoc_i2c_init())
  {
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc i2c int Failed");
    return;
  }
#endif

  switch(lkey.value)
  {
  case BOSE_ON_OFF:
    break;
    
  case BOSE_VOLUME_UP:
    led_intensity = (led_intensity < (4095 - INTESITY_STEP))? led_intensity + INTESITY_STEP:0;
    return;
    break;
    
  case BOSE_VOLUME_DOWN:
    led_intensity = (led_intensity > INTESITY_STEP)? led_intensity - INTESITY_STEP:4095;
    return;
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
    
    for(int i=3;i<52;i+=2)
    { 
      psoc_cmd[i]   = led_intensity & 0xff;     //intensity 7:0
      psoc_cmd[i+1] = (led_intensity>>8) & 0xf; //intensity 11:8
    }
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
    led_intensity = INTESITY_STEP;
    return;
    break;
    
  case BOSE_NUMBER_6:
    psoc_cmd[0] = LEDsClearAll_cmd_size;
    psoc_cmd[1] = LEDsClearAll_cmd; 
    break;
    
  default:
    return;
    break;
  }
  DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
  diags_ltbar_control_save_key_flag = 0;
}
#endif


int diags_ltbar_control_save_key_flag = 0;
void diags_ltbar_control_save_key(KEY_DATA_t key)
{

  if(!diags_ltbar_control_save_key_flag)
  {
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

// Drive psoc reset pin low for 100Ms then pull high
void psoc_reset(void)
{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc reset!\n\r");
    PSoCInt_en_dis(0);  //stop psoc int detector
//    psoc_int_detect(0);  //stop psoc int detector
    Delay_ms(500);
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN, Bit_RESET);
    Delay_ms(500);
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN, Bit_SET);
}

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
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Too many lines for psoc bootloader, can only handle 255 right now");
                return;
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PSoC Bootload copied line %d, %.8s", psocBufferLineIndex, CommandLine->args[2]);
            strcpy(psocBootloaderInputBuffer[psocBufferLineIndex++], CommandLine->args[2]);
        }
    }
}
#endif //PSOCLOAD_IS_WORKING

#if 0
void psoc_enable_int(void)
{
    uint32_t  arg[] = {1, 0, 0, 0, 0, 0};
    if(psoc_interrupt_enabled){
        if (GPIO_ReadOutputDataBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN))
        {
   PSoCInt_en_dis(1);  //start psoc int detector
//            psoc_int_detect(1);  //start psoc int detector
            DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, arg);
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc in reset!, int not enabled\n\r");
        }
    }
}
#endif
