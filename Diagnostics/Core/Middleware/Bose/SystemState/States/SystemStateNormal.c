//
// SystemStateNormal.c
//
#include "SystemStateTask.h"
#include "SystemStateVariant.h"

SCRIBE_DECL(sys_events);
static SystemStateHandler_t* variantHandler = NULL;

BOOL SystemStateNormal_HandleMessage(GENERIC_MSG_t* message)
{
    if(!(variantHandler && variantHandler->HandleMessage && variantHandler->HandleMessage(message)))
    {
        switch (message->msgID)
        {
            default:
                LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "Unhandled msg %s by state %d", GetEventString(message->msgID), 
                                                                                     SystemStateTask_GetStateString(SYSTEM_STATE_NORMAL));
                return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void SystemStateNormal_Init(void)
{
    variantHandler = SystemStateVariant_GetHandler(SYSTEM_STATE_NORMAL);
    if(variantHandler && variantHandler->Init) variantHandler->Init();
}

void SystemStateNormal_EnterState(void)
{
    if(variantHandler && variantHandler->EnterState) variantHandler->EnterState();

    // Do stuff
#ifdef APQ_SUPPORTS_LPM_SYSTEM_STATES
    PowerAPI_SetPowerState(POWER_STATE_NETWORK_STANDBY);
#else
    // For now still go to normal
    PowerAPI_SetPowerState(POWER_STATE_FULL_POWER);
#endif
}

void SystemStateNormal_ExitState(void)
{
    if(variantHandler && variantHandler->ExitState) variantHandler->ExitState();

    // Do stuff
}

