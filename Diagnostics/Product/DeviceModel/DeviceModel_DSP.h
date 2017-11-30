/*
  File  : DeviceModel_DSP.h
  Title :
  Author  : Derek Richardson
  Created : 10/21/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Structs and api to handle dsp model in system
===============================================================================
*/
#ifndef DEVICE_MODEL_DSP_H
#define DEVICE_MODEL_DSP_H

#include "project.h"
#include "IpcProtocolLpm.h"
#include "KeyData.h"
#include "DeviceInterface.h"

/* Prototypes
===============================================================================*/
void DeviceModel_DSP_Init();
void DeviceModel_DSP_Standby(BOOL enterStandby);
void DeviceModel_DSP_HandleMessage(GENERIC_MSG_t* msg);
void DeviceModel_DSP_HandleIPCMessage(IpcPacket_t* packet);
void DeviceModel_DSP_HandleKey(KEY_DATA_t key);
void DeviceModel_DSP_HandleWatchDog();
uint32_t DeviceModel_DSP_GetTimeout(Device_Delay delay);
void DeviceModel_DSP_Booted();
void DeviceModel_DSP_Temperature_Set(IpcThermistorData_t *pTemperature);
void DeviceModel_DSP_SetToneControl(void);
void DeviceModel_DSP_RebootToImage(DSPImage_t image);
BOOL DeviceModel_DSP_HasBootedToInstaller(void);
BOOL DeviceModel_DSP_GetBassEnableState(void);
void DeviceModel_DSP_Reboot(IpcRebootType_t type, uint32_t waitB4RebootMs);
void DeviceModel_DSP_PowerState_Set(void* pPowerState);
void DeviceModel_DSP_PowerState_Get(void *pPowerState );
void DeviceModel_DSP_Version_Get(char* buffer);
DSPImage_t DeviceModel_DSP_Image_Get(void);
void DeviceModel_DSP_SetSpeakerPackageAndFlags(IpcSpeakerPackage_t speakerPackage, IpcAccessoryFlags_t acessoryflags);

/* Defines
===============================================================================*/
#define DSP_PLAYTONE_DEFAULT_VOLUME 100

#endif /* DEVICE_MODEL_DSP_H */
