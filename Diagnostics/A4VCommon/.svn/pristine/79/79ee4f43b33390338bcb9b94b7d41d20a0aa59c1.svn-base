
#include "project.h"
#include "UITask.h"
#include "AccessoryManager.h"
#include "WiredIDUtilities.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDTask.h"
#include "WiredIDInterrupt.h"

#include "WiredIDConsoleBassConnected.h"

#define  CONNECTED_POLL_MS     200

SCRIBE_DECL(wired_id);

static BOOL bWiredBassDetectEnable = FALSE;
static uint8_t num_bass_discon = 0;

static void WiredIDConsoleBassConnected_HandleTimer(void);

static AccessoryDescription_t WiredBassBoxInfo = {
    .type = ACCESSORY_INVALID,
    .position = ACCESSORY_POSITION_SUB,
    .status = ACCESSORY_CONNECTION_NONE,
    .active = ACCESSORY_ACTIVE_NOT_INITIALIZED,
    .sn = {0},
    .version = {0}
};

AccessoryDescription_t* WiredIDConsoleBassConnected_GetWiredBassBoxInfo(void)
{
    return &WiredBassBoxInfo;
}

BOOL WiredIDConsoleBassConnected_HandleMessage(GENERIC_MSG_t *msg)
{
    BOOL handled = FALSE;
    switch (msg->msgID)
    {
    case WIREDID_TimerExpired:
        WiredIDConsoleBassConnected_HandleTimer();
        handled = TRUE;
    default:        
        break;
    }
    return handled;
}

void WiredIDConsoleBassConnected_Enter(void)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL,"Enter Connected");
    num_bass_discon = 0;

    // JG-TODO!!! Shouldn't need this flag (bWiredBassDetectEnable) for 
    // wired ID state machine - maybe just in the handling of the start message
    // in the disabled state.  In all other cases, the state machine should
    // be pushed to disabled using the BASS_DISABLE message.
    if (bWiredBassDetectEnable)
    {
       AccessoryType_t bass_type = WiredIDConsole_GetBassType();
       LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Bass Type %d", bass_type);
       WiredBassBoxInfo.status = ACCESSORY_CONNECTION_WIRED;
       WiredBassBoxInfo.type = bass_type;
       UIPostMsg(UI_MSG_ID_UpdateAccessoryState, NOP_CALLBACK, (uint32_t) &WiredBassBoxInfo);
    }
    WiredID_ChangeTimerPeriod(TIMER_MSEC_TO_TICKS(CONNECTED_POLL_MS));
    WiredID_StartTimer();
}

void WiredIDConsoleBassConnected_Exit(void)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Exit Connected");    
    WiredID_StopTimer();
}

static void WiredIDConsoleBassConnected_HandleTimer(void)
{
    if (!WiredIDConsole_WiredBassConnected())
    {   
        num_bass_discon++;
    }
    else
    {
        num_bass_discon = 0;
    }
    if (num_bass_discon >= 2)
    {
            LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Bass discon, Go WaitRetry");
            WiredIDConsole_GoToState( WIRED_ID_CONSOLE_STATE_WAITRETRY);
    }    
    else
    {
        WiredID_StartTimer();
    }
}

/*
===============================================================================
@func    IsWiredBassConnected
@brief   Get wired bass status
===============================================================================
*/
BOOL IsWiredBassConnected(void)
{
    return (WiredBassBoxInfo.status == ACCESSORY_CONNECTION_WIRED);
}
/*
===============================================================================
@func    WiredBassDetectEnable
@brief   Enable/Disable Bass Detect
===============================================================================
*/
void WiredBassDetectEnable (BOOL enable)
{
    bWiredBassDetectEnable = enable; 
}

BOOL WiredIDConsole_BassDetectEnabled(void)
{
    return bWiredBassDetectEnable;
}