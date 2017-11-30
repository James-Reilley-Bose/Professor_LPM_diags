//
// IpcVariant.c - Example file for functions a variant must implement to use IPC
//

#include "IPCRouterTask.h"
#include "IpcVariant.h"
#include "CECTask.h"
#include "DeviceModelTask.h"

SCRIBE_DECL(ipc);

static void IpcVariant_HandleF0HS(IpcPacket_t* packet);

IpcF0HealthStatusPayload_t F0HS = {0};

void IpcVariant_GenerateLPMHSPayload(IpcPacket_t* packet)
{
    // TODO -
}

BOOL IpcVariant_RoutePacket(IpcInterface_t* device, IpcPacket_t* packet)
{
    BOOL handled = TRUE;
    switch (packet->s.opcode)
    {
        case IPC_F0_H_S:
            IpcVariant_HandleF0HS(packet);
            break;
        case IPC_CEC_MSG:
            CECPostMsg(CEC_MESSAGE_ID_RX_Message_IPC, NOP_CALLBACK, (uint32_t) packet);
            break;
        case IPC_CEC_LOGICAL_ADDRESS:
            CECPostMsg(CEC_MESSAGE_ID_LogicalAddressAcquired, NOP_CALLBACK, (uint32_t) packet);
            break;
        case IPC_DSP_H_S:
            DeviceModel_IpcRecieve(packet);
            break;
        default:
            handled = FALSE;
            break;
    }
    return handled;
}

static void IpcVariant_HandleF0HS(IpcPacket_t* packet)
{
    IpcF0HealthStatusPayload_t* hs = (IpcF0HealthStatusPayload_t*) packet->s.data.w;
    memcpy(&F0HS, hs, sizeof(IpcF0HealthStatusPayload_t));
    IPCRouter_ReturnRxBuffer(packet);
}
