/*
  File  : UpdateModuleF0.h
  Author  : Tony Palazzolo(Cardinal Peak)
  Created : December 16, 2015
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Updates the F0, this must be last since it will pull
        power from most devices
===============================================================================
*/

#ifndef _UPDATE_MODULE_F0_H
#define _UPDATE_MODULE_F0_H

#include "project.h"
#include "UpdateManagerBlob.h"

typedef enum
{
    UPDATE_F0_NOT_ACTIVE,
    UPDATE_F0_POWER_OFF,
    UPDATE_F0_DOING_TRANSFER,
    UPDATE_F0_DONE,
    UPDATE_F0_NUM_STATES,
} UpdateSubstates_F0_t;

typedef struct
{
    UpdateSubstates_F0_t State;
    uint8_t FilesSent;
    uint8_t TransferAttempts;
    BOOL LastTransferSuccess;
} UpdateState_F0_t;

#define F0_SLAVE_ADDR (0x3C) // slave address of the F0 during boot up.

#define INVALID_STARTING_APP_ADDR  (0xFFFFFFFF)

#define F0_WAIT_FOR_KICK_TIMEOUT_MS   (2000) // two seconds, it can take over a sec to erase the whole user space on the smaller part.

/*
   IAP command layout
   --------------------------------------------------------------------------------------------------------------------
   | command | addr_h1 | addr_h0 | addr_l1 | addr_l0 | count_h | count_l | checksum | data_0 | .... | data_n |
   --------------------------------------------------------------------------------------------------------------------
*/
/* offsets into buffer */
#define IAP_CMD        0
#define IAP_ADDR_H1    1
#define IAP_ADDR_H0    2
#define IAP_ADDR_L1    3
#define IAP_ADDR_L0    4
#define IAP_CNT_H      5
#define IAP_CNT_L      6
#define IAP_CHECKSUM   7
#define IAP_DATA       8
#define IAP_HDR_SIZE   IAP_DATA

/* IAP commands */
#define OPC_READ       (uint8_t)(0x03)
#define OPC_WREN       (uint8_t)(0x06)
#define OPC_ERPG       (uint8_t)(0x20)
#define OPC_APP_ADDR   (uint8_t)(0x22)
#define OPC_ERUSM      (uint8_t)(0x60)
#define OPC_USRCD      (uint8_t)(0x77)
#define OPC_ACK        (uint8_t)(0x44)
#define OPC_NACK       (uint8_t)(0x33)
#define OPC_VER        (uint8_t)(0x55)

void UpdateModuleF0_Init(void);
BOOL UpdateModuleF0_HandleMessage(GENERIC_MSG_t* msg);
void UpdateModuleF0_EnterDoF0(void);
void UpdateModuleF0_Start(UpdateBlobHeader* blob);
void UpdateModuleF0_ExitDoF0(void);

#endif // _UPDATE_MODULE_F0_H
