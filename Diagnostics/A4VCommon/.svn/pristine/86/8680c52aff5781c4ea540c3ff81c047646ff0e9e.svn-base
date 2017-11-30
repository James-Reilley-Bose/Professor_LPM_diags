//
// RemoteTask.c
//

#include "project.h"
#include "RemoteDefs.h"
#include "RemoteTask.h"
#include "RemoteDriver.h"
#include "RemoteSPI.h"
#include "RemoteRx.h"
#include "RemoteTx.h"
#include "RemoteState.h"
#include "RemoteUpdate.h"
#include "genTimer.h"
#include "BMutex.h" //todo: don't need this?
#include "buffermanager.h"  // todo: GET RID OF THIS NONSENSE


SCRIBE_DECL(remote);
ManagedTask* RemoteTaskHandle = NULL;


void Remote_Task_Init (void* p)
{
    Remote_AssertReset(TRUE);
    
    RemoteTaskHandle = GetManagedTaskPointer(REMOTE_TASK_NAME);
    debug_assert(RemoteTaskHandle);

    ConfigureGpioPin(REMOTE_RESET_L_BANK, REMOTE_RESET_L_PIN, GPIO_MODE_OUT_PP, 0);
    
    Remote_BlockingInfo_getStatus.semaphore = xSemaphoreCreateBinary();
    debug_assert(Remote_BlockingInfo_getStatus.semaphore);
    BMutex_Give(Remote_BlockingInfo_getStatus.semaphore);
    Remote_BlockingInfo_opComplete.semaphore = xSemaphoreCreateBinary();
    BMutex_Give(Remote_BlockingInfo_opComplete.semaphore);
    debug_assert(Remote_BlockingInfo_opComplete.semaphore)
}


void Remote_Task_Run (void* pvParamaters)
{
    Remote_SPI_Init();
    
    Remote_AssertReset(FALSE);
    Remote_currentState = &Remote_states[REMOTE_STATE_RUNNING];
    Remote_currentState->enterState();
    
    for(;;)
    {
        Remote_currentState->run();
    }
}

void Remote_Task_HandleMessage(GENERIC_MSG_t* msg)
{
    Remote_currentState->handleMessage(msg);
}

