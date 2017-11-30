//
// SystemStateTask.c
//
#include "BMutex.h"
#include "genTimer.h"
#include "TaskDefs.h"
#include "PowerTask.h"
#include "TaskManager.h"
#include "IPCRouterTask.h"
#include "SystemStateTask.h"
#include "SystemStateBase.h"
#include "SystemStateOff.h"
#include "SystemStateBooting.h"
#include "SystemStateNormal.h"
#include "SystemStateRecovery.h"
#include "SystemStateLowPower.h"
#include "SystemStateUpdate.h"
#include "SystemStateShutdown.h"
#include "SystemStateFactoryDefault.h"


SCRIBE_DECL(sys_events);
ManagedTask* systemStateTaskHandle = NULL;
static TimerHandle_t SystemStateTimerHandle = NULL;

static const SystemStateHandler_t SystemStateHandlers[SYSTEM_STATE_NUM_OF] =
{
    // SYSTEM_STATE_OFF
    {
        SystemStateOff_HandleMessage,
        SystemStateOff_Init,
        SystemStateOff_EnterState,
        SystemStateOff_ExitState,
    },
    // SYSTEM_STATE_BOOTING
    {
        SystemStateBooting_HandleMessage,
        SystemStateBooting_Init,
        SystemStateBooting_EnterState,
        SystemStateBooting_ExitState,
    },
    // SYSTEM_STATE_NORMAL
    {
        SystemStateNormal_HandleMessage,
        SystemStateNormal_Init,
        SystemStateNormal_EnterState,
        SystemStateNormal_ExitState,
    },
    // SYSTEM_STATE_RECOVERY
    {
        SystemStateRecovery_HandleMessage,
        SystemStateRecovery_Init,
        SystemStateRecovery_EnterState,
        SystemStateRecovery_ExitState,
    },
    // SYSTEM_STATE_LOW_POWER
    {
        SystemStateLowPower_HandleMessage,
        SystemStateLowPower_Init,
        SystemStateLowPower_EnterState,
        SystemStateLowPower_ExitState,
    },
    // SYSTEM_STATE_UPDATE
    {
        SystemStateUpdate_HandleMessage,
        SystemStateUpdate_Init,
        SystemStateUpdate_EnterState,
        SystemStateUpdate_ExitState,
    },
    // SYSTEM_STATE_SHUTDOWN
    {
        SystemStateShutdown_HandleMessage,
        SystemStateShutdown_Init,
        SystemStateShutdown_EnterState,
        SystemStateShutdown_ExitState,
    },
    // SYSTEM_STATE_FACTORY_DEFAULT
    {
        SystemStateFactoryDefault_HandleMessage,
        SystemStateFactoryDefault_Init,
        SystemStateFactoryDefault_EnterState,
        SystemStateFactoryDefault_ExitState,
    },
};

static const char* const systemStateStrings[] =
{
    "SYSTEM_STATE_OFF",
    "SYSTEM_STATE_BOOTING",
    "SYSTEM_STATE_NORMAL",
    "SYSTEM_STATE_RECOVERY",
    "SYSTEM_STATE_LOW_POWER",
    "SYSTEM_STATE_UPDATE",
    "SYSTEM_STATE_SHUTDOWN",
    "SYSTEM_STATE_FACTORY_DEFAULT",
    "SYSTEM_STATE_NUM_OF"
};

static IpcLpmSystemState_t CurrentSystemState = SYSTEM_STATE_OFF;
static IpcLpmSystemState_t DesiredSystemState = SYSTEM_STATE_OFF;
static IpcLpmSystemState_t PreviousSystemState = SYSTEM_STATE_OFF;

static BOOL SystemStateInProgress = FALSE;

static void SystemStateTask_DoStateChange(void);


const char* SystemStateTask_GetStateString(IpcLpmSystemState_t state)
{
    if((state >= SYSTEM_STATE_OFF) && (state < SYSTEM_STATE_NUM_OF))
    {
        return systemStateStrings[state];
    }
    else
    {
        return "Invalid System State!";
    }
}

TimerHandle_t SystemStateTask_GetTimer(void)
{
    return SystemStateTimerHandle;
}

void SystemStateTask_Init(void* p)
{
    systemStateTaskHandle = GetManagedTaskPointer("SystemStateTask");
    debug_assert(systemStateTaskHandle);

    SystemStateTimerHandle = createTimer(TIMER_MSEC_TO_TICKS(DEFAULT_SYSTEM_TIMER_PERIOD_MSEC), \
                                   NULL, \
                                   SYSTEM_MESSAGE_ID_SystemStateTimerExpired, \
                                   FALSE,
                                   tBlockIdSystemStateTimer, \
                                   "SystemStateTimer", \
                                   &systemStateTaskHandle->Queue);
    
    for(uint8_t i = 0; i < SYSTEM_STATE_NUM_OF; i++)
    {
        if(SystemStateHandlers[i].Init) 
        {
            SystemStateHandlers[i].Init();
        }
    }
}

void SystemStateTask(void* p)
{
    SystemStateTask_SetStateInternal(SYSTEM_STATE_BOOTING);
    while(1)
    {
        TaskManagerPollQueue(systemStateTaskHandle);
    }
}

static void SystemStateTask_DoStateChange(void)
{
    if (CurrentSystemState == DesiredSystemState) return;

    SystemStateInProgress = TRUE;

    if(SystemStateHandlers[CurrentSystemState].ExitState) 
    {
        SystemStateHandlers[CurrentSystemState].ExitState();
    }
    if(SystemStateHandlers[DesiredSystemState].EnterState) 
    {
        SystemStateHandlers[DesiredSystemState].EnterState();
    }
    PreviousSystemState = CurrentSystemState;
    CurrentSystemState = DesiredSystemState;

    SystemStateInProgress = FALSE;
}

void SystemStateTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case SYSTEM_MESSAGE_ID_SystemStateSet:
            {
                IpcPacket_t* pkt = (IpcPacket_t*)msg->params[0];
                SystemStateTask_SetState((IpcLpmSystemState_t) pkt->s.data.b[0], pkt->s.sequence, pkt->s.connectionID);
            }
            break;
        default:
            if(!SystemStateHandlers[CurrentSystemState].HandleMessage(msg))
            {
                SystemStateBase_HandleMessage(msg);
            }
            break;
    }
}

void SystemStateTask_SetStateInternal(IpcLpmSystemState_t state)
{
    SystemStateTask_SetState(state, 0, 0);
}

void SystemStateTask_SetState(IpcLpmSystemState_t state, uint32_t sequence, uint8_t connectionID)
{
    if(!((state >= SYSTEM_STATE_OFF) && (state < SYSTEM_STATE_NUM_OF))) 
    {
        IpcLpmStateResponse_t resp = {
            .sysState = CurrentSystemState,
            .pwrState = PowerAPI_GetCurrentPowerState(),
            .respCode = IPC_TRANSITION_INVALID,
        };

        IpcSendPacket_t response = 
        {
            .dest = IPC_DEVICE_SOUNDTOUCH,
            .op = IPC_SET_SYSTEM_STATE,
            .sequence = sequence,
            .connectionID = connectionID,
            .data = (void*)&resp,
            .length = sizeof(IpcLpmStateResponse_t)
        };
        IPCRouter_SendResponse(&response);
        LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "Recieved invalid system state request %d", state);
        return;
    }
    else if(state != CurrentSystemState) 
    {

        DesiredSystemState = state;

        SystemStateTask_DoStateChange();

        LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "System State Change: %s -> %s ", SystemStateTask_GetStateString(PreviousSystemState), 
                                                                                  SystemStateTask_GetStateString(CurrentSystemState));
    }

    if(sequence != 0 || connectionID != 0)
    {
        IpcLpmStateResponse_t resp = {
            .sysState = CurrentSystemState,
            .pwrState = PowerAPI_GetCurrentPowerState(),
            .respCode = IPC_TRANSITION_COMPLETE,
        };

        IpcSendPacket_t response = 
        {
            .dest = IPC_DEVICE_SOUNDTOUCH,
            .op = IPC_SET_SYSTEM_STATE,
            .sequence = sequence,
            .connectionID = connectionID,
            .data = (void*)&resp,
            .length = sizeof(IpcLpmStateResponse_t)
        };
        IPCRouter_SendResponse(&response);
    }
}

void SystemStateTask_GetState(uint32_t sequence, uint8_t connectionID)
{
    IpcLpmStateResponse_t resp = {
        .sysState = CurrentSystemState,
        .pwrState = PowerAPI_GetCurrentPowerState(),
        .respCode = IPC_TRANSITION_COMPLETE,
    };

    IpcSendPacket_t response = 
    {
        .dest = IPC_DEVICE_SOUNDTOUCH,
        .op = IPC_GET_SYSTEM_STATE,
        .sequence = sequence,
        .connectionID = connectionID,
        .data = (void*)&resp,
        .length = sizeof(IpcLpmStateResponse_t)
    };
    IPCRouter_SendResponse(&response);
}


