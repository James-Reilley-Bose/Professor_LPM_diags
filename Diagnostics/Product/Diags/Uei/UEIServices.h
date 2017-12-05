/**
  ******************************************************************************
  * @file    UEIServices.h
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   UEI service layer header
  *          This file provides firmware functions to manage the following
  *          functionalities of the UEI:
  *           + Services offered by the UEI BLASTER
 @verbatim
 ===============================================================================
 **/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UEISERVICES_H
#define __UEISERVICES_H

#include "QueueManager.h"
#include "UEIDefinitions.h"
#include "unifySourceList.h"

/* Services provided by IR Blaster */
extern BOOL WaitAndCheckUeiResponse (uint32_t timeOutTicks);

extern void UEI_InitService(void);
extern void UEI_SendKey(GENERIC_MSG_t* msg);
extern void DownLoadDeviceToFDRA(GENERIC_MSG_t* msg);
extern void GetVersion (void);
extern void RemoveSingleCodeset(GENERIC_MSG_t* msg);
extern void ListAllUpgradeCode(GENERIC_MSG_t* msg);
extern void MasterReset(GENERIC_MSG_t* msg);
extern void GetKeyMap(uint16_t codeID);
extern void UEI_WakeUp(void);

extern void PopulateVersionString(void);
extern void PopulateResponseCode(void);
extern void PopulateCodesetString(void);
extern void PopulateKeyMapString(void);
extern void SetDefaultUEIResponse(void);
uint16_t UEIDeviceStringToDeviceType(uint8_t * deviceString);

typedef struct {
  uint8_t commandID;
  uint8_t CodesetNumber;
  uint8_t ResponseCode;
  uint16_t NumberOfKeysInMap;
  uint8_t LibVersionString[5];
  uint8_t SwVersionString[5];
  uint16_t *CodesetStringPtr;
  uint8_t *KeyMapStringPtr;
  uint8_t deviceStr[UEI_KEY_RECORD_SIZE];
} UEIServices_Globals;

extern   UEIServices_Globals g_Services;

#endif /* __UEISERVICES_H */
