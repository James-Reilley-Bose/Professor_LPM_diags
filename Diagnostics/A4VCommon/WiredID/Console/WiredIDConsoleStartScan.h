#ifndef WIREDID_CONSOLE_START_SCAN_H
#define WIREDID_CONSOLE_START_SCAN_H

#include "project.h"

BOOL WiredIDConsoleStartScan_HandleMessage(GENERIC_MSG_t *msg);
void WiredIDConsoleStartScan_Init(void);
void WiredIDConsoleStartScan_Enter(void);
void WiredIDConsoleStartScan_Exit(void);

#endif