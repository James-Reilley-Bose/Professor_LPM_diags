//
// DiagTask.c - Example task that logs received Diag keys.
//

#include "project.h"
#include "LoggerTask.h"
#include "TaskDefs.h"
#include "DiagTask.h"
#include "ConsoleKeys.h"
#include "genTimer.h"
#include "timers.h"
#include "DiagsDispatcher.h"
#include "Diag_opt3001_test.h"
#include "Diag_ir_test.h"
#include "Diag_gpio_test.h"
#include "Diag_button_test.h"
#include "Diag_led_test.h"
#include "Diag_i2c_test.h"
#include "Diag_ir_blaster_test.h"
#include "Diag_adc_test.h"
#include "Diag_lcd_test.h"
#include "Diag_snap_test.h"
#include "Diag_psoc_test.h"
#include "serialPSOCListener.h"
#include <stdio.h>
#include "IpcInterface.h"
#include "IPCRouterTask.h"
#include "DeviceInterface.h"
#include "Diag_bus_test.h"
SCRIBE_DECL(diag);


ManagedTask* ManagedDiagHandlerTask = NULL;


static void DiagTask_HandleDiagMessage(GENERIC_MSG_t* msg);
static TimerHandle_t s_DiagScanTimerHandle;

int testrepeatcnt[DIAGTESTCNTLAST];

ManagedTask* DiagTaskHandle = NULL;

void DiagTask_Init(void* p)
{
    DiagTaskHandle = GetManagedTaskPointer(DIAG_TASK_NAME);
    debug_assert(DiagTaskHandle);

    memset(testrepeatcnt, 0, sizeof(testrepeatcnt)); // clear all the flags

    ManagedDiagHandlerTask = GetManagedTaskPointer(DIAG_TASK_NAME);
    debug_assert(ManagedDiagHandlerTask != NULL);


    /* Create timers used */
    /* diag scanning timer */
    s_DiagScanTimerHandle = createTimer(TIMER_MSEC_TO_TICKS(DIAG_CONSOLE_SCAN_RATE_MSEC),
                                        NULL,
                                        DIAG_MESSAGE_ID_Timer,
                                        FALSE,
                                        tBlockDiagScanTimer,
                                        "Diag_ScanTmr",
                                        NULL);
    debug_assert(s_DiagScanTimerHandle != NULL);

    timerStart(s_DiagScanTimerHandle, 0, &DiagTaskHandle->Queue);

}

void DiagTask(void* pvParameters)
{
    for (;;)
    {
        TaskManagerPollQueue(DiagTaskHandle);
    }
}

int timeout_tickcnt = 0;
static uint32_t adcmask = 0x1f, adcverb=1;
static uint32_t ledmask = 0xf;
#if (PRODUCT_HARDWARE_VARIANT == 0)
static uint32_t buttontestmask = 0xf;
#endif
char ag1[] = {'1', 0};
char ag2[] = {'t', 0};
char* ag[] = {ag1, ag2};

char gag1[] = {0x61, 0};
char gag2[] = {'1', 0};
char* gag[] = {gag1, gag2};

uint32_t opt[] = {0, 0, 0, 0}; //save opt request args

#if (PRODUCT_HARDWARE_VARIANT == 0)
static uint16_t backlight_arg[] = {0, 0, 0}; //freq, pwm%, pwm%
static uint32_t lux_scale = ONE_PCNT_LUX_FULL_SCALE;
#endif

static void diags_help(void){
	LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt help", "this help");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt adc,repeatcnt,mask", "adc read values");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt i2c,bus#", "i2cscan bus 1,2 or 3");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt gpio,bank", "dump gpiox info,bank=a,b,c,d,e,f,g,h,i");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt show", "show all gpio pins");
    #if (PRODUCT_HARDWARE_VARIANT == 0)
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt button", "enable button press check");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt lcdb <freq>,<nn.nn>", "lcdbacklight,freq,pwm");
    #endif
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt psoc,<>,<>", "commands to psoc");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt opt3001,init", "init opt, not required" );
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt opt3001,<wr,rd>,<reg>,<val>", "write or read reg value, hex" );
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt opt3001,<lux>,<MSdelay>,<repeatcnt>", "get output conterted to LUX, decimal intputs" );
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt opt3001,<bus>,<bus #>", "set the i2c bus 1,2,3, and init" );
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt bopt,<freq>,<scale>", "PWM display, driven by OPT output, scale sets range" );
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt ir,<help, rx>,<start, stop>", "help: show help for this command, rx: displaying of IR data received start or stop" );
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt irb, <info,dl, send>, <vr, cs, 0x4>", "info,vr/cs: display version/codeset, dl: download code, send, 0x4: send key 0x4" );
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-25s - %s", "dt bus, <dsp, f0, psoc, irb, all>, <on, off>, <repetition>", "turn bus test on/off with repetition (0 means non-stop for on)" );
	LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\n\r");
}

void DiagTask_HandleMsg(GENERIC_MSG_t* msg)
{
	int ac = 2;
	#if (PRODUCT_HARDWARE_VARIANT == 0)   
    	uint32_t lux[2];
	#endif

    switch (msg->msgID)
    {
        case DIAG_MESSAGE_ID_HELP:
          diags_help();
          break;

        case DIAG_MESSAGE_ID_ADC:
            testrepeatcnt[ADCCNT] = 1;
            dadc_config();  //init adc registers
            adcmask = 0x1f; // default to all
            if(msg->params[0])
            {
              testrepeatcnt[ADCCNT] = msg->params[0]&0xff;
              if(msg->params[1])
              {
                adcmask = msg->params[1]&0xff;
              }
              if(msg->params[2])
              {
                adcverb = msg->params[2];
              }
            }
            break;
			
        case DIAG_MESSAGE_ID_LED:
            testrepeatcnt[LEDCNT] = 1; //default is toggle once
            ledmask = 0xf;
            if (msg->params[0])
            {
                testrepeatcnt[LEDCNT] = msg->params[0];
			}
            if (msg->params[1])
            {
                ledmask = msg->params[1] & 0xff;
            }
            break;

        case DIAG_MESSAGE_ID_I2C:
            testrepeatcnt[I2CCNT] = 1;
            if (msg->params[0])
            {
                ag[0][0] = '0' + msg->params[0]; // I2C bus #
            }
            break;

        case DIAG_MESSAGE_ID_GPIO:
            testrepeatcnt[GPIOCNT] = 1;
            gag[0][0] = msg->params[0];
            if (msg->params[1])
            {
                testrepeatcnt[GPIOCNT] = msg->params[1];
            }
            break;

        case DIAG_MESSAGE_ID_SHOW:
            testrepeatcnt[SHOWCNT] = 1;
            if (msg->params[0])
            {
                gag[0][0] = msg->params[0];
            }
            break;
#if (PRODUCT_HARDWARE_VARIANT == 0)
        case DIAG_MESSAGE_ID_BUTTON:
            testrepeatcnt[BUTTONCNT] = 4;
            buttontestmask = 0xf;
            break;
#endif
        case DIAG_MESSAGE_PON_RESET_N:
            testrepeatcnt[PONRESETCNT] = 1;
            break;
                
        case DIAG_MESSAGE_SNAP_OFF:
            testrepeatcnt[SNAPOFFCNT] = 1;
            break;

        case DIAG_MESSAGE_SNAP_REBOOT:
            testrepeatcnt[SNAPREBOOTCNT] = 200;
            break;

#if (PRODUCT_HARDWARE_VARIANT == 0)
        case DIAG_MESSAGE_ID_LCD_BACKLIGHT:
            backlight_arg[0] = msg->params[0];
            backlight_arg[1] = msg->params[1];
            backlight_arg[2] = msg->params[2];
            testrepeatcnt[LCDBACKLIGHTCNT] = 1;
            break;

        case DIAG_MESSAGE_ID_LCD_BACKLIGHT_OPT:
            if (msg->params[0])
            {
                backlight_arg[0] = msg->params[0];
                if (msg->params[1])
                {
                    backlight_arg[1] = msg->params[1];
                    lux_scale = ONE_PCNT_LUX_FULL_SCALE / msg->params[1];
                }
            }
            testrepeatcnt[LCDBACKLIGHTOPTCNT] = 1;
            break;
#endif

        case DIAG_MESSAGE_ID_OPT:
            opt[0] = msg->params[0];  //command
            opt[1] = msg->params[1];  //register 8bits, or MS delay if command==lux
            opt[2] = msg->params[2];  //value or repeatcnt if command==lux
            if (opt[0] == OPT3001_LUX_RD)
            {
                testrepeatcnt[OPTCNT] = (opt[2]) ? opt[2] : 1;
            }
            else
            {
                testrepeatcnt[OPTCNT] = 1;
            }
            break;

        case DIAG_MESSAGE_ID_IR:
            ag[0][0] = msg->params[0];
            testrepeatcnt[IR] = 1;
            break;

  	case DIAG_MESSAGE_ID_IR_BLASTER:
            opt[0] = msg->params[0];  //info type
            opt[1] = msg->params[1];  //parameter
            DIAG_UEIBlasterCommand_Info((DIAG_IR_BLASTER_COMMANDS)opt[0], opt[1]);    
            break;

        case DIAG_MESSAGE_ID_PSOC_CMD:
            testrepeatcnt[PSOCCMDCNT] = 1;
            break;

        case DIAG_MESSAGE_ID_PSOC_READ_CMD:
            psoc_read_result();
            break;
			
        case DIAG_MESSAGE_ID_BUSTEST_DSP:
            testrepeatcnt[BUSTESTDSPCNT] = msg->params[0];
            testrepeatcnt[BUSTESTIDISPLAYCNT] = 1;
            break;
        case DIAG_MESSAGE_ID_BUSTEST_F0:
            testrepeatcnt[BUSTESTF0CNT] = msg->params[0];
            testrepeatcnt[BUSTESTIDISPLAYCNT] = 1;
            break;
        case DIAG_MESSAGE_ID_BUSTEST_PSOC:
            testrepeatcnt[BUSTESTPSOCCNT] = msg->params[0];
            testrepeatcnt[BUSTESTIDISPLAYCNT] = 1;
            break;
        case DIAG_MESSAGE_ID_BUSTEST_IRB:
            testrepeatcnt[BUSTESTIRBCNT] = msg->params[0];
            testrepeatcnt[BUSTESTIDISPLAYCNT] = 1;
            break;

        case DIAG_MESSAGE_ID_Timer:
            /* Restart the scan timer */
            timerStart(s_DiagScanTimerHandle, 0, &ManagedDiagHandlerTask->Queue);

            //blink led 1, when diags is running
            if ((timeout_tickcnt & 0x3f) == 0)
            {
                led_ctrl(1, 0x12);
            }

            if ((testrepeatcnt[ADCCNT]) && (timeout_tickcnt > 2))
            {

                //cmd_adc(first,last,repeat,verb,ms delay)
                if (adcmask & 1)
                {
                    cmd_adc(0,    0,   0,     adcverb,   1);  //SENSE1
                }
                if (adcmask & 2)
                {
                    cmd_adc(1,    1,   0,     adcverb,   1);  //THERM1
                }
                if (adcmask & 4)
                {
                    cmd_adc(2,    2,   0,     adcverb,   1);  //SENSE2
                }
                if (adcmask & 8)
                {
                    cmd_adc(3,    3,   0,     adcverb,   1);  //THERM2
                }
                if (adcmask & 16)
                {
                    cmd_adc(4,    4,   0,     adcverb,   1);  //STM16_CH
                }
                testrepeatcnt[ADCCNT]--;
                timeout_tickcnt = 0;
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\n\r");
            }

            if (testrepeatcnt[LEDCNT])
            {
                if ((ledmask >> 0) & 1)
                {
                    ag[0][0] = '1';
                    dled(ac, ag);
                }
                if ((ledmask >> 1) & 1)
                {
                    ag[0][0] = '2';
                    dled(ac, ag);
                }
                if ((ledmask >> 2) & 1)
                {
                    ag[0][0] = '3';
                    dled(ac, ag);
                }
                if ((ledmask >> 3) & 1)
                {
                    ag[0][0] = '4';
                    dled(ac, ag);
                }
                testrepeatcnt[LEDCNT]--;
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\n\r");
            }

            if (testrepeatcnt[I2CCNT])
            {
                cmd_i2cscan(2, ag);
                testrepeatcnt[I2CCNT]--;
            }

            if (testrepeatcnt[GPIOCNT])
            {
                dgpio_get(3, gag);
                testrepeatcnt[GPIOCNT]--;
            }

            if (testrepeatcnt[SHOWCNT])
            {
                bb_show_all_gpio_pins();
                testrepeatcnt[SHOWCNT]--;
            }
#if (PRODUCT_HARDWARE_VARIANT == 0)
            if (testrepeatcnt[BUTTONCNT])
            {
                int bmask = d_check_button_press(buttontestmask);
                if (bmask & buttontestmask)
                {
                    testrepeatcnt[BUTTONCNT]--;
                    buttontestmask = buttontestmask & ~bmask;
                }
            }
#endif
            if (testrepeatcnt[PONRESETCNT])
            {
              	if(TRUE == check_pon_int_state())
				{
                	testrepeatcnt[PONRESETCNT] = 0;
                }
            }

            if (testrepeatcnt[SNAPOFFCNT])
            {
                snap_off();
                testrepeatcnt[SNAPOFFCNT] = 0;
            }

            if(testrepeatcnt[SNAPREBOOTCNT])
            {
             if(testrepeatcnt[SNAPREBOOTCNT] == 200)
             {
               snap_force_reboot(0);    //force start APQ reset 
               testrepeatcnt[SNAPREBOOTCNT]--;
             }
             //wait for reset  100ms per pass  = 100x99 = 9.9 seconds
             if(testrepeatcnt[SNAPREBOOTCNT]-- == 1)
             {
               snap_force_reboot(1);    //end APQ reset 
               testrepeatcnt[SNAPREBOOTCNT]--;
               testrepeatcnt[SNAPREBOOTCNT]=0;
             }
            }

#if (PRODUCT_HARDWARE_VARIANT == 0)
            if (testrepeatcnt[LCDBACKLIGHTCNT])
            {

                lcd_backlight(backlight_arg);
                backlight_arg[0] = 0;
                backlight_arg[1] = 0;
                testrepeatcnt[LCDBACKLIGHTCNT] = 0;
            }

            //read optical sensor and adjust the PWM
            if (testrepeatcnt[LCDBACKLIGHTOPTCNT])
            {
                if (SUCCESS != opt3001_rd_lux(lux))
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,  "\r\nrd lux failed!\n\r");
                    testrepeatcnt[OPTCNT] = 0;
                    break;
                }

                backlight_arg[1] = lux[0] / lux_scale;    // LUX value converted to % and scaled

                char tmpstr[20];

                sprintf(tmpstr, "%6d.%-2d", backlight_arg[1], lux[1] / 10000);

                set_pwm_percent(tmpstr);

                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,  "LUXtoPWM: %6d.%-2d PWM\n\r",
                    backlight_arg[1], lux[1] / 10000);

                lcd_backlight(backlight_arg);

                if (backlight_arg[1] == 0)
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,  "STOP LCD backlight pwm\n\r");
                    backlight_arg[0] = 0;
                    lcd_backlight(backlight_arg); //disable the PWM timer
                    testrepeatcnt[LCDBACKLIGHTOPTCNT] = 0;
                }
            }
#endif

    		if(testrepeatcnt[IR])
			{
      			DIAG_IRCommand_Handle((DIAG_IR_COMMANDS)ag[0][0]); 
      			testrepeatcnt[IR]=0;
    		}

            if (testrepeatcnt[OPTCNT])
            {
                opt_cmd(opt);
                testrepeatcnt[OPTCNT]--;
            }
            
            if(diags_ltbar_control_save_key_flag)  // IR key press
            {
              diags_ltbar_control();
              diags_ltbar_control_save_key_flag = 0;
            }
            
            if(diags_rf_remote_key_press_flag)  // RF remote key press
            {
              diags_ltbar_control();
              diags_rf_remote_key_press_flag = 0;
            }

            
            
            
            
            if (testrepeatcnt[PSOCCMDCNT])
            {
              if (FALSE == psoc_send_cmd())
              {
                testrepeatcnt[PSOCCMDCNT] = 0;  //done
              }
  
              testrepeatcnt[PSOCINTCNT] = 1;  // the psoc rd responce flag
            }         

            if(testrepeatcnt[BUSTESTDSPCNT])
            {
              if (DiagsBusTestDSPTimerHandler(testrepeatcnt[BUSTESTDSPCNT]))
              {
                if (testrepeatcnt[BUSTESTDSPCNT] != 0xffffffff )
                {
                  testrepeatcnt[BUSTESTDSPCNT]--;
                }
              }
            }
            if(testrepeatcnt[BUSTESTF0CNT])
            {
              if (DiagsBusTestF0TimerHandler(testrepeatcnt[BUSTESTF0CNT]))
              {
                if (testrepeatcnt[BUSTESTF0CNT] != 0xffffffff )
                {
                  testrepeatcnt[BUSTESTF0CNT]--;
                }
              }
            }
            if(testrepeatcnt[BUSTESTPSOCCNT])
            {
              if (DiagsBusTestPSOCTimerHandler(testrepeatcnt[BUSTESTPSOCCNT]))
              {
                if (testrepeatcnt[BUSTESTPSOCCNT] != 0xffffffff )
                {
                  testrepeatcnt[BUSTESTPSOCCNT]--;
                }
              }
            }
            if(testrepeatcnt[BUSTESTIRBCNT])
            {
              if (DiagsBusTestIRBTimerHandler(testrepeatcnt[BUSTESTIRBCNT]))
              {
                if (testrepeatcnt[BUSTESTIRBCNT] != 0xffffffff )
                {
                  testrepeatcnt[BUSTESTIRBCNT]--;
                }
              }
            }
            if(testrepeatcnt[BUSTESTIDISPLAYCNT])
            {
              if (DiagsBusTestDisplay())
              {
                testrepeatcnt[BUSTESTIDISPLAYCNT]=1;
              }
              else
              {
                testrepeatcnt[BUSTESTIDISPLAYCNT]= 0;
              }
            }

            timeout_tickcnt++;
            break;



    }
    DiagTask_HandleDiagMessage(msg);
}


static void DiagTask_HandleDiagMessage(GENERIC_MSG_t* msg)
{
    if (msg->msgID == DIAG_MESSAGE_ID_Timer)
    {
        LOG(diag, ROTTEN_LOGLEVEL_INSANE, "diag tick %d %d\n\r", msg->params[0], timeout_tickcnt);
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "%d  %d\n\r", msg->params[0], msg->params[1]);
    }
}

