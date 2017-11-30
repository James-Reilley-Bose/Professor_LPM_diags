#ifndef ETAPBACKLIGHT_H
#define  ETAPBACKLIGHT_H

#include "etapcoms.h"

/**
 * @DOCETAP
 * COMMAND:   backlight
 *
 * ALIAS:     pt
 *
 * DESCRIP:   backlight tap command
 *
 * PARAM:     destination, command
 *
 * REPLY:     Depends
 *
 * EXAMPLE:   pt
 *
 * @DOCETAPEND 
 */

#define BACK_LIGHT_HELP_TEXT "Control the back light\n\r\t Usage: bl params\n\r"

void TAPBackLight(CommandLine_t*);

#endif //  ETAPBACKLIGHT_H