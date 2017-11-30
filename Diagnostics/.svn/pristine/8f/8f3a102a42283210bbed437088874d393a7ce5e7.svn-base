//
// TimerExampleTask.c - Example of creating/using a timer.
//

#include "project.h"
#include "TaskDefs.h"
#include "TimerExampleTask.h"
#include "genTimer.h"

SCRIBE_DECL(timer);

#define MY_TIMER_PERIOD TIMER_SEC_TO_TICKS(2)

static void TimerExampleTask_HandleTimerExpired(void);

ManagedTask* TimerExampleTaskHandle = NULL;
TimerHandle_t ExampleTimerHandle = NULL;

void TimerExampleTask_Init(void* p)
{
    TimerExampleTaskHandle = GetManagedTaskPointer(TIMER_EXAMPLE_TASK_NAME);
    debug_assert(TimerExampleTaskHandle);

    // It's a good idea to use nonrecurring timers and rearm them
    // after they are handled. This prevents queue overflows.
    ExampleTimerHandle = createTimer(MY_TIMER_PERIOD, // Period
                                     NULL, // No args
                                     Example_Message_ID_MyTimerExpired, // Message ID
                                     FALSE, // Nonrecurring
                                     tBlockIdIPCBulk, // Timer ID
                                     "MyExampleTimer", // Name
                                     &TimerExampleTaskHandle->Queue); // Queue to post to
}

void TimerExampleTask(void* pvParameters)
{
    timerStart(ExampleTimerHandle, 0, &TimerExampleTaskHandle->Queue);
    for (;;)
    {
        TaskManagerPollQueue(TimerExampleTaskHandle);
    }
}

void TimerExampleTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case Example_Message_ID_MyTimerExpired:
            TimerExampleTask_HandleTimerExpired();
            break;
        default:
            LOG(timer, ROTTEN_LOGLEVEL_NORMAL, "TimerExampleTask unhandled message %d", msg->msgID);
            break;
    }
}

static void TimerExampleTask_HandleTimerExpired(void)
{
    LOG(timer, ROTTEN_LOGLEVEL_VERBOSE, "Example timer expired.");
    timerReset(ExampleTimerHandle, 0, &TimerExampleTaskHandle->Queue);
}
