//
// WiredIDConsoleTask.h
//

#ifndef WIRED_ID_CONSOLE_TASK_H
#define WIRED_ID_CONSOLE_TASK_H

#include "project.h"
#include "RivieraLPM_IpcProtocol.h"

#define CONSOLE_DEFAULT_PULSE_MS    700

typedef enum
{
    WIRED_ID_CONSOLE_STATE_DISABLED,
    WIRED_ID_CONSOLE_STATE_STARTSCAN,
    WIRED_ID_CONSOLE_STATE_WFFSCAN,
    WIRED_ID_CONSOLE_STATE_WFRSCAN,
    WIRED_ID_CONSOLE_STATE_CONFIRM,
    WIRED_ID_CONSOLE_STATE_CONNECTED,
    WIRED_ID_CONSOLE_STATE_WAITRETRY,
    WIRED_ID_NUM_STATES
} WiredIDConsoleState_t;

struct WIREDID_STATE_HANDLERS
{
    BOOL (*HandleMessage)(GENERIC_MSG_t*);
    void (*Init)(void);
    void (*EnterState)(void);
    void (*ExitState)(void);
};

WiredIDConsoleState_t  WiredIDConsole_GetState(void);
void WiredIDConsole_GoToState(WiredIDConsoleState_t state);

//Legacy calls by other subsystems, don't rename them
void WiredBassDetectEnable(BOOL enable);
BOOL WiredIDConsole_BassDetectEnabled(void);
void WiredBass_DC_TX_SetState(BOOL enable);
BOOL IsWiredBassConnected(void);
BOOL WiredIDConsole_WiredBassConnected(void);


// Data common to multiple states
#define  MAX_EXPECTED_EDGES     5
extern uint16_t input_edge_index;
extern uint32_t edge_timestamps[MAX_EXPECTED_EDGES];
extern uint32_t bass_avg_low;
extern uint32_t bass_avg_high;

AccessoryType_t WiredIDConsole_GetBassType(void);
void WiredIDConsole_SetBassType(AccessoryType_t bb_type);

#endif // WIRED_ID_CONSOLE_TASK_H
