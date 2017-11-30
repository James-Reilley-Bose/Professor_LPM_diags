/*
 * RingBufferStore implements Rotten_Store as a
 * fixed-size circular buffer.
 * Brought to you from the keyboard of Nate Bragg
 */
#include <string.h>

#include "ringbufferstore.h"

/*
 * Procedure prototypes
 */
static void ring_buffer_store_write(RingBufferStore* this, Rotten_Record* r);
static void ring_buffer_store_dump(RingBufferStore* from, Rotten_Store* to);
static void ring_buffer_store_flush(RingBufferStore* this);

/*
 * Public interface
 */
void ring_buffer_store_init(RingBufferStore* this)
{
    this->base.write = (void(*)(Rotten_Store*, Rotten_Record*))ring_buffer_store_write;
    this->base.dump = (void(*)(Rotten_Store*, Rotten_Store*))ring_buffer_store_dump;
    this->base.flush = (void(*)(Rotten_Store*))ring_buffer_store_flush;
    this->idx = 0;
    memset(this->ring, 0, sizeof(Rotten_Record) * LOG_BUFFER_SIZE);
}

/*
 * Implementation
 */
static void ring_buffer_store_write(RingBufferStore* this, Rotten_Record* r)
{
    memmove(&this->ring[this->idx], r, sizeof(Rotten_Record));
    this->idx = (this->idx + 1) % LOG_BUFFER_SIZE;
}

static void ring_buffer_store_dump(RingBufferStore* from, Rotten_Store* to)
{
    for (size_t i = 0; i < LOG_BUFFER_SIZE; ++i)
    {
        Rotten_Record* record = &from->ring[(i + from->idx) % LOG_BUFFER_SIZE];
        if (record->msg != 0)
        {
            to->write(to, record);
        }
    }
}

static void ring_buffer_store_flush(RingBufferStore* this)
{
    // memset to zero: don't want dump() picking up stray data
    memset(this->ring, 0, sizeof(Rotten_Record) * LOG_BUFFER_SIZE);
}
