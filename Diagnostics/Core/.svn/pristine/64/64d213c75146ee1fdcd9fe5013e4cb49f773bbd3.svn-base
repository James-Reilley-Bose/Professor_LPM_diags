/*
  File  : PowerTask.h
  Title :
  Author  : ja70076
  Created : 08/18/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Header for A4V Power task.

===============================================================================
*/

#ifndef _PWR_TASK_H_
#define _PWR_TASK_H_

#include "TaskManager.h"
#include "RivieraLPM_IpcProtocol.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern ManagedTask* PowerTaskHandle;
#define PowerTaskPostMsg(_theMsg, ...) QueueManagerPostCallbackMsg(&PowerTaskHandle->Queue, _theMsg, (uint32_t []){__VA_ARGS__}, NOP_CALLBACK, NO_BLOCK_TIME);

extern const char* PowerStateStrings[POWER_STATE_NUM];

//the Power Manager API
void PowerAPI_SetPowerState(IpcLPMPowerState_t NewPowerState);
IpcLPMPowerState_t PowerAPI_GetCurrentPowerState(void);

//for TaskDefs
void PowerTask(void* pvParamaters);
void PowerTask_Init(void* p);
void PowerTask_HandleMessage(GENERIC_MSG_t* message);

//These are the functions each varaints recipe should have
void PowerVariant_Init(void);
void PowerVariant_HandleMessage(GENERIC_MSG_t* message);
void PowerVariant_TransitionColdBootToLowPower(void);
void PowerVariant_TransitionLowPowerToNetworkStandby(void);
void PowerVariant_TransitionNetworkStandbyToAutoWakeStandby(void);
void PowerVariant_TransitionAutoWakeStandbyToFullPower(void);
void PowerVariant_TransitionFullPowerToAutoWakeStandby(void);
void PowerVariant_TransitionAutoWakeStandbyToNetworkStandby(void);
void PowerVariant_TransitionNetworkStandbyToLowPower(void);

#ifdef __cplusplus
}
#endif


#endif //_PWR_TASK_H_
