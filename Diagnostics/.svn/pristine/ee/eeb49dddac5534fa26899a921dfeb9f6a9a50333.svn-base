//
// SystemStateVariant.h
//
#include "SystemStateTask.h"

SystemStateHandler_t variantSystemStateHandlers[SYSTEM_STATE_NUM_OF] =
{
    // SYSTEM_STATE_OFF
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    // SYSTEM_STATE_BOOTING
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    // SYSTEM_STATE_NORMAL
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    // SYSTEM_STATE_RECOVERY
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    // SYSTEM_STATE_LOW_POWER
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    // SYSTEM_STATE_UPDATE
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    // SYSTEM_STATE_SHUTDOWN
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    // SYSTEM_STATE_FACTORY_DEFAULT
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
};


SystemStateHandler_t * SystemStateVariant_GetHandler(IpcLpmSystemState_t state)
{
    debug_assert((state >= SYSTEM_STATE_OFF) && (state < SYSTEM_STATE_NUM_OF));
    return &variantSystemStateHandlers[state];
}
