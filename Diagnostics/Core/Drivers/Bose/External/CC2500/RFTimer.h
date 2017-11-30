/*
    File    :   RFTimer.h
    Title   :
    Author  :   jc45540
    Created :   01/16/2016
    Language:   C
    Copyright:  (C) 2016 Bose Corporation, Framingham, MA

    Description:   RF Timer header for Bardeen

===============================================================================
*/

#ifndef _RFTIMER_H_
#define _RFTIMER_H_

#define RFTIMER_PRESCALER                           APB1_PERIPHERAL_CLK / 500000         // 1us resolution
#define RFTIMER_TO_ACTION_DELAY_OVERHEAD_IN_USEC    105

void RFTimer_Configure( void );
void RFTimer_StartMS( uint32_t TimeInMilliseconds );
void RFTimer_StartUS( uint32_t TimeInMicroseconds );

#endif // _RFTIMER_H_

