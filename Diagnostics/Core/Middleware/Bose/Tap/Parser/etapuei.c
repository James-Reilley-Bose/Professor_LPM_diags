/**
  ******************************************************************************
  * @file    etapuei.c
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   TAP interface.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the UEI:
  *           + TAP interface for UEI functionality  
 @verbatim
 ===============================================================================
 **/

#include "project.h"
#include "stdio.h"
#include "etapuei.h"
#include "etap.h"
#include "TapListenerAPI.h"
#include "BufferManager.h"
#include "UEIServices.h"
#include "UEITask.h"
#include "UEIDefinitions.h"

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

  

/*
===============================================================================
@fn TAP_UEIBlasterCommand
@brief Implementation for the UEI related TAP functionality
===============================================================================
*/
TAPCommand(TAP_UEIBlasterCommand)
{
    BOOL valid;
    uint8_t *blasterMsg;
    uint16_t length;
    uint8_t *msgPtr; 
    
            
    if (CommandLine->numArgs < MINIMAL_UEIBLASTER_NUMBER_ARGUMENTS)
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }
    
    // Is it an end key (key release)?
    if (TAP_HexArgToInt(CommandLine, 0, &valid) == 0)
    {
       UEIBlasterPostMsg(UEIBLASTER_MESSAGE_ID_KeyRelease, NOP_CALLBACK , NULL); 
       return;       
    }

    length = CommandLine->numArgs;

    // The download device to fdra command (0x22) is an exception because it will download the 
    // T2051 codeset hardcoded above.  Otherwise the message length is the number of arguments
    if (TAP_HexArgToInt(CommandLine, 0, &valid) == UEI_DWNLD_CODE_SET_ID)
    {
        length += sizeof(T2051_code_set);
    }
      
    blasterMsg = BufferManagerGetBuffer(length+2);  // Add two for the length bytes
    debug_assert(blasterMsg);
    msgPtr = blasterMsg;

    // Add the length to the front of the message
    *msgPtr++ = (length & 0xFF00)>>8;
    *msgPtr++ = length & 0x00FF;

    // Copy the rest of the TAP bytes into the buffer
    for (int i=0; i<CommandLine->numArgs; i++)
    {
        *msgPtr++ = TAP_HexArgToInt(CommandLine, i, &valid);
    }
    
    // If download codeset message, copy the message to the buffer
    if (TAP_HexArgToInt(CommandLine, 0, &valid) == UEI_DWNLD_CODE_SET_ID)
    {
        memcpy(msgPtr, T2051_code_set, sizeof(T2051_code_set));
    }
    UEIBlasterPostMsg(UEIBLASTER_MESSAGE_ID_SendFromTAP, BufferManagerFreeBufferCallback ,(uint32_t)blasterMsg, length+2);
}





