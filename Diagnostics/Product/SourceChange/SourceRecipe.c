////////////////////////////////////////////////////////////////////////////////
/// @file          SourceRecipe.c
/// @brief         Professor source specific source change interface
///
/// @author        dj1005472
/// Copyright      2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "project.h"
#include "SourceRecipe.h"
#include "UITask.h"
#include "SourceChangeTask.h"

const SourceInterface sourceArray[NUM_SOURCES] =
{
    {SOURCE_UNUSED},//SOURCE_STANDBY
    {SOURCE_UNUSED},//SOURCE_HDMI_1
    {SOURCE_UNUSED},//SOURCE_HDMI_2
    {SOURCE_UNUSED},//SOURCE_HDMI_3
    {SOURCE_UNUSED},//SOURCE_HDMI_4
    {SOURCE_UNUSED},//SOURCE_HDMI_5
    {SOURCE_UNUSED},//SOURCE_HDMI_6
    {SOURCE_UNUSED},//SOURCE_SIDE_AUX
    {SOURCE_UNUSED},//SOURCE_TV
    {SOURCE_UNUSED},//SOURCE_SHELBY
    {SOURCE_UNUSED},//SOURCE_BLUETOOTH
    {SOURCE_UNUSED},//SOURCE_ADAPTIQ
    {SOURCE_UNUSED},//SOURCE_DEMO
    {SOURCE_UNUSED},//SOURCE_PTS
    {SOURCE_UNUSED},//SOURCE_UPDATE
    {SOURCE_UNUSED},//SOURCE_ANALOG_FRONT
    {SOURCE_UNUSED},//SOURCE_ANALOG1
    {SOURCE_UNUSED},//SOURCE_ANALOG2
    {SOURCE_UNUSED},//SOURCE_COAX1
    {SOURCE_UNUSED},//SOURCE_COAX2
    {SOURCE_UNUSED},//SOURCE_OPTICAL1
    {SOURCE_UNUSED},//SOURCE_OPTICAL2
    {SOURCE_UNUSED},//SOURCE_UNIFY
    {SOURCE_UNUSED},//SOURCE_ASOC_INTERNAL
    {SOURCE_UNUSED},//SOURCE_DARR
    {SOURCE_UNUSED},//SOURCE_LFE
    {SOURCE_UNUSED},//SOURCE_WIRED
    {SOURCE_UNUSED},//SOURCE_COLDBOOT
    {SOURCE_UNUSED},//SOURCE_FACTORY_DEFAULT
};

void doAudioSourceMute(BOOL mute)
{
#if 0 // TODO - pull this stuff in
    BOOL internal_mute = FALSE;
    DeviceModelTask_SetField(DeviceModel_DSP_SourceMute, (void*) & (mute));
    DeviceModelTask_SetField(DeviceModel_DSP_InternalMute, (void*) & (internal_mute));
    VolumeModel_SetAudioParam(VM_USER_MUTE, FALSE);
#endif
}

void doVideoSourceMute(BOOL mute)
{

}

void doAudioRoute(SOURCE_ID target)
{
#if 0 // TODO - pull this stuff in
    if (GetSourceInterface(target)->dspInput != DSP_SOURCE_NONE)
    {
        DeviceModelTask_SetField(DeviceModel_DSP_SourceSelect, (void*) & (GetSourceInterface(target)->dspInput));
    }

    if (SystemBehaviors_GetEDIDReplicate())
    {
        if (UI_IsTVSource(target) == TRUE || UI_IsHDMISource(target) == TRUE)
        {
            HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioInput, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_INPUT_ARC);
            HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioOutputSource, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_TO_SINK_RX);
        }
    }
    else
    {
        // DOWNMIX, Except in TV source and standby.
        if ((FALSE == UI_IsStandbySource(target)) && (FALSE == UI_IsTVSource(target)) && (FALSE == UI_IsInternalSource(target)))
        {
            HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioOutputSource, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_TO_SINK_SPDIF);
        }
        else
        {
            HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioOutputSource, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_TO_SINK_NONE);
        }

        if (UI_IsHDMISource(target))
        {
            HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioInput, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_INPUT_RX);
        }
        else if (UI_IsTVSource(target))
        {
            HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioInput, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_INPUT_ARC);
        }
        else
        {
            HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioInput, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_INPUT_NONE);
        }
    }
#endif
}

// TODO (lb1032816): Remove when refactoring Source Change task. No need for video routing without HDMI mux
void doVideoRoute(SOURCE_ID target)
{
#if 0 // TODO - pull this stuff in
    if (UI_IsHDMISource(target) == TRUE || UI_IsTVSource(target) == TRUE)
    {
        HDMIPostMsg(HDMI_MESSAGE_ID_SourceChange, NOP_CALLBACK, UI_GetHDMIPortFromSourceID(target));
    }
#endif
}

// TODO (lb1032816): Remove when refactoring Source Change task. No need for video routing without HDMI mux
BOOL doVideoPresentCheck(void)
{
#if 0 // TODO - pull this stuff in
    BOOL hdmiVideoPresent = FALSE;
    if (UI_IsHDMISource(SourceChangeAPI_GetCurrentSource()))
    {
        hdmiVideoPresent = HDMI_GetVideoPresent();
    }
    return hdmiVideoPresent;
#endif
    return FALSE;
}

// TODO (lb1032816): Pull in latest ginger implementation of this code
void doCECRoute(SOURCE_ID current, SOURCE_ID target)
{
#if 0 // TODO - pull this stuff in
    if (UI_IsStandbySource(target) && (SOURCE_COLDBOOT != current))
    {
        CECPostMsg(CEC_MESSAGE_ID_SendSystemAudioRequest, NOP_CALLBACK, FALSE);
        CECPostMsg(CEC_MESSAGE_ID_SendStandby, NOP_CALLBACK, NULL);
    }
    else
    {
        // Always turn on System Audio
        CECPostMsg(CEC_MESSAGE_ID_SendSystemAudioRequest, NOP_CALLBACK, TRUE);

        // This is for the SoundTouch Forum ARC issue.
        CECPostMsg(CEC_MESSAGE_ID_ReportPhysicalAddress, NOP_CALLBACK, NULL);

        // Only turn on the TV if not in an internal source.
        if (FALSE == UI_IsInternalSource(target))
        {
            CECPostMsg(CEC_MESSAGE_ID_ImageViewOn, NOP_CALLBACK, NULL);
        }

        if (UI_IsTVSource(target))
        {
            if (CEC_CanWeARC(CEC_GetPhysicalAddress(), CEC_GetDeviceListPhysicalAddress(CEC_DEVICE_TV)))
            {
                CECPostMsg(CEC_MESSAGE_ID_InitiateARC, NOP_CALLBACK, CEC_DEVICE_TV);
            }
        }
        // * -> HDMIRx
        else if (UI_IsHDMISource(target))
        {
            CECPostMsg(CEC_MESSAGE_ID_PowerOnDevice, NOP_CALLBACK, UI_GetHDMIPortFromSourceID(target));

            // Switching away from TV, we'll need to send a routing change and terminate ARC.
            if (UI_IsTVSource(current))
            {
                CECPostMsg(CEC_MESSAGE_ID_SendRoutingChange, NOP_CALLBACK, UI_GetHDMIPortFromSourceID(current), UI_GetHDMIPortFromSourceID(target));
                CECPostMsg(CEC_MESSAGE_ID_TerminateARC, NOP_CALLBACK, CEC_DEVICE_TV);
            }
        }
        else if (UI_IsInternalSource(target))
        {
            CECPostMsg(CEC_MESSAGE_ID_SendRoutingChange, NOP_CALLBACK, UI_GetHDMIPortFromSourceID(current), UI_GetHDMIPortFromSourceID(target));
            CECPostMsg(CEC_MESSAGE_ID_SendSystemAudioRequest, NOP_CALLBACK, TRUE);
            CECPostMsg(CEC_MESSAGE_ID_TerminateARC, NOP_CALLBACK, CEC_DEVICE_TV);
        }

        // Wait for System Audio timer to timeout (successful or not)
        if (s_CECEventGroupHandle != NULL)
        {
            xEventGroupWaitBits(s_CECEventGroupHandle, SYS_AUDIO_TIMER_BIT, pdTRUE, pdFALSE, TIMER_MSEC_TO_TICKS(WAIT_FOR_TV_SYSTEM_AUDIO_EVENT_BITS_WAIT));
        }
    }
#endif
}

