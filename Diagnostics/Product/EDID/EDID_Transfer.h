#ifndef EDID_TRANSFER_H
#define EDID_TRANSFER_H

/*
  File  : EDID_Transfer.h
  Title : EDID Bulk Transfer
  Author  : DR
  Created : 11/23/2016
  Language: C
  Copyright:  (C) 2016 Bose Corporation, Framingham, MA

  Description:
        API for sending the EDID to another processor over IPC
===============================================================================
*/
#include "IpcInterface.h"

void EDID_Transfer_Start(Ipc_Device_t device);
void EDID_Transfer_HandleMessage(GENERIC_MSG_t* msg);
#endif /* EDID_TRANSFER_H */
