#include "project.h"
#include "viewNotify.h"

#define MAX_NUM_VIEWS 11

/* Logs
===============================================================================*/
SCRIBE_DECL(uitask);

/* Local Defines
===============================================================================*/
static UIViewNotifyEntry UIViewNotifyTable[MAX_NUM_VIEWS] = { 0 };
static void viewNotifyDispatch(uint32_t notifyEvent);

/*
===============================================================================
@func viewNotifyDispatch
@brief calls notify function when a view is registered for the event
===============================================================================
*/
static void viewNotifyDispatch(uint32_t notifyEvent)
{
    debug_assert(notifyEvent > 0);
    //If a view has registered for this event, call the notify function or post a message to the task
    for (uint8_t i = 0; i < MAX_NUM_VIEWS; i++)
    {
        if ((UIViewNotifyTable[i].notificationFunc) && (notifyEvent & UIViewNotifyTable[i].notifyEventMask))
        {
            UIViewNotifyTable[i].notificationFunc(notifyEvent);
        }
        if ((UIViewNotifyTable[i].notificationTask) && (notifyEvent & UIViewNotifyTable[i].notifyEventMask))
        {
            debug_assert(&UIViewNotifyTable[i].notificationTask->Queue);
            QueueManagerPostCallbackMsg(&UIViewNotifyTable[i].notificationTask->Queue, UIViewNotifyTable[i].notificationMsg, (uint32_t [])
            {
                notifyEvent
            }, NOP_CALLBACK, NO_BLOCK_TIME);
        }
    }
}

/*
===============================================================================
@func UI_ViewNotify
@brief view notify public function
===============================================================================
*/
void UI_ViewNotify(uint32_t notifyEvent)
{
    viewNotifyDispatch(notifyEvent);
}
/*
===============================================================================
@func UI_ViewFunctionRegistration
@brief Registers a view for UI events
===============================================================================
*/
BOOL UI_ViewFunctionRegistration(NotificationFuncPtr notify, uint32_t events)
{
    //If there is space in the list, add this notify
    for (uint8_t i = 0; i < MAX_NUM_VIEWS; i++)
    {
        if ((!UIViewNotifyTable[i].notificationFunc) && (!UIViewNotifyTable[i].notificationTask))
        {
            UIViewNotifyTable[i].notificationFunc = notify;
            UIViewNotifyTable[i].notificationTask = NULL;
            UIViewNotifyTable[i].notificationMsg = INVALID_EVENT;
            UIViewNotifyTable[i].notifyEventMask = events;
            return (TRUE);
        }
        else if ((UIViewNotifyTable[i].notificationFunc == notify) &&
                 (UIViewNotifyTable[i].notificationTask == NULL))
        {
            // Set new events they want
            UIViewNotifyTable[i].notifyEventMask |= events;
            return (TRUE);
        }
    }

    //The list is full
    LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Unable to register view notify");
    return (FALSE);
}

BOOL UI_ViewTaskRegistration(ManagedTask* task, MESSAGE_ID_t message, uint32_t events)
{
    //If there is space in the list, add this notify
    for (uint8_t i = 0; i < MAX_NUM_VIEWS; i++)
    {
        if ((!UIViewNotifyTable[i].notificationFunc) && (!UIViewNotifyTable[i].notificationTask))
        {
            UIViewNotifyTable[i].notificationFunc = NULL;
            UIViewNotifyTable[i].notificationTask = task;
            UIViewNotifyTable[i].notificationMsg = message;
            UIViewNotifyTable[i].notifyEventMask = events;
            return (TRUE);
        }
        else if ((UIViewNotifyTable[i].notificationTask == task) &&
                 (UIViewNotifyTable[i].notificationFunc == NULL))
        {
            UIViewNotifyTable[i].notificationMsg = message;
            UIViewNotifyTable[i].notifyEventMask |= events;
            return (TRUE);
        }
    }

    //The list is full
    LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Unable to register view notify");
    return (FALSE);
}


/*
===============================================================================
@func UI_ViewDeregistration
@brief remove View from event notification
===============================================================================
*/
void UI_ViewDeregistration(NotificationFuncPtr notify)
{

    uint8_t i;

    //Remove the event from the list
    for (i = 0; i < MAX_NUM_VIEWS; i++)
    {
        if (UIViewNotifyTable[i].notificationFunc == notify)
        {
            UIViewNotifyTable[i].notificationFunc = NULL;
            UIViewNotifyTable[i].notifyEventMask = 0;
            break;
        }
    }
}

/*
===============================================================================
@func UI_ViewUpdateNotifyEvent
@brief  modifies the events for view notification
===============================================================================
*/
void UI_ViewUpdateNotifyEvent(NotificationFuncPtr notify, uint32_t events)
{

    uint8_t i;

    for (i = 0; i < MAX_NUM_VIEWS; i++)
    {
        if (UIViewNotifyTable[i].notificationFunc == notify)
        {
            UIViewNotifyTable[i].notifyEventMask = events;
        }
    }
}

/*
===============================================================================
@func UI_ViewGetCurrEventMask
@brief returns the currently set event notification mask for a view
===============================================================================
*/
uint32_t UI_ViewGetCurrEventMask(NotificationFuncPtr notify)
{

    uint8_t i;

    for (i = 0; i < MAX_NUM_VIEWS; i++)
    {
        if (UIViewNotifyTable[i].notificationFunc == notify)
        {
            return (UIViewNotifyTable[i].notifyEventMask);
        }
    }

    LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "View not registered, no event mask\n");
    return (0);
}

