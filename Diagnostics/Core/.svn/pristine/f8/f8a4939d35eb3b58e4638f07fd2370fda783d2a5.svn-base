//
// SystemStateFactoryDefault.c
//
#include "SystemStateTask.h"
#include "SystemStateVariant.h"

SCRIBE_DECL(sys_events);
static SystemStateHandler_t* variantHandler = NULL;

BOOL SystemStateFactoryDefault_HandleMessage(GENERIC_MSG_t* message)
{
    if(!(variantHandler && variantHandler->HandleMessage && variantHandler->HandleMessage(message)))
    {
        switch (message->msgID)
        {
            default:
                LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "Unhandled msg %s by state %d", GetEventString(message->msgID), 
                                                                                     SystemStateTask_GetStateString(SYSTEM_STATE_FACTORY_DEFAULT));
                return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void SystemStateFactoryDefault_Init(void)
{
    variantHandler = SystemStateVariant_GetHandler(SYSTEM_STATE_FACTORY_DEFAULT);
    if(variantHandler && variantHandler->Init) variantHandler->Init();

    // Do stuff
}

void SystemStateFactoryDefault_EnterState(void)
{
    if(variantHandler && variantHandler->EnterState) variantHandler->EnterState();

    // Do stuff
}

void SystemStateFactoryDefault_ExitState(void)
{
    if(variantHandler && variantHandler->ExitState) variantHandler->ExitState();

    // Do stuff
}

