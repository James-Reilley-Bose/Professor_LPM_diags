/*
 * KeyTimings.h - Macros for hold, release, and repeat key timings.
 *
 * Author: Dillon Johnson
 */

#ifndef _KEY_TIMINGS_H_
#define _KEY_TIMINGS_H_

/* Default */
#define DEFAULT_HOLD_TIME       300 // 300ms
#define DEFAULT_RELEASE_TIME    250 // 250ms
#define DEFAULT_REPEAT_TIME     100 // 100ms

/* IR */
#define IR_HOLD_TIME       DEFAULT_HOLD_TIME
#define IR_RELEASE_TIME    DEFAULT_RELEASE_TIME
#define IR_REPEAT_TIME     DEFAULT_REPEAT_TIME

/* TAP */
#define TAP_HOLD_TIME       DEFAULT_HOLD_TIME
#define TAP_RELEASE_TIME    DEFAULT_RELEASE_TIME
#define TAP_REPEAT_TIME     DEFAULT_REPEAT_TIME

/* RF_REMOTE */
// Those timing parameters will be adjusted when Bardeen RF remote is available,
//      since they are now based on Farben remote.
#define RF_REMOTE_HOLD_TIME       DEFAULT_HOLD_TIME
#define RF_REMOTE_RELEASE_TIME    20000
#define RF_REMOTE_REPEAT_TIME     DEFAULT_REPEAT_TIME/2

/* CONSOLE */
#define CONSOLE_HOLD_TIME       DEFAULT_HOLD_TIME
#define CONSOLE_RELEASE_TIME    DEFAULT_RELEASE_TIME
#define CONSOLE_REPEAT_TIME     DEFAULT_REPEAT_TIME

/* CEC */
#define CEC_HOLD_TIME       DEFAULT_HOLD_TIME
#define CEC_RELEASE_TIME    (DEFAULT_RELEASE_TIME * 4) // CEC keys come in ever ~500 ms so 
#define CEC_REPEAT_TIME     DEFAULT_REPEAT_TIME

#endif // _KEY_TIMINGS_H_

