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
#ifndef ETAPUEI_H
#define ETAPUEI_H

#include "etapcoms.h"
#include "UEIDefinitions.h"
#include "HwMacros.h"

/*
 * @DOCETAP
 * COMMAND:   ueiblaster
 *
 * ALIAS:     ut
 *
 * DESCRIP:   Forward commands to UEI MAX610LP chip as defined in the UEI spec -
 *            UEI OEM QS IR Blaster (LP) Product Specification, Revision: v2.1
 *
 * PARAM:     byte string to send to blaster (NOTE: the two leading message length bytes are NOT entered)
 *
 * REPLY:     Raw byte string reported by blaster.
 *
 * EXAMPLE: ut 0b - blaster firmware version
 *          ut 11 - lists the code sets currently stored in the blaster chip
 *          ut 22, 08, 03 - downloads the only code set stored in the code, Samsung TV (T2051)
 *          ut 0c, 08, 03 - removes a single code set, in this case the code set is T2051 (refer to spec listed
 *                             above for information as to how to translate code set string to integer)
 *          ut 02, 08, 03 - prints the key map for the code set T2051
 *          ut 01, 08, 03, 01, 00, 00, 00 - sends the 0x01 UEI key using code set T2051.  This key is transmitted
 *                                             continuously until the end key is sent  
 *
 * @DOCETAPEND
 * 
 */                                   

#define DISPLAY_UEI_COMMAND_TEXT "* Use this command to test UEI Blaster functionality \n\r\t Usage : ut blaster byte string"
#define DISPLAY_UEI_HELP_TEXT "* HELP_TEXT: UEI Blaster usage:\n\r"  \
        "\t'ut <raw byte stream minus length bytes>'\n\r" \
        "\tExample: 'ut 0b' - gets firmware version on blaster chip \n\r"


void TAP_UEIBlasterCommand(CommandLine_t*);

#define MINIMAL_UEIBLASTER_NUMBER_ARGUMENTS 1


#endif

/* ======== END OF FILE ======== */
