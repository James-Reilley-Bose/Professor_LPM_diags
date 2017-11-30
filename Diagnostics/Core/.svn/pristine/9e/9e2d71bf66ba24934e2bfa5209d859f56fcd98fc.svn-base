/*!
* @file        EventBlocking.h
* @version     $Id:
* @author      ja70076
* @brief       API for Event Blocking
*
* Copyright 2014 Bose Corporation.
*/

#ifndef EVENTBLOCKING_H
#define EVENTBLOCKING_H

#include "platform_settings.h"

#define INVALID_HANDLE 0xFF
typedef uint8_t EVENT_BLOCKING_HANDLE;

EVENT_BLOCKING_HANDLE RegisterForEventBlocking (void);
boolean BlockOnEvent (EVENT_BLOCKING_HANDLE TheBlockingHandle, MESSAGE_ID_t TheEvent, uint32_t TimeoutInTicks );
void CheckAndMaybeWakeBlockers (MESSAGE_ID_t TheEvent);

/* Blocking Event stress test
 * uncomment the #define below to enable stress testing of event blocking
 * Type 'BE STRESS' into TAP and a number of tasks will be created to
 * compete blocking on a particular event
 */
//#define ENABLE_BLOCKING_EVENT_STRESS_TEST

#ifdef ENABLE_BLOCKING_EVENT_STRESS_TEST
#define NUM_BE_STRESS_TEST_TASKS 5
void beInitStressTest(void);
#endif

#endif /* EVENTBLOCKING_H */
