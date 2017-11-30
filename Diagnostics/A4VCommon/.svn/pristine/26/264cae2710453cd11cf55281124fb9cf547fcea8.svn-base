
#include "project.h"
#include "DeviceModel_DSP.h"
#include "WiredIDUtilities.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDTask.h"
#include "WiredIDInterrupt.h"

#define  SCAN_FOR_BASS_MS  100

SCRIBE_DECL(wired_id);

BOOL WiredIDConsoleDisabled_HandleMessage(GENERIC_MSG_t *msg)
{
    BOOL handled = FALSE;
    switch(msg->msgID)
    {
    case WIREDID_CONSOLE_MESSAGE_ID_START_SCAN:
        if (WiredIDConsole_BassDetectEnabled())
        {
            WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_STARTSCAN);
        }
        handled = TRUE;
        break;
    case WIREDID_CONSOLE_MESSAGE_ID_BASS_DISABLE:
        // Override default behavior and stay in this state if bass disable
        // notification received.
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Disabled - received BASS_DISABLE");
        handled = TRUE;
        break;
    default:
        break;
    } 
    return handled;    
}

void WiredIDConsoleDisabled_EnterState(void)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Entered Disabled, DC_TX 0");
    WiredBass_DC_TX_SetState(FALSE);
    
    // If DSP has set bass enabled to TRUE, immediately start scanning.
    if(DeviceModel_DSP_GetBassEnableState()) {
        LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Disabled - got bass enabled upon entry");

        // Send a message to this state machine to force transition to 
        // StartScan state
        WiredIDPostMsg(WIREDID_CONSOLE_MESSAGE_ID_START_SCAN, NOP_CALLBACK, NULL);
    }
}

void WiredIDConsoleDisabled_ExitState(void)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Exit Disabled, DC_TX 1");
    
    // Set DC output to high
    WiredBass_DC_TX_SetState(TRUE);
}