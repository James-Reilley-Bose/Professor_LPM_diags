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
#include "UpdateManagerTask.h"
#include "SpiIpcTask.h"
#include "IpcI2CRxTxTask.h"
#include "IpcTxTask.h"
#include "DeviceModelTask.h"
#include "UITask.h"
#include "SourceChangeTask.h"
#include "Sii9437Task.h"
#include "CECTask.h"
#include "UEITask.h"
#include "WirelessAudioTask.h"
#include "LightBarCapSenseTask.h"
#include "WiredIDTask.h"
#include "RemoteTask.h"

#if defined(INCLUDE_DIAGS_TEST)
#include "DiagTask.h"
#endif

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

#ifdef LPM_HAS_INTER_PROCESSOR_COMM
    {  /*** Start IPC Router Task ***/
        /* Task Name */                     "IPCRouterTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(20, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */  {IPCRouter_TaskInit, IPCRouter_HandleMessage},
        /* Task run function */             IPCRouterTask,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    400,
        /* Parameters */                    NULL,
    },   /*** End IPC Router Task ***/
#endif

#ifdef LPM_HAS_SPI_IPC
    {
        /* TASK NAME */                     SPI_IPC_TASK_NAME,
        /* TASK HANDLE KEEP NULL */         NULL,
        /* QUEUE SIZE */                    NEW_MANAGED_QUEUE(20, DEFAULT_NUM_PARAMS),
        /* BLOCKING HANDLE KEEP NULL */     INVALID_HANDLE,
        /* NUMBER OF MODULES */             1,
        /* MODULE INIT , MESSAGE HANDLER */ {SpiIpcTask_Init, SpiIpcTask_HandleMessage},
        /* TASK RUN FUNCTION */             SpiIpcTask,
        /* TASK PRIORITY */                 configNORMAL_TASK_PRIORITY,
        /* STACK SIZE */                    200,
        /* Parameters */                    NULL,
    },
#endif

#ifdef LPM_HAS_STANDBY_CONTROLLER
    {  /*** START IPC_DEVICE_F0_NAME TASK ***/
        /* TASK NAME */                     IPC_DEVICE_F0_NAME,
        /* TASK HANDLE KEEP NULL */         NULL,
        /* QUEUE SIZE */                    NEW_MANAGED_QUEUE(50, IPC_TX_NUM_PARAMS),
        /* BLOCKING HANDLE KEEP NULL */     INVALID_HANDLE,
        /* NUMBER OF MODULES */             1,
        /* MODULE INIT , MESSAGE HANDLER */ { IpcI2CRxTxTask_Init, IpcI2CRxTxTask_HandleMessage },
        /* TASK RUN FUNCTION */             IpcI2CRxTxTask,
        /* TASK PRIORITY */                 configNORMAL_TASK_PRIORITY,
        /* STACK SIZE */                    400,
        /* PARAMETERS */                    NULL,
    }, /*** Wireless Audio monitor task ***/
#endif


#ifdef LPM_HAS_DSP
   {  /*** START IPC DSP TASK ***/
      /* TASK NAME */                     "DSPModel",
      /* TASK HANDLE KEEP NULL */         NULL,
      /* QUEUE SIZE */                    NEW_MANAGED_QUEUE(20, IPC_TX_NUM_PARAMS),
      /* BLOCKING HANDLE KEEP NULL */     INVALID_HANDLE,
      /* NUMBER OF MODULES */             2,
      /* MODULE INIT , MESSAGE HANDLER */ {{DeviceModelTask_Init, DeviceModelTask_HandleMessage},
                                          {NULL, IpcTxTask_HandleMessage }},
      /* TASK RUN FUNCTION */             DeviceModelTask,
      /* TASK PRIORITY */                 configNORMAL_TASK_PRIORITY,
      /* STACK SIZE */                    300,
      /* PARAMETERS */                    {&DeviceInterfaceTable[IPC_DEVICE_DSP] , NULL},
   }, /*** END IPC_DEVICE_DSP_NAME TASK ***/
#endif

#ifdef LPM_HAS_WIRELESS_AUDIO
   {  /*** Wireless Audio Task ***/
    /* TASK NAME */                          "WATask",
    /* TASK HANDLE KEEP NULL */              NULL,
    /* QUEUE SIZE */                         NEW_MANAGED_QUEUE(10, DEFAULT_NUM_PARAMS),
    /* BLOCKING HANDLE KEEP NULL */          INVALID_HANDLE,
    /* NUMBER OF MODULES */                  1,
    /* MODULE INIT , MESSAGE HANDLER */      { WirelessAudioTask_Init, WirelessAudioTask_HandleMessage }, /* Initialization done in task */
    /* TASK RUN FUNCTION */                    WirelessAudioTask,
    /* TASK PRIORITY */                      configNORMAL_TASK_PRIORITY,
    /* STACK SIZE */                         400,
    /* Parameters */                         NULL,

   }, /*** Wireless Audio Task ***/
#endif

#ifdef LPM_HAS_UPDATE_MANAGER
    {
        /* TASK NAME */                     UPDATE_MANAGER_TASK_NAME,
        /* TASK HANDLE KEEP NULL */         NULL,
        /* QUEUE SIZE */                    NEW_MANAGED_QUEUE(30, DEFAULT_NUM_PARAMS),
        /* BLOCKING HANDLE KEEP NULL */     INVALID_HANDLE,
        /* NUMBER OF MODULES */             1,
        /* MODULE INIT , MESSAGE HANDLER */ {UpdateManagerTask_Init, UpdateManagerTask_HandleMessage},
        /* TASK RUN FUNCTION */             UpdateManagerTask,
        /* TASK PRIORITY */                 configHIGH_TASK_PRIORITY,
        /* STACK SIZE */                    400,
        /* Parameters */                    NULL,
    },
#endif

#ifdef LPM_HAS_USER_INTERFACE
    {  /*** START UI Task ***/
        /* Task Name */                     "UITask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(20, 5),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */  { UI_TaskInit, UI_HandleMessage },
        /* Task run function */             UITask,
        /* Task Priority */                 configCRITICAL_TASK_PRIORITY,
        /* Stack size */                    400,
        /* Parameters */                    NULL,
    }, /*** END UI Task ***/
#endif

#ifdef LPM_HAS_SOURCE_CHANGE
    {  /*** START Source Change Task ***/
        /* Task Name */                     "SourceChangeTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(SOURCE_CHANGE_QUEUE_DEPTH, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */  { SourceChangeTaskInit, SourceChangeHandleMessage },
        /* Task run function */             SourceChangeTask,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    200,
        /* Parameters */                    NULL,
    }, /*** END Source Change Task ***/
#endif
    
#ifdef LPM_HAS_UEI_BLASTER
   {  /*** UEI Blaster Task ***/
    /* Task Name */                         "UEIBlasterTask",
    /* Task handle keep NULL */             NULL,
    /* Queue Size */                        NEW_MANAGED_QUEUE(20, DEFAULT_NUM_PARAMS),
    /* Blocking handle keep NULL */         INVALID_HANDLE,
    /* Number of modules */                 1,
    /* Module Init, Message Handler */      { UEIBlasterModule_Init, UEIBlasterModule_HandleMessage },
    /* Task run function */                 UEIBlasterModule,
    /* Task Priority */                     configNORMAL_TASK_PRIORITY,
    /* Stack size */                        300,
    /* Parameters */                        NULL,
   },   /*** UEI Blaster Task ***/
#endif
   


#ifdef LPM_HAS_SII9437_CHIP
    {  /*** START SII9437 Task ***/
        /* Task Name */                     "Sii9437Task",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(50, IPC_TX_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */  { Sii9437_TaskInit, Sii9437_HandleMessage },
        /* Task run function */             Sii9437Task,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    400,
        /* Parameters */                    NULL,
    }, /*** END SII9437 Task ***/

    {  /*** START SII9437 Task ***/
        /* Task Name */                     "CECTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(50, IPC_TX_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */  { CEC_TaskInit, CEC_HandleMessage },
        /* Task run function */             CECTask,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    400,
        /* Parameters */                    NULL,
    }, /*** END SII9437 Task ***/
#endif

#ifdef LPM_HAS_LIGHTBARCAPSENSE
    {  /*** START LightBarCapSense Task ***/
        /* Task Name */                     "LBCSTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(20, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */  { LightBarCapSense_TaskInit, LightBarCapSenseTask_HandleMessage },
        /* Task run function */             LightBarCapSenseTask,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    200,
        /* Parameters */                    NULL,
    }, /*** END LightBarCapSense Task ***/
#endif
    
#ifdef LPM_HAS_WIRED_BASS_DETECT
    { /*** Wired Bass Detect Task ***/
        /* Task Name */                     "WiredIDTask",
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(20, DEFAULT_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */ { WiredID_TaskInit, WiredID_HandleMessage},
        /* Task run function */             WiredID_Task,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    200,
        /* Parameters */                    NULL,
    }, /*** End Misc Task ***/
#endif

#if defined(INCLUDE_DIAGS_TEST)
    { /*** Start DiagsTest Task ***/
        /* Task Name */                     DIAG_TASK_NAME,
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(100, DEFAULT_NUM_PARAMS + 1),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */  {DiagTask_Init, DiagTask_HandleMsg},
        /* Task run function */             DiagTask,
        /* Task Priority */                 configNORMAL_TASK_PRIORITY,
        /* Stack size */                    200,
        /* Parameters */                    NULL,
    }, /*** End Misc Task ***/
#endif

#ifdef LPM_HAS_RF_REMOTE
    { /*** Start RF Remote Task ***/
        /* Task Name */                     REMOTE_TASK_NAME,
        /* Task handle keep NULL */         NULL,
        /* Queue Size */                    NEW_MANAGED_QUEUE(10, REMOTE_MESSAGE_NUM_PARAMS),
        /* Blocking handle keep NULL */     INVALID_HANDLE,
        /* Number of modules */             1,
        /* Module Init, Message Handler */ { Remote_Task_Init, Remote_Task_HandleMessage},
        /* Task run function */             Remote_Task_Run,
        /* Task Priority */                 configLOW_TASK_PRIORITY,
        /* Stack size */                    330,
        /* Parameters */                    NULL,
    }, /*** End RF Remote Task ***/
#endif
};

const unsigned int NUM_MANAGED_TASKS = sizeof(ProductTasks) / sizeof(ManagedTask);
