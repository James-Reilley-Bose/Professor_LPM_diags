//
// SystemStateOff.h
//
#ifndef _SYSTEM_STATE_BASE_H_
#define _SYSTEM_STATE_BASE_H_

#include "SystemStateTask.h"

BOOL SystemStateBase_HandleMessage(GENERIC_MSG_t* msg);
void SystemStateBase_Init(void);
void SystemStateBase_EnterState(void);
void SystemStateBase_ExitState(void);

#endif /* _SYSTEM_STATE_BASE_H_ */
