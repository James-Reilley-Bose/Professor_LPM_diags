
#include "RivieraLPM_IpcProtocol.h"
#include "etapcoms.h"

void Diag_bus_test_command (CommandLine_t *CommandLine, uint32_t  *arg, BOOL *valid);
BOOL DiagsBusTestDSPTimerHandler(uint32_t testcnt);
BOOL DiagsBusTestF0TimerHandler(uint32_t testcnt);
BOOL DiagsBusTestPSOCTimerHandler(uint32_t testcnt);
BOOL DiagsBusTestIRBTimerHandler(uint32_t testcnt);
BOOL DiagsBusTestDisplay(void);
void Diags_HandleIPCMessage_DSP_H_S(IpcPacket_t* packet);
void Diags_HandleIPCMessage_F0_H_S(IpcPacket_t* packet);