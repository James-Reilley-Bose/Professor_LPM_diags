/**
 *
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

#ifndef LIGHT_SENSOR_TASK_H
#define LIGHT_SENSOR_TASK_H

#include "project.h"
#include "TaskDefs.h"
#include "RivieraLPM_IpcProtocol.h"

#define LIGHT_SENSOR_TASK_NAME           "LightSensorTask"

void LightSensorTask                  (void* pvParamaters);
void LightSensor_TaskInit             (void* p);
void LightSensorTask_HandleMessage    (GENERIC_MSG_t* msg);
void LightSensorTask_IpcPacketHandler (IpcPacket_t* packet);
BOOL LightSensorTask_PostMsg          (MESSAGE_ID_t msg_id, uint32_t* params);
BOOL LightSensorTask_Read             (uint16_t* lux_decimal, uint16_t* lux_fractional);
BOOL LightSensorTask_SetConversionMode(BOOL continuous); // set to false==single shot in low power mode

#endif // LIGHT_SENSOR_TASK_H
