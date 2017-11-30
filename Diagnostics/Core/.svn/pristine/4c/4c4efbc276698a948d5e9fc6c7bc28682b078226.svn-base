/*
File  : ipc_rx.c
Title :
Author  : jd71069
Created : 03/27/2015
Language: C
Copyright:  (C) 2015 Bose Corporation, Framingham, MA

Description:   Implementation of IPC RX using DMA

You must provide the RX buffers for the larger payloads.

When the Callback fires, you will get a pointer to the data, but must give
back a pointer for the next buffer. pThis way there's no copying out data here.

State Machine:

1. DMA RX 16 Bytes
2. Checksum 16 Bytes (Fail: Go to 1.)
3. Big Payload Waiting? Go to 4. Else Go to 6.
4. DMA RX Payload
5. Checksum Payload (Fail: Go to 1.)
6. Deliver Header and Payload (or NULL). Go to 1.
End

If you want the data to be guarded by mutex, simply create the mutex in the
IpcRxConfig_t. If it's null, we won't grab it. If it is initialized, we'll
use it.

===============================================================================
*/
#include "IpcRx.h"
#include "DmaManager.h"
#include "IpcInterface.h"
#include "IpcProtocolLpm.h"
#include "IpcRouterTask.h"
#include "rottenlog.h"
#include "SystemAnalysis.h"
#include "buffermanager.h"

#include "etap.h"

SCRIBE_DECL(ipc_rx);

/* Local Defines
===============================================================================
*/
#define RX_DMA_BLOCKING_TIME_MS 5
#define RX_BUFFER_HEADER_LEN 16
#define RX_DMA_MONITOR_TIMER_PERIOD_MS 50
#define GET_MICROS_NOW GET_MEASURE_TIME_NOW

/* Local Variable
===============================================================================
*/

/* Local Prototypes
===============================================================================
*/
static void IPC_RX_PacketCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read);
BOOL IPC_RX_ProperlyInitialized(IpcRxConfig_t* pThis);
void IPC_RX_GetPayloadFromSource(IpcRxConfig_t* pThis);
void IPC_RX_CheckDMAError(IpcInterface_t* pThis);

/*
===============================================================================
@func IPC_RX_ProperlyInitialized
@desc
===============================================================================
*/
BOOL IPC_RX_ProperlyInitialized(IpcRxConfig_t* pThis)
{
    return (pThis->DmaRxHandle != NULL &&
            pThis->RegisterBase != NULL &&
            pThis->_RXDoneCallback != NULL);
}

/*
===============================================================================
@func IPC_RX_StartService
@desc
===============================================================================
*/
void IPC_RX_StartService(IpcInterface_t* device)
{
    IpcRxConfig_t* pThis = &device->uart.rx;
    if (FALSE == pThis->_bServiceStarted)
    {
        if (TRUE == IPC_RX_ProperlyInitialized(pThis))
        {
            pThis->packet = IPCRouter_GetNextRxBuffer();
            pThis->nextPacket = IPCRouter_GetNextRxBuffer();

            DmaManager_LockStream(pThis->DmaRxHandle, &pThis->DmaRxError);

            DmaManager_SubmitTransfer(pThis->DmaRxHandle, (uint8_t*) pThis->packet, (uint8_t*) pThis->nextPacket, (uint32_t)&pThis->RegisterBase->DR, sizeof(IpcPacket_t),
                                      RX_DMA_BLOCKING_TIME_MS, IPC_RX_PacketCallback, device, TRUE, &pThis->DmaRxError);

            IPC_RX_CheckDMAError(device);

            pThis->_bServiceStarted = TRUE;

            LOG(ipc_rx, ROTTEN_LOGLEVEL_VERBOSE, "IPC Rx service started: %s", IpcTaskNames[device->DeviceID]);
        }
        else
        {
            LOG(ipc_rx, ROTTEN_LOGLEVEL_NORMAL, "Invalid IPC RX object: %s", IpcTaskNames[device->DeviceID]);
            debug_assert(0);
        }
    }
    else
    {
        LOG(ipc_rx, ROTTEN_LOGLEVEL_NORMAL, "Uh, the service is already started: %s", IpcTaskNames[device->DeviceID]);
    }
}

/*
===============================================================================
@func IPC_RX_RestartService
@param pThis
@desc
===============================================================================
*/
BOOL IPC_RX_RestartService(IpcInterface_t* device)
{
    return DmaManager_SubmitTransferFromISR(device->uart.rx.DmaRxHandle, (uint8_t*) device->uart.rx.nextPacket, sizeof(IpcPacket_t), IPC_RX_PacketCallback);
}

/*
===============================================================================
@func IPC_RX_StopService
@desc pThis boolean controls whether or not the service continues.
===============================================================================
*/
void IPC_RX_StopService(IpcInterface_t* device)
{
    LOG(ipc_rx, ROTTEN_LOGLEVEL_NORMAL, "IPC RX service stopped: %s", IpcTaskNames[device->DeviceID]);

    DmaManager_UnlockStream(device->uart.rx.DmaRxHandle, &device->uart.rx.DmaRxError);

    IPC_RX_CheckDMAError(device);

    device->uart.rx._bServiceStarted = FALSE;
}

/*
===============================================================================
@func IPC_RX_HeaderCallback
@desc DMA calls us back.
===============================================================================
*/
static void IPC_RX_PacketCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read)
{
    INTERRUPT_RUN_LATENCY_BEGIN(IPC);

    IpcInterface_t* device = (IpcInterface_t*) vYourStateBlob;
    IpcRxConfig_t* pThis = &device->uart.rx;
    debug_assert(TRUE == pThis->_bServiceStarted);
    pThis->_RXDoneCallback(device);
    pThis->packet = pThis->nextPacket;
    pThis->nextPacket = IPCRouter_GetNextRxBuffer();
    if (!IPC_RX_RestartService(device))
    {
        debug_assert(0);
    }

    INTERRUPT_RUN_LATENCY_END(IPC, 0);
}

/*
===============================================================================
@func IPC_RX_CheckDMAError
@desc helper function to check the error from DMA stuff and log it off.
===============================================================================
*/
void IPC_RX_CheckDMAError(IpcInterface_t* device)
{
    if (device->uart.rx.DmaRxError != DMA_MANAGER_NO_ERROR)
    {
        LOG(ipc_rx, ROTTEN_LOGLEVEL_NORMAL, "IPC Rx ERROR %d: %s", device->uart.rx.DmaRxError, IpcTaskNames[device->DeviceID]);
        device->stat.dmaErrors++;
    }
}

/*
===============================================================================
@func IPC_RX_DelayUsec
@desc Delay for (approximately) the specified number of microseconds
===============================================================================
*/
static void IPC_RX_DelayUsec(measure_time_t usec)
{
    measure_time_t start = GET_MICROS_NOW();

    while (DELTA_TIME(start, GET_MICROS_NOW()) < usec)
    {
        taskYIELD();
    }
}

/*
===============================================================================
@func IPC_RX_DmaMonitor
@desc Monitor all rx dma channels to see if any of them appear to be "lost"
===============================================================================
*/
// @115200, a byte time is 87usec, @921600 it's about 11 usec
#define DMA_CHECK_INTERVAL_USEC 120
#define DMA_CHECK_TIMES 3
void IPC_RX_DmaMonitor(void)
{
    int i, j;
    IpcInterface_t* iface;
    IpcRxConfig_t* rx;
    uint32_t r0, r1;

    for (i = 0; i < IPC_NUM_DEVICES; i++)
    {
        iface = IpcInterfaceTable[i];

        if (iface->type != IPC_INTERFACE_UART)
        {
            continue;
        }

        rx = &IpcInterfaceTable[i]->uart.rx;

        DmaManager_GetBytesRemaining(rx->DmaRxHandle, &r0);
        if ((r0 != 0) && (r0 != sizeof(IpcPacket_t)))
        {
            for (j = 0; j < DMA_CHECK_TIMES; j++)
            {
                IPC_RX_DelayUsec(DMA_CHECK_INTERVAL_USEC);
                DmaManager_GetBytesRemaining(rx->DmaRxHandle, &r1);
                if (r1 != r0)
                {
                    // DMA is moving, check back later
                    break;
                }
            }

            if (j == DMA_CHECK_TIMES)
            {
                rx->_RXDMAResetCount++;
                // DMA is stuck, we need to recover it
                LOG(ipc_rx, ROTTEN_LOGLEVEL_NORMAL,
                    "Interface %d is stuck (%d bytes, reset count %d)!!!\n", i, r0,
                    rx->_RXDMAResetCount);
                DmaManager_CancelTransfer(rx->DmaRxHandle);

                DmaManager_SubmitTransfer(rx->DmaRxHandle, (uint8_t*) rx->packet,
                                          (uint8_t*) rx->nextPacket, (uint32_t)&rx->RegisterBase->DR,
                                          sizeof(IpcPacket_t), RX_DMA_BLOCKING_TIME_MS,
                                          IPC_RX_PacketCallback, iface, TRUE, &rx->DmaRxError);
            }
        }
    }
}

/*
===============================================================================
@func IPCRxMonitor_TaskInit
@desc Initialize the IPC rx monitor task
===============================================================================
*/
void IPC_RX_Monitor_TaskInit(void* p)
{
}

/*
===============================================================================
@func IPCRxMonitor_TaskInit
@desc Monitor IPC interfaces for proper operation
===============================================================================
*/
void IPC_RX_MonitorTask(void* p)
{
    while ( 1 )
    {
        IPC_RX_DmaMonitor();
        vTaskDelay(TIMER_MSEC_TO_TICKS(RX_DMA_MONITOR_TIMER_PERIOD_MS));
    }
}
