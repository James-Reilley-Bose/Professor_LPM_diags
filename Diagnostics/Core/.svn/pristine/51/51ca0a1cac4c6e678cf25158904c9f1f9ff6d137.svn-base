/*!
* @file        TapParserAPI.h
* @version     $Id:
* @author      Joe Dzado (JD71069)
* @brief       Etap OSM handler header
*
* Copyright 2010 Bose Corporation.
*/
#ifndef INCLUDE_TAP_PARSER_API_H
#define INCLUDE_TAP_PARSER_API_H

#include "platform_settings.h"
#include "TaskManager.h"

extern ManagedTask* ManagedTapParserTask;

// Task Module Management Interface
void TapParserTask (void* pvParamaters);
void ETAPTaskModule_HandleMessage(GENERIC_MSG_t* msg);
void ETAPTaskModule_Init(void* p);
void ETAPTaskModule_DeInit();
void ETAPTaskModule_HandleMessage(GENERIC_MSG_t* msg);

//Additional public interface for UI Task Module
//This interfaces should be implemented as thread safe (protected)
//and so any blocking behavior should be documented.
#define SIZE_OF_ETAP_OUTPUT_BUFFER  200

#define ETAPPostMsg(_theMsg, _p1) QueueManagerPostCallbackMsg(&ManagedTapParserTask->Queue, _theMsg, (uint32_t []){_p1},NOP_CALLBACK,NO_BLOCK_TIME)
#define ETAPPostMsg2(_theMsg, _p1, _p2) QueueManagerPostCallbackMsg(&ManagedTapParserTask->Queue, _theMsg, (uint32_t []){_p1, _p2},NOP_CALLBACK,NO_BLOCK_TIME)
#define TapParserPostTapCommand(_p1,_callback) QueueManagerPostCallbackMsg(&ManagedTapParserTask->Queue, TAPPARSER_MESSAGE_ID_ProcessCommand, (uint32_t []){_p1},_callback,NO_BLOCK_TIME)

boolean BlockTapOnEvent(MESSAGE_ID_t event, uint32_t timeoutTicks);

#endif
