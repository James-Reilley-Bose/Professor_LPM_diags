#include "project.h"
#include "genTimer.h"
#include "EventDefinitions.h"
#include "WiredIDConsoleWaitRetry.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDTask.h"
#include "WiredIDInterrupt.h"

//Bass Retry timer is 2000ms, Console Retry timer should be greater than that.
#define CONSOLE_WAIT_RETRY_TIME_MSEC   3500

static void WiredIDConsoleWaitRetry_HandleTimer(void);
SCRIBE_DECL(wired_id);

void WiredIDConsoleWaitRetry_Init(void)
{
    //nothing for now
}

BOOL WiredIDConsoleWaitRetry_HandleMessage(GENERIC_MSG_t *msg)
{
    BOOL handled = FALSE;
    switch (msg->msgID)
    {
    case WIREDID_TimerExpired:
        WiredIDConsoleWaitRetry_HandleTimer();
        handled = TRUE;
        break;
    default:
        break;
    }
    return handled;
}

void WiredIDConsoleWaitRetry_Enter(void)
{
    // Disable interrupts
    WiredID_EnableEdgeDetectEXTI(FALSE);
    
    WiredBass_DC_TX_SetState(FALSE);
    WiredID_ChangeTimerPeriod(TIMER_MSEC_TO_TICKS(CONSOLE_WAIT_RETRY_TIME_MSEC));
    WiredID_StartTimer();
}

void WiredIDConsoleWaitRetry_Exit(void)
{
    WiredID_StopTimer();    
}

static void WiredIDConsoleWaitRetry_HandleTimer(void)
{
    WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_DISABLED);
}