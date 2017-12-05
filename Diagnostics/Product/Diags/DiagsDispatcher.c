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
#include "Diag_opt3001_test.h"
#include "Diag_ir_test.h"
#include "Diag_gpio_test.h"
#include "Diag_led_test.h"
#include "Diag_clock_test.h"
#include "Diag_i2c_test.h"
#include "Diag_adc_test.h"
#include "Diag_optbyter_test.h"
#include "Diag_snap_test.h"
#include "Diag_psoc_test.h"
#include <math.h>
#include  "BoseUSART_API.h"
#include  "serialPSOCListener.h"
#include "Diag_ir_blaster_test.h"
#include "PowerRiviera.h"
#include "Diag_bus_test.h"
#include "psocbootloader.h"
#include "Diag_lcd_test.h"

#include "KeyData.h"

SCRIBE_DECL(diag);

uint32_t diag_help_cmd(int argc, char *argv[]);
int dacd_init = 1;

BOOL IsDecimal(char character)
{
    return ((character >= '0') && (character <= '9'));
}

BOOL IsArgValid (CommandLine_t* CommandLine)
{
    unsigned char i;

    for (i=1; i<CommandLine->numArgs; i++) 
    {
        char* subStr = CommandLine->args[i];
        if (*subStr == '-')
        {
            return FALSE;
        }        
        while (*subStr != '\0')
        {
            if (IsDecimal(*subStr))
            {
                subStr++;
            }
            else
            {
                return FALSE;
            }          
        }
    }
    return TRUE;
}

void Diag_IR_command (CommandLine_t *CommandLine, uint32_t  *arg)
{
    if(CommandLine->numArgs >1){     
      if(!(strcmp(CommandLine->args[1], "help"))){
          arg[0] = DIAG_IR_HELP;
      } else if(!(strcmp(CommandLine->args[1], "rx"))){
          if(!(strcmp(CommandLine->args[2], "start"))){
              arg[0] = DIAG_IR_RECEIVE_START;
          } else if(!(strcmp(CommandLine->args[2], "stop"))){
                     arg[0] = DIAG_IR_RECEIVE_STOP;
          }
      } else if (!(strcmp(CommandLine->args[1], "sound"))) {
          arg[0] = DIAG_IR_SOUND;
      } else {
          TAP_PrintString("dt help for help\n\r");
          return;
      }
      DiagHandlerPostMsg(DIAG_MESSAGE_ID_IR, arg); 
    } else{
      TAP_PrintString("dt help for help\n\r");
      return;
    }
}

void Diag_opt3001_command (CommandLine_t *CommandLine, uint32_t  *arg, BOOL *valid)
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
                arg[1] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 2, valid);
                if (CommandLine->numArgs > 2)
                {
                    arg[2] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 3, valid);
                }
            }
        }
        else if (!(strncmp(CommandLine->args[1], "rd", 1)))
        {
            arg[0] = OPT3001_RD;
            //get the reg# and value in HEX
            if (CommandLine->numArgs > 1)
            {
                arg[1] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 2, valid);
                if (CommandLine->numArgs > 2)
                {
                    arg[2] = (DIAG_VALUE)TAP_HexArgToInt(CommandLine, 3, valid);
                }
            }
        }
        else if (!(strncmp(CommandLine->args[1], "lux", 1)))
        {
            arg[0] = OPT3001_LUX_RD;
            //get the delay and repeat value in DEC
            if (CommandLine->numArgs > 1)
            {
                arg[1] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 2, valid);
                if (CommandLine->numArgs > 2)
                {
                    arg[2] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 3, valid);
                }
            }
        }
        else if (!(strncmp(CommandLine->args[1], "bus", 1)))
        {
            arg[0] = OPT3001_I2C_BUS;
            //get the delay and repeat value in DEC
            if (CommandLine->numArgs > 1)
            {
                arg[1] = (DIAG_VALUE)TAP_DecimalArgToInt(CommandLine, 2, valid);
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

static uint8_t isxnum(uint8_t character)
{
  if((character >= 'a') && (character <= 'f')){return character-'a' + 10;}
  if((character >= 'A') && (character <= 'F')){return character-'A' + 10;}
  if((character >= '0') && (character <= '9')){return character-'0';}
  return 0;
}

void Diag_psoc_command (CommandLine_t *CommandLine, uint32_t  *arg)
{
    static int psoc_interrupt_enabled = 1;
#if 0
    if (ERROR == psoc_i2c_init())
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc i2c int Failed");
        return;
    }
#endif

    if(psoc_interrupt_enabled)
    { 
      if (i2cMaster_Ping(psocI2cHandle, PSOC_I2C3_ADDR) == FALSE)
      {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"PSOC not found I2C3 dev[%02x]!",
            PSOC_I2C3_ADDR);
        return;
      }

      PSoCInt_en_dis(1);  //start psoc int detector
//      psoc_int_detect(1);  //start psoc int detector 
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
//            psoc_int_detect(0);  //stop psoc int detector 
        }
        else if (!(strncmp(CommandLine->args[1], "int",    3)))
        {
            psoc_interrupt_enabled = 1;
   PSoCInt_en_dis(1);  //start psoc int detector
//            psoc_int_detect(1);  //start psoc int detector 
        }
        else if (!(strncmp(CommandLine->args[1], "all",    3)))
        {
            psoc_cmd[0] = LEDsSetAll_cmd_size;
            psoc_cmd[1] = LEDsSetAll_cmd; 
            psoc_cmd[2] = 0x00;  //unused 
            int intensity = 0;
            if(CommandLine->numArgs > 2)
            {
                intensity = (atoi(CommandLine->args[2]));
                for(int i=0;i<48;i+=2)
                {
                    psoc_cmd[i+3] = intensity & 0xff;
                    psoc_cmd[i+4] = (intensity>>8)&0xf;
                }
            }
            else
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc led pattern, missing arg\n\r");
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc all led intesity = %d\n\r", (intensity & 0xfff)); 
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
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc led cmd  %d cmd[%d] led[%d] intensity[%02x %02x] \n\r", 
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
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc version cmd  %02x\n\r", psoc_cmd[0]);
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
  
TAPCommand(TAP_dlpmplist)
{
    bb_show_all_gpio_pins();
}

TAPCommand(TAP_dgpio_get)
{
    dgpio_get(CommandLine->numArgs, CommandLine->args);
}

TAPCommand(TAP_dgpio_set)
{
    dgpio_set(CommandLine->numArgs, CommandLine->args);
}

TAPCommand(TAP_dled)
{
    dled(CommandLine->numArgs, CommandLine->args);
}

TAPCommand(TAP_dget_clk_speed_cmd)
{
    CLKSPD_GET_SET mode = CLKSPD_RD;
    get_set_clk_speed_cmd(mode);
}

TAPCommand(TAP_di2cscan)
{
    cmd_i2cscan(CommandLine->numArgs, CommandLine->args);
}

TAPCommand(TAP_dsnap)
{
    cmd_snap(CommandLine->numArgs, CommandLine->args);
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

TAPCommand(TAP_optbyter)
{
    cmd_optbyter(CommandLine->numArgs, CommandLine->args);
}

TAPCommand(TAP_Diags_Display)
{
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dadc, dadc",       ADC_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dclk, dclkget",    CLK_SPD_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dgpiog, dgpioget", IOD_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dgpios, dgpioset", IOS_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "di2c, di2cscan",   I2CSCAN_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dt, dtask",        DIAGPRESS_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dlpm, dlpmlist",   LPM_PINLIST_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "dopt, dopt",       LPM_OPT_BYTES_HELP);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%-16s - %s", "ds, dsnap",        SNAP_HELP);
    DiagHandlerPostMsg(DIAG_MESSAGE_ID_HELP, (uint32_t*)0);
}

void Diag_IR_Blaster_command(CommandLine_t *CommandLine, uint32_t  *arg)
{
    if(CommandLine->numArgs >1){
     
    if(!(strncmp(CommandLine->args[1], "info", 1))){
       if(!(strncmp(CommandLine->args[2], "vr", 1))){
         arg[0] = DIAG_IR_BLASTER_VERSION;
       } else if (!(strncmp(CommandLine->args[2], "cs", 1))){
         arg[0] = DIAG_IR_BLASTER_CODESET;
       }
    } else if (!(strncmp(CommandLine->args[1], "dl", 1))){
         arg[0] = DIAG_IR_BLASTER_DOWNLOAD_CODESET;
    } else if (!(strncmp(CommandLine->args[1], "send", 1))){
         arg[0] = DIAG_IR_BLASTER_SEND_KEY;
         
         uint32_t key;
         BOOL valid;
         // the key to send
         key = TAP_HexArgToInt(CommandLine, 2, &valid);
         if (valid)
            arg[1] = key;
    } else if (!(strncmp(CommandLine->args[1], "endsend", 1))){
         arg[0] = DIAG_IR_BLASTER_SEND_KEY_END;
    }   

    DiagHandlerPostMsg(DIAG_MESSAGE_ID_IR_BLASTER, arg);
    } else {
      TAP_PrintString("dt help for help\n\r");
      return;
    }
}

TAPCommand(TAP_DiagPress)
{
    BOOL valid = FALSE; // valid flag for parsing various arguments

    memset(psoc_cmd, 0, PSOC_CMD_BUFFER_SZ);  //clear psoc cmd buffer

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
        if (IsArgValid(CommandLine)) 
        {
            DiagHandlerPostMsg(DIAG_MESSAGE_ID_ADC, arg);
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Invalid arg\n\r");
        }
    }
    else if (!(strcmp(CommandLine->args[0], "i2c")))
    {
        if (IsArgValid(CommandLine)) 
        {
            DiagHandlerPostMsg(DIAG_MESSAGE_ID_I2C, arg);
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Invalid arg\n\r");
        }
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
    #if (PRODUCT_HARDWARE_VARIANT == 0)
    else if (!(strcmp(CommandLine->args[0], "button")))
    {
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUTTON, arg);
    }
    else if (!(strncmp(CommandLine->args[0], "lcdbacklight", 4)))
    {
        if (CommandLine->numArgs > 2)
        {
            if (IsArgValid(CommandLine)) 
            {
                strtoflt(CommandLine->args[2], arg);
            }
		}
        else
        {
        	LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "no PWM value found!\n\r");
        }
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PWM = %02d.%04d%\n\r", arg[1], arg[2]);

        DiagHandlerPostMsg(DIAG_MESSAGE_ID_LCD_BACKLIGHT, arg);
    }
    #endif
    else if(!(strcmp(CommandLine->args[0], "irb")))
    {
        Diag_IR_Blaster_command(CommandLine, arg);    
    }
    #if (PRODUCT_HARDWARE_VARIANT == 0)
    else if(!(strncmp(CommandLine->args[0], "bopt", 4)))
    {
        set_led4_af();
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_LCD_BACKLIGHT_OPT, arg);
    }
    #endif
    else if(!(strncmp(CommandLine->args[0], "psoc", 2)))
    {
        Diag_psoc_command(CommandLine, arg);
    }
    else if(!(strcmp(CommandLine->args[0], "ir")))
    {
        Diag_IR_command(CommandLine, arg);
    }
	else if(!(strncmp(CommandLine->args[0], "opt3001",3)))
    {
        Diag_opt3001_command(CommandLine, arg, &valid);
    }
	else if(!(strncmp(CommandLine->args[0], "bus",3)))
    {
        Diag_bus_test_command(CommandLine, arg, &valid);
    }else{
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "dt help for help\n\r");
//        TAP_PrintString("dt help for help\n\r");
    }
    return;
}
