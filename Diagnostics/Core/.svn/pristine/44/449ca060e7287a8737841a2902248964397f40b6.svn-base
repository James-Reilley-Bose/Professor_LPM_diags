//
// SystemStateBooting.h
//
#include "SystemStateTask.h"
#include "SystemStateBooting.h"
#include "SystemStateVariant.h"

SCRIBE_DECL(sys_events);
static SystemStateHandler_t* variantHandler = NULL;

static void SystemStateBooting_HandleWatchdog(void);

BOOL SystemStateBooting_HandleMessage(GENERIC_MSG_t* message)
{
    if(!(variantHandler && variantHandler->HandleMessage && variantHandler->HandleMessage(message)))
    {
        switch (message->msgID)
        {
            case SYSTEM_MESSAGE_ID_SystemStateTimerExpired:
                SystemStateBooting_HandleWatchdog();
                return TRUE;
            default:
                LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "Unhandled msg %s by state %d", GetEventString(message->msgID), 
                                                                                     SystemStateTask_GetStateString(SYSTEM_STATE_BOOTING));
                return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void SystemStateBooting_Init(void)
{
    variantHandler = SystemStateVariant_GetHandler(SYSTEM_STATE_BOOTING);
    if(variantHandler && variantHandler->Init) variantHandler->Init();
}

void SystemStateBooting_EnterState(void)
{
    if(variantHandler && variantHandler->EnterState) variantHandler->EnterState();

    // Do stuff

    // Start APQ Boot Timer
    changeTimerPeriod(SystemStateTask_GetTimer(), SYSTEM_STATE_BOOTING_APQ_TIMEOUT, 0);
    timerStart(SystemStateTask_GetTimer(), 0, &systemStateTaskHandle->Queue);

    // Head to network standby and APQ will direct us from there
    PowerAPI_SetPowerState(POWER_STATE_NETWORK_STANDBY);
}

void SystemStateBooting_ExitState(void)
{
    if(variantHandler && variantHandler->ExitState) variantHandler->ExitState();

    // Do stuff
    // Kill timer
    timerStop(SystemStateTask_GetTimer(), 0);
}


static void SystemStateBooting_HandleWatchdog(void)
{
#ifdef APQ_SUPPORTS_LPM_SYSTEM_STATES
    SystemStateTask_SetStateInternal(SYSTEM_STATE_RECOVERY);
#else
    // For now still go to normal
    SystemStateTask_SetStateInternal(SYSTEM_STATE_NORMAL);
#endif
}
