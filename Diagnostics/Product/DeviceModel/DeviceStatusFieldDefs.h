/*
  File  : DeviceStatusFieldDefs.h
  Title :
  Author  : Derek Richardson
  Created : 10/21/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Web of enums and macros to allow for dynamic dispatch of getters
        and setters for status fields
===============================================================================
*/
#ifndef DEVICE_FIELD_DEFS_H
#define DEVICE_FIELD_DEFS_H

/*
 *
 *  Each field is listed as an enum which maps 1:1 with a entry in each of the
 *  getter and setter arrays. It is then up to the individual device model to
 *  handle or NOP the call. All functions must be implemented by policy. Because
 *  this is meant to be generic all fields are passed as void* and the casting is
 *  on the device model to handle.
 */

// List of all the fields to get/set
#define FOREACH_FIELD(FIELD) \
        FIELD(DeviceModel_ASOC_Version) \
        FIELD(DeviceModel_ASOC_PowerState)  \
\
        FIELD(DeviceModel_DSP_AudioFormat) \
        FIELD(DeviceModel_DSP_TrebleLevel) \
        FIELD(DeviceModel_DSP_BassLevel) \
        FIELD(DeviceModel_DSP_CenterLevel) \
        FIELD(DeviceModel_DSP_SurroundLevel) \
        FIELD(DeviceModel_DSP_SourceSelect) \
        FIELD(DeviceModel_ASOC_AppVersion) \
        FIELD(DeviceModel_ASOC_SystemVersion) \
        FIELD(DeviceModel_ASOC_LogID) \
        FIELD(DeviceModel_ASOC_UpdatePending) \
        FIELD(DeviceModel_ASOC_AppIsAlive) \
        FIELD(DeviceModel_ASOC_BootedImageType) \
        FIELD(DeviceModel_DSP_UserMute) \
        FIELD(DeviceModel_DSP_SourceMute) \
        FIELD(DeviceModel_DSP_InternalMute)  \
        FIELD(DeviceModel_DSP_Volume)  \
        FIELD(DeviceModel_DSP_EnergyPresent)  \
        FIELD(DeviceModel_DSP_PowerState)  \
        FIELD(DeviceModel_DSP_LipSyncDelay) \
        FIELD(DeviceModel_DSP_NetworkLatency) \
        FIELD(DeviceModel_DSP_AudioMode) \
        FIELD(DeviceModel_DSP_PlayTone) \
        FIELD(DeviceModel_DSP_BootAiqCurveB) \
        FIELD(DeviceModel_DSP_AiqEnable) \
        FIELD(DeviceModel_DSP_AiqCurveSelect) \
        FIELD(DeviceModel_DSP_DualMonoSelect) \
        FIELD(DeviceModel_DSP_LanguageSelect) \
        FIELD(DeviceModel_DSP_Region) \
        FIELD(DeviceModel_DSP_FrontConnector) \
        FIELD(DeviceModel_DSP_HSMinimumOutputLatencyMs) \
        FIELD(DeviceModel_DSP_HSTotalLatencyMs) \
        FIELD(DeviceModel_DSP_HSPowerState)  \
        FIELD(DeviceModel_DSP_AudioInputFormat)\
        FIELD(DeviceModel_DSP_SampleRate)\
        FIELD(DeviceModel_DSP_FullRangeChannels)\
        FIELD(DeviceModel_DSP_LfeChannels)\
\
        FIELD(DeviceModel_ST_Version) \
        FIELD(DeviceModel_ST_PowerState)  \
        FIELD(DeviceModel_ST_NetworkStatus) \
        FIELD(DeviceModel_ST_BluetoothStatus) \
        FIELD(DeviceModel_ST_wlan0) \
        FIELD(DeviceModel_ST_wlan1) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_GET_PROTOTYPE(PROTO) void PROTO ## _Get(void* buffer);
#define GENERATE_SET_PROTOTYPE(PROTO) void PROTO ## _Set(void* data);
#define GENERATE_GET_NAME(FIELD) FIELD ## _Get,
#define GENERATE_SET_NAME(FIELD) FIELD ## _Set,

// Takes all fields above and generates an enum for them
typedef enum
{
    FOREACH_FIELD(GENERATE_ENUM)
}DeviceStatusField_t;

// Declares each getter and setter
FOREACH_FIELD(GENERATE_GET_PROTOTYPE)
FOREACH_FIELD(GENERATE_SET_PROTOTYPE)

// Array of getters that maps to above enums
extern void (*deviceGetters[])(void* buffer);

// Array of setters that maps to above enums
extern void (*deviceSetters[])(void* data);

#endif /* DEVICE_FIELD_DEFS_H */
