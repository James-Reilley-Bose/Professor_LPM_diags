/*
  File  : IpcTx.c
  Title :
  Author  : Dillon Johnson
  Created : 04/10/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions to send IPC to a DMA handle. Sends block until completion.
===============================================================================
*/

#include "project.h"
#include "IpcProtocolLpm.h"
#include "IpcTx.h"
#include "driverLibCommon.h"
#include "IpcRouterTask.h"

/* Logs */
SCRIBE_DECL(ipc_tx);
SCRIBE_DECL(ipc);

uint32_t check1;
uint32_t check2;


/* Private functions */
static void IpcTxCallbackISR_DmaCommon(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read);
static BOOL IpcTx_waitForTransactionDone(EventGroupHandle_t IpcTxDmaEventHandle,
        EventBits_t bitsToWait, uint32_t timeoutInMs);


/*
 * @func SendIpc
 *
 * @brief Sends an IPC message using a preconfigured DMA handle.
 *
 * @param void *pvParameters - Pointer to the configuration for a specific IPC device.
 *
 * @return n/a
 */
void SendIpc(uint8_t dest, uint8_t* buffer, uint8_t length)
{
    //Clear bits from previos transactions
    xEventGroupClearBits(IpcInterfaceTable[dest]->uart.tx.TxDmaEventGroupHandle, (ebDMA_TRANSFER_COMPLETE_BIT | ebDMA_TRANSFER_ERROR_BIT));

    if (FALSE == DmaManager_SubmitTransfer(IpcInterfaceTable[dest]->uart.tx.DmaTxHandle, buffer, NULL,
                                           (uint32_t)&IpcInterfaceTable[dest]->uart.tx.RegisterBase->DR, length,
                                           TIMER_MSEC_TO_TICKS(IPC_WAIT_FOR_DMA_MS), IpcTxCallbackISR_DmaCommon,
                                           (void*) &IpcInterfaceTable[dest]->uart.tx, FALSE, &IpcInterfaceTable[dest]->uart.tx.DmaTxError))
    {
        LOG(ipc_tx, ROTTEN_LOGLEVEL_NORMAL, "Failed to submit DMA transfer: %s", IpcTaskNames[dest]);
        return;
    }

    LOG(ipc_tx, ROTTEN_LOGLEVEL_VERBOSE, "IPC sent: %s", IpcTaskNames[dest]);
    IpcLogPacket((IpcPacket_t*) buffer);


    if (FALSE == IpcTx_waitForTransactionDone(IpcInterfaceTable[dest]->uart.tx.TxDmaEventGroupHandle,
            ebDMA_TRANSFER_COMPLETE_BIT | ebDMA_TRANSFER_ERROR_BIT, IPC_WAIT_TRANSACTION_COMPLETE_MS))
    {
        DmaManager_CancelTransfer(IpcInterfaceTable[dest]->uart.tx.DmaTxHandle);
        LOG(ipc_tx, ROTTEN_LOGLEVEL_NORMAL, "Cancelled transaction: %s", IpcTaskNames[dest]);
    }
}

/*
 * @func CancelIpc
 *
 * @brief Sends CancelTransfer to free DMA resources when comms time out.
 *
 * @param dest - The DMA Transfer endpoint owner.
 *
 * @return n/a
 */
void CancelIpc(Ipc_Device_t dest)
{
    if (DmaManager_CancelTransfer(IpcInterfaceTable[dest]->uart.tx.DmaTxHandle))
    {
        LOG(ipc_tx, ROTTEN_LOGLEVEL_NORMAL, "Cancelled DMA transfer: %s", IpcTaskNames[dest]);
    }
}

/*
 * @func IpcTxCallbackISR_DmaCommon
 *
 * @brief Callback for DMA completion. Sets result bits.
 *
 * @param uint16_t bytesRemaining - number of bytes not sent
 * @param DMA_TRANSFER_RESULT_TYPE result - how'd the DMA transfer go?
 * @param void* vYourStateBlob - Tx Device information
 *
 * @return n/a
 */
static void IpcTxCallbackISR_DmaCommon(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read)
{
    IpcTxConfig_t* TxDeviceInfo = (IpcTxConfig_t*) vYourStateBlob;

    if (result.transferComplete)
    {
        driverSetEventBitsInISR(TxDeviceInfo->TxDmaEventGroupHandle,
                                ebDMA_TRANSFER_COMPLETE_BIT);
    }

    if (result.transferError)
    {
        driverSetEventBitsInISR(TxDeviceInfo->TxDmaEventGroupHandle,
                                ebDMA_TRANSFER_ERROR_BIT);
    }
}


/*
 * @func IpcTx_waitForTransactionDone
 *
 * @brief Blocks on an event group until the specified DMA transfer completes.
 *
 * @param EventGroupHandle_t IpcTxDmaEventHandle - the DMA handle we're waiting on
 * @param EventBits_t bitsToWait - which bits to wait on
 * @param uint32_t timeoutInMs - maximum wait time
 *
 * @return TRUE if the transaction completed
 */
static BOOL IpcTx_waitForTransactionDone(EventGroupHandle_t IpcTxDmaEventHandle,
        EventBits_t bitsToWait, uint32_t timeoutInMs)
{
    EventBits_t retval = xEventGroupWaitBits(IpcTxDmaEventHandle,
                         bitsToWait,
                         pdTRUE, // ClearOnExit,
                         pdFALSE, // WaitForAll Bits
                         TIMER_MSEC_TO_TICKS(timeoutInMs) );

    return ((retval & ebDMA_TRANSFER_COMPLETE_BIT) != 0);
}
