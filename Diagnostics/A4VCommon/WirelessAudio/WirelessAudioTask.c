//
// WirelessAudioTask.c
//

#include "project.h"
#include "WirelessAudioVariant.h"
#include "TaskDefs.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioUpdateTransfer.h"
#include "genTimer.h"
#include "AccessoryManager.h"

#define WA_TIMER_DEFAULT_PERIOD TIMER_SEC_TO_TICKS(5)

ManagedTask* wirelessAudioTaskHandle = NULL;
static TimerHandle_t wirelessAudioTimer = NULL;

#if defined(BARDEEN)
static TimerHandle_t wirelessAudioSpeakerVerTimer = NULL;
#endif

void WirelessAudioTask_Init(void* p)
{
    wirelessAudioTaskHandle = GetManagedTaskPointer("WATask");
    debug_assert(wirelessAudioTaskHandle);

    wirelessAudioTimer = createTimer(WA_TIMER_DEFAULT_PERIOD,
                                     NULL,
                                     WA_MSG_ID_TimerExpired,
                                     FALSE,
                                     tBlockIdWaMon,
                                     "WATimer",
                                     NULL);
    debug_assert(wirelessAudioTimer);

#if defined(BARDEEN)
    wirelessAudioSpeakerVerTimer = createTimer(WA_TIMER_DEFAULT_PERIOD,
                                               NULL,
                                               WA_MSG_ID_GetSpeakerVersion,
                                               FALSE,
                                               tBlockIdWaSpkrVer,
                                               "WASpkrVerTimer",
                                               NULL);
    debug_assert(wirelessAudioSpeakerVerTimer);
#endif


    WirelessAudioVariant_Init();
    WirelessAudioUtilities_Init();
    waTransfer_Init();
}

void WirelessAudioTask(void* p)
{
    AccessoryManager_Init();
    for (;;)
    {
        TaskManagerPollQueue(wirelessAudioTaskHandle);
    }
}

void WirelessAudioTask_HandleMessage(GENERIC_MSG_t* msg)
{
    WirelessAudioVariant_HandleMessage(msg);
}

void WirelessAudioTask_StartTimer(void)
{
    timerReset(wirelessAudioTimer, portMAX_DELAY, &wirelessAudioTaskHandle->Queue);
}

void WirelessAudioTask_StopTimer(void)
{
    timerStop(wirelessAudioTimer, portMAX_DELAY);
}

void WirelessAudioTask_ChangeTimerPeriod(TickType_t newPeriod)
{
    changeTimerPeriod(wirelessAudioTimer, newPeriod, portMAX_DELAY);
}

#if defined(BARDEEN)
void WirelessAudioTask_StartVerTimer(void)
{
    timerReset(wirelessAudioSpeakerVerTimer, portMAX_DELAY, &wirelessAudioTaskHandle->Queue);
}

void WirelessAudioTask_StopVerTimer(void)
{
    timerStop(wirelessAudioSpeakerVerTimer, portMAX_DELAY);
}
#endif

WAState_t WirelessAudioTask_GetState(void)
{
    return WirelessAudioVariant_GetState();
}
