//
// SystemStateOff.h
//
#ifndef _SYSTEM_STATE_OFF_H_
#define _SYSTEM_STATE_OFF_H_

#include "SystemStateTask.h"

BOOL SystemStateOff_HandleMessage(GENERIC_MSG_t* message);
void SystemStateOff_Init(void);
void SystemStateOff_EnterState(void);
void SystemStateOff_ExitState(void);

#endif /* _SYSTEM_STATE_OFF_H_ */