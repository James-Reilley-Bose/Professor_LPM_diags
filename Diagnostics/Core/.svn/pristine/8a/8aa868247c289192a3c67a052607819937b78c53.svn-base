/*
  File  : DeviceModelTask.h
  Title :
  Author  : Derek Richardson
  Created : 10/21/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Model to handle external devices in system
===============================================================================
*/
#ifndef DEVICE_MODEL_TASK_H
#define DEVICE_MODEL_TASK_H

#include "project.h"
#include "TaskManager.h"
#include "IpcProtocolLpm.h"
#include "IpcInterface.h"
#include "DeviceInterface.h"
#include "DeviceInterfaceVariant.h"


extern ManagedTask* ManagedDeviceModelTasks[];

#define DeviceEnterStandby(destination) if(IpcInterfaceTable[destination]->type != IPC_INTERFACE_INVALID) { \
  QueueManagerPostCallbackMsg( \
  &ManagedDeviceModelTasks[DeviceModel_GetIndexFromIpcDevice(destination)]->Queue, \
  DEVICE_MODEL_MESSAGE_ID_EnterStandby, \
  (uint32_t []){(uint32_t)&DeviceInterfaceTable[DeviceModel_GetIndexFromIpcDevice(destination)], TRUE}, \
  NOP_CALLBACK, \
  NO_BLOCK_TIME); }

#define DeviceExitStandby(destination) if(IpcInterfaceTable[destination]->type != IPC_INTERFACE_INVALID) { \
  QueueManagerPostCallbackMsg( \
  &ManagedDeviceModelTasks[DeviceModel_GetIndexFromIpcDevice(destination)]->Queue, \
  DEVICE_MODEL_MESSAGE_ID_EnterStandby, \
  (uint32_t []){(uint32_t)&DeviceInterfaceTable[DeviceModel_GetIndexFromIpcDevice(destination)], FALSE}, \
  NOP_CALLBACK, \
  NO_BLOCK_TIME); }

#define DeviceReboot(destination) if(IpcInterfaceTable[destination]->type != IPC_INTERFACE_INVALID) { \
  QueueManagerPostCallbackMsg( \
  &ManagedDeviceModelTasks[DeviceModel_GetIndexFromIpcDevice(destination)]->Queue, \
  DEVICE_MODEL_MESSAGE_ID_Reboot, \
  (uint32_t []){(uint32_t)&DeviceInterfaceTable[DeviceModel_GetIndexFromIpcDevice(destination)], (uint32_t)packet}, \
  NOP_CALLBACK, \
  NO_BLOCK_TIME); }

#define DeviceModel_IpcRecieve(packet) if(IpcInterfaceTable[DeviceModel_GetIndexFromIpcDevice(packet->s.sender)]->type != IPC_INTERFACE_INVALID) { \
  QueueManagerPostCallbackMsg( \
  &ManagedDeviceModelTasks[DeviceModel_GetIndexFromIpcDevice(packet->s.sender)]->Queue, \
  DEVICE_MODEL_MESSAGE_ID_MessageRecieved, \
  (uint32_t []){(uint32_t)&DeviceInterfaceTable[DeviceModel_GetIndexFromIpcDevice(packet->s.sender)], (uint32_t)packet}, \
  DeviceModelTask_ReturnRxPacketCallback, \
  NO_BLOCK_TIME); }

#define DeviceModelSendMessage(destination, message, ...) if(IpcInterfaceTable[destination]->type != IPC_INTERFACE_INVALID) { \
  QueueManagerPostCallbackMsg( \
  &ManagedDeviceModelTasks[DeviceModel_GetIndexFromIpcDevice(destination)]->Queue, \
  message, \
  (uint32_t []){(uint32_t)&DeviceInterfaceTable[DeviceModel_GetIndexFromIpcDevice(destination)], __VA_ARGS__}, \
  NOP_CALLBACK, \
  NO_BLOCK_TIME); }

void DeviceModelTask_Init(void* devicePointer);
void DeviceModelTask(void* pvParameters);
BOOL DeviceModelTask_DeviceAvailable(Ipc_Device_t device);
void DeviceModelTask_HandleMessage(GENERIC_MSG_t* msg);
void DeviceModelTask_HandleHealthAndStatus(Generic_Device* device, IpcPacket_t* packet);
void DeviceModelTask_DefaultHandleKey(Generic_Device* device, KEY_DATA_t key);
uint32_t DeviceModelTask_GetTimeout(Generic_Device* device, Device_Delay delay);
void DeviceModelTask_ReturnRxPacketCallback(uint32_t* params, uint8_t numParams);
void DeviceModelTask_ForwardMessage(Generic_Device* device, GENERIC_MSG_t* msg);
void DeviceModelTask_SendIPCMessageToAllDevices(uint8_t op, uint8_t* params, void* payload, uint8_t payloadSize);
void DeviceModelTask_GatherSpeakerInformation(IpcAccessoryList_t* accList);
void DeviceModelTask_SendLPMHealthAndStatusToAllDevices(void);
#endif /* DEVICE_MODEL_TASK_H */
