//
// VariantTimers.h
//

#ifndef VARIANT_TIMERS_H
#define VARIANT_TIMERS_H

#define FOREACH_TIMER(TIMER) \
        TIMER(tBlockIdAccessoryMuteTimer) \
        TIMER(tBlockIdExampleTimer) \
        TIMER(tBlockCKScanTimer) \
        TIMER(tBlockCKDebounceTimer) \
        TIMER(tBlockIdCheckAccessoriesTimer) \
        TIMER(tBlockVulcanKeysTimer) \
        TIMER(tBlockIdIPCBulk) \
        TIMER(tBlockIdIPCPassThrough) \
        TIMER(tBlockIdUpdateWatchdog) \
        TIMER(tBlockIdWaMon) \
        TIMER(tBlockIdwaTransfer) \
        TIMER(tBlockIdVampRawStartupTimer) \
        TIMER(tBlockIdCECSystemAudioTimeout) \
        TIMER(tBlockIdCECAudioStatusHoldoffTimer) \
        TIMER(tBlockIdDspTemperatureTimer) \
        TIMER(tBlockDSPDeviceModelTimer) \
        TIMER(tBlockIdWiredID) \
        TIMER(tBlockIdWaitRise) \
        TIMER(tBlockIdWaitFall) \
        TIMER(tBlockIdWiredDetected) \
        TIMER(tBlockIdRetryTimer) \
        TIMER(tBlockIdNoWiredBass) \
        TIMER(tBlockDiagScanTimer) \
        TIMER(tBlockIdSystemStateTimer) \
        

#endif // VARIANT_TIMERS_H
