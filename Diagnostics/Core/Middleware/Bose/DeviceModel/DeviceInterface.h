/*
  File  : DeviceInterface.h
  Title :
  Author  : Derek Richardson
  Created : 10/21/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Structs to hold generic device model functions
===============================================================================
*/
#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include "IpcProtocolLpm.h"
#include "TaskManager.h"
#include "KeyData.h"
#include "genTimer.h"

#define DEVICE_NAME_MAX_SIZE 10


typedef enum Device_Delay_t
{
    DEVICE_BOOT_DELAY,
    DEVICE_H_S_PING_INTERVAL,
    DEVICE_H_S_PING_RESPONSE_DELAY,
    DEVICE_NUM_OF_DELAYS
} Device_Delay;

typedef struct
{
    uint32_t BootDelay;
    uint32_t NormalPingDelay;
    uint32_t PingResponseDelay;
} DeviceDelayList;

typedef union
{
    uint32_t values[DEVICE_NUM_OF_DELAYS];
    DeviceDelayList list;
} DeviceDelays;

typedef enum Device_PowerState_t
{
    POWERSTATE_STANDBY,
    POWERSTATE_BOOTING,
    POWERSTATE_ON,
    POWERSTATE_INVALID
} Device_PowerState;

typedef enum
{
    H_S_OFF,            // Device is off
    H_S_BOOT_WAITING,   // Waiting for first h&s
    H_S_NORMAL,         // Got 1+ h&s and device is responding on time
    H_S_NORMAL_WAITING, // Got 1+ h&s and device h&s requested
    H_S_TIMEOUT,        // Watchdog timed out now lets recover device
    H_S_RECOVERY,       // Got user indication to recover SM2(Ginger) or failed to ping SM2(Bardeen)
    H_S_UNRECOVERABLE,  // We tried to recover but that failed. Nothing else we can do.
    H_S_INVALID
} H_S_State_t;

typedef union
{
    uint8_t data[IPC_NUM_DATA_BYTES];
    IpcDspHealthStatusPayload_t dspHS;
    IpcStHealthStatusPayload_t stHS;
    IpcAsocHealthStatusPayload_t asocHS;
    IpcF0HealthStatusPayload_t f0HS;
} H_S_Payload_t;

typedef struct Generic_Device
{

    /* "Private" variables */
    char              Name[DEVICE_NAME_MAX_SIZE];
    ManagedTask*      Task;
    xSemaphoreHandle  StatusMutex;
    TimerHandle_t     Timer;
    eTimerBlockId_t   TimerBlock;
    Device_PowerState PowerState;
    H_S_State_t       HealthState;
    Ipc_Device_t      DeviceID;
    DeviceDelays      Delays;

    H_S_Payload_t CurrentStatus;

    /* Standard functions all devices should have */
    void     (*Init)(void* device); // has to be void* sadly due to task manager
    void     (*Booted)(void);
    void     (*Standby)(BOOL enterStandby);
    void     (*Reboot)(IpcRebootType_t type, uint32_t waitB4Reboot);
    void     (*HandleIPCMessage)(IpcPacket_t* packet);
    void     (*HandleKey)(KEY_DATA_t key);
    void     (*HandleWatchdog)(void);
    uint32_t (*GetTimeout)(Device_Delay delay);
    void     (*HandleMessage)(GENERIC_MSG_t* msg); // Handle arbitrary messages (set to NULL to not use this)
    void     (*HandleH_S)(void);
} Generic_Device;

uint32_t DeviceModel_GetDeviceModelIndexFromIpcDevice(Ipc_Device_t dev);
#define DEVICEMODEL_INTERFACE_INVALID 0xFFFFFFFF
extern Generic_Device DeviceInterfaceTable[];

#endif /* DEVICE_INTERFACE_H */
