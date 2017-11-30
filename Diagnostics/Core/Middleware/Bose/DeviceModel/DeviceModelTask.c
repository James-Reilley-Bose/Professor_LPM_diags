/*
  File  : DeviceModelTask.h
  Title :
  Author  : Derek Richardson
  Created : 10/21/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Structs and api to handle external devices in system and keep tabs
        on the doings of said devices
===============================================================================
*/

#include "project.h"
#include "TaskDefs.h"
#include "TaskManager.h"
#include "IpcKeys.h"
#include "DeviceInterface.h"
#include "DeviceModelTask.h"
#include "IPCRouterTask.h"
#include "IpcInterface.h"
#include "IpcStatus.h"
#include "IPCBulk.h"
#include "PowerTask.h"
#include "DeviceInterfaceVariant.h"

SCRIBE(device_model, ROTTEN_LOGLEVEL_NORMAL);

static void DeviceModelTask_HandleIpcPacket(Generic_Device* device, IpcPacket_t* packet);
static void DeviceModelTask_EnterStandby(Generic_Device* device, BOOL enterStandby);
static void DeviceModelTask_Reboot(Generic_Device* device, IpcPacket_t* IpcPacket);
static void DeviceModelTask_WatchdogExpired(Generic_Device* device);
static Generic_Device* DeviceModelTask_GetCurrentDeviceModel(void);

/*
 * @func DeviceModelTask_Init
 *
 * @brief Initializes a device model and given devices IPC TX
 *
 * @param void* p - Pointer to the configuration for a specific device.
 *
 * @return n/a
 */
void DeviceModelTask_Init(void* devicePointer)
{
    Generic_Device* device = (Generic_Device*)devicePointer;
    devicePointerList[device->DeviceID] = device;

    device->StatusMutex = xSemaphoreCreateMutex();
    debug_assert(device->StatusMutex);
    xSemaphoreGive(device->StatusMutex);

    device->PowerState = POWERSTATE_STANDBY;
    device->HealthState = H_S_OFF;
    LOG(device_model, ROTTEN_LOGLEVEL_VERBOSE, "%s : H_S_OFF", device->Name);

    (device->Init)(device);

    device->Task = GetManagedTaskPointer(device->Name);
    ManagedDeviceModelTasks[device->DeviceID] = device->Task;
    debug_assert(device->Task != NULL);

    device->Timer = createTimer( TIMER_MSEC_TO_TICKS(device->Delays.values[DEVICE_BOOT_DELAY]), \
                                 &devicePointerList[device->DeviceID], \
                                 DEVICE_MODEL_MESSAGE_ID_WatchdogExpired, \
                                 FALSE, \
                                 device->TimerBlock, \
                                 device->Name, \
                                 NULL);
    debug_assert(device->Timer);
}

/*
 * @func DeviceModelTask
 *
 * @brief The task, handles health and st
 *
 * @param void *pvParameters - Pointer to the configuration for a specific IPC device.
 *
 * @return n/a
 */
void DeviceModelTask(void* devicePointer)
{
    const Generic_Device* device = (Generic_Device*)devicePointer;
    for (;;)
    {
        TaskManagerPollQueue(device->Task);
    }
}

/*
 * @func DeviceModelTask_HandleMessage
 *
 * @brief Calls the appropriate message handler.
 *
 * @param GENERIC_MSG_t* msg - the message to process
 *
 * @return n/a
 */
void DeviceModelTask_HandleMessage(GENERIC_MSG_t* msg)
{
    if (((msg->msgID > DEVICE_MODEL_MESSAGE_ID_Start) && (msg->msgID < DEVICE_MODEL_MESSAGE_ID_End)) || IPCBulk_isBulkTransferMessage(msg->msgID))
    {
        switch (msg->msgID)
        {
            case DEVICE_MODEL_MESSAGE_ID_MessageRecieved:
                DeviceModelTask_HandleIpcPacket((Generic_Device*)msg->params[0], (IpcPacket_t*)msg->params[1]);
                break;
            case DEVICE_MODEL_MESSAGE_ID_EnterStandby:
                DeviceModelTask_EnterStandby((Generic_Device*)msg->params[0], (BOOL)msg->params[1]);
                break;
            case DEVICE_MODEL_MESSAGE_ID_Reboot:
                DeviceModelTask_Reboot((Generic_Device*)msg->params[0], (IpcPacket_t*)msg->params[1]);
                break;
            case DEVICE_MODEL_MESSAGE_ID_WatchdogExpired:
                DeviceModelTask_WatchdogExpired((Generic_Device*)msg->params[0]);
                break;
            default:
                if (IPCBulk_isBulkTransferMessage(msg->msgID))
                {
                    Generic_Device* device = DeviceModelTask_GetCurrentDeviceModel();
                    if (device)
                    {
                        DeviceModelTask_ForwardMessage(device, msg);
                    }
                    else
                    {
                        LOG(device_model, ROTTEN_LOGLEVEL_NORMAL, "Unable to find current device model");
                    }
                }
                else
                {
                    // Let the specific model handle its own arbitrary messages if it wants to
                    DeviceModelTask_ForwardMessage((Generic_Device*)msg->params[0], msg);
                }
                break;
        }
    }
}

/*
 * @func DeviceModelTask_HandleMessage
 *
 * @brief Calls the appropriate ipc packet handler.
 *
 * @param IpcPacket_t* packet - pointer to the ipc packet to process
 *
 * @return n/a
 */
static void DeviceModelTask_HandleIpcPacket(Generic_Device* device, IpcPacket_t* packet)
{
    (device->HandleIPCMessage)(packet);
}


/*
 * @func DeviceModelTask_ReturnRxPacketCallback
 *
 * @brief Needed because regular return callback thinks packet is in param 0
 *
 * @param uint32_t* params, uint8_t numParam
 *
 * @return n/a
 */
void DeviceModelTask_ReturnRxPacketCallback(uint32_t* params, uint8_t numParams)
{
    IpcPacket_t* packet = (IpcPacket_t*) params[1];

    IPCRouter_ReturnRxBuffer(packet);
}

/*
 * @func DeviceModelTask_WatchdogExpired
 *
 * @brief Calls the appropriate watchdog handler
 *
 * @param Generic_Device* device - device whose watchdog expired
 *
 * @return n/a
 */
static void DeviceModelTask_WatchdogExpired(Generic_Device* device)
{
    (device->HandleWatchdog)();
}


/*
 * @func DeviceModelTask_EnterStandby
 *
 * @brief Calls the appropriate device standby function
 *
 * @param device - the device to control
 *        enterStandby - TRUE to go into standby
 *
 * @return n/a
 */
static void DeviceModelTask_EnterStandby(Generic_Device* device, BOOL enterStandby)
{
    (device->Standby)(enterStandby);
}


/*
 * @func DeviceModelTask_Reboot
 *
 * @brief Calls the appropriate device reboot function
 *
 * @param device - the device to reboot
 *
 * @return n/a
 */
static void DeviceModelTask_Reboot(Generic_Device* device, IpcPacket_t* ipcPacket)
{
    (device->Reboot)((IpcRebootType_t)REV_WORD_BYTES(ipcPacket->s.data.w[0]), REV_WORD_BYTES(ipcPacket->s.data.w[1]));
}


/*
 * @func DeviceModelTask_HandleHealthAndStatus
 *
 * @brief Generic handling of h & s message called from devices HandleIPC
 *
 * @param Generic_Device* device - device to handle
 *        IpcPacket_t* packet - pointer to the ipc packet to process
 *
 * @return n/a
 */
void DeviceModelTask_HandleHealthAndStatus(Generic_Device* device, IpcPacket_t* packet)
{
    switch (device->HealthState)
    {
        case H_S_BOOT_WAITING:
            {
                //Start normal operation
                device->HealthState = H_S_NORMAL;
                device->PowerState = POWERSTATE_ON;
                LOG(device_model, ROTTEN_LOGLEVEL_VERBOSE, "%s : H_S_NORMAL", device->Name);

                //Adjust timer period for new state
                changeTimerPeriod(device->Timer, TIMER_MSEC_TO_TICKS(device->Delays.values[DEVICE_H_S_PING_INTERVAL]), 0);

                //Set as current h&s
                xSemaphoreTake(device->StatusMutex, portMAX_DELAY);
                memcpy(&device->CurrentStatus.data, &packet->s.data.b, IPC_NUM_DATA_BYTES);
                xSemaphoreGive(device->StatusMutex);

                //Start timer
                timerReset(device->Timer, 0, &device->Task->Queue);

                (device->Booted)();

                if (device->HandleH_S != NULL)
                {
                    (device->HandleH_S)();
                }

                IpcStatus_SendLPMHealthAndStatus(device->DeviceID);

                LOG(device_model, ROTTEN_LOGLEVEL_NORMAL, "Received initial health and status");
            }
            break;

        case H_S_NORMAL_WAITING:
        case H_S_NORMAL:
            {
                if (device->HealthState != H_S_NORMAL)
                {
                    device->HealthState = H_S_NORMAL;
                }

                if (device->PowerState != POWERSTATE_ON)
                {
                    device->PowerState = POWERSTATE_ON;
                    (device->Booted)();
                }

                LOG(device_model, ROTTEN_LOGLEVEL_INSANE, "%s : H_S_NORMAL", device->Name);

                //Adjust timer period for new state
                changeTimerPeriod(device->Timer, TIMER_MSEC_TO_TICKS(device->Delays.values[DEVICE_H_S_PING_INTERVAL]), 0);

                //Set as current h&s
                xSemaphoreTake(device->StatusMutex, portMAX_DELAY);
                memcpy(&device->CurrentStatus.data, &packet->s.data.b, IPC_NUM_DATA_BYTES);
                xSemaphoreGive(device->StatusMutex);

                if (device->HandleH_S != NULL)
                {
                    (device->HandleH_S)();
                }

                //Start timer
                timerStart(device->Timer, 0, &device->Task->Queue);

                LOG(device_model, ROTTEN_LOGLEVEL_VERBOSE, "Received normal health and status");
            }
            break;

        case H_S_OFF :
            LOG(device_model, ROTTEN_LOGLEVEL_NORMAL, "Received health and status while device was off?");
            break;

        case H_S_TIMEOUT:
            LOG(device_model, ROTTEN_LOGLEVEL_NORMAL, "Received health and status after timeout");
            break;

        case H_S_RECOVERY:
            LOG(device_model, ROTTEN_LOGLEVEL_NORMAL, "H & S in Recovery");
            break;

        case H_S_INVALID:
        default:
            LOG(device_model, ROTTEN_LOGLEVEL_NORMAL, "Invalid H & S state");
            break;
    }
}

/*
 * @func DeviceModelTask_DeviceAvailable
 *
 * @brief Checks if given device is availible
 *
 * @param Generic_Device* device - device to check
 *
 * @return BOOL - TRUE if device is availible
 */
BOOL DeviceModelTask_DeviceAvailable(Ipc_Device_t device)
{
    return ((DeviceInterfaceTable[device].PowerState == POWERSTATE_ON || DeviceInterfaceTable[device].PowerState == POWERSTATE_STANDBY) &&
            ((DeviceInterfaceTable[device].HealthState == H_S_NORMAL)
             || (DeviceInterfaceTable[device].HealthState == H_S_NORMAL_WAITING)
             || (DeviceInterfaceTable[device].HealthState == H_S_RECOVERY)));
}


/*
 * @func DeviceModelTask_GetTimeout
 *
 * @brief Get a given time out values for a device
 *
 * @param Generic_Device* device - device to get timeout from
 *        Device_Delay delay - delay type to request
 *
 * @return uint32_t - the timeout requested
 */
uint32_t DeviceModelTask_GetTimeout(Generic_Device* device, Device_Delay delay)
{
    return (device->Delays.values[delay]);
}


/*
 * @func DeviceModelTask_DefaultHandleKey
 *
 * @brief Handles a key press to send to device
 *
 * @param n/a
 *
 * @return n/a
 */
void DeviceModelTask_DefaultHandleKey(Generic_Device* device, KEY_DATA_t key)
{
    IpcKeys_SendKey(device->DeviceID, key);
}

/**
 * @brief Forward arbitrary messages onto the device if it wants them
 * @param device
 * @param params
 */
void DeviceModelTask_ForwardMessage(Generic_Device* device, GENERIC_MSG_t* msg)
{
    if (device->HandleMessage != NULL)
    {
        device->HandleMessage(msg);
    }
}

static Generic_Device* DeviceModelTask_GetCurrentDeviceModel(void)
{
    TaskHandle_t current_task = xTaskGetCurrentTaskHandle();
    for (uint8_t i = 0; i < NUM_DEVICE_MODELS; i++)
    {
        if (ManagedDeviceModelTasks[i] &&
                (ManagedDeviceModelTasks[i]->TaskHandle == current_task))
        {
            return devicePointerList[i];
        }
    }
    return NULL;
}

void DeviceModelTask_SendLPMHealthAndStatusToAllDevices(void)
{
    for (uint8_t i = 0; i < NUM_DEVICE_MODELS; i++)
    {
        if (DeviceInterfaceTable[i].DeviceID != IPC_DEVICE_INVALID)
        {
            DeviceModelSendMessage(i, DEVICE_MODEL_MESSAGE_ID_SendLPMHealthAndStatus);
        }
    }
}

