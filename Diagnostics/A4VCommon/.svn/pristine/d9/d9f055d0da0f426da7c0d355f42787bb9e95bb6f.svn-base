#include "WiredIDConsoleWFFScan.h"
#include "EventDefinitions.h"
#include "WiredIDTask.h"
#include "WiredIDConsoleTask.h"
#include "AccessoryManager.h"
#include "WiredIDUtilities.h"
#include "WiredIDInterrupt.h"

// Local functions
static void WiredIDConsoleWFFScan_HandleInputIDPulse(void);
static inline void StartSkipperDetectTimer(void);
static inline void StopSkipperDetectTimer(void);
static BOOL WiredIDConsoleWFFScan_GetResult(void);
static void WiredIDConsoleWFFScan_HandleSkipperTimerExpired(void);

#define SKIPPER_DETECT_TIMEOUT_MS  2000
SCRIBE_DECL(wired_id);

BOOL WiredIDConsoleWFFScan_HandleMessage(GENERIC_MSG_t *msg) {
    BOOL handled = FALSE;

    switch (msg->msgID) {

        case WIREDID_InputPulse:
            WiredIDConsoleWFFScan_HandleInputIDPulse();
            handled = TRUE;
        break;
        
        case WIREDID_TimerExpired:
            WiredIDConsoleWFFScan_HandleSkipperTimerExpired();
            handled = TRUE;  
        break;

        default:
        break;
    }

    return handled;
}

void WiredIDConsoleWFFScan_HandleInputIDPulse(void) {
    uint8_t level = WiredIDUtils_GetInputPulseLevel();
    // If DC input level is low, this is a falling edge.
    if(!level) {
              
        // Record the time stamp.
        if(input_edge_index < MAX_EXPECTED_EDGES) {
            edge_timestamps[input_edge_index] =  GET_SYSTEM_UPTIME_MS();
            
            LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,
            "WFFScan - Recorded falling edge.  Index=%d, timestamp=%d", input_edge_index, edge_timestamps[input_edge_index]);
        }

        // If this is not the last edge
        if(++input_edge_index < MAX_EXPECTED_EDGES) {
            
            // Start waiting for a rising edge.
            WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WFRSCAN);
        }
        else {        
            // If this is the last pulse, check pulse widths to see if
            // we are connected to a PBB.
            
            // If PBB
            if(WiredIDConsoleWFFScan_GetResult()) {
                // Go to confirm state 
                WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_CONFIRM);
            }
            else {
                // Retry
                WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WAITRETRY);
            }
        }
    }
    else {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"WFFScan - Unexpected Rising Edge");
    }    
}

// Function is here for readability, so make inline to reduce added
// overhead.
inline void StartSkipperDetectTimer(void)
{
    WiredID_StartTimer();
}

// Function is here for readability, so make inline to reduce added
// overhead.
inline void StopSkipperDetectTimer(void)
{
    WiredID_StopTimer();
}

void WiredIDConsoleWFFScan_HandleSkipperTimerExpired(void) {
  
    if (WiredIDConsole_WiredBassConnected())
    {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"WFFScan - Detected Skipper");
        WiredIDConsole_SetBassType(ACCESSORY_SKIPPER);
        WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_CONNECTED);       
    }
}

// JG-TODO!!! Remove this function if not needed.  So far, nothing to put here.
void WiredIDConsoleWFFScan_Init(void) {
}

void WiredIDConsoleWFFScan_Enter(void) {
    // Set timer period and start Skipper detect timer
    // Re-use timer for skipper detect.
    // JG-TODO!!! Might not want to re-use just in case a queued timer event
    // generated before entering this state causes a false detection of skipper.
    WiredID_ChangeTimerPeriod(SKIPPER_DETECT_TIMEOUT_MS);
    StartSkipperDetectTimer();
    
    // Enable interrupts
    WiredID_EnableEdgeDetectEXTI(TRUE);
}

void WiredIDConsoleWFFScan_Exit(void) {
    
    // Disable interrupts
    WiredID_EnableEdgeDetectEXTI(FALSE);

    // Stop skipper detect timer
    StopSkipperDetectTimer();
}

// Compare edges to expected values, record averages to use to determine
// duty cycle for confirmation pulses.
BOOL WiredIDConsoleWFFScan_GetResult(void)
{
    BOOL result = FALSE;
    uint32_t first_low   = edge_timestamps[1] - edge_timestamps[0];
    uint32_t first_high  = edge_timestamps[2] - edge_timestamps[1];
    uint32_t second_low  = edge_timestamps[3] - edge_timestamps[2];
    uint32_t second_high = edge_timestamps[4] - edge_timestamps[3];
    bass_avg_low   = (first_low + second_low)/ 2;
    bass_avg_high  = (first_high + second_high)/2;
    uint16_t high_time = 0;
    uint16_t low_time = 0;
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Got Pulses: LOW1 %d, HI1 %d, LOW2 %d, HI2 %d", \
                        first_low, first_high, second_low, second_high);
    
    if ((bass_avg_low > PBB_LOW_TIME - BASS_VARIANCE) && (bass_avg_low < PBB_LOW_TIME + BASS_VARIANCE) && \
        (bass_avg_high > PBB_HIGH_TIME - BASS_VARIANCE) && (bass_avg_high < PBB_HIGH_TIME + BASS_VARIANCE))
    {
        //console pulse low and high level time for confirmation
        low_time  = CONSOLE_HIGH_TIME;
        high_time = CONSOLE_HIGH_TIME;
        WiredIDConsole_SetBassType(ACCESSORY_LOVEY);
        result = TRUE;
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "bass: LOVEY");
    }
    else if (((bass_avg_low > BASS2_LOW_TIME - BASS_VARIANCE) && (bass_avg_low < BASS2_LOW_TIME + BASS_VARIANCE) && \
              (bass_avg_high > BASS2_HIGH_TIME - BASS_VARIANCE) && (bass_avg_high < BASS2_HIGH_TIME + BASS_VARIANCE)))
    {
        //Future, Don't know yet
        low_time = CONSOLE2_LOW_TIME;
        high_time = CONSOLE2_HIGH_TIME;
        //Don't know the name of next bassbox yet, set to INVALID now. 
        WiredIDConsole_SetBassType(ACCESSORY_INVALID);
        result = TRUE;
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "bass: future");
    }
    else
    {
        low_time = 0;
        high_time = 0;
        result = FALSE;
    }             

    if (result)
    {        
        //Confirmation state will use the time to send pulses
        WiredIDConsole_SetOutputPulseTimes(low_time, high_time);    
    }
    else
    {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "unknown bass");        
    }          
    return result;    
}
