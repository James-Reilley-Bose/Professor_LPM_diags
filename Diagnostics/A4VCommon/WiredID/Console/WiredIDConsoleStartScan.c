#include "WiredIDConsoleStartScan.h"
#include "genTimer.h"
#include "EventDefinitions.h"
#include "RivieraLPM_IpcProtocol.h"
#include "AccessoryManager.h"
#include "UITask.h"
#include "WiredIDTask.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDUtilities.h"
#include "WiredIDInterrupt.h"

// JG-TODO!!! Get rid of this dependency
#include "WiredIDConsoleBassConnected.h"

// Local functions
static void WiredIDConsoleSartScan_HandleInputIDPulse(void);
static void WiredIDConsoleStartScan_HandleNoWiredBassTimer(void);

// Local variables
static TimerHandle_t no_wired_bass_timer = NULL;

//No wired bass timer 30 seconds
#define NO_BASS_TIMEOUT_SEC 30

SCRIBE_DECL(wired_id);

BOOL WiredIDConsoleStartScan_HandleMessage(GENERIC_MSG_t *msg) {

    BOOL handled = FALSE;

    switch (msg->msgID) {

        case WIREDID_InputPulse:
            WiredIDConsoleSartScan_HandleInputIDPulse();
            handled = TRUE;
        break;

        case WIREDID_NoWiredBassTimerExpired:
            WiredIDConsoleStartScan_HandleNoWiredBassTimer();
            handled = TRUE;
        break;

        default:
          LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"StartScan - Unhandled Event: %d",msg->msgID);
        break;
    }

    return handled;
}

void WiredIDConsoleStartScan_Init(void)
{
    //Create "no-bass" timer
    no_wired_bass_timer = createTimer(TIMER_SEC_TO_TICKS(NO_BASS_TIMEOUT_SEC),
                                   NULL,
                                   WIREDID_NoWiredBassTimerExpired,
                                   FALSE,
                                   tBlockIdNoWiredBass,
                                   "NoWiredBassTimer",
                                   NULL);
    debug_assert(no_wired_bass_timer);
}

void WiredIDConsoleStartScan_Enter(void)
{
    // Start the no-bass timer
    timerStart(no_wired_bass_timer, 0, &wiredIDTaskHandle->Queue);
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,
        "StartScan - starting no-bass timer");
    
    // Initialize scan variables
    // JG-TODO!!! Use accessor function, common init function for shared data
    input_edge_index = 0;
    bass_avg_low = 0;
    bass_avg_high = 0;
    for(uint8_t i = 0; i < MAX_EXPECTED_EDGES; i++)
    {
        edge_timestamps[i] = 0;
    }
    
    WiredIDConsole_SetBassType(ACCESSORY_INVALID);
    
    uint8_t level = WiredIDUtils_GetInputPulseLevel();
    
    // If the level is currently high, immediately go to WFF state.
    if(level) {
        // Generate an input pulse event to force a transition to the WFFScan 
        // state.  When this state receives the event it will check the level
        // again and transition to the WFFScan state if it's still high.
        WiredIDPostMsg(WIREDID_InputPulse, NOP_CALLBACK, NULL);
    }
    
    // Enable interrupts
    WiredID_EnableEdgeDetectEXTI(TRUE);
}

void WiredIDConsoleStartScan_Exit(void) {

    // Disable interrupts
    WiredID_EnableEdgeDetectEXTI(FALSE);
        
    if (timerIsRunning(no_wired_bass_timer))
    {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,
            "StartScan - stopping no-bass timer");
        timerStop(no_wired_bass_timer, 0);
    }
}

void WiredIDConsoleSartScan_HandleInputIDPulse(void)
{
    uint8_t level = WiredIDUtils_GetInputPulseLevel();
    
    // If DC input level is high, a console is present so start 
    // measuring pulse widths.  The first step is to wait for the next
    // falling edge.
    if(level) {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"StartScan - received rising edge");
        WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WFFSCAN);
    }
    else {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"StartScan - received falling edge");
    }
}

void WiredIDConsoleStartScan_HandleNoWiredBassTimer(void)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "StartScan - No Wired Bass");
    
    // JG-TODO!!! Move this common data/accessor to WiredIDConsoleTask.
    AccessoryDescription_t* bass_info = 
      WiredIDConsoleBassConnected_GetWiredBassBoxInfo();
    
    bass_info->status = ACCESSORY_CONNECTION_NONE;
    bass_info->type =   ACCESSORY_INVALID;
    UIPostMsg(UI_MSG_ID_UpdateAccessoryState, NOP_CALLBACK, (uint32_t) bass_info);  
}
