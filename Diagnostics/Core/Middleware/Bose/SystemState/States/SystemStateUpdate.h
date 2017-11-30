//
// SystemStateUpdate.h
//
#ifndef _SYSTEM_STATE_UPDATE_H_
#define _SYSTEM_STATE_UPDATE_H_

#include "SystemStateTask.h"

BOOL SystemStateUpdate_HandleMessage(GENERIC_MSG_t* message);
void SystemStateUpdate_Init(void);
void SystemStateUpdate_EnterState(void);
void SystemStateUpdate_ExitState(void);

#endif /* _SYSTEM_STATE_UPDATE_H_*/
