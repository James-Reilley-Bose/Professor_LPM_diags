#include "project.h"
#include "RemoteDefs.h"
#include "RemoteDriver.h"
#include "RemoteSPI.h"
#include "RemoteTask.h"
#include "RemoteTx.h"
#include "BMutex.h"
#include "buffermanager.h"
#include "RemoteUtility.h"



SCRIBE_DECL(remote);
extern char Remote_versionString[REMOTE_VERSION_STR_LEN+1];

// A task which is blocking while waiting for an SPI response.
Remote_BlockingInfo_t Remote_BlockingInfo_getStatus;
Remote_BlockingInfo_t Remote_BlockingInfo_opComplete;

void Remote_HardReset(void)
{
    Remote_AssertReset(TRUE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(200));
    Remote_AssertReset(FALSE);
}

void Remote_SoftReset(void)
{
    RemoteTaskPostMsg(REMOTE_MESSAGE_SEND_SOFT_RESET, 0);
}


void Remote_AssertReset(BOOL assert)
{
    if(assert)
    {
        DEASSERT_GPIO_PIN_INT(REMOTE_RESET_L_BANK, REMOTE_RESET_L_PIN);
    }
    else
    {
        ASSERT_GPIO_PIN_INT(REMOTE_RESET_L_BANK, REMOTE_RESET_L_PIN);
    }
}


BOOL Remote_GetVersion(char* buf, uint8_t len)
{
    if(len < REMOTE_VERSION_STR_LEN+1) // +1 for null character
    {
        REMOTE_LOG("Buffer provided to Remote_GetVersion is too small");
        return FALSE;
    }
    if(Remote_versionString[0] == 0)
    {
        REMOTE_LOG("version has not been initialized");
        return FALSE; //was never initialized
    }
    memcpy(buf, Remote_versionString, REMOTE_VERSION_STR_LEN+1);
    return TRUE;
}

void Remote_PrepForUpdate(void)
{
    RemoteTaskPostMsg(REMOTE_MESSAGE_SEND_PREP_FOR_UPDATE, 0);
}


BOOL Remote_GetStatus(Remote_RxPktData_Status_t* status, uint16_t msTimeout)
{
    if(BMutex_Take(Remote_BlockingInfo_getStatus.semaphore, 0) == pdFALSE)
    {
        REMOTE_LOG("somebody is already blocking for status");
        return FALSE;
    }
    Remote_BlockingInfo_getStatus.data = status;
    Remote_Tx_SendGetStatus();
    BOOL didNotTimeout = BMutex_Take(Remote_BlockingInfo_getStatus.semaphore,
                                     pdMS_TO_TICKS(msTimeout));
    BMutex_Give(Remote_BlockingInfo_getStatus.semaphore);
    return didNotTimeout;
}


BOOL Remote_StartScan(uint8_t* result, uint8_t secTimeout)
{
    if(BMutex_Take(Remote_BlockingInfo_opComplete.semaphore, 0) == pdFALSE)
    {
        REMOTE_LOG("somebody is already blocking for startscan");
        return FALSE;
    }
    Remote_BlockingInfo_opComplete.data = result;
    Remote_Tx_SendStartScan(secTimeout);
    BOOL didNotTimeout = BMutex_Take(Remote_BlockingInfo_opComplete.semaphore,
                                     pdMS_TO_TICKS(secTimeout*1000 + 300));
    BMutex_Give(Remote_BlockingInfo_opComplete.semaphore);
    return didNotTimeout;
}

BOOL Remote_ClearPairing(uint8_t* result, uint16_t msTimeout)
{
    if(BMutex_Take(Remote_BlockingInfo_opComplete.semaphore, 0) == pdFALSE)
    {
        REMOTE_LOG("somebody is already blocking for clearpairing");
        return FALSE;
    }
    Remote_BlockingInfo_opComplete.data = result;
    Remote_Tx_SendClearPairing();
    BOOL didNotTimeout = BMutex_Take(Remote_BlockingInfo_opComplete.semaphore,
                                     pdMS_TO_TICKS(msTimeout));
    BMutex_Give(Remote_BlockingInfo_opComplete.semaphore);
    return didNotTimeout;
}
