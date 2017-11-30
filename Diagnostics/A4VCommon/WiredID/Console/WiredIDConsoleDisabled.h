
#ifndef WIREDID_CONSOLE_DISABLED_H
#define WIREDID_CONSOLE_DISABLED_H

#include "project.h"

BOOL WiredIDConsoleDisabled_HandleMessage(GENERIC_MSG_t *msg);
void WiredIDConsoleDisabled_EnterState(void);
void WiredIDConsoleDisabled_ExitState(void);

#endif