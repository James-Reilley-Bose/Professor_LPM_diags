#ifndef WIREDID_CONSOLE_WFF_SCAN_H
#define WIREDID_CONSOLE_WFF_SCAN_H

#include "project.h"

BOOL WiredIDConsoleWFFScan_HandleMessage(GENERIC_MSG_t *msg);
void WiredIDConsoleWFFScan_Init(void);
void WiredIDConsoleWFFScan_Enter(void);
void WiredIDConsoleWFFScan_Exit(void);

#endif