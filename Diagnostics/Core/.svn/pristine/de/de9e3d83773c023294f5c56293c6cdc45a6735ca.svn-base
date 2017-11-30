//
// ProductTasks.c - Table of all tasks to be created.
//

#include "project.h"
#include "TaskDefs.h"
#include "TapParserAPI.h"
#include "TapListenerAPI.h"
#include "PowerTask.h"
#include "LoggerTask.h"
#include "KeyHandlerTask.h"
#include "KeyTask.h"
#include "TimerExampleTask.h"

// The table of tasks and task modules that will be created at run time
ManagedTask ProductTasks[] =
{
#ifdef LPM_HAS_TAP
    {
        /* Task Name */                      "TapParserTask",
        /* Task handle keep NULL */          NULL,
        /* Queue Size */                     NEW_MANAGED_QUEUE(40, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */      INVALID_HANDLE,
        /* Number of modules */              1,
        /* Module Init, Message Handler */   { ETAPTaskModule_Init, ETAPTaskModule_HandleMessage },
        /* Task run function */              TapParserTask,
        /* Task Priority */                  configNORMAL_TASK_PRIORITY,
        /* Stack size */                     400,
        /* Parameters */                     NULL,
    },
#endif

#ifdef LPM_HAS_TAP
    {
        /* Task Name */                     "TapListenerTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(100, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */ { TapListenerModule_Init, TapListenerModule_HandleMessage },
        /* Task run function */             TapListenerModule,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    200,
        /* Parameters */                     NULL,
    },
#endif

#ifdef LPM_HAS_LOG_SERIAL_STORE
    {
        /* Task Name */                     "LoggerTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(0, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */ { LoggerTaskInit, NULL },
        /* Task run function */             LoggerTask,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    200,
        /* Parameters */                    NULL,
    },
#endif

#ifdef LPM_HAS_KEY_HANDLER
    {
        /* Task Name */                     "KeyHandlerTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(5, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */ { KeyHandlerTask_Init, KeyHandlerTask_HandleMessage },
        /* Task run function */             KeyHandlerTask,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    200,
        /* Parameters */                    NULL,
    },
#endif

#ifdef LPM_HAS_POWER_MANAGER
    {
        /* TASK NAME */                     "PowerTask",
        /* TASK HANDLE KEEP NULL */         NULL,
        /* QUEUE SIZE */                    NEW_MANAGED_QUEUE(15, DEFAULT_NUM_PARAMS),
        /* BLOCKING HANDLE KEEP NULL */     INVALID_HANDLE,
        /* NUMBER OF MODULES */             1,
        /* MODULE INIT , MESSAGE HANDLER */ {PowerTask_Init, PowerTask_HandleMessage},
        /* TASK RUN FUNCTION */             PowerTask,
        /* TASK PRIORITY */                 configNORMAL_TASK_PRIORITY,
        /* STACK SIZE */                    400,
        /* Parameters */                    NULL,
    },
#endif

#ifdef LPM_HAS_KEY_HANDLER
    {
        /* TASK NAME */                     KEY_TASK_NAME,
        /* TASK HANDLE KEEP NULL */         NULL,
        /* QUEUE SIZE */                    NEW_MANAGED_QUEUE(5, DEFAULT_NUM_PARAMS),
        /* BLOCKING HANDLE KEEP NULL */     INVALID_HANDLE,
        /* NUMBER OF MODULES */             1,
        /* MODULE INIT , MESSAGE HANDLER */ {KeyTask_Init, KeyTask_HandleMessage},
        /* TASK RUN FUNCTION */             KeyTask,
        /* TASK PRIORITY */                 configNORMAL_TASK_PRIORITY,
        /* STACK SIZE */                    200,
        /* Parameters */                    NULL,
    },
#endif

#ifdef LPM_HAS_TIMER_EXAMPLE
    {
        /* TASK NAME */                     TIMER_EXAMPLE_TASK_NAME,
        /* TASK HANDLE KEEP NULL */         NULL,
        /* QUEUE SIZE */                    NEW_MANAGED_QUEUE(5, DEFAULT_NUM_PARAMS),
        /* BLOCKING HANDLE KEEP NULL */     INVALID_HANDLE,
        /* NUMBER OF MODULES */             1,
        /* MODULE INIT , MESSAGE HANDLER */ {TimerExampleTask_Init, TimerExampleTask_HandleMessage},
        /* TASK RUN FUNCTION */             TimerExampleTask,
        /* TASK PRIORITY */                 configNORMAL_TASK_PRIORITY,
        /* STACK SIZE */                    200,
        /* Parameters */                    NULL,
    },
#endif
};

const unsigned int NUM_MANAGED_TASKS = sizeof(ProductTasks) / sizeof(ManagedTask);
