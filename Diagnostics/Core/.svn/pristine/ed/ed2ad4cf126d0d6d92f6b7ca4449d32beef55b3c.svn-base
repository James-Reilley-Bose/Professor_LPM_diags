/*
    File    :   genTimer.c
    Title   :
    Author  :   lec
    Created :   01/20/15
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:  generic timer abstractions functions

===============================================================================
*/


#include "project.h"
#include "TaskDefs.h"
#include "genTimer.h"

#ifdef LAST_GASP
#include "lastGasp.h"
#endif

SCRIBE_DECL(system);

typedef struct
{
    /* Associated with create, dissasociated with delete */
    TimerHandle_t timerHandle; /* Hold handle returned by xTimerCreate */
    ManagedQ* Queue;            /* Hold queue to which message will be delivered (can be deferred 'till startTimer)*/
    void* arguments;           /* arguments to be passed to receiving task (can be deferred 'till startTimer)*/
    uint32_t argCnt;           /* Argument count */
    BOOL timerUsed;            /* To avoid assigning same block for multiple events */
    MESSAGE_ID_t mid;          /* Message id ( can be deferred 'till start timer ) */
} genTimerBlock_t;

static void* SafeTimerGetTimerID( const TimerHandle_t xTimer );
static void callBackGeneric ( TimerHandle_t xTimer );

static genTimerBlock_t generalTimerBlocks[tBlockIdMax];

/*
** The following are wrapper versions of the base timer functions, modified
** to use timerBlock structure.
*/

/*
** These are local accessor methods, not meant to be callable outside of this function
*  Note that pvTimerGetTimerID does not return NULL when passed a NULL Timer Handle
*/
static void* SafeTimerGetTimerID( const TimerHandle_t xTimer )
{
    if (xTimer != NULL)
    {
        return (pvTimerGetTimerID(xTimer));
    }
    return ((void*)NULL);
}

static genTimerBlock_t* fetchTimerBlock(eTimerBlockId_t id )
{
    genTimerBlock_t* tb = NULL;

    if ( id < tBlockIdMax )
    {
        tb = &generalTimerBlocks[id];
    }
    return (tb);
}

/*
** function: createTimer          - Create a generic timer
** input:
**          unit32_t counts       - Number of ticks to expiry
**          void    *args         - Arguments to be passed in to generic callback function
**          MESSAGE_ID_t mid      - Enum identifying this timer event
**          BaseType_t recurring  - Flag indicating if this is a recurring timer
**          eTimerBlock_t tid     - A timer block id cookie corresponding to internal timerBlock structure
**          const char *name      - take a wild guess
**          ManagedQ *Queue       - the queue to use on expiry can be null as this field can be added later
** Output:
**          timer handle          - A valid timer handle or a NULL if for any reason we failed to obtain handle
*/

TimerHandle_t createTimer(uint32_t counts,
                          void* args,
                          MESSAGE_ID_t mid,
                          BaseType_t recurring,
                          eTimerBlockId_t tID,
                          const char* name,
                          ManagedQ* qptr)
{
    TimerHandle_t myTimer = NULL;
    genTimerBlock_t* timer = fetchTimerBlock(tID);

    do
    {
        if ( NULL == timer )
        {
            /*
            ** The time ID must be invalid
            */
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "locate timer block fail: %d %s", tID, __func__);
            break;
        }
        if ( timer->timerUsed == TRUE )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Timer for ID: %d, is already in use %s", tID,  __func__);
            break;
        }

        timer->arguments = (void*)args;
        timer->mid = mid;
        if ( qptr != NULL )
        {
            /*
            ** Override value only if not NULL
            */
            timer->Queue = qptr;
        }

        timer->timerHandle = xTimerCreate( name,
                                           /* The timer period in ticks. */
                                           counts,
                                           /* The timers will auto-reload themselves when they expire. */
                                           recurring,
                                           /* Assign each timer a unique id equal to its array index. */
                                           (void*)timer,
                                           /* Each timer calls the same callback when it expires. */
                                           callBackGeneric
                                         );
        myTimer = timer->timerHandle;
        if ( NULL == timer->timerHandle )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Failed xTimerCreate %s", __func__);
            /*
            ** If there was a timerblock allocation
            ** this is where it would be returned to the pool.
            ** e.g. if( tb != NULL ) { returnToTimerBlockPool(timer); }
            */
            break;
        }
        /*
        ** We succeded in associating this timer block, mark block in use.
        */
        timer->timerUsed = TRUE;
    }
    while (0);

    return ( myTimer );
}

/*
** Function: changeTimerPeriod
** Input: TimerHandle               - a handle obtained from createTimer
**        TickType_t                - Amount of time to change period to
** output:
**        BaseType_t                - Success or failure indication
*/
void changeTimerPeriod( TimerHandle_t xTimer, TickType_t xNewPeriod , TickType_t xBlockTime )
{
    if (!InInterrupt())
    {
        if (xTimerChangePeriod( xTimer, xNewPeriod, xBlockTime) != pdPASS)
        {
            debug_assert(0);
        }
    }
    else
    {
        if (xTimerChangePeriodFromISR (xTimer, xNewPeriod, NULL) != pdPASS)
        {
            debug_assert(0);
        }
    }
}

#ifdef SUPPORT_TIMER_DELETE
/*
** Function: deleteTimer
** Input: TimerHandle               - a handle obtained from createTimer
**        TickType_t                - Amount of time to wait before deleting timer
** output:
**        BaseType_t                - Success or failure indication
*/
BaseType_t deleteTimer( TimerHandle_t xTimer, TickType_t xBlockTime  )
{
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    BaseType_t ret = pdFALSE;

    /*
    ** We disassociate the a timer block from the timer here.
    */
    do
    {

        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            /*
            ** strange no timer block found this is an error
            */
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Invalid Timer Block. \t%s", __func__);
            error = pdTRUE;
            break;
        }

        ret = xTimerDelete( xTimer, xBlockTime );
        if ( pdFAIL == ret )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "unable to delete timer: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        /*
        ** OK time to decouple the timer block
        */

        timerBlock->state = timerBlockStateFree;
        break;
    }
    while (0);

    debug_assert(!error);
    return (ret);
}
#endif

/*
** function: callBackGeneric
** input:
**        TimerHandle_t        - Timer handle returned from create timer
**
** output:
**        send message to timer associated queue
** Notes:
**        This function callable only from expiry, not directly callable
*/

static void callBackGeneric ( TimerHandle_t xTimer )
{
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    signed portBASE_TYPE ret = -1;
    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);

        if ( NULL == timerBlock )
        {
            /*
            ** This is an error condition, one possible way of handling it
            ** is to create a generic orphan queue and post the expiry there.
            ** for now flag as error and will assert below.
            */
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Invalid Timer Block: \t%s", __func__);
            error = pdTRUE;
            break;
        }

        if ( ( NULL == timerBlock->Queue) || ( NULL == timerBlock->Queue->qHandle ) )
        {
            /*
            ** This is an error condition, one possible way of handling it
            ** is to create a generic orphan queue and post the expiry there.
            ** for now flag as error and will assert below.
            */
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Invalid Queue: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        /*
        ** We are fine, now post the expiry message to the preprogrammed queue
        */
#ifdef LAST_GASP
        if (timerBlock->mid == WA_MESSAGE_ID_TimerExpired ) /* Change to whatever message ID you need */
        {
            LgBumpTick(GET_INSTRUMENT_TIME_NOW());
        }
#endif
        ret = QueueManagerPostCallbackMsg(timerBlock->Queue, timerBlock->mid, timerBlock->arguments, NOP_CALLBACK, NO_BLOCK_TIME);

    }
    while (0);
    if ( ret != 0 )
    {
        error = pdTRUE;
    }
    debug_assert(!error);
}

/*
** Function: timerStart
** input:
**       TimerHandle_t              - timer cookie from createTimer function
**       TickType_t                 - number of tick to wait before staring timer
**       ManagedQ *                 - the queue to use on expiry, if queue set before use NULL to bypass setting
** outout:
**       BaseType_t                 - Success or failure indication
*/

BaseType_t timerStart( TimerHandle_t xTimer, TickType_t xBlockTime, ManagedQ* qptr)
{
    /*
    ** This function is a wrapper to xTimerStart, all it does is verify that a timer block has been
    ** properly formatted before it call the actual funtion.
    */
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    BaseType_t ret = pdFALSE;

    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Unassociated timer Block: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        if ( qptr != NULL )
        {
            /*
            ** Override value only if not NULL
            */
            timerBlock->Queue = qptr;
        }

        if ( NULL == timerBlock->Queue )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Warning, invalid queue value in timerBlock: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        ret = xTimerStart( xTimer, xBlockTime );
        break;
    }
    while (0);
    debug_assert(!error);
    return ret;
}

BaseType_t timerStop(TimerHandle_t xTimer, TickType_t xBlockTime)
{
    /*
    ** This function is a wrapper to xTimerStop, all it does is verify that a timer block has been
    ** properly formatted before it call the actual funtion.
    */
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    BaseType_t ret = pdFALSE;

    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Unassociated timer Block: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        timerBlock->Queue = NULL;
        if (!InInterrupt())
        {
            ret = xTimerStop( xTimer, xBlockTime);
        }
        else
        {
            ret = xTimerStopFromISR (xTimer, NULL);
        }
        break;
    }
    while (0);
    debug_assert(!error);
    return ret;
}

BOOL timerIsRunning(TimerHandle_t xTimer)
{
    genTimerBlock_t* timerBlock = NULL;
    BOOL retVal = FALSE;
    BaseType_t error = pdFALSE;

    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Unassociated timer Block: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        else
        {
            if (xTimerIsTimerActive(xTimer) == pdTRUE)
            {
                retVal = TRUE;
            }
        }
    }
    while (0);
    debug_assert(!error);
    return retVal;
}

BaseType_t timerStopFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken)
{
    /*
    ** This function is a wrapper to xTimerStop, all it does is verify that a timer block has been
    ** properly formatted before it call the actual funtion.
    */
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    BaseType_t ret = pdFALSE;

    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            error = pdTRUE;
            break;
        }
        timerBlock->Queue = NULL;
        ret = xTimerStopFromISR(xTimer, pxHigherPriorityTaskWoken);
        break;
    }
    while (0);
    debug_assert(!error); /* A critical error occured */
    return ret;
}

/*
** This is mainly a wrapper xTimerStartFromISR which takes into consideration
** management of the genTimerBlock_t structure.
** Function: timerStartFromISR
** input:
**    TimerHandle_t                - Timer handle returned from createTimer
**    BaseType_t *                 - Pointer to indicator of need to perform context switch
**    ManagedQ *                   - the queue to use on expiry, if queue set before use NULL to bypass setting
** Output:
**    BaseType_t                   - Success = 0 or failure != 0 indication
*/

BaseType_t timerStartFromISR( TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken, ManagedQ* qptr )
{
    /*
    ** This function is a wrapper to xTimerStart, all it does is verify that a timer block has been
    ** properly formatted before it call the actual funtion.
    */
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    BaseType_t ret = pdFALSE;

    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            /* it is meaningless to log from ISR so left as comment just in case this changes */
            /* LOG(system,ROTTEN_LOGLEVEL_NORMAL,"Unassociated timer Block: \t%s",__func__); */
            error = pdTRUE;
            break;
        }
        if ( qptr != NULL )
        {
            /*
            ** Override value only if not NULL
            */
            timerBlock->Queue = qptr;
        }

        if ( NULL == timerBlock->Queue )
        {
            /* it is meaningless to log from ISR so left as comment just in case this changes */
            /* LOG(system,ROTTEN_LOGLEVEL_NORMAL,"Warning, invalid queue value in timerBlock: \t%s",__func__); */

            error = pdTRUE;
            break;
        }
        ret = xTimerStartFromISR( xTimer, pxHigherPriorityTaskWoken );
        break;
    }
    while (0);
    debug_assert(!error); /* A critical error occured */
    return ret;
}

BaseType_t timerReset( TimerHandle_t xTimer, TickType_t xBlockTime, ManagedQ* qptr)
{
    /*
    ** This function is a wrapper to xTimerReset, all it does is verify that a timer block has been
    ** properly formatted before it call the actual funtion.
    */
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    BaseType_t ret = pdFALSE;

    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Unassociated timer Block: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        if ( qptr != NULL )
        {
            /*
            ** Override value only if not NULL
            */
            timerBlock->Queue = qptr;
        }

        if ( NULL == timerBlock->Queue )
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Warning, invalid queue value in timerBlock: \t%s", __func__);
            error = pdTRUE;
            break;
        }
        ret = xTimerReset( xTimer, xBlockTime );
        break;
    }
    while (0);
    debug_assert(!error);
    return ret;
}

BaseType_t timerResetFromISR( TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken, ManagedQ* qptr )
{
    /*
    ** This function is a wrapper to xTimerReset, all it does is verify that a timer block has been
    ** properly formatted before it call the actual funtion.
    */
    genTimerBlock_t* timerBlock = NULL;
    BaseType_t error = pdFALSE;
    BaseType_t ret = pdFALSE;

    do
    {
        timerBlock = (genTimerBlock_t*)SafeTimerGetTimerID(xTimer);
        if ( NULL == timerBlock )
        {
            /* it is meaningless to log from ISR so left as comment just in case this changes */
            /* LOG(system,ROTTEN_LOGLEVEL_NORMAL,"Unassociated timer Block: \t%s",__func__); */
            error = pdTRUE;
            break;
        }
        if ( qptr != NULL )
        {
            /*
            ** Override value only if not NULL
            */
            timerBlock->Queue = qptr;
        }

        if ( NULL == timerBlock->Queue )
        {
            /* it is meaningless to log from ISR so left as comment just in case this changes */
            /* LOG(system,ROTTEN_LOGLEVEL_NORMAL,"Warning, invalid queue value in timerBlock: \t%s",__func__); */

            error = pdTRUE;
            break;
        }
        ret = xTimerResetFromISR( xTimer, pxHigherPriorityTaskWoken );
        break;
    }
    while (0);
    debug_assert(!error); /* A critical error occured */
    return ret;
}
