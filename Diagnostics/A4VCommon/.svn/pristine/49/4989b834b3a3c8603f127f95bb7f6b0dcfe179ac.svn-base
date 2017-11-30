//
// WirelessAudioAPI.c
//

#include "project.h"
#include "WirelessAudioAPI.h"
#include "WirelessAudioUpdate.h"
#include "BMutex.h"

void WirelessAudio_SetStandby(BOOL enterStandby, BOOL synchronous)
{
    MESSAGE_ID_t msg = enterStandby ? WA_MSG_ID_Quiesce : WA_MSG_ID_Init;
    if(synchronous)
    {
        WirelessAudio_PostAndWaitFor(msg);
    }
    else
    {
        WirelessAudioPostMsg(msg, NOP_CALLBACK, NULL);
    }
}

void WirelessAudio_PostAndWaitFor(MESSAGE_ID_t msg)
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    debug_assert(sem);

    WirelessAudioPostMsg(msg, NOP_CALLBACK, (uint32_t) sem);

    BMutex_Take(sem, BLOCK_FOREVER);
    vSemaphoreDelete(sem);
}

void WirelessAudio_FactoryDefault(void)
{
    WirelessAudio_PostAndWaitFor(WA_MSG_ID_FactoryDefault);
}

BOOL WirelessAudio_IsPreviouslyPaired(void)
{
    return WirelessAudioUtilities_IsPreviouslyPaired();
}

uint8_t WirelessAudio_GetVersion(void)
{
    return WirelessAudioUtilities_GetVersion();
}

WAState_t WirelessAudio_GetState(void)
{
    return WirelessAudioVariant_GetState();
}

BOOL WirelessAudio_WasDarrQuickBooted(void)
{
    return WirelessAudioUtilities_WasDarrQuickBooted();
}

void WirelessAudio_QuickToFullBoot(void)
{
    WirelessAudio_PostAndWaitFor(WA_MSG_ID_QuickToFullBoot);
}

BOOL WirelessAudio_IsDarrUpdating(void)
{
    return WirelessAudioUpdate_IsDarrUpdating();
}
