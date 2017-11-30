/*
    File    :   etapHeap.c
    Title   :   Extended TAP (Test Access Port) Heap
    Author  :   ar1016648
    Created :   08/07/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA
===============================================================================
*/

#include "project.h"
#include "etap.h"
#include "etapHeap.h"
#include "buffermanager.h"

SCRIBE_DECL(system);

static void PrintHeapFreeMap(void)
{
#pragma section = "HEAP"
    extern uint8_t __heap_end__;
    extern BlockLink_t heap_4_xStart;
    BlockLink_t* s = __section_begin("HEAP");
    BlockLink_t* p = &heap_4_xStart;
    BlockLink_t* e = (BlockLink_t*)&__heap_end__;

    TAP_Printf("Heap Total: %ld\n", (uintptr_t)e - (uintptr_t)s);
    TAP_Printf("Heap Remaining: %ld\n", BufferManagerMemoryRemaining());
    TAP_Printf("Heap Start: 0x%08x\n", (uintptr_t)s);
    TAP_Printf("Heap End: 0x%08x\n", (uintptr_t)e);
    TAP_Printf("Heap Free Map:\n");
    while (p)
    {
        // there may be zero-length nodes such as start/end
        if (p->xBlockSize)
        {
            TAP_Printf("   0x%08x - 0x%08x (%d bytes)\n", (uintptr_t)p,
                       (uintptr_t)p + p->xBlockSize - 1, p->xBlockSize);
        }
        p = p->pxNextFreeBlock;
        if (p && (p > e))
        {
            TAP_Printf("Heap went too far!\n");
            break;
        }
    }
}

#if configTRACE_HEAP_ALLOCATIONS == 1
static void PrintHeapAllocMap(void)
{
#pragma section = "HEAP"
    extern uint8_t __heap_end__;
    extern BlockLink_t heap_4_xAllocStart, *heap_4_xAllocEnd;
    BlockLink_t* s = __section_begin("HEAP");
    BlockLink_t* p = &heap_4_xAllocStart;
    BlockLink_t* e = (BlockLink_t*)&__heap_end__;
    unsigned sz;

    TAP_Printf("Heap Total: %ld\n", (uintptr_t)e - (uintptr_t)s);
    TAP_Printf("Heap Remaining: %ld\n", BufferManagerMemoryRemaining());
    TAP_Printf("Heap Start: 0x%08x\n", (uintptr_t)s);
    TAP_Printf("Heap End: 0x%08x\n", (uintptr_t)e);
    TAP_Printf("Heap Free Map:\n");
    while (p)
    {
        // there may be zero-length nodes such as start/end
        if (p->xBlockSize)
        {
            sz = p->xBlockSize & ~(1 << 31);
            TAP_Printf("   0x%08x - 0x%08x (%d bytes) (%s)\n", (uintptr_t)p,
                       (uintptr_t)p + sz - 1, sz, p->source);
        }
        p = p->pxNextAllocatedBlock;
        if (p > e)
        {
            TAP_Printf("Heap went too far!\n");
            break;
        }
        if (p == heap_4_xAllocEnd)
        {
            break;
        }
    }
}
#endif


TAPCommand(TAP_Heap)
{
    if (CommandLine->args[0] && !strcmp(CommandLine->args[0], "free"))
    {
        PrintHeapFreeMap();
    }
#if configTRACE_HEAP_ALLOCATIONS == 1
    else if (CommandLine->args[0] && !strcmp(CommandLine->args[0], "alloc"))
    {
        PrintHeapAllocMap();
    }
#endif
    else
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Heap Remaining: %ld", BufferManagerMemoryRemaining());
        TAP_Printf("Heap Remaining: %ld", BufferManagerMemoryRemaining());
    }
}

