/*
 * TapStore implements Rotten_Store as a
 * connection to etap.
 * Brought to you from the keyboard of Nate Bragg
 */
#ifndef TAP_STORE_H
#define TAP_STORE_H

#include "rottenlog.h"
/*
 * Public interface
 */
#define TAP_STORE_NO_ANNOTATION 0x01
#define TAP_STORE_NO_CRLF       0x02

#define TAP_STORE_FRAME_START_STR "~s~"
#define TAP_STORE_FRAME_END_STR "~e~"
#define TAP_STORE_TERM_FRAME_LENGTH (4) //Keep start/end equal length + null

typedef struct TapStore {
    Rotten_Store base;
} TapStore;
void tap_store_init(TapStore *);

#endif
