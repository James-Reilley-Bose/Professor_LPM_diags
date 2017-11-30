/*
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#pragma once

#include "project.h"
#include "TaskDefs.h"
#include "PSoCComms.h"

extern ManagedTask* lbcsTaskHandle;

#define LBCSPostMsg(_theMsg, ...) QueueManagerPostCallbackMsg(&lbcsTaskHandle->Queue, _theMsg, (uint32_t []){__VA_ARGS__},NOP_CALLBACK, NO_BLOCK_TIME);

// LightBarCapSenseTask Functions
void LightBarCapSenseTask(void* pvParamaters);
void LightBarCapSense_TaskInit(void* p);
void LightBarCapSenseTask_HandleMessage(GENERIC_MSG_t* msg);

//PSoC SW version Functions
BOOL GetPsocSwVersion (uint8_t * majorVersion, uint8_t * minorVersion);

// PSoC bootloader functions
void PsocReset(void);
void PsocStartBootload(void);
BOOL PsocBootloadHeader(unsigned char *header);
BOOL PsocBootloadData(unsigned char *data);
BOOL PsocEndBootload(void);

// LightBar commands
void LBCS_SendSimpleCommand(PSoCCommsCommand_t command);
void LBCS_LEDsSetAll(uint8_t * pattern, uint8_t pattern_size);
void LBCS_LEDsSetOne(uint8_t led_id, uint16_t led_intensity);
void LBCS_AnimationLoadStart(uint8_t num_of_patterns, uint8_t autostart, uint8_t loop);
BOOL LBCS_AnimationLoadStartFromDB(uint16_t animationId, uint8_t autostart, uint8_t loop);
void LBCS_AnimationLoadPattern(uint16_t duration, uint8_t * pattern, uint8_t pattern_size);
