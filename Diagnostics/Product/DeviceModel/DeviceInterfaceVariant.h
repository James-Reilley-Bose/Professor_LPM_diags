#ifndef _DEVICE_INTERFACE_VARIANT_H_
#define _DEVICE_INTERFACE_VARIANT_H_

#define NUM_DEVICE_MODELS 2

extern ManagedTask* ManagedDeviceModelTasks[NUM_DEVICE_MODELS];
extern Generic_Device* devicePointerList[NUM_DEVICE_MODELS];

uint32_t DeviceModel_GetIndexFromIpcDevice(ipc_uint8_t dev);

#endif /*_DEVICE_INTERFACE_VARIANT_H_*/