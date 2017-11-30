/****************************************************************************
 * Copyright:  (C) 2017 Bose Corporation, Framingham, MA                    *
 ****************************************************************************/

/**
 * @file Sii9437Task.h
 * @author lb1032816
 * @date 5/26/2017
 * @brief Defenitions of public interface for the Sii9437Task.
 *
 * This task's main purpose is to service interrupts from the Sii9437 chip as well
 * as queries from the the HDMI task.
 */

#ifndef SII_9437_TASK_H
#define SII_9437_TASK_H

#include "project.h"
#include "TaskDefs.h"
#include "i2cMaster_API.h"
#include "sii_datatypes.h"
#include "sii9437_api.h"

#define Sii9437PostMsg(_theMsg, cb, ...) QueueManagerPostCallbackMsg(&Sii9437TaskHandle->Queue, _theMsg, (uint32_t []){__VA_ARGS__}, cb, NO_BLOCK_TIME);

extern ManagedTask* Sii9437TaskHandle;
extern I2C_BUS_HANDLE_TYPE SiiI2CBusHandle;
extern sii_inst_t Sii9437Instance;
extern struct sii9437_config Sii9437Config;
extern void Sii9437_EnableEXTI();

/**
 * @brief Task entry point
*/
void Sii9437Task(void* pvParamaters);

/**
 * @brief Task Init function called on startup
*/
void Sii9437_TaskInit(void* p);

/**
 * @brief Message handler for Sii9437 Task
 *
 * @param message(GENERIC_MSG_t) Incoming IPC message
*/
void Sii9437_HandleMessage(GENERIC_MSG_t* message);

/**
 * @brief Callback function to process events occuring on the Sii9437 chip
 *
 * This function is passed to the Sii9437 Driver upon driver instatiation. The function
 * is used when servicing hardware intrrupts for the chip.
 *
 * @param eventFlags(uint32_t*): bit mask of evens provided by lattice Sii9437 driver
*/
void Sii9437_EventCallback(uint32_t* eventFlags);

//Temp function to be removed
void Sii9437_GetHotPlugDetected(void);

#endif //SII_9437_TASK_H
