/*
 * KeyHandler.h - Converts keys from Key Sources to a common form for distribution.
 *
 * Author: Dillon Johnson
 *
 * KeyHandler provides a way for handling the timing of keys from various sources (aka producers).
 * Two timers are set for key presses. The first timer is used to send a key release after a user
 * specified amount of time. This timer is reset each time another key press/repeat for the same
 * key is received. The second timer is used to generate repeats and is stopped whenever a key
 * release is sent. Also, a hold time can be specified so incoming presses/repeats can be ignored
 * for a user specificed amount of time after the first press of a new key.
 *
 * To Use:
 *  1. Create two timer ID's (currently in TimerIDs.h) for your producer.
 *  2. Use the CREATE_KEY_PRODUCER macro to create a key producer in your module.
 *      Example Usage:
 *          CREATE_KEY_PRODUCER(tap, KEY_PRODUCER_TAP, TAP_HOLD_TIME, TAP_RELEASE_TIME, TIMER_ID_TAPRelease, TAP_REPEAT_TIME, TIMER_ID_TAPRepeat);
 *      Param explanation:
 *          1. tap - the name of the key producer
 *          2. KEY_PRODUCER_TAP - the key producer's ID (listed in KeyData.h)
 *          3. TAP_HOLD_TIME - time to ignore incoming presses/repeats after first press (defined in KeyTimings.h)
 *          4. TAP_RELEASE_TIME - time to release the key after the last press
 *          5. TIMER_ID_TAPRelease - Release timer ID created in step 1
 *          6. TAP_REPEAT_TIME - time in between repeats (sent, not received!)
 *          7. TIMER_ID_TAPRepeat - Repeat timer ID created in step 1
 *  3. Whenever a key is received in your module, map it to the correct Bose IR code (listed in KeyData.h)
 *  4. To send the key, pass the value and producer address to HandleKey()
 */

#ifndef _KEYHANDLER_H_
#define _KEYHANDLER_H_

#include "KeyData.h"
#include "KeyTimings.h"
#include "TimerIDs.h"
#include "platform_settings.h"

#define SHORT_PRESS_AND_HOLD_START   500
#define PRESS_AND_HOLD_START        1750
#define VENTI_PRESS_AND_HOLD_START 3750
#define LONG_PRESS_AND_HOLD_START   5000
#define XL_PRESS_AND_HOLD_START    10000

/* TYPES */
typedef struct
{
    TimerHandle_t ReleaseTimer;
    TIMER_ID_t ReleaseTimerID;
    const char* ReleaseTimerName;
    TimerHandle_t RepeatTimer;
    TIMER_ID_t RepeatTimerID;
    const char* RepeatTimerName;
} KEY_TIMERS_t;

typedef struct
{
    KEY_PRODUCER ProducerID;
    uint16_t HoldInterval;      // ignore repeats after first press for this much time (ms)
    uint16_t ReleaseInterval;   // send a release after this much time (ms)
    uint16_t RepeatInterval;    // send another press after this much time (ms)
    KEY_TIMERS_t* Timers;       // to keep track of presses & releases
    KEY_DATA_t HeldKey;         // the currently pressed key
    BOOL PassedHoldInterval;    // flag that it's okay to repeat
    void (*ReleaseCallback)(xTimerHandle);
    void (*RepeatCallback)(xTimerHandle);
    SemaphoreHandle_t KeyMutex; // mutex to control access to shared key data
} KEY_PRODUCER_ATTRIBUTES;


/* MACROS */

// CREATE_KEY_PRODUCER params:
//  1. name - name of producer
//  2. pid - producer ID (KeyData.h)
//  3. hold_time - time to ignore presses after new key
//  4. release_time - time to send release after latest press
//  5. repeat_time - time to send repeats for held keys
//  6. release_timer_id - ID of the producer's release timer (TimerIDs.h)
//  7. repeat_timer_id - ID of the producer's repeat timer (TimerIDs.h)
#define CREATE_KEY_PRODUCER(name, \
                            pid, \
                            hold_time, \
                            release_time, \
                            repeat_time, \
                            release_timer_id, \
                            repeat_timer_id ) \
    void name##ReleaseCallback (xTimerHandle pxTimer); \
    void name##RepeatCallback (xTimerHandle pxTimer); \
    KEY_TIMERS_t name##Timers = { \
        .ReleaseTimer = TIMER_INVALID_HANDLE, \
        .ReleaseTimerID = release_timer_id, \
        .ReleaseTimerName = #name "ReleaseTimer", \
        .RepeatTimer = TIMER_INVALID_HANDLE, \
        .RepeatTimerID = repeat_timer_id, \
        .RepeatTimerName = #name "RepeatTimer", \
    }; \
    KEY_PRODUCER_ATTRIBUTES name = { \
        .ProducerID = pid, \
        .HoldInterval = hold_time, \
        .ReleaseInterval = release_time, \
        .RepeatInterval = repeat_time, \
        .Timers = &name##Timers, \
        .HeldKey.value = BOSE_INVALID_KEY, \
        .HeldKey.state = LPM_KEY_STATE_INVALID, \
        .HeldKey.group = KEY_GROUP_INVALID, \
        .HeldKey.producer = pid, \
        .HeldKey.timestamp = 0, \
        .HeldKey.ph_timestamp = 0, \
        .PassedHoldInterval = FALSE, \
        .ReleaseCallback = name##ReleaseCallback, \
        .RepeatCallback = name##RepeatCallback, \
        .KeyMutex = NULL, \
    }; \
    void name##ReleaseCallback (xTimerHandle pxTimer) \
    { \
        KeyReleaseCallback(&name); \
    } \
    void name##RepeatCallback (xTimerHandle pxTimer) \
    { \
        KeyRepeatCallback(&name); \
    } \
    void name##_init(void) \
    { \
        name.KeyMutex = xSemaphoreCreateMutex(); \
        debug_assert(name.KeyMutex); \
        name.Timers->ReleaseTimer = xTimerCreate( \
                name.Timers->ReleaseTimerName, \
                TIMER_MSEC_TO_TICKS(name.ReleaseInterval), \
                pdFALSE, \
                (void*) name.Timers->ReleaseTimerID, \
                name.ReleaseCallback \
            ); \
        debug_assert(name.Timers->ReleaseTimer); \
        name.Timers->RepeatTimer = xTimerCreate( \
                name.Timers->RepeatTimerName, \
                TIMER_MSEC_TO_TICKS(name.RepeatInterval), \
                pdFALSE, \
                (void*) name.Timers->RepeatTimerID, \
                name.RepeatCallback \
            ); \
        debug_assert(name.Timers->RepeatTimer); \
    }



/* Functions */
void HandleKey(KEY_VALUE held_key, KEY_PRODUCER_ATTRIBUTES* producer);
void HandleKeyRelease(KEY_VALUE held_key, KEY_PRODUCER_ATTRIBUTES* producer);
void KeyReleaseCallback (KEY_PRODUCER_ATTRIBUTES* producer);
void KeyRepeatCallback (KEY_PRODUCER_ATTRIBUTES* producer);
void ResetKeyList (void);
void PrintKeyList (void);

#endif // _KEYHANDLER_H_
