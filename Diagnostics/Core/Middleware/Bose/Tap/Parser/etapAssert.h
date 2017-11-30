/*
    File    :   etapAssert.h
    Author  :   ja70076
    Created :   07/07/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Assert related TAP commands

===============================================================================
*/
#ifndef ETAPASSERT_H
#define ETAPASSERT_H

#include "etapcoms.h"


/*
 * @DOCETAP
 * COMMAND:   assert
 *
 * ALIAS:     as
 *
 * DESCRIP:   Assert related TAP utility.
 *
 * PARAM:     <fake|check|reset>
 *
 * REPLY:     empty
 *
 * EXAMPLE:   assert fake - Causes a fake assert
 *
 * @DOCETAPEND
 *
 */

#define ASSERT_HELP_TEXT "* HELP_TEXT: Assert Related TAP command\n\r  \
   \tUsage : assert <fake, check, reset< "

void TAP_Assert(CommandLine_t*);


#endif //ETAPASSERT_H