//
// IpcVariant.h - Header for functions a variant must implement to use IPC
//
#ifndef _IPC_VARIANT_H_
#define _IPC_VARIANT_H_

#include "IpcInterface.h"

// Msg RX
BOOL IpcVariant_RoutePacket(IpcInterface_t* device, IpcPacket_t* packet);

// Status
void IpcVariant_GenerateLPMHSPayload(IpcPacket_t* packet);

// Bulk Functions
BOOL IpcBulkVariant_GetQueueForBulkType(uint32_t bulkType, ManagedQ** qToSendTo);

#endif /* _IPC_VARIANT_H_ */
