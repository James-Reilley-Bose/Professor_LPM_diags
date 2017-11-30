/*
  File  : CECTask.h
  Title :
  Author  : jd71069
  Created : 06/24/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Header for CEC task.

===============================================================================
*/
#ifndef CEC_TASK_H
#define CEC_TASK_H

#include "TaskDefs.h"
#include "CEC_KeyIndex.h"
#include "event_groups.h"
#include "cec.h"
#include "nv_system.h"

#define CECPostMsg(_theMsg, cb, ...) QueueManagerPostCallbackMsg(&CECTaskHandle->Queue, _theMsg, (uint32_t[]){__VA_ARGS__}, cb, NO_BLOCK_TIME);

extern ManagedTask* CECTaskHandle;

extern EventGroupHandle_t s_CECEventGroupHandle;

// Event bit definitions
#define SYS_AUDIO_TIMER_BIT (1 << 0)

void CECTask(void* pvParamaters);
void CEC_TaskInit(void* p);
void CEC_HandleMessage(GENERIC_MSG_t* message);
void CEC_SendMessage(uint8_t* message, uint8_t len);
void CEC_SendBulkMessage(uint8_t* msg, uint8_t len);
void CEC_AllocateLogicalAddress(uint16_t physicalAddress);
BOOL CEC_IsDeviceInfoPollingActive(void);
void CECViewNotify(uint32_t notificationEvent);

typedef struct
{
    uint8_t destination : 4;
    uint8_t initiator : 4;
    union
    {
        uint8_t data[CEC_MSG_MAX_LEN_BYTES - 1];
        struct
        {
            uint8_t opcode;
            uint8_t params[CEC_MSG_MAX_LEN_BYTES - 2];
        };
    } payload;
    uint8_t length;
} CEC_t;

#endif  // CEC_TASK_H
