//
// SystemStateOff.c
//
#include "SystemStateTask.h"
#include "SystemStateVariant.h"

SCRIBE_DECL(sys_events);

BOOL SystemStateBase_HandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
        default:
            LOG(sys_events, ROTTEN_LOGLEVEL_VERBOSE, "Unhandled msg %s by state %d", GetEventString(message->msgID), "System State Base");
            return FALSE;
    }
}