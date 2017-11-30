/*
 * NvStore implements Rotten_Store as a
 * buffer in flash.
 * Brought to you from the keyboard of Nate Bragg
 */
#ifndef NV_STORE_H
#define NV_STORE_H

#include <stddef.h>

#include "rottenlog.h"

#define NV_DUMP_DELAY TIMER_MSEC_TO_TICKS (10)

/*
 * Public interface
 */
typedef struct NvStore
{
    Rotten_Store base;

    int ring[100];
    Rotten_Record* rs;
    size_t rc;
    size_t idx;
    size_t block_size;
} NvStore;
void nv_store_init(NvStore*, Rotten_Record*, size_t mem_size, size_t block_size);

#endif
