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
#include "Diag_i2c_test.h"
#include "serialPSOCListener.h"
#include <stdio.h>

SCRIBE_DECL(diag);


ManagedTask* ManagedDiagHandlerTask = NULL;


static void DiagTask_HandleDiagMessage(GENERIC_MSG_t* msg);
static TimerHandle_t s_DiagScanTimerHandle;

int testrepeatcnt[DIAGTESTCNTLAST];

ManagedTask* DiagTaskHandle = NULL;

static int micmute_state = 0;
void check_mute_button(void)
{
  int curval;
  curval = GPIO_ReadInputDataBit(MIC_MUTE_BANK, 0x1 << MIC_MUTE_PIN);
  if(micmute_state !=  curval)
  {
    micmute_state = curval;
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "MIC MUTE %s", (curval)?"ON":"OFF");  
  }
}
  


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
//static uint32_t buttontestmask = 0xf;

char ag1[] = {'1', 0};
char ag2[] = {'t', 0};
char* ag[] = {ag1, ag2};

char gag1[] = {0x61, 0};
char gag2[] = {'1', 0};
char* gag[] = {gag1, gag2};

uint32_t opt[] = {0, 0, 0, 0}; //save opt request args

//static uint32_t lux_scale = ONE_PCNT_LUX_FULL_SCALE;

static void diags_help(void){
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt help", "this help");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt adc,repeatcnt,mask,verbosity", "adc read values");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt i2c,bus#", "i2cscan bus 1,2 or 3");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt gpio,bank", "dump gpiox info,bank=a,b,c,d,e,f,g,h,i");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt show", "show all gpio pins");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt psoc,<>,<>", "commands to psoc");
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt opt3001,init", "init opt, not required" );
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt opt3001,<wr,rd>,<reg>,<val>", "write or read reg value, hex" );
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt opt3001,<lux>,<MSdelay>,<repeatcnt>", "get output conterted to LUX, decimal intputs" );
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt opt3001,<bus>,<bus #>", "set the i2c bus 1,2,3, and init" );
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt lopt,<delay>,<gain>", "opt controls light bar intesity" );
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%-36s - %s", "dt lopt ", "execute cmd to start, again to stop" );
  LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\n\r");
}

void DiagTask_HandleMsg(GENERIC_MSG_t* msg)
{
    int ac = 2;

    switch (msg->msgID)
    {
        case DIAG_MESSAGE_ID_HELP:
          diags_help();
          break;

        case DIAG_MESSAGE_ID_ADC:
            testrepeatcnt[ADCCNT] = 1;
            dadc_config();  //init adc registers
            adcmask = 0x1f; // default to all
            if(msg->params[0]){
              testrepeatcnt[ADCCNT] = msg->params[0]&0xff;
              if(msg->params[1]){
                adcmask = msg->params[1]&0xff;
              }
              if(msg->params[2]){ 
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
                if (msg->params[1])
                {
                    ledmask = msg->params[1] & 0xff;
                }
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

        case DIAG_MESSAGE_ID_BUTTON:
//            testrepeatcnt[BUTTONCNT] = 4;
//            buttontestmask = 0xf;
            break;

        case DIAG_MESSAGE_PON_RESET_N:
            testrepeatcnt[PONRESETCNT] = 1;
            break;
                
        case DIAG_MESSAGE_SNAP_OFF:
            testrepeatcnt[SNAPOFFCNT] = 1;
            break;

        case DIAG_MESSAGE_SNAP_REBOOT:
            testrepeatcnt[SNAPREBOOTCNT] = 200;
            break;

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
            
        case DIAG_MESSAGE_ID_LED_OPT:
          testrepeatcnt[LEDOPTCNT] = (testrepeatcnt[LEDOPTCNT])?0:1; //toggle
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "LOPT %sabled\n\r",(testrepeatcnt[LEDOPTCNT])?"en":"dis");
          break;   
          
        case DIAG_MESSAGE_ID_PSOC_CMD:
            testrepeatcnt[PSOCCMDCNT] = 1;
            break;
            
        case DIAG_MESSAGE_ID_PSOC_READ_CMD:
            psoc_read_result();
            break;

        case DIAG_MESSAGE_ID_Timer:
            /* Restart the scan timer */
            timerStart(s_DiagScanTimerHandle, 0, &ManagedDiagHandlerTask->Queue);

            //blink led 1, when diags is running
            if ((timeout_tickcnt & 0x3f) == 0)
            {
                led_ctrl(1, 0x12);
            }

            check_mute_button();
            
            if(testrepeatcnt[LEDOPTCNT])
            {
              diags_opt_ltbar_control(); //read OPT and set LtBar intensity
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

            if (testrepeatcnt[BUTTONCNT])
            {
#if 0
                int bmask = d_check_button_press(buttontestmask);
                if (bmask & buttontestmask)
                {
                    testrepeatcnt[BUTTONCNT]--;
                    buttontestmask = buttontestmask & ~bmask;
                }
#endif
            }

            if (testrepeatcnt[PONRESETCNT])
            {
              if(TRUE == check_pon_int_state()){
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

            if (testrepeatcnt[PSOCCMDCNT])
            {
              if (FALSE == psoc_send_cmd())
              {
                testrepeatcnt[PSOCCMDCNT] = 0;  //done
              }
  
              testrepeatcnt[PSOCINTCNT] = 1;  // the psoc rd responce flag
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

