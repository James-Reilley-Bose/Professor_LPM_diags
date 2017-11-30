//
// VariantTimers.h
//

#ifndef VARIANT_TIMERS_H
#define VARIANT_TIMERS_H

#define FOREACH_TIMER(TIMER) \
        TIMER(tBlockIdExampleTimer) \
        TIMER(tBlockIdIPCBulk) \
        TIMER(tBlockIdIPCPassThrough) \
        TIMER(tBlockIdUpdateWatchdog) \
        TIMER(tBlockIdVampRawStartupTimer) \

#endif // VARIANT_TIMERS_H
