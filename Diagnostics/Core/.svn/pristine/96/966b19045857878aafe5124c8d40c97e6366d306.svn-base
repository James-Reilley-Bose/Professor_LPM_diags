/*
  File  : SourceChangeTask.h
  Title :
  Author  : ja70076
  Created : 08/18/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Header for A4V SourceChange task.

===============================================================================
*/

#ifndef _SRC_CHG_TASK_H_
#define _SRC_CHG_TASK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "UITask.h"

extern ManagedTask* SourceChangeTaskHandle;

#define SOURCE_CHANGE_QUEUE_DEPTH 10
#define SourceChangePostMsg(_theMsg) QueueManagerPostCallbackMsg(&SourceChangeTaskHandle->Queue, _theMsg, NULL, NOP_CALLBACK, NO_BLOCK_TIME);


//the SourceChange Manager API
void SourceChangeAPI_SetCurrentSource (SOURCE_ID NewSource);
SOURCE_ID SourceChangeAPI_GetCurrentSource(void);
SOURCE_ID SourceChangeAPI_GetNextSource (void);
SOURCE_ID SourceChangeAPI_GetPreviousSource (void);
BOOL SourceChangeAPI_GetSourceChangePending(void);

//for TaskDefs
void SourceChangeTask(void* pvParamaters);
void SourceChangeTaskInit(void* p);
void SourceChangeHandleMessage(GENERIC_MSG_t* message);


#ifdef __cplusplus
}
#endif


#endif //_SRC_CHG_TASK_H_
