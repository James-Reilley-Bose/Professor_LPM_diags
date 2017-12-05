
#include "TaskDefs.h"
#include "DiagTask.h"
#include "IPCRouterTask.h"
#include "IpcInterface.h"
#include "DeviceInterface.h"
#include "f0_cmd_interface.h"
#include "etapF0.h"
#include "stdio.h"
#include "stdlib.h"
#include "Diag_psoc_test.h"
#include "Diag_ir_blaster_test.h"
#include "UEIServices.h"
#include "UEITask.h"
#include "DiagsDispatcher.h"
#include "serialPSOCListener.h"
#include "IpcStatus.h"

SCRIBE_DECL(diag);

typedef enum {
  DIAGS_BUS_TEST_DSP_STATE_START = 0x00,
  DIAGS_BUS_TEST_DSP_STATE_SEND,
  DIAGS_BUS_TEST_DSP_STATE_WAIT,
  DIAGS_BUS_TEST_DSP_STATE_RESPONSE,
  DIAGS_BUS_TEST_DSP_STATE_INVALID,
} DIAGS_BUS_TEST_DSP_STATE;

typedef enum {
  DIAGS_BUS_TEST_F0_STATE_START = 0x00,
  DIAGS_BUS_TEST_F0_STATE_SEND,
  DIAGS_BUS_TEST_F0_STATE_WAIT,
  DIAGS_BUS_TEST_F0_STATE_RESPONSE,
  DIAGS_BUS_TEST_F0_STATE_INVALID,
} DIAGS_BUS_TEST_F0_STATE;

typedef enum {
  DIAGS_BUS_TEST_PSOC_STATE_START = 0x00,
  DIAGS_BUS_TEST_PSOC_STATE_SEND,
  DIAGS_BUS_TEST_PSOC_STATE_WAIT,
  DIAGS_BUS_TEST_PSOC_STATE_RESPONSE,
  DIAGS_BUS_TEST_PSOC_STATE_INVALID,
} DIAGS_BUS_TEST_PSOC_STATE;
typedef enum {
  DIAGS_BUS_TEST_IRB_STATE_START = 0x00,
  DIAGS_BUS_TEST_IRB_STATE_SEND,
  DIAGS_BUS_TEST_IRB_STATE_RESPONSE,
  DIAGS_BUS_TEST_IRB_STATE_INVALID,
} DIAGS_BUS_TEST_IRB_STATE;

typedef enum {
  DIAGS_BUS_TEST_DEST_INVALID = 0x00,
  DIAGS_BUS_TEST_DEST_DSP,
  DIAGS_BUS_TEST_DEST_F0,
  DIAGS_BUS_TEST_DEST_PSOC,
  DIAGS_BUS_TEST_DEST_IRB,
  DIAGS_BUS_TEST_DEST_ALL,
} DIAGS_BUS_TEST_DEST;

static DIAGS_BUS_TEST_DSP_STATE bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_INVALID ;
static DIAGS_BUS_TEST_F0_STATE bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_INVALID ;
static DIAGS_BUS_TEST_DEST bus_test_dest = DIAGS_BUS_TEST_DEST_INVALID ;
static DIAGS_BUS_TEST_PSOC_STATE bus_test_psoc_state= DIAGS_BUS_TEST_PSOC_STATE_INVALID;
static DIAGS_BUS_TEST_IRB_STATE bus_test_irb_state= DIAGS_BUS_TEST_IRB_STATE_INVALID;

static BOOL diag_BusTestDSPResponded = FALSE;
static BOOL diag_BusTestF0Responded = FALSE;
static BOOL diag_BusTestDSPEnabled = FALSE;
static BOOL diag_BusTestF0Enabled = FALSE;
BOOL diag_BusTestPSOCEnabled = FALSE;
static BOOL diag_BusTestIRBEnabled = FALSE;
static uint32_t diag_BusTestDSP_TestCnt = 0;
static uint32_t diag_BusTestDSP_OKCnt = 0;
static uint32_t diag_BusTestDSP_FailCnt = 0;
static uint32_t diag_BusTestF0_TestCnt = 0;
static uint32_t diag_BusTestF0_OKCnt = 0;
static uint32_t diag_BusTestF0_FailCnt = 0;
static uint32_t diag_BusTestPSOC_TestCnt = 0;
static uint32_t diag_BusTestPSOC_OKCnt = 0;
static uint32_t diag_BusTestPSOC_FailCnt = 0;
static uint32_t diag_BusTestIRB_TestCnt = 0;
static uint32_t diag_BusTestIRB_OKCnt = 0;
static uint32_t diag_BusTestIRB_FailCnt = 0;

BOOL bustest_stringToDestination(char* str, DIAGS_BUS_TEST_DEST* destination)
{
    if (strcmp("dsp", str) == 0)
    {
        *destination = DIAGS_BUS_TEST_DEST_DSP;
        return TRUE;
    }
    else if (strcmp("f0", str) == 0)
    {
        *destination = DIAGS_BUS_TEST_DEST_F0;
        return TRUE;
    }
    else if (strcmp("psoc", str) == 0)
    {
        *destination = DIAGS_BUS_TEST_DEST_PSOC;
        return TRUE;
    }
    else if (strcmp("irb", str) == 0)
    {
        *destination = DIAGS_BUS_TEST_DEST_IRB;
        return TRUE;
    }
    else if (strcmp("all", str) == 0)
    {
        *destination = DIAGS_BUS_TEST_DEST_ALL;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static char dsp_hs_reference[10];
static char dsp_hs[10];
static char f0_vr_reference[45];
static char f0_vr[45];
/* packet buffer */
//static uint8_t g_f0_i2c_packet[ I2C_TX_BUFFERSIZE ];
static uint8_t bus_test_timer_cnt;

//SEND: ipc send, destination, opcode, data...
void DiagsBusTestDSPTx(void)
{
    IpcDeviceId_t dest = IPC_DEVICE_DSP;
    uint8_t data[IPC_NUM_DATA_BYTES] = { 0x00,0x00,0x00,0x05} ;
    uint8_t opcode = 0x11;
    //BOOL valid;
    IPCRouter_Send(dest, opcode, NULL, data, sizeof(data));
}

void Diags_HandleIPCMessage_DSP_H_S(IpcPacket_t* packet)
{
    memcpy(&dsp_hs, packet->s.data.b, sizeof(dsp_hs));
    diag_BusTestDSPResponded = TRUE;
}

void Diags_HandleIPCMessage_F0_H_S(IpcPacket_t* packet)
{
    memcpy(&f0_vr, packet->s.data.b, sizeof(f0_vr));
    diag_BusTestF0Responded = TRUE;
}

#if 0
//-------------------------------------------------------------------
// Function:  BOOL f0_variant_cmd( void )
// Description: gets F0 variant and versioning information.
// Returns: TRUE==success FALSE==failure
//--------------------------------------------------------------------
BOOL DiagsBusTestF0Tx(F0_VARIANT_struct* f0_target)
{
        F0_VARIANT_struct f0 = {0};
        uint16_t pktSize = (HDR_SIZE + F0_VARIANT_struct_size);
 
        for(char i=0;i<50;i++)
        {
          f0.variant[i]=i;
        }

        
        /* cmd, etc. loaded in the pkt here */
        build_f0_packet( g_f0_i2c_packet,            
                         I2C_TX_BUFFERSIZE,          
                         (uint8_t)F0_VERSION,            
                         (uint8_t *)&f0,             
                         F0_VARIANT_struct_size );   

        if(send_cmd_and_get_response(g_f0_i2c_packet, pktSize)==FALSE)
        {
                return(FALSE);
        }

        
        /* get our variant strings (strncpy done on the F0 side) */
        memcpy( f0_target, 
                (const void *)(&g_f0_i2c_packet[ HDR_DATA ]), 
                F0_VARIANT_struct_size );

        return(TRUE);
}
#endif

static BOOL callBackDone = FALSE;
static UEIServices_Globals* g_UEIServicePtr;
static UEIServices_Globals tapUEIResp;

/*
===============================================================================
@fn UEIBlasterBusTestCallback
@brief Callback function for post message.
===============================================================================
*/
void UEIBlasterBusTestCallback(uint32_t* params, uint8_t numParams)
{
    g_UEIServicePtr = (UEIServices_Globals *)params[1];
    
    if (g_UEIServicePtr->ResponseCode != UEI_NO_RESPONSE)
    {
      callBackDone = TRUE;
    }
    else
    {
      callBackDone = FALSE;  
    }
 
}

uint32_t dsp_start;

BOOL DiagsBusTestDSPTimerHandler(uint32_t testcnt)
{
  // DSP
  BOOL result = FALSE;
  
  if (diag_BusTestDSPEnabled == TRUE)
  {
    switch (bus_test_dsp_state)
    {
      case DIAGS_BUS_TEST_DSP_STATE_START:
        memset(&dsp_hs, 0, sizeof(dsp_hs));
        memset(&dsp_hs_reference, 0, sizeof(dsp_hs_reference));
        diag_BusTestDSP_TestCnt = 0;
        diag_BusTestDSP_OKCnt = 0;
        diag_BusTestDSP_FailCnt = 0;       
        bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_SEND;
        break;
      case DIAGS_BUS_TEST_DSP_STATE_SEND:
        if (strnlen((char *)&dsp_hs_reference,  sizeof(dsp_hs_reference)) != 0)
        {
          LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "dsp_hs_reference not empty\n\r");
          diag_BusTestDSP_TestCnt++;
        }
        DiagsBusTestDSPTx();
        dsp_start = GET_INSTRUMENT_TIME_NOW();
        bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_WAIT;
        break;
      case DIAGS_BUS_TEST_DSP_STATE_WAIT:
        bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_RESPONSE;
        break;
      case DIAGS_BUS_TEST_DSP_STATE_RESPONSE:
        if (diag_BusTestDSPResponded)
        {
          if (strnlen((char *)&dsp_hs_reference,  sizeof(dsp_hs_reference)) == 0)
          {
            // get reference
            LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "dsp_hs_reference empty\n\r");
            memcpy(&dsp_hs_reference, &dsp_hs, sizeof(dsp_hs_reference));
            memset(&dsp_hs, 0, sizeof(dsp_hs));
            bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_SEND;
          }
          else
          {
            if (strncmp((char *)&dsp_hs_reference, (char *)&dsp_hs, sizeof(dsp_hs_reference)) == 0)
            {
              LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "dsp ok\n\r");
              diag_BusTestDSP_OKCnt++;
            }
            else
            {
              LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "dsp not ok\n\r");
              diag_BusTestDSP_FailCnt++;
            }  
            if ((--testcnt) == 0)
            {
                // test finish
                diag_BusTestDSPEnabled  = FALSE;
            }
            else 
            {
              memset(&dsp_hs, 0, sizeof(dsp_hs));
              bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_SEND;
            }
            result = TRUE;
          }
          diag_BusTestDSPResponded = FALSE;
        }
        else if (MEASURE_TIME_SINCE(dsp_start)>1000)
        {
          LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "dsp response timeout\n\r");
          bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_SEND;
          diag_BusTestDSP_FailCnt++;
          result = TRUE;
          if ((--testcnt) == 0)
          {
            // test finish
            diag_BusTestDSPEnabled  = FALSE;
          }
        }
        break;
      default:
        LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "dsp invalid response\n\r");
        break;
    }
  }
  return result;
}

uint32_t f0_start;

BOOL DiagsBusTestF0TimerHandler(uint32_t testcnt)
{
  //F0
  BOOL result = FALSE;
  if (diag_BusTestF0Enabled == TRUE)
  {
    switch (bus_test_f0_state)
    {
      case DIAGS_BUS_TEST_F0_STATE_START:
        memset(&f0_vr, 0, sizeof(f0_vr));
        memset(&f0_vr_reference, 0, sizeof(f0_vr_reference));
        diag_BusTestF0_TestCnt = 0;
        diag_BusTestF0_OKCnt = 0;
        diag_BusTestF0_FailCnt = 0;
        bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_SEND;
        break;
      case DIAGS_BUS_TEST_F0_STATE_SEND:
        if (strnlen((char *)&f0_vr_reference,  sizeof(f0_vr_reference)) != 0)
        {
          LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "f0_vr_reference not empty\n\r");
          diag_BusTestF0_TestCnt++;
        }
        IpcStatus_SendHealthAndStatusRequest(IPC_DEVICE_F0, F0_STATUS);
        f0_start = GET_INSTRUMENT_TIME_NOW();
        bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_WAIT;
        break;
      case DIAGS_BUS_TEST_F0_STATE_WAIT:
        bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_RESPONSE;
        break;
      case DIAGS_BUS_TEST_F0_STATE_RESPONSE:
        if (diag_BusTestF0Responded)
        {
          if (strnlen((char *)&f0_vr_reference,  sizeof(f0_vr_reference)) == 0)
          {
            // get reference
            LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "f0_vr_reference empty\n\r");
            memcpy(&f0_vr_reference, &f0_vr, sizeof(f0_vr_reference));
            memset(&f0_vr, 0, sizeof(f0_vr));
            bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_SEND;
          }
          else
          {
              if (strncmp((char *)&f0_vr_reference, (char *)&f0_vr, sizeof(f0_vr_reference)) == 0)
              {
                  LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "f0 ok\n\r");
                  diag_BusTestF0_OKCnt++;
              }
              else
              {
                  LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "f0 not ok\n\r");
                  diag_BusTestF0_FailCnt++;
              }          
              if ((--testcnt) == 0)
              {
                  // test finish
                  diag_BusTestF0Enabled  = FALSE;
              }
              else
              {
                  memset(&f0_vr, 0, sizeof(f0_vr));
                  bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_SEND;
              }
              result = TRUE;
          }
          diag_BusTestF0Responded = FALSE;
        }
        else if (MEASURE_TIME_SINCE(f0_start)>1000)
        {
          LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "f0 response timeout\n\r");
          bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_SEND;
          diag_BusTestF0_FailCnt++;
          result = TRUE;
          if ((--testcnt) == 0)
          {
            // test finish
            diag_BusTestF0Enabled  = FALSE;
          }
        }
        break;
      default:
        LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "f0 invalid response\n\r");
        break;
    }
  }
  return result;
}

BOOL DiagsBusTestPSOCTimerHandler(uint32_t testcnt)
{
  // psoc
  BOOL result = FALSE;
  
  if (diag_BusTestPSOCEnabled == TRUE)
  {
    switch (bus_test_psoc_state)
    {
      case DIAGS_BUS_TEST_PSOC_STATE_START:
        diag_BusTestPSOC_TestCnt = 0;
        diag_BusTestPSOC_OKCnt = 0;
        diag_BusTestPSOC_FailCnt = 0;
        //psoc_reset();
        #if 0
        if (ERROR == psoc_i2c_init())
        {
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc i2c int Failed");
          return result;
        }
        #endif
        //psoc_enable_int();
        bus_test_psoc_state = DIAGS_BUS_TEST_PSOC_STATE_SEND;
        break;
      case DIAGS_BUS_TEST_PSOC_STATE_SEND:
        diag_BusTestPSOC_TestCnt++;
        psoc_cmd[0] = GetVersion_cmd_size;
        psoc_cmd[1] = GetVersion_cmd;
        DiagHandlerPostMsg(DIAG_MESSAGE_ID_PSOC_CMD, NULL);
        bus_test_psoc_state = DIAGS_BUS_TEST_PSOC_STATE_WAIT;
        break;
    case DIAGS_BUS_TEST_PSOC_STATE_WAIT:
        bus_test_psoc_state = DIAGS_BUS_TEST_PSOC_STATE_RESPONSE;
        break;
      case DIAGS_BUS_TEST_PSOC_STATE_RESPONSE:
      #if 0
      // receive
        if (d_check_psoc_int() == TRUE )
        {
          // i2c int enabled
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PSOC int enabled");
        } 
        else 
        {
          // i2c int disabled
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PSOC int disabled");
        }
      #endif  
        if (psoc_rcv[0] == 1)
        {
            LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "Result okay");
            diag_BusTestPSOC_OKCnt++;          
        } 
        else 
        {
            LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "Result not okay");
            diag_BusTestPSOC_FailCnt++;
        }
        if ((--testcnt) == 0)
        {
            // test finish
            diag_BusTestPSOCEnabled  = FALSE;
        } 
        else 
        {
            memset(&psoc_rcv, 0, sizeof(psoc_rcv));
            bus_test_psoc_state = DIAGS_BUS_TEST_PSOC_STATE_SEND;
        }
        result = TRUE;       
        break;
      default:
        break;
    }
  }
  return result;
}

BOOL DiagsBusTestIRBTimerHandler(uint32_t testcnt)
{
  // irb
  BOOL result = FALSE;
  static uint32_t TimeoutStartTimeMs;
  
  static uint8_t IRBRefLibVersionString[5];
  static uint8_t IRBRefSwVersionString[5];
  static uint8_t IRBRxLibVersionString[5];
  static uint8_t IRBRxSwVersionString[5];
  
  if (diag_BusTestIRBEnabled == TRUE)
  {
    switch (bus_test_irb_state)
    {
      case DIAGS_BUS_TEST_IRB_STATE_START:
        diag_BusTestIRB_TestCnt = 0;
        diag_BusTestIRB_OKCnt = 0;
        diag_BusTestIRB_FailCnt = 0;
        g_UEIServicePtr = 0;
        memset(&IRBRefLibVersionString,0, sizeof(IRBRefLibVersionString));
        memset(&IRBRefSwVersionString,0, sizeof(IRBRefSwVersionString));
        memset(&IRBRxLibVersionString,0, sizeof(IRBRxLibVersionString));
        memset(&IRBRxSwVersionString,0, sizeof(IRBRxSwVersionString));
        bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_SEND;
        break;
      case DIAGS_BUS_TEST_IRB_STATE_SEND:
        if ( (strnlen((char *)&IRBRefLibVersionString, sizeof(IRBRefLibVersionString)) !=0)
              ||  (strnlen((char *)&IRBRefSwVersionString, sizeof(IRBRefSwVersionString)) !=0) ) 
        {
          diag_BusTestIRB_TestCnt++;
        }
        TimeoutStartTimeMs = GET_SYSTEM_UPTIME_MS();
        UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_GetSoftwareVersion, 0, (uint32_t)&tapUEIResp, UEIBlasterBusTestCallback);
        bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_RESPONSE;
        break;
      case DIAGS_BUS_TEST_IRB_STATE_RESPONSE:
        if (callBackDone == FALSE)
        {
          // not responded
          if(GET_MILLI_SINCE(TimeoutStartTimeMs) >= 1000 /*timeoutMs*/)
          {
            //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Timeout");
            //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%d",GET_MILLI_SINCE(TimeoutStartTimeMs));
            diag_BusTestIRB_FailCnt++;
            bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_SEND;
            if ((--testcnt) == 0)
            {
              // test finish
              diag_BusTestIRBEnabled  = FALSE;
            }
            break;
          }
          //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Not yet Timeout");
          //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%d",GET_MILLI_SINCE(TimeoutStartTimeMs));
          
        }
        else
        {
          //responded 
          if ( (strnlen((char *)&IRBRefLibVersionString, sizeof(IRBRefLibVersionString)) ==0)
              ||  (strnlen((char *)&IRBRefSwVersionString, sizeof(IRBRefSwVersionString)) ==0) ) 
          {
            memcpy( IRBRefLibVersionString, &g_UEIServicePtr->LibVersionString, sizeof(IRBRefLibVersionString));
            memcpy( IRBRefSwVersionString, &g_UEIServicePtr->SwVersionString, sizeof(IRBRefSwVersionString));
            bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_SEND;
          } 
          else 
          {
            memcpy( IRBRxLibVersionString, &g_UEIServicePtr->LibVersionString, sizeof(IRBRxLibVersionString));
            memcpy( IRBRxSwVersionString, &g_UEIServicePtr->SwVersionString, sizeof(IRBRxSwVersionString));
            if ( (strcmp((char const *)IRBRxLibVersionString , (char const *)IRBRefLibVersionString) == 0)
                && (strcmp((char const *)IRBRxSwVersionString , (char const *)IRBRefSwVersionString) == 0 ) ) 
            {
                  // ok
                  diag_BusTestIRB_OKCnt++;
            } else {
                  diag_BusTestIRB_FailCnt++;
            }
            
            //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Library Version 0x%s S/W Version 0x%s \n\r", g_UEIServicePtr->LibVersionString,g_UEIServicePtr->SwVersionString);
            //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "%d",GET_MILLI_SINCE(TimeoutStartTimeMs));
            callBackDone = FALSE;
            if ((--testcnt) == 0)
            {
              // test finish
              diag_BusTestIRBEnabled  = FALSE;
              bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_INVALID;
            }
            else
            {
              bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_SEND;
            }
            result = TRUE;
          }
        }
        break;
      default:
        break;
    }
  }
  return result;
}

BOOL DiagsBusTestDisplay(void)
{
  BOOL result = TRUE;
  bus_test_timer_cnt++;
  if ( (bus_test_timer_cnt % 10) == 0)
  {
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Bus Test");
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "dsp  tested: %d ok:%d ,fail:%d",diag_BusTestDSP_TestCnt, diag_BusTestDSP_OKCnt, diag_BusTestDSP_FailCnt);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "f0  tested: %d ok:%d ,fail:%d",diag_BusTestF0_TestCnt, diag_BusTestF0_OKCnt, diag_BusTestF0_FailCnt);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc  tested: %d ok:%d ,fail:%d",diag_BusTestPSOC_TestCnt, diag_BusTestPSOC_OKCnt, diag_BusTestPSOC_FailCnt);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "irb  tested: %d ok:%d ,fail:%d",diag_BusTestIRB_TestCnt, diag_BusTestIRB_OKCnt, diag_BusTestIRB_FailCnt);

    if ( !diag_BusTestDSPEnabled && !diag_BusTestF0Enabled && !diag_BusTestPSOCEnabled && !diag_BusTestIRBEnabled)
    {
      result = FALSE;
    }
  }
  return result;
}

void Diag_bus_test_command (CommandLine_t *CommandLine, uint32_t  *arg, BOOL *valid)
{
    
    BOOL dest_valid;
    BOOL Enabled = FALSE;
    uint32_t repetition = 0;
    if(CommandLine->numArgs != 4)
    {
      TAP_PrintString("dt help for help\n\r");
      return;
    }
    
    dest_valid = bustest_stringToDestination(CommandLine->args[1], &bus_test_dest);
    if (dest_valid == FALSE)
    {
        TAP_PrintString("dt help for help\n\r");
        return;
    }
    // on/off
    if (strcmp(CommandLine->args[2], "on") == 0)
    {
      /* enabled */
      //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Enable");
      Enabled = TRUE;
      
      // repetition
      repetition = atoi(CommandLine->args[3]);
      
      if (repetition == 0)
      {
        // no limit
        repetition = 0xffffffff;
      }
    }
    else if (strcmp(CommandLine->args[2], "off") == 0)
    {
      /* disable */
      //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Disable");
      Enabled = FALSE;
      
    }
    else
    {
      TAP_PrintString("dt help for help\n\r");
      return;
    }
    
    
    if (bus_test_dest == DIAGS_BUS_TEST_DEST_DSP)
    {
        //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "DSP");
        if (diag_BusTestDSPEnabled != Enabled)
        {
          // current off
          if ((Enabled == TRUE))
          {
            // restart the sequence
            bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_START;
            DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_DSP, (uint32_t *)&repetition);
          }
          
          diag_BusTestDSPEnabled = Enabled;
        }
    }
    else if (bus_test_dest == DIAGS_BUS_TEST_DEST_F0)
    {
        //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "F0");
        if (diag_BusTestF0Enabled != Enabled)
        {
          // current off
          if (Enabled == TRUE)
          {
            // restart the sequence
            bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_START;
            DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_F0, (uint32_t *)&repetition);
          }
          diag_BusTestF0Enabled = Enabled;
        }
    }
    else if (bus_test_dest == DIAGS_BUS_TEST_DEST_PSOC)
    {
        //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "PSOC");
        if (diag_BusTestPSOCEnabled != Enabled)
        {
          // current off
          if (Enabled == TRUE)
          {
            // restart the sequence
            bus_test_psoc_state = DIAGS_BUS_TEST_PSOC_STATE_START;
            DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_PSOC, (uint32_t *)&repetition);
          }
          diag_BusTestPSOCEnabled = Enabled;
        }
    }
    else if (bus_test_dest == DIAGS_BUS_TEST_DEST_IRB)
    {
        //LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "IRB");
        if (diag_BusTestIRBEnabled != Enabled)
        {
          // current off
          if (Enabled == TRUE)
          {
            // restart the sequence
            bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_START;
            DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_IRB, (uint32_t *)&repetition);
          }
          diag_BusTestIRBEnabled = Enabled;
        }
    }
    else if (bus_test_dest == DIAGS_BUS_TEST_DEST_ALL)
    {
        if (Enabled == TRUE)
        {
          // restart the sequence
          bus_test_dsp_state = DIAGS_BUS_TEST_DSP_STATE_START;
          bus_test_f0_state = DIAGS_BUS_TEST_F0_STATE_START;
          bus_test_psoc_state = DIAGS_BUS_TEST_PSOC_STATE_START;
          bus_test_irb_state = DIAGS_BUS_TEST_IRB_STATE_START;
          DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_DSP, (uint32_t *)&repetition);
          DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_F0, (uint32_t *)&repetition);
          DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_PSOC, (uint32_t *)&repetition);
          DiagHandlerPostMsg(DIAG_MESSAGE_ID_BUSTEST_IRB, (uint32_t *)&repetition);
        }
        
        diag_BusTestDSPEnabled = Enabled;
        diag_BusTestF0Enabled = Enabled;
        diag_BusTestPSOCEnabled = Enabled;
        diag_BusTestIRBEnabled = Enabled;
        
    }
    else
    {
        TAP_PrintString("dt help for help\n\r");
    }
}