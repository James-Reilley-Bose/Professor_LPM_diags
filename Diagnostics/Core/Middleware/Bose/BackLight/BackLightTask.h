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

#ifndef BACKLIGHTTASK_H_
#define BACKLIGHTTASK_H_

#include "project.h"
#include "TaskDefs.h"
#include "RivieraLPM_IpcProtocol.h"

#define BACK_LIGHT_TASK_NAME           "BackLightTask"
#define BackLightPostMsg(_theMsg, ...) QueueManagerPostCallbackMsg(&backLightTaskHandle->Queue, _theMsg, (uint32_t []){__VA_ARGS__},NOP_CALLBACK, NO_BLOCK_TIME);

extern ManagedTask* backLightTaskHandle;
   
void        BackLightTask                 (void* pvParamaters);
void        BackLight_TaskInit            (void* p);
void        BackLightTask_HandleMessage   (GENERIC_MSG_t* msg);
void        BackLightTask_IpcPacketHandler(IpcPacket_t* packet);
ipc_uint8_t BackLightTask_Get             ();
void        BackLightTask_Set             (ipc_uint8_t intensity);

#endif /* BACKLIGHTTASK_H_ */
