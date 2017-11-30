/*
    File    :   CommonAPI.c
    Title   :
    Author  :   dx1007255
    Created :   01/23/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Implementation of common APIs used by all drivers.

===============================================================================
*/

#include "driverLibCommon.h"


/*****************************************************************//**
* @brief    Helper function to set event bits inside ISR
*           Following the example in FreeRTOS API's doc
*
* @param    eventGroupHandle - Handle to EventGroupHandle_t
* @param    bitsToSet - Bit masks to set, must be non-zero
*
* @return   void
**********************************************************************/
void driverSetEventBitsInISR(EventGroupHandle_t eventGroupHandle,
                             EventBits_t bitsToSet)
{
    if (bitsToSet == 0)
    {
        return;
    }

    BaseType_t xHigherPriorityTaskWoken, xResult;

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
    xHigherPriorityTaskWoken = pdFALSE;

    xResult = xEventGroupSetBitsFromISR(eventGroupHandle, bitsToSet,
                                        &xHigherPriorityTaskWoken );

    /* Was the message posted successfully? */
    if ( xResult != pdFAIL )
    {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        switch should be requested. */
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
}



void driverGiveSemaphoreInISR(xSemaphoreHandle semaphoreHandle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR( semaphoreHandle, &xHigherPriorityTaskWoken );

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

}