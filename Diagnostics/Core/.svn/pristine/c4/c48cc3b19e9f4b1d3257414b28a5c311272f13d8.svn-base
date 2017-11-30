/*
  File  : SourceChangeTask.c
  Title :
  Author  : ja70076
  Created : 08/18/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:  The task responsible for source change

 ==============================================================================*/

#include "project.h"
#include "SourceChangeTask.h"
#include "PowerTask.h"
#include "TaskDefs.h"
#include "rottenlog.h"
#include "SourceRecipe.h"
//#include "DeviceModel_ST.h"
#include "viewNotify.h"
#include "BMutex.h"

#define BINARY_SEMAPHORE_LENGTH 1

/* Logs
===============================================================================*/
SCRIBE_DECL(sourcechange);

/* Global Variable
===============================================================================*/
ManagedTask* SourceChangeTaskHandle = NULL;

/* Local Defines
===============================================================================*/

/* Local Variables
===============================================================================*/
static SOURCE_ID currentSource = SOURCE_COLDBOOT;
static SOURCE_ID desiredSource = SOURCE_COLDBOOT;
static SOURCE_ID previousSource = SOURCE_COLDBOOT;
static BOOL sourceChangePending = FALSE;
static QueueSetHandle_t sourceChangeQueueSet = NULL;
static SemaphoreHandle_t doSourceChange = NULL;

/* Local Prototypes
===============================================================================*/
static void DeactivateSource(SOURCE_ID src);
static void ActivateSource(SOURCE_ID src);
static void SourceChange_DoSourceChange(void);

/*
===============================================================================
@func SourceChange_GetCurrentSource
@brief API for providing the current SourceChange State to the system.
===============================================================================
*/
SOURCE_ID SourceChangeAPI_GetCurrentSource(void)
{
    return (currentSource);
}

SOURCE_ID SourceChangeAPI_GetNextSource(void)
{
    return (desiredSource);
}

SOURCE_ID SourceChangeAPI_GetPreviousSource(void)
{
    return (previousSource);
}

BOOL SourceChangeAPI_GetSourceChangePending(void)
{
    return sourceChangePending;
}

/*
============================================================================================
@func SourceChange_SetSourceChangeState
@brief API for the Source Switch task to initiate SourceChange transition.
============================================================================================
*/
void SourceChangeAPI_SetCurrentSource(SOURCE_ID src)
{
    if (src >= NUM_SOURCES)
    {
        return;
    }

    desiredSource = src;
    BMutex_Give(doSourceChange);
}

static void DeactivateSource(SOURCE_ID src)
{
    GetSourceInterface(src)->deactivate();
}

static void ActivateSource(SOURCE_ID src)
{
#if 0 // TODO - do we need this
    UnifySetCurrentAudioProfile(src);
#endif
    GetSourceInterface(src)->activate();
}

static void SourceChange_DoSourceChange(void)
{
    if (currentSource == desiredSource)
    {
        return;
    }

    sourceChangePending = TRUE;

    do
    {
        // this is looping because desiredSource may be changed by another task
        UI_ViewNotify(SOURCE_CHANGE_START);
        SOURCE_ID target = desiredSource;

        /* DO NOT CHANGE THIS LOG LINE - THE DEMO ENGINE NEEDS THIS INFO IN THIS SYNTAX */
        LOG(sourcechange, ROTTEN_LOGLEVEL_NORMAL, "Source change from: %d to: %d ", currentSource, desiredSource);

        if (currentSource < NUM_SOURCES)
        {
            if (GetSourceInterface(target)->dspInput != DSP_SOURCE_SPDIF_NETWORK ||
                    GetSourceInterface(currentSource)->dspInput != DSP_SOURCE_SPDIF_NETWORK)
            {
                // Do not mute if currentSource and target are both the SM2 (PAELLA-7063, 7268)
                doAudioSourceMute(TRUE);
                doVideoSourceMute(TRUE);
            }
            DeactivateSource(currentSource);
        }

        ActivateSource(target);
        doAudioRoute(target);
        doVideoRoute(target);
        doCECRoute(currentSource, target);

        previousSource = currentSource;
        currentSource = target;
    }
    while (currentSource != desiredSource);

    if (GetSourceInterface(currentSource)->unMuteAudioAfterSourceChange)
    {
        doAudioSourceMute(FALSE);
    }

    if (GetSourceInterface(currentSource)->unMuteVideoAfterSourceChange)
    {
        doVideoSourceMute(FALSE);
    }

    UI_ViewNotify(SOURCE_CHANGE_FINISH);
    sourceChangePending = FALSE;
}

/*
===============================================================================
@func SourceChangeTask
@brief Main loop for thread/task.
===============================================================================
*/
void SourceChangeTask(void* pvParamaters)
{
    while (1)
    {
        QueueSetMemberHandle_t xActivatedMember = xQueueSelectFromSet(sourceChangeQueueSet, portMAX_DELAY);
        if (!xActivatedMember)
        {
            continue;
        }

        if (xActivatedMember == doSourceChange)
        {
            BMutex_Take(doSourceChange, 0);
            SourceChange_DoSourceChange();
        }
        else if (xActivatedMember == SourceChangeTaskHandle->Queue.qHandle)
        {
            TaskManagerPollQueue(SourceChangeTaskHandle);
        }
    }
}


/*
===============================================================================
@func CEC_TaskInit
@brief Initialize things that can be done pre-scheduler.
===============================================================================
*/
void SourceChangeTaskInit (void* p)
{
    SourceChangeTaskHandle = GetManagedTaskPointer("SourceChangeTask");
    debug_assert(SourceChangeTaskHandle);

    doSourceChange = xSemaphoreCreateBinary();
    debug_assert(doSourceChange);

    sourceChangeQueueSet = xQueueCreateSet(SOURCE_CHANGE_QUEUE_DEPTH + BINARY_SEMAPHORE_LENGTH);
    debug_assert(sourceChangeQueueSet);

    xQueueAddToSet(SourceChangeTaskHandle->Queue.qHandle, sourceChangeQueueSet);
    xQueueAddToSet(doSourceChange, sourceChangeQueueSet);
}

/*
===============================================================================
@func SourceChangeHandleMessage
@brief  Handles messages for this task.
===============================================================================
*/
void SourceChangeHandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
#if 0 // TODO - do we still need this
        case SOURCE_CHANGE_MESSAGE_ID_ReassertCurrentCEC:
            if (!UI_IsStandbySource(currentSource))
            {
                doCECRoute(currentSource, desiredSource);
            }
            break;
#endif
        default:
            LOG(sourcechange, ROTTEN_LOGLEVEL_NORMAL, "SourceChangeHandleMessage: no handler for %s", GetEventString(message->msgID));
            break;
    }
}
