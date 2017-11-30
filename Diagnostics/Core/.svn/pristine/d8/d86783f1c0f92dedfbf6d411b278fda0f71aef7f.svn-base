/*!
* @file SystemEventsTAP.h
* @version
* @author
* @brief
*
* Copyright 2014 Bose Corporation.
*/
#ifndef SYSTEM_EVENTS_TAP_H
#define SYSTEM_EVENTS_TAP_H

#include "etapcoms.h"

/*
 * @DOCETAP
 * COMMAND:   postevent
 *
 * ALIAS:     pe
 *
 * DESCRIP:   Post an event to all tasks or given q.
 *
 * PARAM:     Name of Q, Name of event, params[]
 *
 * REPLY:     None
 *
 * @DOCETAPEND
 */

#define POST_EVENT_HELP_TEXT "* HELP_TEXT: Use this command to post a Event.\n\t Usage: pe [Q], [Event Name], [param1], ... [param3]"

void TAP_PostEvent(CommandLine_t*);

/*
 * @DOCETAP
 * COMMAND:   block
 *
 * ALIAS:     be
 *
 * DESCRIP:   Block waiting for an event.
 *
 * PARAM:     Name of a event i.e. "SYSTEM_EVENT_ID_TestMessage"
 *
 * REPLY:     None
 *
 * @DOCETAPEND
 */

#define BLOCK_ON_EVENT_HELP_TEXT "* HELP_TEXT: Block the TAP task waiting for a specified Event\n\t Usage: be [Event Name], [Timeout MSec] - also 'be test'"

void TAP_BlockOnEvent(CommandLine_t*);


#endif //SYSTEM_EVENTS_TAP_H
