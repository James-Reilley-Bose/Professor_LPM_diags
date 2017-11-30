
#include "project.h"
#include "genTimer.h"
#include "EventDefinitions.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDTask.h"
#include "WiredIDUtilities.h"
#include "WiredIDInterrupt.h"

#include "WiredIDConsoleBase.h"
#include "WiredIDCOnsoleDisabled.h"
#include "WiredIDConsoleStartScan.h"
#include "WiredIDConsoleWFFScan.h"
#include "WiredIDConsoleWFRScan.h"
#include "WiredIDCOnsoleConfirming.h"
#include "WiredIDConsoleBassConnected.h"
#include "WiredIDConsoleWaitRetry.h"

SCRIBE_DECL(wired_id);

// Data common to multiple states
// JG-TODO!!! - Use accessor functions, init function
uint16_t input_edge_index = 0;
uint32_t edge_timestamps[MAX_EXPECTED_EDGES] = {0};
uint32_t bass_avg_low = 0;
uint32_t bass_avg_high = 0;
// End common data

static AccessoryType_t bassbox_type = ACCESSORY_INVALID;

static WiredIDConsoleState_t  current_state = WIRED_ID_CONSOLE_STATE_DISABLED;
static WiredIDConsoleState_t  prev_state = WIRED_ID_CONSOLE_STATE_DISABLED;
static void WiredIDConsole_HandleMessage(GENERIC_MSG_t *msg);

static const struct WIREDID_STATE_HANDLERS consoleStateHandlers[WIRED_ID_NUM_STATES] = 
{ 
    //Disabled
    {
        WiredIDConsoleDisabled_HandleMessage,
        NULL,
        WiredIDConsoleDisabled_EnterState,
        WiredIDConsoleDisabled_ExitState,        
    },
    // StartScan
    {
         WiredIDConsoleStartScan_HandleMessage,
         WiredIDConsoleStartScan_Init,
         WiredIDConsoleStartScan_Enter,
         WiredIDConsoleStartScan_Exit,        
    },
    // WFF-Scan
    {
         WiredIDConsoleWFFScan_HandleMessage,
         WiredIDConsoleWFFScan_Init,
         WiredIDConsoleWFFScan_Enter,
         WiredIDConsoleWFFScan_Exit,        
    },
    // WFR-Scan
    {
         WiredIDConsoleWFRScan_HandleMessage,
         WiredIDConsoleWFRScan_Init,
         WiredIDConsoleWFRScan_Enter,
         WiredIDConsoleWFRScan_Exit,              
    },
    //Confirm
    {
        WiredIDConsoleConfirm_HandleMessage,
        WiredIDConsoleConfirm_Init,
        WiredIDConsoleConfirm_Enter,
        WiredIDConsoleConfirm_Exit,
    },
    //Connected
    {
        WiredIDConsoleBassConnected_HandleMessage,
        NULL,
        WiredIDConsoleBassConnected_Enter,
        WiredIDConsoleBassConnected_Exit,        
    },
    //WaitRetry
    {
        WiredIDConsoleWaitRetry_HandleMessage,
        WiredIDConsoleWaitRetry_Init,
        WiredIDConsoleWaitRetry_Enter,
        WiredIDConsoleWaitRetry_Exit,
    },
};

static const char* state_names[WIRED_ID_NUM_STATES] =  
{
    "Disabled",
    "StartScan",
    "WFF-Scan",
    "WFR-Scan",
    "Confirm",
    "Connected",
    "WaitRetry"
};

/* 
 * @func  WiredIDVariant_Init
 * @brief Console ID task Init, called before scheduler
 */
void WiredIDVariant_Init(void)
{
    WiredIDVariantUtils_ConfigIO();
    WiredIDUtils_InitTimer(TIMER_MSEC_TO_TICKS(CONSOLE_DEFAULT_PULSE_MS));
    
    // Set up for interrupts on edge
    WiredID_ConfigEdgeDetectEXTI();
}

/* 
 * @func    WiredIDVariant_HandleMessage 
 * @brief   Console Wired ID task message handler
 */

void WiredIDVariant_HandleMessage(GENERIC_MSG_t *msg)
{
    WiredIDConsole_HandleMessage(msg);
}

/* 
 * @func    WiredIDConsole_GetState
 * @brief   Return current Wired ID state
 */
WiredIDConsoleState_t  WiredIDConsole_GetState(void)
{
    return current_state;
}

/* 
 * @func  WiredIDConsole_HandleMessage
 * @brief Message Handler. 
 */
static void WiredIDConsole_HandleMessage(GENERIC_MSG_t *msg)
{
    //LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "msg: %s",GetEventString(msg->msgID));
    if((!consoleStateHandlers[current_state].HandleMessage) ||
       !consoleStateHandlers[current_state].HandleMessage(msg))
    {
        WiredIDConsoleBase_HandleMessage(msg);
    }
}

/* 
 * @func   WiredIDConsole_GoToState
 * @brief  Go to another state
 */
void WiredIDConsole_GoToState(WiredIDConsoleState_t state)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "GoToState: %s", state_names[state]);
    if ((state >= WIRED_ID_NUM_STATES) || (state == current_state))
    {
        return;
    }
    if (consoleStateHandlers[current_state].ExitState)
    {
        consoleStateHandlers[current_state].ExitState();
    }
    prev_state = current_state;
    current_state = state;
    if (consoleStateHandlers[current_state].EnterState)
    {
        consoleStateHandlers[current_state].EnterState();
    }
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "state: %s -> %s", state_names[prev_state], state_names[current_state]); 
}

void WiredIDVariant_StartIDStateMachine(void)
{
    WiredIDConsoleState_t state = WIRED_ID_CONSOLE_STATE_DISABLED;
    for (; state < WIRED_ID_NUM_STATES; state++)
    {
        if (consoleStateHandlers[state].Init)
        {
            consoleStateHandlers[state].Init();
        }
    }
    if(consoleStateHandlers[current_state].EnterState)
    {
        consoleStateHandlers[current_state].EnterState();
    }
}

AccessoryType_t WiredIDConsole_GetBassType(void)
{
    return bassbox_type;
}

void WiredIDConsole_SetBassType(AccessoryType_t bb_type)
{
    bassbox_type = bb_type;
}

