/*
    File    :   TimerIDs.h
    Title   :
    Author  :   Richard Jackson
    Created :   12/1/2011
    Language:   C
    Copyright:  (C) 2011 Bose Corporation, Framingham, MA

    Description:   List of all timer IDs in the system

===============================================================================
*/
#ifndef TIMER_IDS_H
#define TIMER_IDS_H


/* Timer IDs. These cover the entire system. */
typedef enum
{
    /* General System */
    TIMER_ID_Idle,
    TIMER_ID_PowerManagerMinOnDelay,
    TIMER_ID_MuteTimer,
    TIMER_ID_MuteReleasePollTimer,
    TIMER_ID_CEC_HLI,
    TIMER_ID_ConKeysDebounce,
    TIMER_ID_ConKeysVulcan,
    TIMER_ID_ConKeysVulcanBoot,
    TIMER_ID_ConKeysScanTimer,
    TIMER_ID_BoseLinkMasterPollInterval,
    TIMER_ID_RFDiscoverable,
    TIMER_ID_ETAPWait,
    TIMER_ID_TAPKeyTimer,

    /* Keys */
    START_OF_KEY_TIMER_IDS,
    TIMER_ID_ConsoleRepeatOne = START_OF_KEY_TIMER_IDS,
    TIMER_ID_ConsoleReleaseOne,
    TIMER_ID_ConsoleRepeatTwo,
    TIMER_ID_ConsoleReleaseTwo,
    TIMER_ID_RFRepeat,
    TIMER_ID_RFRelease,
    TIMER_ID_IRRepeat,
    TIMER_ID_IRRelease,
    TIMER_ID_TAP1Repeat,
    TIMER_ID_TAP1Release,
    TIMER_ID_TAP2Repeat,
    TIMER_ID_TAP2Release,
    TIMER_ID_IpcRepeat,
    TIMER_ID_IpcRelease,
    TIMER_ID_CECRepeat,
    TIMER_ID_CECRelease,
    END_OF_KEY_TIMER_IDS = TIMER_ID_CECRelease,

    /* Board Test - BT_Task.c */
    TIMER_ID_BT_GPIO_Toggle,

    /* Power OSM timer */
    TIMER_ID_Power_InitLEDTimer,

    /* Lisa */
    TIMER_ID_PowerLisaTimer,

    TIMER_ID_HDMITick,
    TIMER_ID_VolumeDisplay,
    TIMER_ID_SourceDisplayTimeout,
    TIMER_ID_DCE,
    TIMER_ID_BALLAST,
    TIMER_ID_Thermistor,
    TIMER_ID_ActiveSourceMessage,
    TIMER_ID_PowerOnCECDevice,
    TIMER_ID_ImageViewOnTimer,
    TIMER_ID_DisplayPowerDown,
    TIMER_ID_InitiateARCTimer,
    TIMER_ID_GiveAudioStatusTimer,
    TIMER_ID_ResetDisplayAfterDelay,
    TIMER_ID_FactoryDefaultsReboot,

} TIMER_ID_t;


#endif // TIMER_IS_H
