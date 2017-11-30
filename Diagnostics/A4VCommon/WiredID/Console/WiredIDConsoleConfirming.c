
#include "project.h"
#include "genTimer.h"
#include "EventDefinitions.h"
#include "WiredIDUtilities.h"
#include "WiredIDInterrupt.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDTask.h"

#define  MAX_CONFIRM_TIME_MS   5000
/*When console sends confirmation pusles, it continues to get pulses
 *from bassbox. When bass gets two confirm cycles, it should set BDT high
 *If console sees BDT remails high for 1sec, it knows bass is enabled
 *and can enable the bass channe; If console sees BDT low for one second,
 *it goes back to scan
 */
#define  BASS_ENABLE_MS        1000
#define  BASS_DISABLE_MS       1000

SCRIBE_DECL(wired_id);

static void WiredIDConsoleConfirm_HandlePulseTimerOff(void);
static void WiredIDConsoleConfirm_HandleInputPulse(void);

static void WiredIDConsoleConfirm_HandleRiseTimer(void);
static void WiredIDConsoleConfirm_HandleFallTimer(void);
static void WiredIDConsoleConfirm_HandleRiseEdge(void);
static void WiredIDConsoleConfirm_HandleFallEdge(void);

static TimerHandle_t wait_rise_timer = NULL;
static TimerHandle_t wait_fall_timer = NULL;

typedef enum
{
    UNKNOWN,
    WAIT_FOR_RISE,
    WAIT_FOR_FALL,
} CONFIRM_SUBSTATE_t;

CONFIRM_SUBSTATE_t confirm_substate = UNKNOWN; 

/* 
 * @func    WiredIDConsoleConfirm_Init
 * @brief   Init resources used in confirm state
 */
void WiredIDConsoleConfirm_Init(void)
{
    wait_rise_timer = createTimer(TIMER_MSEC_TO_TICKS(BASS_DISABLE_MS),
                                  NULL,
                                  WIREDID_WaitRiseExpired,
                                  FALSE,
                                  tBlockIdWaitRise,
                                  "WaitRiseTimer",
                                  NULL);
    debug_assert(wait_rise_timer);
    
    wait_fall_timer = createTimer(TIMER_MSEC_TO_TICKS(BASS_ENABLE_MS),
                                  NULL,
                                  WIREDID_WaitFallExpired,
                                  FALSE,
                                  tBlockIdWaitFall,
                                  "WaitFallTimer",
                                  NULL);
    debug_assert(wait_fall_timer);
}

/* 
 * @func     WiredIDConsoleConfirm_HandleMessage
 * @brief    Confirm message handler
 */
BOOL WiredIDConsoleConfirm_HandleMessage(GENERIC_MSG_t *msg)
{
    BOOL handled = FALSE;
    switch (msg->msgID)
    {
    case WIREDID_PulseTimerOff:
        WiredIDConsoleConfirm_HandlePulseTimerOff();
        handled = TRUE;
        break;
    case WIREDID_InputPulse:
        WiredIDConsoleConfirm_HandleInputPulse();
        handled = TRUE;
        break;
    case WIREDID_WaitFallExpired:
        WiredIDConsoleConfirm_HandleFallTimer();
        handled = TRUE;
        break;
    case WIREDID_WaitRiseExpired:
        WiredIDConsoleConfirm_HandleRiseTimer();
        handled = TRUE;
        break;
    default:
        break;
    }
    return handled;
}

/* 
 * @func     WiredIDConsoleConfirm_Enter
 * @brief    Console Confirm Entry
 */
void WiredIDConsoleConfirm_Enter(void)
{    
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Enter Confirm");
    uint16_t console_high = 0;
    uint16_t console_low  = 0;     
    uint8_t  start_level = 0;   
  
    //Get the confirm pulse timer high/low duration based on bass pulses
    //TODO: Better algorithm and time
    WiredIDConsole_GetOutputPulseTimes(&console_low, &console_high);
   
    if ((console_low != 0) && (console_high != 0))
    {
        //Send Confirm pulses
        WiredIDUtils_StartIDPulse(console_high, console_low, start_level);
    }
    else
    {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "invalid pulse times");
    }
    
    // Enable interrupts
    WiredID_EnableEdgeDetectEXTI(TRUE);
}

/* 
 * @func    WiredIDConsoleConfirm_Exit
 * @brief   Console Confirm Exit
 */
void WiredIDConsoleConfirm_Exit(void)
{
    // Disable interrupts
    WiredID_EnableEdgeDetectEXTI(FALSE);

    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Exit Confirm");

    WiredIDUtils_StopIDPulse();
    //stop all SW timers
    timerStop(wait_rise_timer, portMAX_DELAY);
    timerStop(wait_fall_timer, portMAX_DELAY);
     //Make sure ConsoleDC_TX high    
    WiredBass_DC_TX_SetState(TRUE);
}

/* 
 * @func    WiredID_HandlePulsetimerOff
 * @brief   Console generates pulses as confirmation to bassbox. 
 * pulse duty cycle same as received from bassbox, this is output
 */
static void WiredIDConsoleConfirm_HandlePulseTimerOff(void)
{
    //HW timer ISR changes output level, this is only for debug and test purpose
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "confirm: %d", WiredIDVariantUtils_GetOutputPulseLevel());
}

/* 
 * @func   WiredIDConsoleConfirm_HandleInputPulse
 * @brief  Process bass pulses during confirm. See comments on top
 */
static void WiredIDConsoleConfirm_HandleInputPulse(void)
{
    uint8_t level = WiredIDUtils_GetInputPulseLevel();
    LOG(wired_id, ROTTEN_LOGLEVEL_VERBOSE, "input %d", level);

#if TEST    
    //For Test only
    GPIO_WriteBit(BASSBOX_RX_TEST_BANK, BASSBOX_RX_TEST_MASK, level? Bit_SET: Bit_RESET);
#endif
    
    if(level)
    {
        WiredIDConsoleConfirm_HandleRiseEdge();
    }
    else
    {
        WiredIDConsoleConfirm_HandleFallEdge();
    }
}
static void WiredIDConsoleConfirm_HandleRiseEdge(void)
{
    timerStop(wait_rise_timer, portMAX_DELAY);
    timerStart(wait_fall_timer, 0, &wiredIDTaskHandle->Queue);
    confirm_substate = WAIT_FOR_FALL;
}

static void WiredIDConsoleConfirm_HandleFallEdge(void)
{
    timerStop(wait_fall_timer, portMAX_DELAY);
    timerStart(wait_rise_timer, 0, &wiredIDTaskHandle->Queue);
    confirm_substate = WAIT_FOR_RISE;
}

//If pulse not received, one sw timer goes off
void  WiredIDConsoleConfirm_HandleRiseTimer(void)
{
    //BDT stays 0 for one second, back to scan
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "WFR Expired, Rescan");
    WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WAITRETRY);
}

void  WiredIDConsoleConfirm_HandleFallTimer(void)
{
    //BDT stays hi a second, bassConnected
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "WFF Expired, connected");
    WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_CONNECTED);
}


