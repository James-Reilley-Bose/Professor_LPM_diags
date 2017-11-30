//
// SystemStateRecovery.h
//
#ifndef _SYSTEM_STATE_RECOVERY_H_
#define _SYSTEM_STATE_RECOVERY_H_

#include "SystemStateTask.h"

BOOL SystemStateRecovery_HandleMessage(GENERIC_MSG_t* message);
void SystemStateRecovery_Init(void);
void SystemStateRecovery_EnterState(void);
void SystemStateRecovery_ExitState(void);

#endif /* _SYSTEM_STATE_RECOVERY_H_ */
