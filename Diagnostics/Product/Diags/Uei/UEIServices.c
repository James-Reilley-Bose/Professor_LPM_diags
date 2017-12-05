/**
  ******************************************************************************
  * @file    UEIServices.c
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   UEI service layer.
  *          This file provides firmware functions to manage the following
  *          functionalities of the UEI:
  *           + Services offered by the UEI BLASTER
 @verbatim
 ===============================================================================
 **/
#include "UEIDefinitions.h"
#include "UEISerial.h"
#include "UEITask.h"
#include "UEIServices.h"
#include "UEIConfig.h"
#include "buffermanager.h"

SCRIBE_DECL(uei_service);

extern uint8_t g_CommandArray[4];

#if (USED_IN_TEST==1) // Used only code set verification, not in the production software.
/*
static uint8_t N2049_code_set[] =
{
0x00, 0x02, 0x84, 0x3D, 0x4B, 0xB3, 0x1C, 0xBA, 0x48, 0x6B, 0x37, 0x35, 0xE1, 0x4A, 0xDB, 0x35,
0x5B, 0xF6, 0x25, 0xFF, 0xF9, 0x12, 0x53, 0x3C, 0xB8, 0xDD, 0xBA, 0xB1, 0x91, 0xF2, 0xBC, 0xDF,
0xBA, 0xC2, 0xD3, 0x9E, 0xEC, 0xA9, 0x37, 0x27, 0xA2, 0x27, 0xFD, 0xFE, 0x67, 0xAD, 0xD6, 0x0C,
0x43, 0xB1, 0x37, 0x41, 0xD5, 0x5E, 0x4A, 0x83, 0x47, 0x8E, 0xDA, 0x6F, 0x61, 0xAC, 0x8F, 0x21,
0x7A, 0x6E, 0xD8, 0x6F, 0xDC, 0xEE, 0xDB, 0x10, 0x4D, 0x4A, 0xFE, 0xE5, 0x25, 0x22, 0x29, 0xF8,
0x52, 0xBF, 0xA1, 0xAE, 0x2F, 0x8E, 0xE5, 0xC2, 0xA7, 0xA3, 0x98, 0xAA, 0xB4, 0x89, 0xE5, 0x93,
0xD2, 0xC8, 0x02, 0x1F, 0x46, 0x0D, 0x46, 0x26, 0x3E, 0xB4, 0x57, 0x5C, 0x78, 0xD5, 0x9B, 0x72,
0x8B, 0xFF, 0x13, 0xE4, 0xFB, 0x03, 0x0D, 0x65, 0xA1, 0xD9, 0x20, 0x06, 0x0F, 0x50, 0x08, 0x72,
0x07, 0xBE, 0x0F, 0x07, 0x62, 0x3B, 0x2B, 0x10, 0x75, 0xCE, 0x71, 0xA6, 0x43, 0xD0, 0xB3, 0xED,
0x83, 0x6D, 0x1A, 0x43, 0x72, 0xD9, 0x10, 0xCC, 0x8D, 0xEC, 0xE3, 0x71, 0xB2, 0x9E, 0xC2, 0xED,
0xD1, 0x6F, 0xDD, 0x3C, 0xB4, 0xCD, 0xE2, 0x43, 0x9B, 0x68, 0xDB, 0x24
};
*/

/*
static uint8_t Y0199_code_set[]=
{
0x00, 0x02, 0x84, 0x3d, 0x4b, 0xb3, 0x1c, 0xba, 0x48, 0x6b, 0x37, 0x35, 0xe1, 0x4a, 0xdb, 0x35,
0x5b, 0xf6, 0x08, 0x90, 0x5b, 0x87, 0xde, 0x03, 0xe8, 0xf1, 0x6b, 0x25, 0xee, 0xf7, 0xe2, 0xdf,
0x14, 0xc0, 0x64, 0xd3, 0x5c, 0x42, 0x8d, 0x5c, 0x53, 0x1c, 0xa3, 0xd1, 0xcc, 0x16, 0xcb, 0xa7,
0x29, 0x38, 0x0f, 0xae, 0x04, 0xb3, 0x28, 0x84, 0x46, 0xfa, 0xda, 0x6f, 0x61, 0xac, 0x8f, 0x21,
0x7a, 0x6e, 0x0c, 0xd1, 0xc7, 0x02, 0x3f, 0x52, 0x41, 0x32, 0x00, 0x1a, 0x25, 0xd1, 0xfa, 0xf7,
0x79, 0x4d, 0x6a, 0x6a, 0x92, 0xf2, 0x78, 0xd5, 0xc1, 0x11, 0x98, 0xd4, 0xe1, 0xef, 0x14, 0xc1,
0x64, 0x40, 0xf3, 0x26, 0xa4, 0x8d, 0x94, 0xaa, 0x22, 0x1b, 0x16, 0xc3, 0x9e, 0xfe, 0x74, 0xd6,
0x4a, 0x1a, 0x1b, 0xde, 0x57, 0x4e, 0xfb, 0xab, 0x9d, 0x9e, 0x65, 0xdf, 0xb9, 0xf5, 0x3b, 0xfe,
0x7b, 0xff, 0x99, 0x94, 0x6b, 0xb9, 0x16, 0xf1, 0x4c, 0x91, 0x65, 0xf8, 0x5b, 0x27, 0x2e, 0x52,
0x31, 0x2e, 0x00, 0x1a, 0x25, 0xd1, 0xfa, 0xf7, 0x79, 0x4d, 0xa0, 0x7f, 0xc0, 0x7f, 0x52, 0x74,
0xd8, 0x17, 0x5d, 0xf0, 0x6c, 0x53, 0x3c, 0x89, 0x88, 0xcb, 0x76, 0x89
};
*/

/*
static uint8_t T2051_code_set[]=
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
*/

#endif /* #if USED_IN_TEST */

/* local library routine */
static void SendSystemID(void);
static void PrepUEI(void);

/* Global variables */
uint8_t *g_Message = NULL;
UEIServices_Globals g_Services;
uint16_t g_CodesetString[UEI_MAX_NUMBER_OF_CODESET_IN_FDRA];
uint8_t g_KeyMapString[UEI_MAX_NUMBER_OF_KEYS_IN_KEYMAP];


/* Global functions */
/*
===============================================================================
@fn UEI_InitService
@brief Init for service
===============================================================================
*/
void UEI_InitService(void)
{
  g_Services.CodesetNumber = 0;
  g_Services.NumberOfKeysInMap = 0;
  g_Services.ResponseCode = UEI_NO_RESPONSE;
  g_Services.CodesetStringPtr = g_CodesetString;
  g_Services.KeyMapStringPtr = g_KeyMapString;
  memset(g_Services.LibVersionString, 0, sizeof(g_Services.LibVersionString));
  memset(g_Services.SwVersionString, 0, sizeof(g_Services.SwVersionString));
  memset(g_Services.CodesetStringPtr, 0, sizeof(g_CodesetString));
  memset(g_Services.KeyMapStringPtr, 0, sizeof(g_KeyMapString));
}

/*
===============================================================================
@fn WaitAndCheckUeiResponse
@brief Wait for the response from UEI blaster
===============================================================================
*/
BOOL WaitAndCheckUeiResponse (uint32_t timeOutTicks)
{
    /* wait for a complete RX packet */
    TakeUEISemaphore(timeOutTicks);

    if (g_Message != NULL)
    {
      BufferManagerFreeBuffer(g_Message);
      g_Message = NULL;
    }
    if ((g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_CODE_POS] == UEI_NO_RESPONSE) ||
        (g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_LEN_POS]+1 != g_Serial.ReceivedByteIdx))
    {
      return(FALSE);
    }
    return(TRUE);
}

/*
===============================================================================
@fn GetVersion
@brief Get the software version for the 610LP
===============================================================================
*/
void GetVersion(void)
{
    g_Services.commandID = UEI_SW_VER_ID;
    /* RcdLength(2), 0Bh */
    uint8_t message[] = UEI_GET_VERSION_COMMAND;

    PrepUEI();

    UEI_Serial_SendCommandArray(message, UEI_GET_VERSION_COMMAND_LENGTH);
}

/*
===============================================================================
@fn PopulateVersionString
@brief Put the response in to a version string and store locally
===============================================================================
*/
void PopulateVersionString(void)
{
   g_Services.LibVersionString[0] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+0];
   g_Services.LibVersionString[1] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+1];
   g_Services.LibVersionString[2] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+2];
   g_Services.LibVersionString[3] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+3];
   g_Services.LibVersionString[4] = '\0';

   g_Services.SwVersionString[0] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+4];
   g_Services.SwVersionString[1] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+5];
   g_Services.SwVersionString[2] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+6];
   g_Services.SwVersionString[3] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+7];
   g_Services.SwVersionString[4] = '\0';
}

/*
===============================================================================
@fn PopulateResponseCode
@brief Get the response code and store locally
===============================================================================
*/
void PopulateResponseCode(void)
{
   g_Services.ResponseCode = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_CODE_POS] & 0x0F;
}

/*
===============================================================================
@fn SetDefaultUEIResponse
@brief Set the local response code to an invalid UEI value to determine whether
   a UEI timeout occurred.
===============================================================================
*/
void SetDefaultUEIResponse(void)
{
   g_Services.ResponseCode = UEI_NO_RESPONSE;
}

/*
===============================================================================
@fn UEI_SendKey
@brief Send a key from code set
===============================================================================
*/
void UEI_SendKey(GENERIC_MSG_t* msg)
{
    /* RcdLength(2), 01h, DeviceType(1), CodesetNum(2), KeyCode(1), KeyFlag(1), IRCode(2), */
    /* Example: 00 07 01 03 2a 04 00 00 00 */

    uint8_t message[] = UEI_SEND_KEY_COMMAND;
    uei_ueiMsg_t *kmMsg;

    g_Services.commandID = UEI_SEND_KEY_ID;

    kmMsg = (uei_ueiMsg_t *)msg->params[0];

    g_Services.commandID = UEI_SEND_KEY_ID;
    message[UEI_GET_KEY_MAP_COMMAND_LENGTH_POS_0] = (kmMsg->codeID>>8) & 0xFF;
    message[UEI_GET_KEY_MAP_COMMAND_LENGTH_POS_1] = kmMsg->codeID & 0xFF;

    message[UEI_SEND_KEY_CODE_SET_POS_0] = (kmMsg->codeID>>8) & 0xFF;  // CodeSetNum[0];
    message[UEI_SEND_KEY_CODE_SET_POS_1] = kmMsg->codeID & 0xFF;       // CodeSetNum[1];
    message[UEI_SEND_KEY_KEY_CODE_POS]   = *kmMsg->dataPtr;            // KeyCode;

    if(kmMsg->quick)
    {
      message[UEI_MESSAGE_FLAG_INDEX] = UEI_SEND_QUICK_KEY_FLAG;
    }

    PrepUEI();
    UEI_Serial_SendCommandArray(message, UEI_SEND_KEY_COMMAND_LENGTH);
}


#if (USED_IN_TEST==1)
/*
===============================================================================
@fn MasterReset
@brief Performs a master reset, clearing all downloaded codeset
===============================================================================
*/
void MasterReset(GENERIC_MSG_t* msg)
{
    /* RcdLength(2), 09h, ResetLevel(1) */
    uint8_t message[] = UEI_MASTER_RESET_COMMAND;

    message[UEI_MASTER_RESET_RESET_LEVEL] = msg->params[0];     // Reset level
    PrepUEI();
    UEI_Serial_SendCommandArray(message,UEI_MASTER_RESET_COMMAND_LENGTH);
}

/*
===============================================================================
=====================================================STILL A TEST FUNCTIONALITY
@fn DownLoadDeviceToFDRA
@brief Download code set to FDRA
===============================================================================
*/
void DownLoadDeviceToFDRA(GENERIC_MSG_t* msg)
{
    /* RcdLength(2), 22h,  CodesetNum(2), DownloadData(n) */
    uint8_t message[] = UEI_DOWNLOAD_CODE_SET_COMMAND;
    uei_ueiMsg_t *csMsg;
    csMsg = (uei_ueiMsg_t *)msg->params[0];

    // Set the key id string in the global services variable for
    // status reporting
    strncpy((char *)g_Services.deviceStr, (char *)csMsg->deviceStr, UEI_KEY_RECORD_SIZE);
    g_Services.commandID = UEI_DWNLD_CODE_SET_ID;

    // +3 due to command id (22h) and the 2 bytes for codeset number
    message[UEI_DOWNLOAD_COMMAND_LENGTH_POS_0] = ((csMsg->dataSize+3)>>8) & 0xFF;
    message[UEI_DOWNLOAD_COMMAND_LENGTH_POS_1] = (csMsg->dataSize+3) & 0xFF;
    message[UEI_DOWNLOAD_CODE_SET_POS_0] = (csMsg->codeID>>8) & 0xFF;
    message[UEI_DOWNLOAD_CODE_SET_POS_1] = csMsg->codeID & 0xFF;

    /* Initialize the buffer */
    if (g_Message != NULL)
    {
      // Should never get here.  But if we did ...
      BufferManagerFreeBuffer(g_Message);
      g_Message = NULL;
      LOG(uei_service,ROTTEN_LOGLEVEL_NORMAL,"Error ... second codeset download starting before previous finished");
    }
    if (NULL != (g_Message = BufferManagerGetBuffer(UEI_DOWNLOAD_CODE_SET_COMMAND_LENGTH+csMsg->dataSize)))
    {

      memset(g_Message,0,UEI_DOWNLOAD_CODE_SET_COMMAND_LENGTH+csMsg->dataSize);
      /* The command header*/
      memcpy(g_Message,message,UEI_DOWNLOAD_CODE_SET_COMMAND_LENGTH);
      /* Code set data */
      memcpy(g_Message+UEI_DOWNLOAD_CODE_SET_COMMAND_LENGTH, csMsg->dataPtr, csMsg->dataSize);

      PrepUEI();
      UEI_Serial_SendCommandArray(g_Message, UEI_DOWNLOAD_CODE_SET_COMMAND_LENGTH+csMsg->dataSize);
    }
    else
    {
      LOG(uei_service,ROTTEN_LOGLEVEL_NORMAL,"Unable to get blaster tx buffer");
    }
}

/*
===============================================================================
@fn PopulateCodesetString
@brief Put the response in to a code set string and store locally
===============================================================================
*/
void PopulateCodesetString(void)
{
    uint8_t i =0, Cnt = 0;
    uint8_t *dataPtr;

    Cnt = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_NUM_OF_IDS_POS];

    if ( (Cnt > 0) && (Cnt < UEI_MAX_NUMBER_OF_CODESET_IN_FDRA))
    {
      g_Services.CodesetNumber = Cnt;
    }
    else
    {
      g_Services.CodesetNumber = 0;
      LOG(uei_service,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t Number of codesets %d in blaster invalid ", Cnt);
      return;
    }

    memset(g_Services.CodesetStringPtr,0,sizeof(g_CodesetString));

    dataPtr = &g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+1];

    for (i=0; i<Cnt; i++)
    {
      g_Services.CodesetStringPtr[i] = *dataPtr<<8 | *(dataPtr+1);
      dataPtr+=2;
    }


    g_Services.CodesetStringPtr[UEI_MAX_NUMBER_OF_CODESET_IN_FDRA-1] = '\0';
}

/*
===============================================================================
@fn ListAllUpgradeCode
@brief List all the code available in the blaster
===============================================================================
*/
void ListAllUpgradeCode(GENERIC_MSG_t* msg)
{
    g_Services.commandID = UEI_LIST_ALL_CODE_ID;
    /* RcdLength(2), 11h */
    uint8_t message[] = UEI_LIST_ALL_UPGRADE_CODE_COMMAND;

    message[UEI_LIST_ALL_UPGRADE_CODE_COMMAND_LENGTH_POS_0] = 0x0;
    message[UEI_LIST_ALL_UPGRADE_CODE_COMMAND_LENGTH_POS_1] = 0x1;

    PrepUEI();
    UEI_Serial_SendCommandArray(message, UEI_LIST_ALL_UPGRADE_CODE_COMMAND_LENGTH);
}

/*
===============================================================================
@fn PopulateString
@brief Put the response in to a version string and store locally
===============================================================================
*/
void PopulateString(void)
{
   g_Services.LibVersionString[0] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+0];
   g_Services.LibVersionString[1] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+1];
   g_Services.LibVersionString[2] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+2];
   g_Services.LibVersionString[3] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+3];
   g_Services.LibVersionString[4] = '\0';

   g_Services.SwVersionString[0] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+4];
   g_Services.SwVersionString[1] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+5];
   g_Services.SwVersionString[2] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+6];
   g_Services.SwVersionString[3] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+7];
   g_Services.SwVersionString[4] = '\0';
}

/*
===============================================================================
@fn RemoveSingleCodeset
@brief Delete a selected downloaded code set
===============================================================================
*/
void RemoveSingleCodeset(GENERIC_MSG_t* msg)
{
    /* RcdLength(2), 0Ch, DeviceType(1), CodesetNum(2) */
    uint8_t message[] = UEI_REMOVE_SINGLE_CODESET_COMMAND;
    uei_ueiMsg_t remMsg;

    strncpy((char*)remMsg.deviceStr, (char *)msg->params[0], UEI_KEY_RECORD_SIZE);
    remMsg.codeID = UEIDeviceStringToDeviceType(remMsg.deviceStr);

    // Set the key id string in the global services variable for
    // status reporting
    strncpy((char *)g_Services.deviceStr, (char *)remMsg.deviceStr, UEI_KEY_RECORD_SIZE);
    g_Services.commandID = UEI_DEL_CODE_SET_ID;

    message[UEI_REMOVE_SINGLE_CODESET_POS_0] = (remMsg.codeID>>8) & 0xFF;;
    message[UEI_REMOVE_SINGLE_CODESET_POS_1] = remMsg.codeID & 0xFF;

    PrepUEI();
    UEI_Serial_SendCommandArray(message, UEI_REMOVE_SINGLE_CODESET_COMMAND_LENGTH);
}
#endif /* #if (USED_IN_TEST==1) */

/*
===============================================================================
@fn GetKeyMap
@brief Get the key map for a particular code set
===============================================================================
*/
void GetKeyMap(uint16_t codeID)
{
    g_Services.commandID = UEI_GET_KEY_MAP_ID;
    /* RcdLength(2), 02h, Codeset(2) */
    uint8_t message[] = UEI_GET_KEY_MAP_COMMAND;

    message[UEI_GET_KEY_MAP_COMMAND_LENGTH_POS_0] = (codeID>>8) & 0xFF;
    message[UEI_GET_KEY_MAP_COMMAND_LENGTH_POS_1] = codeID & 0xFF;

    PrepUEI();
    UEI_Serial_SendCommandArray(message, UEI_GET_KEY_MAP_COMMAND_LENGTH);
}

/*
===============================================================================
@fn PopulateKeyMapString
@brief Put the response in to a key map string and store locally
===============================================================================
*/
void PopulateKeyMapString(void)
{
    uint8_t i =0, Cnt = 0;

    /* Message length returned by MAX601LP starts from 0 */
    Cnt = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_LEN_POS]-1;

    if (Cnt > 0)
    {
      /* Status = 1 byte , with remaining the keys for the key map */
      g_Services.NumberOfKeysInMap = Cnt-1;
    }
    else
    {
      LOG(uei_service,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t There are no key map returned from the blaster chip ");
      return;
    }

    memset(g_Services.KeyMapStringPtr,0,sizeof(g_KeyMapString));

    while ((i < Cnt) )
    {
      /* Put everything in the integer array */
      g_Services.KeyMapStringPtr[i] = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_FIRST_DATA_POS+i];
      i = i + 1;
    }


    g_Services.KeyMapStringPtr[UEI_MAX_NUMBER_OF_KEYS_IN_KEYMAP-1] = '\0';
}


/* Local functions */
/*
===============================================================================
@fn UEI_WakeUp
@brief Wakes up the 610LP, required prior to every command, also sent to
       indicate end of continuous key transmission
===============================================================================
*/
void UEI_WakeUp(void)
{
    /* 00 */
    uint8_t wakeup[1]=UEI_WAKE_UP_COMMAND;
    UEI_Serial_SendCommandArray(wakeup,UEI_WAKE_UP_COMMAND_LENGTH);
}

/*
===============================================================================
@fn SendSystemID
@brief Sends the system ID for 610LP, required prior to every command
===============================================================================
*/
static void SendSystemID(void)
{
    /* System ID for MAXQ610 */
    uint8_t sg_system_id[4]= UEI_610LP_SYSTEM_ID;
    /* 0x40, 0x41, 0x42, 0x43 - system id for 610LP */
    UEI_Serial_SendCommandArray(sg_system_id,4);
}

/*
===============================================================================
@fn PrepUEI
@brief Wrapper function to wake up and sends the system id
===============================================================================
*/
static void PrepUEI(void)
{
    /* Wake up the IR Blaster */
    UEI_WakeUp();

    /* Required by the chip */
    Delay_us(1500);

    /* Send the system ID */
    SendSystemID();

    /* enable RX Processing */
    SET_UEI_DATA_RECEPTION_EXPECTATION(UEI_YES);
}

/*
===============================================================================
@fn UEIDeviceStringToDeviceType
@brief Converts device string type to uint16_t  (eg. T2051 to 0x0803)
===============================================================================
*/

uint16_t UEIDeviceStringToDeviceType(uint8_t * deviceString)
{

  uint8_t *stringPtr = deviceString;
  uint8_t i;
  uint16_t devType=0;
  uint8_t devLetter;

  devLetter = *stringPtr++;

  if (devLetter == 0)
  {
    // No codeset stored for this source
    return(0);
  }

  for (i=0; i<4; i++)
  {
    if (*stringPtr >= '0' && *stringPtr <= '9')
    {
      devType = (devType*10) + (uint16_t)(*stringPtr++ - '0');
    }
    else
    {
      LOG(uei_service,ROTTEN_LOGLEVEL_NORMAL,"Invalid device string from ASOC, unexpected non-numeric");
      return(0);
    }
  }


  if (devType < 0x1000)
  {
    switch (devLetter)
    {
      case 'T':
        break;
      case 'C':
        devType = devType | 0x1000;
        break;
      case 'N':
        devType = devType | 0x2000;
        break;
      case 'S':
        devType = devType | 0x3000;
        break;
      case 'V':
        devType = devType | 0x4000;
        break;
      case 'Y':
        devType = devType | 0x6000;
        break;
      case 'R':
      case 'M':
        devType = devType | 0x7000;
        break;
      case 'A':
        devType = devType | 0x8000;
        break;
      case 'D':
        devType = devType | 0x9000;
        break;
      case 'H':
        devType = devType | 0xA000;
        break;
      default:
        LOG(uei_service,ROTTEN_LOGLEVEL_NORMAL,"Invalid device string from ASOC unexpected LOW char %c", devLetter);
        devType = 0;
        break;
    }
  }
  else
  {
    devType -= 0x1000;
    switch (devLetter)
    {
      case 'Y':
        devType = devType | 0x5000;
        break;
      case 'T':
        devType = devType | 0xB000;
        break;
      case 'C':
        devType = devType | 0xC000;
        break;
      case 'N':
        devType = devType | 0xD000;
        break;
      case 'S':
        devType = devType | 0xE000;
        break;
      case 'R':
      case 'M':
      case 'A':
      case 'D':
        devType = devType | 0xF000;
        break;
      default:
        devType = 0;
        LOG(uei_service,ROTTEN_LOGLEVEL_NORMAL,"Invalid device string from ASOC unexpected high char %c", devLetter);
        break;
    }
  }
  LOG(uei_service,ROTTEN_LOGLEVEL_VERBOSE,"Device string from ASOC xlated to %04x", devType);
  return devType;
}
