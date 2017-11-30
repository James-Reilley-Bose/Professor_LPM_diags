/*
  File: A4V_IpcPlatformConfig.h
  Author: Dillon Johnson
  Created: 12/18/2015
  Copyright: (C) 2015 Bose Corporation, Framingham, MA

  Description:
    Platform config for LPM.
*****************************************************************************
*/

#ifndef IPC_PLATFORM_CONFIG_H
#define IPC_PLATFORM_CONFIG_H

#include "project.h"
#include "BoseUSART_API.h"

#define ipc_uint8_t uint8_t
#define ipc_uint16_t uint16_t
#define ipc_uint32_t uint32_t
#define ipc_int8_t int8_t
#define ipc_int16_t int16_t
#define ipc_int32_t int32_t
#define ipc_bool_t uint8_t

#define IPC_ALIGN_PACKET // not needed on LPM, leave blank

#endif // IPC_PLATFORM_CONFIG_H
