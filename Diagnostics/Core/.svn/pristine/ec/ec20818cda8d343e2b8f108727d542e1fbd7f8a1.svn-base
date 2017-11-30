/*
  File  : PowerTask.c
  Title :
  Author  : ja70076
  Created : 08/18/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:  The task responsible for consistent and stable power manipulation.
                of board rails and module power
                Recipies for the following power transitions:
                cold boot <-> low power <-> network standby <-> autowake standby <-> full power
==============================================================================*/

#include "project.h"
#include "PowerTask.h"
#include "TaskDefs.h"
#include "ViewNotify.h"

/* Logs
===============================================================================*/
SCRIBE_DECL(power);

/* Global Variable
===============================================================================*/
ManagedTask* PowerTaskHandle = NULL;
const char* PowerStateStrings[POWER_STATE_NUM] =
{
    "ColdBoot",
    "LowPower",
    "NetworkStandby",
    "AutoWakeStandby",
    "FullPower",
};

/* Local Defines
===============================================================================*/
#define PowerPostMsg(_theMsg) QueueManagerPostCallbackMsg(&PowerTaskHandle->Queue, _theMsg, NULL, NOP_CALLBACK, NO_BLOCK_TIME);

/* Local Variables
===============================================================================*/
static IpcLPMPowerState_t CurrentPowerState = POWER_STATE_COLD_BOOTED;
static SemaphoreHandle_t PowerSemaphore = NULL;
static SemaphoreHandle_t PowerAPIMutex = NULL;

/* Local Prototypes
===============================================================================*/

/*
===============================================================================
@func Power_GetPowerState
@brief API for providing the current Power State to the system.
===============================================================================
*/
IpcLPMPowerState_t PowerAPI_GetCurrentPowerState (void)
{
    return (CurrentPowerState);
}


/*
============================================================================================
@func Power_SetPowerState
@brief API for the Source Switch task to initiate Power transition.
**** Note: This is a blocking call ****
============================================================================================
*/
void PowerAPI_SetPowerState (IpcLPMPowerState_t DesiredPowerState)
{
    // handle only 1 PowerAPI call at a time
    xSemaphoreTake(PowerAPIMutex, BLOCK_FOREVER);

    if (DesiredPowerState != CurrentPowerState)
    {
        // tell the power task what to do
        switch (DesiredPowerState)
        {

            case POWER_STATE_LOW_POWER:
                switch (CurrentPowerState)
                {
                    case POWER_STATE_COLD_BOOTED:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionColdBootToLowPower);
                        break;

                    case POWER_STATE_FULL_POWER:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionFullPowerToAutoWakeStandby);
                    // fall through intentionally

                    case POWER_STATE_AUTO_WAKE_STANDBY:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionAutoWakeStandbyToNetworkStandby);
                    // fall through intentionally

                    case POWER_STATE_NETWORK_STANDBY:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionNetworkStandbyToLowPower);
                        break;
                }
                break;


            case POWER_STATE_NETWORK_STANDBY:
                switch (CurrentPowerState)
                {
                    case POWER_STATE_COLD_BOOTED:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionColdBootToLowPower);
                    // fall through intentionally

                    case POWER_STATE_LOW_POWER:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionLowPowerToNetworkStandby);
                        break;

                    case POWER_STATE_FULL_POWER:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionFullPowerToAutoWakeStandby);
                    // fall through intentionally

                    case POWER_STATE_AUTO_WAKE_STANDBY:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionAutoWakeStandbyToNetworkStandby);
                        break;
                }
                break;

            case POWER_STATE_AUTO_WAKE_STANDBY:
                switch (CurrentPowerState)
                {
                    case POWER_STATE_COLD_BOOTED:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionColdBootToLowPower);
                    // fall through intentionally

                    case POWER_STATE_LOW_POWER:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionLowPowerToNetworkStandby);
                    // fall through intentionally

                    case POWER_STATE_NETWORK_STANDBY:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionNetworkStandbyToAutoWakeStandby);
                        break;

                    case POWER_STATE_FULL_POWER:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionFullPowerToAutoWakeStandby);
                        break;

                }
                break;


            case POWER_STATE_FULL_POWER:
                switch (CurrentPowerState)
                {
                    case POWER_STATE_COLD_BOOTED:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionColdBootToLowPower);
                    // fall through intentionally

                    case POWER_STATE_LOW_POWER:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionLowPowerToNetworkStandby);
                    // fall through intentionally

                    case POWER_STATE_NETWORK_STANDBY:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionNetworkStandbyToAutoWakeStandby);
                    // fall through intentionally

                    case POWER_STATE_AUTO_WAKE_STANDBY:
                        PowerPostMsg(POWER_MESSAGE_ID_TransitionAutoWakeStandbyToFullPower);
                        break;
                }
                break;


            default:
                LOG(power, ROTTEN_LOGLEVEL_NORMAL, "Transition to %d unsupported", DesiredPowerState);
                //Transition not possible/legal - clear desired state
                DesiredPowerState = CurrentPowerState;

        }  // switch(DesiredPowerState)

        // block for completion - may be waiting for more than 1 transition
        while (DesiredPowerState != CurrentPowerState)
        {
            xSemaphoreTake(PowerSemaphore, BLOCK_FOREVER);
        }

    }

    // and allow the next API call
    xSemaphoreGive(PowerAPIMutex);

}


/*
===============================================================================
@func PowerTask
@brief Main loop for thread/task.
===============================================================================
*/
void PowerTask(void* pvParamaters)
{
    while (1)
    {
        TaskManagerPollQueue(PowerTaskHandle);
    }
}


/*
===============================================================================
@func CEC_TaskInit
@brief Initialize things that can be done pre-scheduler.
===============================================================================
*/
void PowerTask_Init(void* p)
{
    // Main will get us to LOW POWER, we need to get us to NS
    CurrentPowerState = POWER_STATE_LOW_POWER;

    // MUTEX to ensure 1 caller at a time
    PowerAPIMutex = xSemaphoreCreateMutex();
    debug_assert(PowerAPIMutex);
    xSemaphoreGive(PowerAPIMutex);

    // Semaphore for task synchronization
    PowerSemaphore = xSemaphoreCreateBinary();
    debug_assert(PowerSemaphore);
    xSemaphoreGive(PowerSemaphore);

    PowerTaskHandle = GetManagedTaskPointer("PowerTask");
    debug_assert(NULL != PowerTaskHandle);

    PowerVariant_Init();
}


void UpdatePowerState (IpcLPMPowerState_t NewCurrentPowerState)
{
    CurrentPowerState = NewCurrentPowerState;

    UI_ViewNotify(POWER_STATE_CHANGE);

    //wake callers blocked on this transition
    xSemaphoreGive(PowerSemaphore);

}


/*
===============================================================================
@func PowerHandleMessage
@brief  Handles messages for this task.
===============================================================================
*/
void PowerTask_HandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
        case POWER_MESSAGE_ID_TransitionColdBootToLowPower:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "CB to LP");
            PowerVariant_TransitionColdBootToLowPower();
            UpdatePowerState(POWER_STATE_LOW_POWER);
            break;
        case POWER_MESSAGE_ID_TransitionLowPowerToNetworkStandby:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "LP to NS");
            PowerVariant_TransitionLowPowerToNetworkStandby();
            UpdatePowerState(POWER_STATE_NETWORK_STANDBY);
            break;
        case POWER_MESSAGE_ID_TransitionNetworkStandbyToAutoWakeStandby:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "NS to AW");
            PowerVariant_TransitionNetworkStandbyToAutoWakeStandby();
            UpdatePowerState(POWER_STATE_AUTO_WAKE_STANDBY);
            break;
        case POWER_MESSAGE_ID_TransitionFullPowerToAutoWakeStandby:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "FP to AW");
            PowerVariant_TransitionFullPowerToAutoWakeStandby();
            UpdatePowerState(POWER_STATE_AUTO_WAKE_STANDBY);
            break;
        case POWER_MESSAGE_ID_TransitionAutoWakeStandbyToNetworkStandby:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "AW to NS");
            PowerVariant_TransitionAutoWakeStandbyToNetworkStandby();
            UpdatePowerState(POWER_STATE_NETWORK_STANDBY);
            break;
        case POWER_MESSAGE_ID_TransitionAutoWakeStandbyToFullPower:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "AW to FP");
            PowerVariant_TransitionAutoWakeStandbyToFullPower();
            UpdatePowerState(POWER_STATE_FULL_POWER);
            break;
        case POWER_MESSAGE_ID_TransitionNetworkStandbyToLowPower:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "NS to LP");
            PowerVariant_TransitionNetworkStandbyToLowPower();
            UpdatePowerState(POWER_STATE_LOW_POWER);
            break;
        default:
            PowerVariant_HandleMessage(message);
            break;
    }
}
