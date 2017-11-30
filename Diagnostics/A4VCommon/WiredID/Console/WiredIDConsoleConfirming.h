
#ifndef WIREDID_CONSOLE_CONFIRM_H
#define WIREDID_CONSOLE_CONFIRM_H

#include "project.h"

BOOL WiredIDConsoleConfirm_HandleMessage(GENERIC_MSG_t *msg);
void WiredIDConsoleConfirm_Init(void);
void WiredIDConsoleConfirm_Enter(void);
void WiredIDConsoleConfirm_Exit(void);

#endif