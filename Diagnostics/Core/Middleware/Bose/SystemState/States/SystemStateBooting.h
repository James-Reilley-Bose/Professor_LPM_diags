//
// SystemStateBooting.h
//
#ifndef _SYSTEM_STATE_BOOTING_H_
#define _SYSTEM_STATE_BOOTING_H_

#include "SystemStateTask.h"

#ifdef APQ_SUPPORTS_LPM_SYSTEM_STATES
#define SYSTEM_STATE_BOOTING_APQ_TIMEOUT TIMER_SEC_TO_TICKS(60)
#else
#define SYSTEM_STATE_BOOTING_APQ_TIMEOUT TIMER_SEC_TO_TICKS(3)
#endif

BOOL SystemStateBooting_HandleMessage(GENERIC_MSG_t* message);
void SystemStateBooting_Init(void);
void SystemStateBooting_EnterState(void);
void SystemStateBooting_ExitState(void);

#endif /* _SYSTEM_STATE_BOOTING_H_ */