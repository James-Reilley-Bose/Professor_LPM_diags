#include "WiredIDConsoleWFFScan.h"
#include "EventDefinitions.h"
#include "WiredIDTask.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDUtilities.h"
#include "WiredIDInterrupt.h"

static void WiredIDConsoleWFRScan_HandleInputIDPulse(void);
static void WiredIDConsoleWFRScan_HandleWFRRetryTimerExpired(void);
static inline void StartWFRRetryTimer(void);
static inline void StopWFRRetryTimer(void);

#define WAIT_RISING_EDGE_MS 1000
SCRIBE_DECL(wired_id);

BOOL WiredIDConsoleWFRScan_HandleMessage(GENERIC_MSG_t *msg) {
    BOOL handled = TRUE;
    switch (msg->msgID) {

        case WIREDID_InputPulse:
            WiredIDConsoleWFRScan_HandleInputIDPulse();
            handled = TRUE;
        break;
        
        case WIREDID_TimerExpired:
            WiredIDConsoleWFRScan_HandleWFRRetryTimerExpired();
            handled = TRUE;  
        break;

        default:
          LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,
              "WFRScan - Unhandled Event: %d", msg->msgID);
        break;
    }

    return handled;
}

void WiredIDConsoleWFRScan_HandleInputIDPulse() {
  
    uint8_t level = WiredIDUtils_GetInputPulseLevel();
    // If DC input level is high, this is a rising edge.
    if(level) {

        // Record the time stamp.
        if(input_edge_index < MAX_EXPECTED_EDGES) {
           
          edge_timestamps[input_edge_index] =  GET_SYSTEM_UPTIME_MS();
           LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,
               "WFRScan - Recorded rising edge.  Index=%d, timestamp=%d", input_edge_index, edge_timestamps[input_edge_index]);
           WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WFFSCAN);
           
           input_edge_index++;

        }
        else {
            LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,
              "WFRScan - unexpected index >= max: %d", input_edge_index);
            WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WAITRETRY);

        }
    }
    else {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"WFRScan - Unexpected Falling Edge");
    }
}


void WiredIDConsoleWFRScan_HandleWFRRetryTimerExpired(void) {

    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"WFR-Scan Retry Timer Expired");
    WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WAITRETRY);     
}

// Function is here for readability, so make inline to reduce added
// overhead.
inline void StartWFRRetryTimer(void)
{
    WiredID_StartTimer();
}

// Function is here for readability, so make inline to reduce added
// overhead.
inline void StopWFRRetryTimer(void)
{
    WiredID_StopTimer();
}

void WiredIDConsoleWFRScan_Init(void) {
    // JG-TODO!!! Nothing to do here.  Remove
}

void WiredIDConsoleWFRScan_Enter(void) {
    // Set timer period and start retry timer
    // Re-use timer
    WiredID_ChangeTimerPeriod(WAIT_RISING_EDGE_MS);
    StartWFRRetryTimer();
    
    // Enable interrupts
    WiredID_EnableEdgeDetectEXTI(TRUE);
}

void WiredIDConsoleWFRScan_Exit(void) {

    // Disable interrupts
    WiredID_EnableEdgeDetectEXTI(FALSE);
  
    // Stop retry timer
    StopWFRRetryTimer();
}
