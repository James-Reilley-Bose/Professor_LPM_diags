#include "project.h"
#include "etap.h"
#include "RemoteTap.h"
#include "buffermanager.h"
#include "RemoteTx.h"
#include "RemoteRx.h"
#include "RemoteTask.h"
#include "RemoteDriver.h"
#include "RemoteUpdate.h"
#include "RemoteUtility.h"
#include "RemoteSPI.h"
#include "RemoteNames.h"
#include "RemoteState.h"
#include "RemoteUpdate.h"
#include "BMutex.h"



TAPCommand(TAP_Remote)
{
    BOOL valid = FALSE;

    // force SPI read
    if(strncmp(CommandLine->args[0], "read", 4) == 0)
    {
        uint16_t numBytes;
        if(CommandLine->numArgs == 1)
            numBytes = 64;
        else if(CommandLine->numArgs == 2)
            numBytes = TAP_DecimalArgToInt(CommandLine, 1, &valid);
        TAP_Printf("Reading %d bytes...\n", numBytes);
        void* data = BufferManagerGetBuffer(numBytes);
        debug_assert(data);
        memset(data, 0, numBytes);
        BOOL ret = Remote_SPI_Read(data, numBytes);
        printData(data, numBytes);
        BufferManagerFreeBuffer(data);
    }

    // reset
    else if(strncmp(CommandLine->args[0], "reset", 5) == 0)
    {
        if(CommandLine->numArgs == 2)
        {
            if(strncmp(CommandLine->args[1], "hard", 4) == 0)
            {
                // use hardware pin
                Remote_HardReset();
            }
            else if(strncmp(CommandLine->args[1], "soft", 4) == 0)
            {
                // send SPI reset message
                Remote_SoftReset();
            }
        }
        if(strncmp(CommandLine->args[1], "assert", 6) == 0)
        {
            if(strncmp(CommandLine->args[2], "true", 4) == 0)
            {
                Remote_AssertReset(TRUE);
            }
            else  if(strncmp(CommandLine->args[2], "false", 5) == 0)
            {
                Remote_AssertReset(FALSE);
            }
        }
    }
    
    // clearpairing
    else if(strncmp(CommandLine->args[0], "clearpairing", 12) == 0)
    {
        uint8_t result = 100;
        BOOL gotResult = Remote_ClearPairing(&result, 500);
        if(gotResult)
        {
            if(result == 0)
            {
                TAP_Printf("\nSuccessfully cleared pairing");
            }
            else
            {
                TAP_Printf("\nUnable to clear pairing");
            }
        }
        else
        {
            TAP_Printf("\nClear pairing message got no response\n");
        }
    }
    
    // startscan
    else if(strncmp(CommandLine->args[0], "startscan", 9) == 0)
    {
        uint8_t timeout = 15;
        if(CommandLine->numArgs == 2)
        {
            timeout = TAP_DecimalArgToInt(CommandLine, 1, &valid);
        }
        TAP_Printf("Starting scan with %d sec timeout...\n", timeout);
        uint8_t result = 100;
        BOOL gotResult = Remote_StartScan(&result, timeout);
        if(gotResult)
        {
            TAP_Printf("startscan: result = %d", result);
        }
        else
        {
            TAP_Printf("Timed out waiting for startscan response\n");
        }
        
    }
    
    // getversion
    else if(strncmp(CommandLine->args[0], "getversion", 10) == 0)
    {
        char buf[REMOTE_VERSION_STR_LEN + 1];
        BOOL ret = Remote_GetVersion(buf, REMOTE_VERSION_STR_LEN+1);
        if(ret)
        {
            TAP_Printf("\n%s\n", buf);
        }
        else
        {
            TAP_Printf("\nUnable to get version\n");
        }
    }

    // getstatus
    else if(strncmp(CommandLine->args[0], "getstatus", 9) == 0)
    {
        uint32_t timeout;
        if(CommandLine->numArgs == 1)
            timeout = 500;
        else if(CommandLine->numArgs == 2)
            timeout = TAP_DecimalArgToInt(CommandLine, 1, &valid);
        else
        {
            TAP_Printf("re: Too many arguments to getstatus\n");
            return;
        }
        RemoteTaskPostMsg(REMOTE_MESSAGE_SEND_GET_STATUS, 0);
        Remote_RxPktData_Status_t status;
        BOOL gotStatus = Remote_GetStatus(&status, timeout);
        if(gotStatus)
        {
#ifdef USE_REMOTE_NAMES
        // TAP_Printf buffer is only so big, so split this into a few calls
        TAP_Printf("Received status from remote:\n  type: %s\n  scanResult: %s\n",
            Remote_HIDName(status.type), Remote_StateName(status.state),
            Remote_ScanResultName(status.scanResult) );
        TAP_Printf("  gapState: %s\n  gapResult: %s\n",
            Remote_GapStateName(status.gapState),
            Remote_GapScanResultName(status.gapScanResult) );
        TAP_Printf("  gattState: %s\n  gattDiscoverResult: %s\n",
            Remote_GattStateName(status.gattState),
            Remote_GattDiscoverResultName(status.gattDiscoverResult) );
#else
        // Not using names, so just print integer values instead
        TAP_Printf("Received status from remote:\n  type: %d\n  scanResult: %d\n",
            status.type, status.state, status.scanResult );
        TAP_Printf("  gapState: %d\n  gapScanResult: %d\n",
            status.gapState, status.gapScanResult );
        TAP_Printf("  gattState: %d\n  gattDiscoverResult: %d\n",
            status.gattState, status.gattDiscoverResult );
#endif
        }
        else
        {
            TAP_Printf("Timed out waiting for GetStatus response\n");
        }
    } // getstatus
    
    // test post message
    else if(strncmp(CommandLine->args[0], "post", 4) == 0)
    {
        uint8_t num1 = 50;
        uint8_t num2 = 21;
        RemoteTaskPostMsg(REMOTE_MESSAGE_TEST, (uint32_t)&num1, (uint32_t)&num2);
    }
    
    // Update commands
    else if(strncmp(CommandLine->args[0], "update", 6) == 0)
    {
        if(strncmp(CommandLine->args[1], "start", 9) == 0)
        {
            Remote_Update_Start();
        }
        if(strncmp(CommandLine->args[1], "enter", 5) == 0)
        {
            TAP_Printf("Moving to update state..\n");
            RemoteTaskPostMsg(REMOTE_MESSAGE_GO_TO_UPDATE, 0);
        }
        else if(strncmp(CommandLine->args[1], "exit", 3) == 0)
        {
            TAP_Printf("Moving to running state..\n");
            Remote_changeState(REMOTE_STATE_RUNNING);
        }
        else if(strncmp(CommandLine->args[1], "getstatus", 9) == 0)
        {
            uint8_t buf[3];
            Remote_Update_GetStatus(buf);
        }
        else if(strncmp(CommandLine->args[1], "ping", 4) == 0)
        {
            TAP_Printf("Sending ping..\n");
            Remote_Update_Ping();
        }
        else if(strncmp(CommandLine->args[1], "writeping", 9) == 0)
        {
            TAP_Printf("Sending ping..\n");
            uint8_t buf[3];
            buf[0] = 3;
            buf[1]= REMOTE_UPDATE_COMMAND_PING;
            buf[2] = REMOTE_UPDATE_COMMAND_PING;
            Remote_SPI_Write(buf, 3);
        }
    }
    
    // test command
    else if(strncmp(CommandLine->args[0], "test", 4) == 0)
    {
        //TAP_Printf("Test TAP command!\n");
        //-----------------------------------
        /*
        TAP_Printf("sending semaphore\n");
        SemaphoreHandle_t sem = xSemaphoreCreateBinary();
        BMutex_Give(sem);
        BMutex_Give(sem); //same as giving only once.. as this sem is binary
        RemoteTaskPostMsg(REMOTE_MESSAGE_TEST, (uint32_t)sem);
        */
        //TAP_Printf("sending semaphore\n");
        //SemaphoreHandle_t sem = xSemaphoreCreateCounting(10, 1); 
        //RemoteTaskPostMsg(REMOTE_MESSAGE_TEST, (uint32_t)sem);
        //uint8_t buf[] = {0x54, 0x28, 0x9c, 0x13};
        //TAP_Printf("\n0x%x\n", generateChecksum(buf, 4));
    }
    
    // default
    else
    {
        TAP_Printf("Invalid argument\nTry \"help re\" to display options\n");
    }
}