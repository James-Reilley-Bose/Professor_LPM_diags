/*
    File    :   CommonAPI.h
    Title   :
    Author  :   dx1007255
    Created :   01/23/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Headers of Common APIs used by all drivers.

===============================================================================
*/
#ifndef COMMON_API_H
#define COMMON_API_H

#include "platform_settings.h"
#include "event_groups.h"

void driverSetEventBitsInISR(EventGroupHandle_t eventGroupHandle,
                             EventBits_t bitsToSet);

void driverGiveSemaphoreInISR(xSemaphoreHandle semaphoreHandle);

#endif // COMMON_API_H
