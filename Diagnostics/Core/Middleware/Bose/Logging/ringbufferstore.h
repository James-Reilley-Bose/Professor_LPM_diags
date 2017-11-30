/*
 * RingBufferStore implements Rotten_Store as a
 * fixed-size circular buffer.
 * Brought to you from the keyboard of Nate Bragg
 */
#ifndef RING_BUFFER_STORE_H
#define RING_BUFFER_STORE_H

#include <stddef.h>

#include "rottenlog.h"

#ifndef LOG_BUFFER_SIZE
#define LOG_BUFFER_SIZE 0x40
#endif

/*
 * Public interface
 */
typedef struct RingBufferStore
{
    Rotten_Store base;

    Rotten_Record ring[LOG_BUFFER_SIZE];
    size_t idx;
} RingBufferStore;
void ring_buffer_store_init(RingBufferStore*);

#endif
