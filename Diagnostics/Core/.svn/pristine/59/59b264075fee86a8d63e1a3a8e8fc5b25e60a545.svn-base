/*
 * logimpl is the implementation-specific
 * details supporting rotten log.
 * Brought to you from the keyboard of Nate Bragg
 */
#include "rottenlog.h"
#include "ringbufferstore.h"
#include "nvstore.h"
#include "ipcstore.h"
#include "tapstore.h"
#include "HwMacros.h"
#include "etap.h"

static xSemaphoreHandle log_mutex()
{
    static xSemaphoreHandle mutex = NULL;
    if (mutex == NULL)
    {
        mutex = xSemaphoreCreateRecursiveMutex ();
        RTOS_assert (mutex != NULL, "log mutex create failed");
    }
    return mutex;
}

static Rotten_Result lock()
{
    static portTickType timeout = portMAX_DELAY;
    if (!InInterrupt() && xSemaphoreTakeRecursive (log_mutex(), timeout) == pdTRUE)
    {
        return ROTTEN_SUCCESS;
    }
    return ROTTEN_FAILURE;
}

static Rotten_Result unlock()
{
    if (!InInterrupt() && xSemaphoreGiveRecursive (log_mutex()) == pdTRUE)
    {
        return ROTTEN_SUCCESS;
    }
    return ROTTEN_FAILURE;
}

static uint32_t time()
{
    return GET_SYSTEM_UPTIME_MS();
}

static Rotten_Store* tap_store()
{
    static TapStore tap;
    static enum
    {
        TAP_STORE_UNINITIALIZED,
        TAP_STORE_INITIALIZED
    } initialized = TAP_STORE_UNINITIALIZED;

    if (initialized != TAP_STORE_INITIALIZED)
    {
        tap_store_init(&tap);
        initialized = TAP_STORE_INITIALIZED;
    }

    return (Rotten_Store*)&tap;
}

Rotten_Logger* rotten()
{
    static Rotten_Logger logger;
    static enum
    {
        ROTTEN_UNINITIALIZED,
        ROTTEN_INITIALIZED
    } initialized = ROTTEN_UNINITIALIZED;

    if (initialized == ROTTEN_INITIALIZED)
    {
        return &logger;
    }

    if (lock() != ROTTEN_SUCCESS)
    {
        return NULL;
    }

    if (initialized != ROTTEN_INITIALIZED)
    {
        static RingBufferStore rb = {.ring = NULL};
        ring_buffer_store_init(&rb);
        
        static IpcStore ipc = { NULL };
        ipc_store_init(&ipc);

        rotten_init(&logger, lock, unlock, time, TAP_Printf);

        /* These can be enabled later as needed. */
        logger.store_add(&logger, (Rotten_Store*)&rb, "rb", ROTTEN_STORE_ENABLED);
        logger.store_add(&logger, tap_store(), "tap", ROTTEN_STORE_ENABLED);
        logger.store_add(&logger, (Rotten_Store*)&ipc, "ipc", ROTTEN_STORE_DISABLED);

        initialized = ROTTEN_INITIALIZED;
    }
    unlock();

    return &logger;
}

Rotten_Logger* rotten_tap()
{
    static Rotten_Logger logger;
    static enum
    {
        ROTTEN_UNINITIALIZED,
        ROTTEN_INITIALIZED
    } initialized = ROTTEN_UNINITIALIZED;

    if (lock() != ROTTEN_SUCCESS && initialized != ROTTEN_INITIALIZED)
    {
        return NULL;
    }

    if (initialized != ROTTEN_INITIALIZED)
    {
        rotten_init(&logger, lock, unlock, time, TAP_Printf);
        logger.store_add(&logger, tap_store(), "tap", ROTTEN_STORE_ENABLED);
        initialized = ROTTEN_INITIALIZED;
    }
    unlock();

    return &logger;
}
