/*
    File    :   lastGasp.h
    Title   :   Post Mortem debug functions
    Author  :   lc1007209
    Created :   12/11/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:    This file contains postmortem hooks to help developers
                        determine the cause of assertions. Add static inline
                        functions to this file to expand capabilities


===============================================================================
*/

#ifndef LAST_GASP_H
#define LAST_GASP_H
/* Tweek these to accomodate available resources */
#define LG_MAX_MSG_BUF 2048
#define LG_MAX_TICKS   30
#include <stdio.h>
#include <stdint.h>
#ifdef I_AM_LAST_GASP_C
char  last_gasp[LG_MAX_MSG_BUF];
char* lastgaspHead = last_gasp;
uint32_t lgTickRing[LG_MAX_TICKS];
int lgTickIndex = 0;
#else
extern char last_gasp[];
extern uint32_t lgTickRing[];
extern char* lastgaspHead;
extern int lgTickIndex;
#endif
/*
** Note that there is no need to include module specifi files here.
** It is assumed that the static inline function that will resolve your issue is instantiated in
** your module ( by calling it), and that this module includes these files already.
** All that need be done is to add this file to the end of the inclusion chain.
** the un instatiated functions will not cause external definition issues
*/

static inline void UnwindQueue(xQueueHandle qHandle, int depth )
{
    /*
    ** We are going to die we may as well tell the world who is responsible for our demise
    */

    char* queue_dump = lastgaspHead;
    uint8_t cnt = 0;
    int i = 0;
    GENERIC_MSG_t message;
    for (i = 0; i < depth; i++ )
    {
        if ( xQueueReceive(qHandle, &message, 100 ) )
        {
            cnt++;
            queue_dump += snprintf(queue_dump, &last_gasp[2047] - queue_dump, "ID: %d,Prms %x, %x, %x, %x %x \n",
                                   message.msgID, message.params[0], message.params[1], message.params[2], message.params[3], message.params[4]);
        }

    }
    cnt = 0; /* Hang breakpoint here */

}
static inline void LgBumpTick(uint32_t tick)
{
    lgTickRing[lgTickIndex++] = tick;
    lgTickIndex = (lgTickIndex >= LG_MAX_TICKS) ? 0 : lgTickIndex;
}


#endif
