//
// SystemStateShutdown.h
//
#include "SystemStateTask.h"
#include "SystemStateVariant.h"

SCRIBE_DECL(sys_events);
static SystemStateHandler_t* variantHandler = NULL;

BOOL SystemStateShutdown_HandleMessage(GENERIC_MSG_t* message)
{
    if(!(variantHandler && variantHandler->HandleMessage && variantHandler->HandleMessage(message)))
    {
        switch (message->msgID)
        {
            default:
                LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "Unhandled msg %s by state %d", GetEventString(message->msgID), 
                                                                                     SystemStateTask_GetStateString(SYSTEM_STATE_SHUTDOWN));
                return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void SystemStateShutdown_Init(void)
{
    variantHandler = SystemStateVariant_GetHandler(SYSTEM_STATE_SHUTDOWN);
    if(variantHandler && variantHandler->Init) variantHandler->Init();
}

void SystemStateShutdown_EnterState(void)
{
    if(variantHandler && variantHandler->EnterState) variantHandler->EnterState();

    // Do stuff
    PowerAPI_SetPowerState(POWER_STATE_LOW_POWER);
    system_reboot();

}

void SystemStateShutdown_ExitState(void)
{
    if(variantHandler && variantHandler->ExitState) variantHandler->ExitState();

    // Do stuff
}

