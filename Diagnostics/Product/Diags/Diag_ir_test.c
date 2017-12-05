/**
  ******************************************************************************
  * @file    Diag_ir_test.c
  *           + Diagnostic test for IR
 @verbatim
 ===============================================================================
 **/

#include "project.h"
#include "stdio.h"
#include "Diag_ir_test.h"
#include "etap.h"
#include "TapListenerAPI.h"
#include "KeyHandlerTask.h"
#include "etapKeys.h"
#include "DiagsDispatcher.h"
#include "Diag_led_test.h"

SCRIBE_DECL(diag);

CommandLine_t* abc = 0;
BOOL printIR = TRUE;

/*
===============================================================================
@fn Diag_IR_Print
@brief enabled/disable printing of IR key received 
===============================================================================
*/
void Diag_IR_Print (BOOL enabled)
{
    printIR = enabled;
}

/*
===============================================================================
@fn Diag_IR_Blink_Led
@brief enabled/disable printing of IR key received 
===============================================================================
*/
void Diag_IR_Blink_Led (BOOL pressed)
{
    #if (PRODUCT_HARDWARE_VARIANT == 0)
    if (pressed == TRUE) {
        led_ctrl(1, 1);
    } else {
        led_ctrl(1, 0);
    }   
    #endif
}

/*
===============================================================================
@fn DIAG_IRCommand_Handle
@brief help, output to display, output thru sound
===============================================================================
*/
void DIAG_IRCommand_Handle(DIAG_IR_COMMANDS commandType)
{
    switch(commandType)
    {
    case DIAG_IR_HELP:

        break;

    case DIAG_IR_RECEIVE_START:
        Diag_IR_Print(TRUE);
        break;
  
     case DIAG_IR_RECEIVE_STOP:
        Diag_IR_Print(FALSE);              
        break;    

    default:
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Invalid argument");
        return;
    }

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n");
}

