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
#include "etap.h"

SCRIBE_DECL(uei_service);

/* local library routine */
static void PrepUEI(void);

/* Global functions */

/*
===============================================================================
@fn UEI_BlasterSendMsg
@brief Send a formatted message to the blaster
===============================================================================
*/
void UEI_BlasterSendMsg(uint8_t *blasterMsg, uint16_t length)
{
    PrepUEI();
    UEI_Serial_SendCommandArray(blasterMsg, length);
}

/*
===============================================================================
@fn UEI_BlasterGetResp
@brief Send response message back (or only log, if from TAP);
===============================================================================
*/
void UEI_BlasterGetResp(BOOL tap_only)
{
    if (tap_only)
    {
        TAP_Printf("Blaster_resp = \n");
        uint16_t Cnt = g_Serial.UEIInterruptRXBuffer[UEI_RECEIVE_BUFFER_RESPONSE_LEN_POS]+1;  // Add one for the length byte
        for (uint16_t i=0; i<Cnt; i++)
        {
            TAP_Printf("0x0%x ", g_Serial.UEIInterruptRXBuffer[i] );
                
            //puts keys in rows of 10 for easier view
            if( ((i + 1) % 10) == 0) TAP_Printf("\n");
                
        }
        TAP_Printf("\n");
    }
}

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


