
#ifndef WIREDID_CONSOLE_BASS_CONNECTED_H
#define WIREDID_CONSOLE_BASS_CONNECTED_H

#include "project.h"
#include "RivieraLPM_IpcProtocol.h"

BOOL WiredIDConsoleBassConnected_HandleMessage(GENERIC_MSG_t *msg);
void WiredIDConsoleBassConnected_Enter(void);
void WiredIDConsoleBassConnected_Exit(void);
AccessoryDescription_t* WiredIDConsoleBassConnected_GetWiredBassBoxInfo(void);
#endif
