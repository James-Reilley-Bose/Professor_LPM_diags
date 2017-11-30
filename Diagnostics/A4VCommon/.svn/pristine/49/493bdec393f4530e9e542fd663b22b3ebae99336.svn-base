//
// UpdateModuleDsp.h
//

#ifndef _UPDATE_MODULE_DSP_H
#define _UPDATE_MODULE_DSP_H

#include "project.h"
#include "IPCBulk.h"
#include "UpdateManagerBlob.h"

#define DSP_UPDATE_TIMEOUT_MS 120000 // two minutes

typedef enum // DSP Update Substates
{
    UPDATE_DSP_NOT_ACTIVE,
    UPDATE_DSP_WAIT_READY,
    UPDATE_DSP_DOING_BULK,
    UPDATE_DSP_DONE,

    UPDATE_DSP_NUM_STATES,
} DSPUpdateSubstates_t;

typedef struct
{
    DSPUpdateSubstates_t State;
    uint8_t FilesSent;
    uint8_t TransferAttempts;
    BulkEndpoint_t Bulk;
    BOOL LastTransferSuccess;
} DSPUpdateState_t;

void UpdateModuleDsp_Init(void);
BOOL UpdateModuleDsp_HandleMessage(GENERIC_MSG_t* msg);
void UpdateModuleDsp_Start(UpdateBlobHeader* blob);
void UpdateModuleDsp_EnterDoDsp(void);
void UpdateModuleDsp_ExitDoDsp(void);

#endif // _UPDATE_MODULE_DSP_H
