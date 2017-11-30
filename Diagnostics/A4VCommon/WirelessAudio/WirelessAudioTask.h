//
// WirelessAudioTask.h
//

#ifndef WIRELESS_AUDIO_TASK_H
#define WIRELESS_AUDIO_TASK_H

#include "TaskManager.h"
#include "WirelessAudioVariant.h"

extern ManagedTask* wirelessAudioTaskHandle;

#define WirelessAudioPostMsg(msg,cb, ...) QueueManagerPostCallbackMsg(&wirelessAudioTaskHandle->Queue,msg,(uint32_t []){__VA_ARGS__},cb,NO_BLOCK_TIME)

void WirelessAudioTask_Init(void* p);
void WirelessAudioTask(void* p);
void WirelessAudioTask_HandleMessage(GENERIC_MSG_t* msg);
void WirelessAudioTask_StartTimer(void);
void WirelessAudioTask_StopTimer(void);
void WirelessAudioTask_ChangeTimerPeriod(TickType_t newPeriod);
WAState_t WirelessAudioTask_GetState(void);

#endif // WIRELESS_AUDIO_TASK_H
