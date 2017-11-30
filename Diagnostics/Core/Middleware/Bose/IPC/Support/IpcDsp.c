#include "IPCRouterTask.h"
#include "IpcDsp.h"
//#include "volumeModel.h"


SCRIBE_DECL(device_model_dsp);


void IpcDsp_Initialize()
{

}

void IpcDsp_HealthAndStatusReverseByteOrder(IpcDspHealthStatusPayload_t* dspHS)
{
    dspHS->image = REV_HALF_WORD_BYTES(dspHS->image);
    dspHS->activeSource = REV_HALF_WORD_BYTES(dspHS->activeSource);
    dspHS->uptimeMs = REV_WORD_BYTES(dspHS->uptimeMs);
    dspHS->minimumOutputLatencyMs = REV_HALF_WORD_BYTES(dspHS->minimumOutputLatencyMs);
    dspHS->totalLatencyMs = REV_HALF_WORD_BYTES(dspHS->totalLatencyMs);
}

static void IpcDsp_ProfileReverseByteOrder(IpcDspProfilePayload_t* profile)
{
    profile->sourceSelect   = REV_HALF_WORD_BYTES(profile->sourceSelect);
    profile->powerState     = REV_HALF_WORD_BYTES(profile->powerState);
    profile->targetLatencyMs = REV_HALF_WORD_BYTES(profile->targetLatencyMs);
    profile->networkLatencyMs = REV_HALF_WORD_BYTES(profile->networkLatencyMs);
}

void IpcDsp_SendProfile(IpcDspProfilePayload_t* profile)
{
    IpcDspProfilePayload_t sendBuffer = {NULL};
    memcpy(&sendBuffer, profile, sizeof(IpcDspProfilePayload_t));
    IpcDsp_ProfileReverseByteOrder(&sendBuffer);
    IPCRouter_Send(IPC_DEVICE_DSP, IPC_DSP_PROFILE, NULL, &sendBuffer, sizeof(IpcDspProfilePayload_t));
}

void IpcDsp_SendRebootToImage(DSPImage_t image)
{
    uint32_t payload = REV_WORD_BYTES(image);
    IPCRouter_Send(IPC_DEVICE_DSP, IPC_DSP_REBOOT_TO_IMAGE, NULL, &payload, sizeof(DSPImage_t));
}

void IpcDsp_SendAudioFormat(IpcDSPAudioMetaDataPayload_t* data)
{
    IPCRouter_Send(IPC_DEVICE_DSP, IPC_DSP_AUDIO_INFO, NULL, data, sizeof(IpcDSPAudioMetaDataPayload_t));
}

void IpcDsp_SendVolMuteToDSP(IpcDSPVolumePayload* dspVol)
{
    IpcDSPVolumePayload sendBuffer = {NULL};
    memcpy(&sendBuffer, dspVol, sizeof(IpcDSPVolumePayload));

    sendBuffer.muteState = REV_HALF_WORD_BYTES(dspVol->muteState);
    sendBuffer.muteProfile = REV_HALF_WORD_BYTES(dspVol->muteProfile);
    sendBuffer.volume = REV_HALF_WORD_BYTES(dspVol->volume);
    sendBuffer.rampProfile = REV_HALF_WORD_BYTES(dspVol->rampProfile);

    IPCRouter_Send(IPC_DEVICE_DSP, IPC_DSP_VOL_MUTE, NULL, &sendBuffer, sizeof(IpcDSPVolumePayload));
}

void IpcDsp_SendSysTemperatureToDSP(int16_t* sysTemp)
{
    IpcSystemTemperatureData_t sendBuffer = {0};

    for (uint16_t i = 0; i < IPC_THERM_LOCATION_TYPES; i++)
    {
        sendBuffer.ThermistorData[i].thermLocation = REV_HALF_WORD_BYTES(i);
        sendBuffer.ThermistorData[i].tempDegC = REV_HALF_WORD_BYTES(sysTemp[i]);
    }

    sendBuffer.numThermistorReadings = REV_HALF_WORD_BYTES(IPC_THERM_LOCATION_TYPES);
    IPCRouter_Send(IPC_DEVICE_DSP, IPC_SYS_TEMP, NULL, (uint8_t*) &sendBuffer, sizeof(IpcSystemTemperatureData_t));
}

void IpcDsp_SendPlayTone(IpcDspPlayTonePayload_t* playTone)
{
    IpcDspPlayTonePayload_t sendBuffer = {NULL};

    sendBuffer.locationBits = REV_HALF_WORD_BYTES(playTone->locationBits);
    sendBuffer.repeatCount = REV_HALF_WORD_BYTES(playTone->repeatCount);
    sendBuffer.minVolume = REV_HALF_WORD_BYTES(playTone->minVolume);
    sendBuffer.maxVolume = REV_HALF_WORD_BYTES(playTone->maxVolume);
    sendBuffer.toneId = REV_WORD_BYTES(playTone->toneId);

    IPCRouter_Send(IPC_DEVICE_DSP, IPC_DSP_PLAY_TONE, NULL, (uint8_t*) &sendBuffer, sizeof(IpcDspPlayTonePayload_t));
}
