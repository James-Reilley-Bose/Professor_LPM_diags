/*
  File  : IpcDsp.h
  Title :
  Author  : dr1005920
  Created : 10/7/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions/types to handle and send IPC key messages
===============================================================================
*/

#ifndef _IPC_DSP_H_
#define _IPC_DSP_H_

#include "IpcProtocolLpm.h"
#include "nv_mfg.h"

//Temp lives here
void DspModel_SourceSelect(DSPSource_t source);

void IpcDsp_Initialize();
void IpcDsp_SendProfile(IpcDspProfilePayload_t* profile);
void IpcDsp_SendRebootToImage(DSPImage_t image);
void IpcDsp_SendAudioFormat(IpcDSPAudioMetaDataPayload_t* data);
void IpcDsp_SendVolMuteToDSP(IpcDSPVolumePayload* dspVol);
void IpcDsp_SendSysTemperatureToDSP(int16_t* sysTemp);
void IpcDsp_SendPlayTone(IpcDspPlayTonePayload_t* playTone);
void IpcDsp_HealthAndStatusReverseByteOrder(IpcDspHealthStatusPayload_t* dspHS);
#endif /* _IPC_DSP_H_ */
