/**
  ******************************************************************************
  * @file    Diag_ir_blaster_test.c
  * @brief   Diag test interface.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the IR blaster:
  *           + Diag test interface for IR blaster functionality  
 ===============================================================================
 **/
 
#include "project.h"
#include "stdio.h"
#include "etap.h"
#include "TapListenerAPI.h"
#include "UEIServices.h"
#include "UEITask.h"
#include "UEIDefinitions.h"
#include "Diag_ir_blaster_test.h"

SCRIBE_DECL(diag);

static BOOL callBackDone = FALSE;

static const uint8_t T2051_code_set[]=
{
0x00, 0x02, 0x84, 0x3D, 0x4B, 0xB3, 0x1C, 0xBA, 0x48, 0x6B, 0x37, 0x35, 0xE1, 0x4A, 0xDB, 0x35, 
0x5B, 0xF6, 0x7E, 0x43, 0x0A, 0x51, 0x3E, 0x17, 0x01, 0xEA, 0xEA, 0x75, 0x21, 0x8D, 0x4F, 0x9F, 
0x60, 0x61, 0x3D, 0x77, 0x67, 0x04, 0x51, 0x70, 0x4C, 0x1D, 0x4B, 0x96, 0x5E, 0xEC, 0xE3, 0x23, 
0xFD, 0x6C, 0x76, 0xA5, 0x3B, 0x39, 0x92, 0x99, 0x1C, 0xEF, 0xF4, 0x41, 0x64, 0x1A, 0x58, 0x0B, 
0x03, 0xC4, 0xF6, 0x69, 0x66, 0xDC, 0x2C, 0xFE, 0x82, 0x73, 0x14, 0x05, 0xAA, 0x99, 0x44, 0x74, 
0x08, 0xF1, 0x3B, 0x06, 0xB5, 0x58, 0xDA, 0x1F, 0x58, 0xAE, 0x17, 0x04, 0xED, 0x30, 0xED, 0xE5, 
0x83, 0x93, 0xE3, 0xCF, 0xE7, 0x48, 0x68, 0x95, 0xF0, 0x8C, 0x01, 0xAD, 0x2F, 0xC1, 0x09, 0x5A, 
0x56, 0x1F, 0x8C, 0x61, 0x15, 0x4D, 0x44, 0x78, 0x7A, 0xF1, 0xD9, 0x65, 0x38, 0x95, 0xAD, 0x17, 
0xCA, 0x8B, 0xFD, 0x77, 0x28, 0xDE, 0x3A, 0x67, 0x31, 0xE3, 0x62, 0x26, 0x2F, 0xF2, 0xC2, 0xCB, 
0x95, 0x4C, 0xF9, 0x5B, 0xFD, 0xD5, 0x7B, 0xD6, 0xC2, 0x2B, 0x8A, 0x7F, 0x73, 0x0B, 0x32, 0xC9, 
0x7A, 0xA8, 0xC9, 0x3D, 0x2A, 0xE4, 0x1E, 0xD4, 0x2A, 0x01, 0x67, 0x38, 0x6D, 0xD3, 0xF7, 0xEF, 
0x09, 0x99, 0xF5, 0xAB, 0x5A, 0x91, 0xC2, 0x8F, 0xBB, 0x27, 0xC8, 0x65, 0x11, 0x27, 0xD6, 0x38, 
0xDF, 0x6F, 0x76, 0x8D, 0xC4, 0xD3, 0xEE, 0x8A, 0x4B, 0x66, 0xF5, 0xAB, 0x5A, 0x91, 0xC2, 0x8F, 
0xBB, 0x27, 0xC8, 0x65, 0x11, 0x27, 0xD6, 0x38, 0xDF, 0x6F, 0x97, 0x3A, 0x4C, 0xF7, 0xC6, 0xE2, 
0x92, 0x0C, 0x2D, 0xD3, 0xCC, 0xF7, 0x70, 0xD2, 0x0C, 0xD4, 0xD3, 0x8C, 0x7D, 0xB8, 0xDB, 0xAE, 
0x8A, 0xAA, 0x53, 0x31, 0xCC, 0x52, 0x87, 0x74, 0xBD, 0xB4, 0xB5, 0xEB, 0x5A, 0x94, 0x71, 0x7A, 
0xA6, 0xE5, 0x0D, 0x8E, 0x62, 0x5F, 0x41, 0xEE, 0xB7, 0x02, 0x60, 0xF8, 0x56, 0xD2, 0xC4, 0x5D, 
0x11, 0x76, 0xC3, 0x46, 0xC8, 0x1B, 0xC7, 0xDE, 0x9C, 0x81, 0x13, 0x73, 0xB5, 0xD2, 0x20, 0x93, 
0xD3, 0xBC, 0xF7, 0x7F, 0x10, 0xDA, 0x9F, 0xAE, 0xAB, 0xA1, 0x89, 0xBF, 0x7D, 0x0F, 0xF2, 0xDD, 
0xE2, 0x0E, 0x0E, 0x83, 0x24, 0xF5, 0x3B, 0x3C, 0xC9, 0x01, 0x16, 0x06, 0x11, 0xE0, 0x3C, 0x83, 
0x42, 0xBA, 0x89, 0x92, 0x1F, 0xDA, 0xFF, 0xD8, 0x5B, 0x0E, 0xC9, 0x42, 0xCF, 0x04, 0xD0, 0xA6, 
0x08, 0xF2, 0x9A, 0x09, 0x24, 0x46, 0x0D, 0xF9, 0xE6, 0x0D, 0x41, 0xFF, 0x59, 0xA8, 0x56, 0x21, 
0x5C, 0xA8, 0xA4, 0x5B
};

UEIServices_Globals* g_ServicePtr;
uint8_t g_CommandArray[4];
static UEIServices_Globals tapUEIResp;

/* Local functions */
/*
===============================================================================
@fn UEIToUpper
@brief Previous TAP processing made all lower case.  The first character of the
       ID needs to be upper case for the blaster chip
===============================================================================
*/
char UEIToUpper(char c)
{
  if (c >= 'a' && c <= 'z')
  {
    c &= ~0x20;  // upper character mask
  }

  return c;
}  

/*
===============================================================================
@fn UEIBlasterTapCallback
@brief Callback function for post message.
===============================================================================
*/
void UEIBlasterTapCallback(uint32_t* params, uint8_t numParams)
{
    g_ServicePtr = (UEIServices_Globals *)params[1];

    if (g_ServicePtr->ResponseCode != UEI_NO_RESPONSE)
    {
      callBackDone = TRUE;
    }
    else
    {
      callBackDone = FALSE;  
    }
 
}

/*
===============================================================================
@fn DIAG_UEIBlasterCommand_Info
@brief Info - version, codeset, key map
===============================================================================
*/
void DIAG_UEIBlasterCommand_Info(DIAG_IR_BLASTER_COMMANDS commandType, uint32_t param)
{
    //char action;
    //BOOL valid;
    static uei_ueiMsg_t  msg;
    
    
    // Get arguments
    uint32_t timeoutMs=1000;    
    uint8_t Cnt=0, respCode=0,KeyCode=0;
    
    char codeStr[6]="t2051";
    
    uint32_t TimeoutStartTimeMs = GET_SYSTEM_UPTIME_MS();
    
    switch(commandType)
    {
    case DIAG_IR_BLASTER_VERSION:     // version 

        UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_GetSoftwareVersion, 0, (uint32_t)&tapUEIResp, UEIBlasterTapCallback);
 
        
        while(callBackDone == FALSE)
        {
            if(timeoutMs > 0)
            {                      
                if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= timeoutMs))
                {
                    break; // while
                }
            }                        
            vTaskDelay(TIMER_MSEC_TO_TICKS(1));
        } 

        if(callBackDone == FALSE)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "UEI Blaster Software Version command failed");
        }
        else
        {  
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Library Version 0x%s S/W Version 0x%s \n\r", g_ServicePtr->LibVersionString,g_ServicePtr->SwVersionString);      
          callBackDone = FALSE;
        } 
           
    
        break;

    case DIAG_IR_BLASTER_CODESET:     // list all upgrade code
 
        UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_ListAllUpgradeCode, 0, (uint32_t)&tapUEIResp, UEIBlasterTapCallback);
 
        while(callBackDone == FALSE)
        {
            if(timeoutMs > 0)
            {                      
                if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= timeoutMs))
                {
                    break; // while
                }
            }                        
            vTaskDelay(TIMER_MSEC_TO_TICKS(1));
        } 

        if(callBackDone == FALSE)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "UEI list upgrade code command failed");
        }
        else
        {
      
            //uint8_t respCode =  g_ServicePtr->ResponseCode;
            Cnt = 0;
            // LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "UEI list code set Response 0x%x\n", respCode);
            
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Code set downloaded: ");

            for (Cnt=0; Cnt<g_ServicePtr->CodesetNumber; Cnt++)
            {
              LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "0x%04x ", g_ServicePtr->CodesetStringPtr[Cnt]);
            }
            
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\n\r");
            callBackDone = FALSE;
        }
        break;
   
    case DIAG_IR_BLASTER_DOWNLOAD_CODESET:     // Download code set      
            
        strncpy((char *)msg.deviceStr, codeStr, UEI_KEY_RECORD_SIZE);
        // TAP code makes everything lower case.  The blaster chip needs the first 
        // character to be upper case 
        msg.deviceStr[0] = UEIToUpper(codeStr[0]);
        msg.codeID = UEIDeviceStringToDeviceType("T2051");
        msg.dataPtr = (uint8_t*)T2051_code_set;
        msg.dataSize = sizeof(T2051_code_set);
 
        UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_DownloadCodeSet, (uint32_t )&msg, (uint32_t)&tapUEIResp, UEIBlasterTapCallback);
 
        while(callBackDone == FALSE)
        {
            if(timeoutMs > 0)
            {                      
                if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= timeoutMs))
                {
                    break; // while
                }
            }                        
            vTaskDelay(TIMER_MSEC_TO_TICKS(1));
        } 

        if(callBackDone == FALSE)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "UEI Blaster Download Codeset command failed");
        }
        else
        {    
            respCode =  g_ServicePtr->ResponseCode;
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Download Codeset Response 0x%x\n", respCode);
            callBackDone = FALSE;
        }         
        
        break;
    case DIAG_IR_BLASTER_SEND_KEY:
         strncpy((char *)msg.deviceStr, codeStr, UEI_KEY_RECORD_SIZE);
        // TAP code makes everything lower case.  The blaster chip needs the first 
        // character to be upper case 
        msg.deviceStr[0] = UEIToUpper(msg.deviceStr[0]);
        msg.codeID = UEIDeviceStringToDeviceType(msg.deviceStr);
        if (!msg.codeID)
        {
          LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Invalid CodeSet ID %s", msg.deviceStr);
          break;
        }
       
        KeyCode = (uint8_t)param;    
        msg.dataPtr = &KeyCode;
        msg.dataSize = 1;
       
        UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_SendKey, (uint32_t)&msg, (uint32_t)&tapUEIResp, UEIBlasterTapCallback);        
        
        while(callBackDone == FALSE)
        {
            if(timeoutMs > 0)
            {                      
                if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= timeoutMs))
                {
                    break; // while
                }
            }                        
            vTaskDelay(TIMER_MSEC_TO_TICKS(1));
        } 

        if(callBackDone == FALSE)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "UEI Blaster Send Key, continuous IR started");
        }
        else
        {    
            respCode =  g_ServicePtr->ResponseCode;
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "SendKey Response 0x%x\n", respCode);
            callBackDone = FALSE;
        }         
        break;
    case DIAG_IR_BLASTER_SEND_KEY_END:
       UEIBlasterPostMsg2(UEIBLASTER_MESSAGE_ID_EndContinuousKey, 0, (uint32_t)&tapUEIResp, UEIBlasterTapCallback);
        while(callBackDone == FALSE)
        {
            if(timeoutMs > 0)
            {                      
                if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= timeoutMs))
                {
                    break; // while
                }
            }                        
            vTaskDelay(TIMER_MSEC_TO_TICKS(1));
        } 

        if(callBackDone == FALSE)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "UEI Blaster SendKey command failed");
        }
        else
        {    
            respCode =  g_ServicePtr->ResponseCode;
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "SendKey Response 0x%x\n", respCode);
            callBackDone = FALSE;
        }         
        break;     
    default:
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Invalid Info options");
        return;
    }

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n");
}
