/*
 * Functionality for RemoteStates.
 *
 * Current states:
 *  - running: The normal state which is active most of the time.
 *  - updating: Bootloader is active, possibly updating flash
 *  - error: Board is broken!
*/

#include "SpiBus_API.h"
#include "RemoteDefs.h"
#include "RemoteState.h"
#include "RemoteTx.h"
#include "RemoteSPI.h"
#include "RemoteDriver.h"
#include "RemoteTask.h"
#include "RemoteUpdate.h"
#include "BMutex.h" // todo: remove if not needed anymore

static void RunningEnterState(void);
//static void RunningExitState(void);
static void RunningRun(void);
static void RunningHandleMessage(GENERIC_MSG_t*);
static void UpdatingEnterState(void);
static void UpdatingExitState(void);
static void UpdatingRun(void);
static void UpdatingHandleMessage(GENERIC_MSG_t*);
static void ErrorEnterState(void);
static void ErrorExitState(void);
static void ErrorRun(void);
static void ErrorHandleMessage(GENERIC_MSG_t*);

// immedietly returns
static void EmptyFunction(void);
//static void EmptyMessageHandler(GENERIC_MSG_t*);

extern SPI_BUS_HANDLE_TYPE RemoteSpiHandle;
extern SPI_Bus_Config_t runningSpiConfig;
extern SPI_Bus_Config_t updatingSpiConfig;
extern Remote_Update_Status_t status;  // RemoteUpdate.c

// To change the initial state, go to RemoteTask.c
Remote_State_t* Remote_currentState = NULL; // static
SCRIBE_DECL(remote);


Remote_State_t Remote_states[] = 
{
    // running state
    {
        REMOTE_STATE_RUNNING,
        RunningEnterState,
        EmptyFunction,
        RunningRun,
        RunningHandleMessage
    },
    
    // updating state
    {
        REMOTE_STATE_UPDATING,
        UpdatingEnterState,
        UpdatingExitState,
        UpdatingRun,
        UpdatingHandleMessage
    },
    
    // error state
    {
        REMOTE_STATE_ERROR,
        ErrorEnterState,
        ErrorExitState,
        ErrorRun,
        ErrorHandleMessage
    }
};


void Remote_changeState(Remote_StateID_t nextState)
{
    if(Remote_currentState == NULL)
        return;
    if(Remote_currentState->id == nextState)
        return;
    Remote_AssertReset(TRUE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(5));
    Remote_currentState->exitState();
    Remote_states[nextState].enterState();
    Remote_currentState = &Remote_states[nextState];
    Remote_AssertReset(FALSE);
}

static void RunningEnterState(void)
{
    //SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;
    //RemoteSpiHandle = SPIBus_Initialize(&runningSpiConfig, &SpiError);
    ConfigureGpioPin(REMOTE_IRQ_BANK, REMOTE_IRQ_PIN, GPIO_MODE_IN_PULLUP, 0);
    Remote_SPI_EnableIRQ(TRUE);
    // De-assert reset early to send a getVersion packet
    Remote_AssertReset(FALSE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(50));
    Remote_Tx_SendGetVersion();
}

static void RunningRun(void)
{
    TaskManagerPollQueue(RemoteTaskHandle);
}

static void RunningHandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case REMOTE_MESSAGE_TEST:
        {  // Because scope.
            //uint8_t* num1 = (uint8_t*)msg->params[0];
            //uint8_t* num2 = (uint8_t*)msg->params[1];
            //REMOTE_LOG("num1=%d, num2=%d", *num1, *num2);
            SemaphoreHandle_t sem = (SemaphoreHandle_t)msg->params[0];
            REMOTE_LOG("blocking on sem..");
            BMutex_Take(sem, TIMER_SEC_TO_TICKS(6));
            REMOTE_LOG("unblocked, blocking again");
            BMutex_Take(sem, TIMER_SEC_TO_TICKS(6));
            REMOTE_LOG("Done blocking!");
            break;
        }
        case REMOTE_MESSAGE_SEND_GET_STATUS:
            //REMOTE_LOG("Got REMOTE_MESSAGE_SEND_GET_STATUS");
            Remote_Tx_SendGetStatus();
            break;
        case REMOTE_MESSAGE_SEND_SOFT_RESET:
            //REMOTE_LOG("Got REMOTE_MESSAGE_SEND_SOFT_RESET");
            Remote_Tx_SendReset();
            break;
        case REMOTE_MESSAGE_SEND_PREP_FOR_UPDATE:
            //REMOTE_LOG("Got REMOTE_MESSAGE_SEND_PREP_FOR_UPDATE");
            Remote_Tx_SendPrepForUpdate();
            break;
        case REMOTE_MESSAGE_GO_TO_UPDATE:  // For TAP testing, really
            //REMOTE_LOG("Got REMOTE_GO_TO_UPDATE");
            Remote_changeState(REMOTE_STATE_UPDATING);
            break;
        case REMOTE_MESSAGE_DO_UPDATE:
        {
            status = REMOTE_UPDATE_STATUS_UPDATING;
            //REMOTE_LOG("Got REMOTE_MESSAGE_DO_UPDATE");
            Remote_changeState(REMOTE_STATE_UPDATING);
            
            Remote_Update_Error_t result = Remote_Update_Start();
            if(result == REMOTE_UPDATE_ERROR_FLASH_ERROR)
            {
                REMOTE_LOG("Error while programming device");
                Remote_changeState(REMOTE_STATE_ERROR);
                status = REMOTE_UPDATE_STATUS_COMPLETED_FAIL;
            }
            else if(result == REMOTE_UPDATE_ERROR_INVALID_BLOB)
            {
                REMOTE_LOG("Invalid blob, update skipped");
                Remote_changeState(REMOTE_STATE_RUNNING);
                status = REMOTE_UPDATE_STATUS_COMPLETED_FAIL;
            }
            else
            {
                status = REMOTE_UPDATE_STATUS_COMPLETED_SUCCESS;
            }
            break;
        }
        case REMOTE_MESSAGE_IRQ_CHANGED:
            //REMOTE_LOG("IRQ value changed");
            Remote_SPI_ReadPackets();
            break;
        default:
            REMOTE_LOG("Unhandled: %d", msg->msgID);
            break;
    }
}


static void UpdatingEnterState(void)
{
    Remote_SPI_EnableIRQ(FALSE);
    ConfigureGpioPin(REMOTE_UPDATE_BL_BANK, REMOTE_UPDATE_BL_PIN, GPIO_MODE_OUT_OD_PULLUP, 0);    
    // now change SPI settings
    //SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;
    //RemoteSpiHandle = SPIBus_Initialize(&runningSpiConfig, &SpiError);
    //vTaskDelay(TIMER_MSEC_TO_TICKS(40)); //todo: adjust
    // De-assert reset early to send a ping packet
    Remote_AssertReset(FALSE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(50)); // allow some time for bl to start up
    Remote_Update_Ping();
}

static void UpdatingExitState(void)
{
    // todo: read possible garbage data in SPI?
}

static void UpdatingRun(void)
{
    TaskManagerPollQueue(RemoteTaskHandle);
}

static void UpdatingHandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case REMOTE_MESSAGE_UPDATE_SPI_TIMEOUT:
            //REMOTE_LOG("Got REMOTE_MESSAGE_UPDATE_SPI_TIMEOUT");
            Remote_Update_readingSPI = FALSE;
            break;
        default:
            REMOTE_LOG("Unhandled message");
    }
}


// Not sure what to do with these yet
static void ErrorEnterState(void){}
static void ErrorExitState(void){}
static void ErrorRun(void){}
static void ErrorHandleMessage(GENERIC_MSG_t* msg){}


// immedietly returns
static void EmptyFunction(void){}
//static void EmptyMessageHandler(GENERIC_MSG_t* msg){}