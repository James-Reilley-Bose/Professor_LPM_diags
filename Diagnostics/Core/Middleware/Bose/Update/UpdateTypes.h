//
// UpdateTypes.h
//

#ifndef UPDATE_TYPES_H
#define UPDATE_TYPES_H

#include "EnumGenerator.h"

#define INVALID_UPDATE_STATE UPDATE_NUM_STATES

#define FOREACH_UPDATERESULT(RESULT) \
    RESULT(UPDATE_SKIPPED) \
    RESULT(UPDATE_SKIPPED_VERSION) \
    RESULT(UPDATE_FILE_NOT_FOUND) \
    RESULT(UPDATE_BLOB_ERROR) \
    RESULT(UPDATE_COMPLETED) \
    RESULT(UPDATE_FAILED) \
    RESULT(UPDATE_TIMEOUT) \
    RESULT(UPDATE_NUM_RESULTS) \
    RESULT(INVALID_UPDATE_RESULT)

typedef enum
{
    FOREACH_UPDATERESULT(GENERATE_ENUM)
} UpdateResult_t;

typedef struct
{
    BOOL (*MessageHandler)(GENERIC_MSG_t*); // Handles messages while the state is active
    void (*Init)(void); // Initialize the state, called at boot time (update task creation)
    void (*EnterState)(void); // Called when state is entered for setup routines
    void (*ExitState)(void); // Called when state is exited for cleanup
    BOOL ignoreProgress; // Flag to indicate if progress should be ignored (eg. if the state is very
    // brief or state progress calculations aren't implemented yet.
} UpdateStateHandler_t;

#endif // UPDATE_TYPES_H
