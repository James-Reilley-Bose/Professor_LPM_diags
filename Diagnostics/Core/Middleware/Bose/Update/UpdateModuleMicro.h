//
// UpdateModuleMicro.h
//

#ifndef _UPDATE_MODULE_MICRO_H
#define _UPDATE_MODULE_MICRO_H

#include "project.h"
#include "UpdateManagerBlob.h"

typedef enum
{
    UPDATE_MICRO_NOT_ACTIVE,
    UPDATE_MICRO_CHECKING_STATUS,
    UPDATE_MICRO_REBOOTING,
    UPDATE_MICRO_NUM_STATES,
} MicroUpdateState_t;

void UpdateModuleMicro_Init(void);
BOOL UpdateModuleMicro_HandlePacket(GENERIC_MSG_t* msg);
void UpdateModuleMicro_EnterDoMicro(void);
void UpdateModuleMicro_Start(UpdateBlobHeader* blob);
void UpdateModuleMicro_EnterDoMicroMfg(void);
void UpdateModuleMicro_StartMfg(UpdateBlobHeader* blob);
void UpdateModuleMicro_EnterDoMicroBL(void);
void UpdateModuleMicro_StartBL(UpdateBlobHeader* blob);


#endif /* _UPDATE_MODULE_MICRO_H */
