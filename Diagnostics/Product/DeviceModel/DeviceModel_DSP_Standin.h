//
// DeviceModel_DSP_Standin.h
//

#ifndef DEVICE_MODEL_DSP_STANDIN_H
#define DEVICE_MODEL_DSP_STANDIN_H

#include "IpcTxTask.h"

#define DspDMStandingPostMsg(_theMsg,...) QueueManagerPostCallbackMsg(&ManagedIpcTxTasks[IPC_DEVICE_DSP]->Queue, _theMsg, (uint32_t []){__VA_ARGS__}, NOP_CALLBACK, NO_BLOCK_TIME);

void DeviceModel_DSP_Standin_Init(void* p);
void DeviceModel_DSP_Standin_Run(void* pvParameters);
void DeviceModel_DSP_Standin_HandleMessage(GENERIC_MSG_t* msg);

#endif // DEVICE_MODEL_DSP_STANDIN_H
