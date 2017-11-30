/*
  File  : DeviceInterfaceVariant.c
  Title :
  Author  : Derek Richardson
  Created : 10/21/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Specific device model interfaces
===============================================================================
*/
#include "DeviceModelTask.h"
#include "DeviceInterfaceVariant.h"
#include "DeviceModel_DSP.h"

ManagedTask* ManagedDeviceModelTasks[NUM_DEVICE_MODELS];
Generic_Device* devicePointerList[NUM_DEVICE_MODELS];

Generic_Device DeviceInterfaceTable[] =
{
    {
        NULL, // TODO
    },
    
    {
    .Name = "DSPModel",
    .Task = NULL,
    .StatusMutex = NULL,

    .Timer = NULL,
    .TimerBlock = tBlockDSPDeviceModelTimer,

    .PowerState = POWERSTATE_ON,
    .HealthState = H_S_NORMAL,
    .DeviceID = IPC_DEVICE_DSP,

    .Delays.list.BootDelay = 2000,
    .Delays.list.NormalPingDelay = 5000,
    .Delays.list.PingResponseDelay = 3000,

    .Init = DeviceModel_DSP_Init,
    .Standby = DeviceModel_DSP_Standby,
    .Reboot = DeviceModel_DSP_Reboot,
    .HandleIPCMessage = DeviceModel_DSP_HandleIPCMessage,
    .HandleKey = DeviceModel_DSP_HandleKey,
    .HandleWatchdog = DeviceModel_DSP_HandleWatchDog,
    .HandleMessage = DeviceModel_DSP_HandleMessage,
    .Booted = DeviceModel_DSP_Booted,
    .HandleH_S = NULL,
  }
};

uint32_t DeviceModel_GetIndexFromIpcDevice(ipc_uint8_t dev)
{
    for (uint32_t i = 0; i < NUM_DEVICE_MODELS; i++)
    {
        if (DeviceInterfaceTable[i].DeviceID == dev)
        {
            return i;
        }
    }
    return DEVICEMODEL_INTERFACE_INVALID;
}
