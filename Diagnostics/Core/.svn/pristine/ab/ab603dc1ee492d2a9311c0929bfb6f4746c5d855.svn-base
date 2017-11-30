//
// IpcTest.h
//

#ifndef IPC_TEST_H
#define IPC_TEST_H

#include "TaskDefs.h"

void IpcTest_HandleMessage(GENERIC_MSG_t* message);
void IpcTest_HandlePacket(IpcInterface_t* device, IpcPacket_t* p);
uint16_t IpcTest_StartBulkTest(uint8_t dest,
                               uint8_t* data,
                               uint32_t length,
                               uint32_t type,
                               uint32_t FileID,
                               const char* FileName,
                               ManagedQ* Q);

#endif // IPC_TEST_H
