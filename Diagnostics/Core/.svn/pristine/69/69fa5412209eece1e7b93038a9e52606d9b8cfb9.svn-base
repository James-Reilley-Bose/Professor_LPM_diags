//
// SystemStateLowPower.c
//
#include "SystemStateTask.h"
#include "SystemStateVariant.h"

SCRIBE_DECL(sys_events);
static SystemStateHandler_t* variantHandler = NULL;

BOOL SystemStateLowPower_HandleMessage(GENERIC_MSG_t* message)
{
    if(!(variantHandler && variantHandler->HandleMessage && variantHandler->HandleMessage(message)))
    {
        switch (message->msgID)
        {
            /* TODO - Wake on key */
            default:
                LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "Unhandled msg %s by state %d", GetEventString(message->msgID), 
                                                                                     SystemStateTask_GetStateString(SYSTEM_STATE_LOW_POWER));
                return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void SystemStateLowPower_Init(void)
{
    variantHandler = SystemStateVariant_GetHandler(SYSTEM_STATE_LOW_POWER);
    if(variantHandler && variantHandler->Init) variantHandler->Init();
}

void SystemStateLowPower_EnterState(void)
{
    if(variantHandler && variantHandler->EnterState) variantHandler->EnterState();

    // Do stuff
    PowerAPI_SetPowerState(POWER_STATE_LOW_POWER);
}

void SystemStateLowPower_ExitState(void)
{
    if(variantHandler && variantHandler->ExitState) variantHandler->ExitState();

    // Do stuff
}

