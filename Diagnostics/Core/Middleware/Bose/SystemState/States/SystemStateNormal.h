//
// SystemStateNormal.h
//
#ifndef _SYSTEM_STATE_NORMAL_H_
#define _SYSTEM_STATE_NORMAL_H_

#include "SystemStateTask.h"

BOOL SystemStateNormal_HandleMessage(GENERIC_MSG_t* message);
void SystemStateNormal_Init(void);
void SystemStateNormal_EnterState(void);
void SystemStateNormal_ExitState(void);

#endif /* _SYSTEM_STATE_NORMAL_H_ */
