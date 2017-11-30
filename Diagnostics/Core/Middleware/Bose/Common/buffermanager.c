// buffermanager.c - a wrapper for FreeRTOS heap
//

#include "project.h"
#include "buffermanager.h"

SCRIBE_DECL(system);

void* BufferManagerGetBuffer(size_t size)
{
    // Never Malloc from an interrupt
    debug_assert(!InInterrupt());

    void* buf = NULL;
    size_t numBlocks = 0;

    if (size)
    {
        numBlocks = size / BLOCK_SIZE;
        if (size % BLOCK_SIZE)
        {
            numBlocks++;
        }
        buf = pvPortMalloc(numBlocks * BLOCK_SIZE);
        if (buf)
        {
            LOG(system, ROTTEN_LOGLEVEL_VERBOSE, "Malloc: %p, %lu bytes. Total Available: %lu bytes", buf, numBlocks * BLOCK_SIZE, BufferManagerMemoryRemaining());
        }
        else
        {
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "BufferManager unable to allocate %lu bytes! Total Available: %lu bytes", numBlocks * BLOCK_SIZE, BufferManagerMemoryRemaining());
        }
    }
    return buf;
}

void BufferManagerFreeBuffer(void* p)
{
    // Never Free from an interrupt
    debug_assert(!InInterrupt());
    // Assert to check to make sure pointers aren't freed twice, or freed before they are malloced
    debug_assert(p)
    vPortFree(p);
    LOG(system, ROTTEN_LOGLEVEL_VERBOSE, "Free: %p. Total Available: %lu bytes", p, BufferManagerMemoryRemaining());
}

void BufferManagerFreeBufferCallback(uint32_t* params, uint8_t numParams)
{
    BufferManagerFreeBuffer((void*) params[0]);
}

size_t BufferManagerMemoryRemaining(void)
{
    return xPortGetFreeHeapSize();
}
