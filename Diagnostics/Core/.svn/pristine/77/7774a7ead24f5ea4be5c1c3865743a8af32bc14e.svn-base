/*
    File    :   DmaManager.c
    Title   :
    Author  :   dx1007255
    Created :   01/29/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Implementation of DMA scheduler for STM32F2xx, the policy is
                       non-preemptive, i.e., a peripheral runs to completion once
                       having obtained a stream.
===============================================================================
*/
#include "project.h"

#include "DmaManagerInternal.h"
#include "SystemAnalysis.h"

// Mutex to protect global variable dmaConfiguredChannels
static xSemaphoreHandle s_DmaManagerConfigMutex;

// Variable per stream data structure, protected by event group bits
static DMA_STREAM_STATUS_t dmaStreamStatus[TOTAL_DMA_STREAMS];

// Per channel data structure
static DMA_REQUEST_INFO_t dmaConfiguredChannels[MAX_CONFIGURED_DMA_CHANNELS];

// Event Group handle for synchronization, a single Event Group is used to save resources.
// This depends on configUSE_16_BIT_TICKS is 0 (it is indeed now).
// Event bits map: bit 0-15 correspond to stream 0-15.
// Each bit protects both a physical stream and data (dmaStreamStatus) of the stream.
static EventGroupHandle_t s_dmaEventGroupHandle;

// This is to support SPI read-only operation without a write buffer
static const uint32_t dummyWrite = 0;

SCRIBE_DECL(system);

/*****************************************************************//**
* @brief    Configure a DMA channel, so it becomes ready to transfer data
*           No transfer has been initiated after this configuration.
*
* @param    pDmaRequest - Pointer to data structure to configure a DMA channel
* @param    pError        - Pointer to error code
*
* @return   Handle of DMA_REQUEST_HANDLE_TYPE on success;
*           NULL otherwise, error code is poitned to by pError.
**********************************************************************/
DMA_REQUEST_HANDLE_TYPE dmaManager_ConfigureChannel(const DMA_MANAGER_REQUEST_TYPE* pDmaRequest,
        DMA_MANAGER_ERROR_CODE* pError)
{
    // Validate input pointer
    if (pDmaRequest == NULL)
    {
        *pError = DMA_MANAGER_INVALID_ARGUMENT;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_CC: Invalid arg");
        return NULL;
    }

    // Check if request is in range
    if ( (pDmaRequest->selectedChannel < MIN_REQUESTS_SUPPORTED) ||
            (pDmaRequest->selectedChannel >= MAX_REQUESTS_SUPPORTED))
    {
        *pError = DMA_MANAGER_INVALID_CHANNEL_SELECTION;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_CC: Invalid channel");
        return NULL;
    }

    portENTER_CRITICAL();
    // Create the global mutex only the first time
    if (s_DmaManagerConfigMutex == NULL)
    {
        s_DmaManagerConfigMutex = xSemaphoreCreateMutex();
        RTOS_assert ((s_DmaManagerConfigMutex != NULL), strFailDMAMutexCreate);
    }

    // Create an EventGroup
    if (s_dmaEventGroupHandle == NULL)
    {
        s_dmaEventGroupHandle = xEventGroupCreate();
        RTOS_assert(s_dmaEventGroupHandle, strFailDMAEventGroupCreate);

        // Init all streams available (all bits = 1)
        RTOS_assert( xEventGroupSetBits( s_dmaEventGroupHandle, DMA_EVENT_BITS_ALL_STREAMS)
                     == DMA_EVENT_BITS_ALL_STREAMS, strFailDMAEventGroupSet);
    }
    portEXIT_CRITICAL();

    // Hold mutex
    if (xSemaphoreTake(s_DmaManagerConfigMutex,
                       TIMER_MSEC_TO_TICKS(DMA_MANAGER_INTERNAL_TIMEOUT_MS)) == pdFALSE)
    {
        *pError = DMA_MANAGER_MUTEX_ERROR;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_CC: can't creat mutex");
        return NULL;
    }

    DMA_REQUEST_INFO_t* pDmaRequestInfo = NULL;

    // Get a free request slot
    for (uint8_t i = 0; i < MAX_CONFIGURED_DMA_CHANNELS; i++)
    {
        if ( dmaConfiguredChannels[i].dmaRequestHandle == 0)
        {
            pDmaRequestInfo = &dmaConfiguredChannels[i];
            pDmaRequestInfo->dmaRequestHandle = pDmaRequestInfo;
            break;
        }
    }

    // Free mutex
    xSemaphoreGive(s_DmaManagerConfigMutex);

    *pError = DMA_MANAGER_NO_ERROR;

    // Check if requests exceeds maximum
    if (pDmaRequestInfo == NULL)
    {
        *pError = DMA_MANAGER_MAX_CHANNELS_EXCEEDED;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_CC: No more channels");
    }
    else if (dmaMangerPrimeChannel(pDmaRequest, pDmaRequestInfo, pError) == FALSE)
    {
        memset(pDmaRequestInfo, 0, sizeof(DMA_REQUEST_INFO_t));
        pDmaRequestInfo = NULL;
    }

    return pDmaRequestInfo;
}


/*****************************************************************//**
* @brief    Delete a previously configured DMA channel
*
* @param    dmaRequestHandle  - DMA request handle
* @param    pError        - Pointer to error code
*
* @return   TRUE if deletion OK, FALSE otherwise, errors is pointed to by pError
**********************************************************************/
BOOL dmaManager_DeleteChannel(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                              DMA_MANAGER_ERROR_CODE* pError)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo = dmaManagerGetInfoFromHandle(dmaRequestHandle);

    if (pDmaRequestInfo == NULL)
    {
        *pError = DMA_MANAGER_INVALID_REQUEST_HANDLE;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_DC: Invalid handle");
        return FALSE;
    }

    if (dmaManagerAcquireStream(pDmaRequestInfo, DMA_MANAGER_INTERNAL_TIMEOUT_MS) == FALSE)
    {
        *pError = DMA_MANAGER_STREAM_OCCUPIED;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_DC: Can't get stream %d",
            pDmaRequestInfo->channelInfo.totalIndex);
        return FALSE;
    }

    // This channel/peripheral is the current owner of a stream,
    //  clean up the stream usage
    const DMA_STREAM_DESCRIPTOR_t* pStreamDescriptor =
        &dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex];

    DMA_Stream_TypeDef* DMAy_Streamx = pStreamDescriptor->DMAy_Streamx;

    DMA_Cmd(DMAy_Streamx, DISABLE);

    // Clear all possible pending interrupt bits
    DMA_FLAGS_PER_STREAM_t dmaFlags = dmaStreamFlags[pDmaRequestInfo->channelInfo.streamIndex];
    DMA_ClearITPendingBit(DMAy_Streamx, (dmaFlags.DMA_IT_TCifx) | (dmaFlags.DMA_IT_TEifx) |
                          (dmaFlags.DMA_IT_HTifx) | (dmaFlags.DMA_IT_FEifx) | (dmaFlags.DMA_IT_DEifx));

    // Disable
    DMA_ITConfig(DMAy_Streamx, DMA_ALL_INTERRUPT_BITS, DISABLE);

    dmaManagerReleaseStream(pDmaRequestInfo, TRUE);

    memset(pDmaRequestInfo, 0, sizeof(DMA_REQUEST_INFO_t));

    return TRUE;
}


/*****************************************************************//**
* @brief    Allows user to select DMA stream and channel for a given periperal
*
* @param   dmaPeripheral - Enumeration from DMA_CHANNEL_SELECTION
* @param   dmaController - 0 for DMA1, 1 for DMA2
* @param   dmaStream     - 0 ~ 7 for stream 0 ~ 7
* @param   dmaChannel    - 0 ~ 7 for channel 0 ~ 7
*
* @return   TRUE if parameters are valid, FALSE otherwise,
**********************************************************************/
BOOL dmaManager_UserAssignStreamChannel(DMA_CHANNEL_SELECTION dmaPeripheral,
                                        uint8_t dmaController, uint8_t dmaStream, uint8_t dmaChannel)
{
    // Check if parameters are in range
    if ( (dmaPeripheral < MIN_REQUESTS_SUPPORTED) ||
            (dmaPeripheral >= MAX_REQUESTS_SUPPORTED) ||
            (dmaController >= NUMBER_DMA_CONTROLLERS) ||
            (dmaStream >= DMA_STREAMS_PER_CONTROLLER) ||
            (dmaChannel >= DMA_CHANNELS_PER_STREAM))
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_UASC Invalid param");
        return FALSE;
    }

    dmaChannelInfo[dmaPeripheral].DMA_Channel = (dmaChannel * 2) << 24;
    dmaChannelInfo[dmaPeripheral].streamIndex = dmaStream;
    dmaChannelInfo[dmaPeripheral].totalIndex = dmaStream +
            (dmaController * DMA_STREAMS_PER_CONTROLLER);

    return TRUE;
}


/*****************************************************************//**
* @brief    Submit a DMA transfer
*
* @param    dmaRequestHandle  - DMA request handle
* @param    pBuffer0 - pointer to the data buffer to transfer
* @param    pBuffer1 - pointer to data buffer (NULL if not using double buffer)
* @param    peripheralAddress  -  Peripheral address to assign
* @param    length   - number of bytes to transfer.
* @param    blockingTimeMs - Maximum waiting time in milli-seconds
* @param    pCallbackISR   - Callback function in ISR context, if enabled.
* @param    vYourStateBlob  - A piece of data the user will want at callback time.
* @param    holdStream - TRUE = Hold the stream; FALSE = Release the stream after transfer
* @param    pError         - Pointer to error code
*
* @return   TRUE if submission OK, FALSE otherwise, errors is pointed to by pError
**********************************************************************/
BOOL DmaManager_SubmitTransfer(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                               uint8_t* pBuffer0, uint8_t* pBuffer1, uint32_t peripheralAddress,
                               const uint16_t length, uint16_t blockingTimeMs,
                               DmaCallback pCallbackISR,
                               void* vYourStateBlob, BOOL holdStream,
                               DMA_MANAGER_ERROR_CODE* pError)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo = dmaManagerGetInfoFromHandle(dmaRequestHandle);

    if (pDmaRequestInfo == NULL)
    {
        *pError = DMA_MANAGER_INVALID_REQUEST_HANDLE;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_ST: Invalid handle");
        return FALSE;
    }

    // Give the User of DMA a way of identfying the state when the callback fires.
    pDmaRequestInfo->vYourStateBlob = vYourStateBlob;

    // Coordinate potential sharing of a stream by multiple channels
    if (dmaManagerAcquireStream(pDmaRequestInfo, blockingTimeMs) == FALSE)
    {
        *pError = DMA_MANAGER_STREAM_OCCUPIED;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_ST: Can't get stream %d",
            pDmaRequestInfo->channelInfo.totalIndex);
        return FALSE;
    }

    pDmaRequestInfo->channelInfo.holdStream = (holdStream == TRUE) ? 1 : 0;

    BOOL result = TRUE;

    const DMA_STREAM_DESCRIPTOR_t* pStreamDescriptor =
        &dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex];

    DMA_STREAM_STATUS_t* pDmaStreamStatus =
        &dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex];

    // If the current request uses different channel than the previous one,
    // the stream's configuration must be modified
    if ((pDmaStreamStatus->pCurrentChannel != pDmaStreamStatus->pPreviousChannel) &&
            (pDmaStreamStatus->pPreviousChannel != NULL))
    {

        result = dmaManagerPrepareTransfer(pDmaRequestInfo, pStreamDescriptor);
    }

    if (result == FALSE)
    {
        *pError = DMA_MANAGER_CONFIGUE_FAILURE;
        // Make the stream available to other requests
        dmaManagerReleaseStream(pDmaRequestInfo, TRUE);
        return FALSE;
    }

    pDmaRequestInfo->pCallbackISR = pCallbackISR;

    DMA_Stream_TypeDef* DMAy_Streamx = pStreamDescriptor->DMAy_Streamx;

    if (dmaManagerWaitDmaReady(DMAy_Streamx, blockingTimeMs) == FALSE)
    {
        // Make the stream available to other requests
        dmaManagerReleaseStream(pDmaRequestInfo, TRUE);
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_ST: DMA stream %d not ready",
            pDmaRequestInfo->channelInfo.totalIndex);
        return FALSE;
    }

    // Finish the remaining configuration and enable DMA to transfer
    // Adjust memory increment according to pBuffer NULL or not NULL
    uint32_t tmpreg = DMAy_Streamx->CR;

    // Clear MINC bits
    tmpreg &= ((uint32_t)~(DMA_SxCR_MINC));

    DMAy_Streamx->PAR = peripheralAddress;

    if (pBuffer0 != NULL)
    {
        DMAy_Streamx->M0AR = (uint32_t)pBuffer0;
        // Set MINC bit
        tmpreg |= DMA_MemoryInc_Enable;

        if ((pBuffer1 != NULL) && (pDmaRequestInfo->dmaRequest.doubleBuffer))
        {
            DMAy_Streamx->M1AR = (uint32_t)pBuffer1;
        }
    }
    else
    {
        DMAy_Streamx->M0AR = (uint32_t)&dummyWrite;
        tmpreg |= DMA_MemoryInc_Disable;
    }

    // Write to DMAy Streamx CR register
    DMAy_Streamx->CR = tmpreg;

    DMA_SetCurrDataCounter(DMAy_Streamx, length);
    DMA_Cmd(DMAy_Streamx, ENABLE);

    // Submission succeeded, don't release the stream now,
    // which will be released in ISR

    return TRUE;
}



/*****************************************************************//**
* @brief    Submit a DMA transfer from ISR, stream must be held by caller
*
* @param    dmaRequestHandle  - DMA request handle
* @param    pBuffer - pointer to the data buffer to transfer
* @param    length   - number of bytes to transfer.
* @param    pCallbackISR   - Callback function in ISR context, if enabled.
*
* @return   None
**********************************************************************/
BOOL DmaManager_SubmitTransferFromISR(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                                      uint8_t* pBuffer, const uint16_t length,
                                      DmaCallback pCallbackISR)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo = dmaManagerGetInfoFromHandleInInterrupt(dmaRequestHandle);

    if (!pDmaRequestInfo)
    {
        return FALSE;
    }

    dmaStreamStatus[pDmaRequestInfo->channelInfo.streamIndex].pCurrentChannel =
        dmaStreamStatus[pDmaRequestInfo->channelInfo.streamIndex].pPreviousChannel;

    DMA_Stream_TypeDef* DMAy_Streamx =
        dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex].DMAy_Streamx;

    pDmaRequestInfo->pCallbackISR = pCallbackISR;

    // Submit DMA transfer. If double buffer, figure out which address register to update.
    uint32_t volatile* MXAR = &DMAy_Streamx->M0AR;
    if ((DMAy_Streamx->CR & DMA_SxCR_DBM) && !(DMAy_Streamx->CR & DMA_SxCR_CT))
    {
        MXAR = &DMAy_Streamx->M1AR;
    }
    *MXAR = (uint32_t)pBuffer;
    DMAy_Streamx->NDTR = length;
    DMAy_Streamx->CR |= (uint32_t)DMA_SxCR_EN;

    return TRUE;
}

/*****************************************************************//**
 * @brief    Submit a DMA transfer from ISR, stream must be held by caller
 *
 * @param    dmaRequestHandle  - DMA request handle
 * @param    pBuffer - pointer to the data buffer to transfer
 * @param    length   - number of bytes to transfer.
 * @param    pCallbackISR   - Callback function in ISR context, if enabled.
 *
 * @return   None
 **********************************************************************/
BOOL DmaManager_GetCurrentBuffer(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                                 uint32_t* addr)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo =
        dmaManagerGetInfoFromHandleInInterrupt(dmaRequestHandle);

    if (!pDmaRequestInfo || !addr)
    {
        return FALSE;
    }

    DMA_Stream_TypeDef* DMAy_Streamx =
        dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex].DMAy_Streamx;

    uint32_t volatile* MXAR = &DMAy_Streamx->M0AR;
    if ((DMAy_Streamx->CR & DMA_SxCR_DBM) && !(DMAy_Streamx->CR & DMA_SxCR_CT))
    {
        MXAR = &DMAy_Streamx->M1AR;
    }
    *addr = *MXAR;

    return TRUE;
}

/*****************************************************************//**
 * @brief    Get # of bytes left in the current DMA transfer
 *
 * @param    dmaRequestHandle  - DMA request handle
 * @param    pLeft - # of bytes remaining
 *
 * @return   FALSE == invalid parameters, TRUE == success
 **********************************************************************/
BOOL DmaManager_GetBytesRemaining(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                                  uint32_t* left)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo =
        dmaManagerGetInfoFromHandleInInterrupt(dmaRequestHandle);

    if (!pDmaRequestInfo || !left)
    {
        return FALSE;
    }

    DMA_Stream_TypeDef* DMAy_Streamx =
        dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex].DMAy_Streamx;
    *left = DMAy_Streamx->NDTR;

    return TRUE;
}

/*****************************************************************//**
* @brief    Release the stream held from ISR.
*
* @param    dmaRequestHandle  - DMA request handle
*
* @return   None
**********************************************************************/
void DmaManager_ReleaseStreamFromISR(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo = dmaManagerGetInfoFromHandleInInterrupt(dmaRequestHandle);

    // Set the bit, i.e., make the stream available
    freeStreamInsideISR(pDmaRequestInfo);

    // Clear flag
    pDmaRequestInfo->channelInfo.holdStream = 0;

}

/*****************************************************************//**
* @brief    Lock the stream for the request/channel
*
* @param    dmaRequestHandle  - DMA request handle
* @param    pError        - Pointer to error code
*
* @return   TRUE if locked OK, FALSE otherwise, errors is pointed to by pError
**********************************************************************/
BOOL DmaManager_LockStream(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                           DMA_MANAGER_ERROR_CODE* pError)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo = dmaManagerGetInfoFromHandle(dmaRequestHandle);

    if (pDmaRequestInfo == NULL)
    {
        *pError = DMA_MANAGER_INVALID_REQUEST_HANDLE;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_LS: Invalid handle");
        return FALSE;
    }

    // Coordinate potential sharing of a stream by multiple channels
    if (dmaManagerAcquireStream(pDmaRequestInfo, DMA_MANAGER_INTERNAL_TIMEOUT_MS) == FALSE)
    {
        *pError = DMA_MANAGER_STREAM_OCCUPIED;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_LS: Can't get stream %d",
            pDmaRequestInfo->channelInfo.totalIndex);
        return FALSE;
    }

    BOOL retval = FALSE;

    DMA_REQUEST_INFO_t* pStreamOwner =
        dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex].pStreamLockOwner;

    if (pStreamOwner == NULL)
    {
        // The stream was not locked, so lock it
        dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex].pStreamLockOwner
            = dmaRequestHandle;
        retval = TRUE;
    }
    else if (dmaRequestHandle == pStreamOwner)
    {
        // The stream was locked but with the same owner
        retval = TRUE;
    }
    else
    {
        // Someone else already locked it
        *pError = DMA_MANAGER_STREAM_LOCKING_ERROR;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_LS: stream %d Already locked",
            pDmaRequestInfo->channelInfo.totalIndex);
    }

    // Make the stream available to other requests
    dmaManagerReleaseStream(pDmaRequestInfo, TRUE);

    return retval;
}

/*****************************************************************//**
* @brief    UnLock the stream for the request/channel
*
* @param    dmaRequestHandle  - DMA request handle
* @param    pError        - Pointer to error code
*
* @return   TRUE if unlocked OK, FALSE otherwise, errors is pointed to by pError
**********************************************************************/
BOOL DmaManager_US(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                   DMA_MANAGER_ERROR_CODE* pError)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo = dmaManagerGetInfoFromHandle(dmaRequestHandle);

    if (pDmaRequestInfo == NULL)
    {
        *pError = DMA_MANAGER_INVALID_REQUEST_HANDLE;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_US: Invalid handle");
        return FALSE;
    }

    // Coordinate potential sharing of a stream by multiple channels
    if (dmaManagerAcquireStream(pDmaRequestInfo, DMA_MANAGER_INTERNAL_TIMEOUT_MS) == FALSE)
    {
        *pError = DMA_MANAGER_STREAM_OCCUPIED;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_US: Can't get stream %d",
            pDmaRequestInfo->channelInfo.totalIndex);
        return FALSE;
    }

    BOOL retval = FALSE;

    if (dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex].pStreamLockOwner == NULL)
    {
        // The stream was NOT locked
        retval = TRUE;
    }
    else if (dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex].pStreamLockOwner
             == pDmaRequestInfo)
    {
        // The stream was locked by myself, so unlock it
        dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex].pStreamLockOwner = NULL;
        retval = TRUE;
    }
    else
    {
        // The stream was locked by someone else, error
        *pError = DMA_MANAGER_STREAM_UNLOCKING_ERROR;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_US: Not owner of stream %d",
            pDmaRequestInfo->channelInfo.totalIndex);
    }

    // Make the stream available to other requests
    dmaManagerReleaseStream(pDmaRequestInfo, TRUE);

    return retval;
}


/*****************************************************************//**
* @brief    Cancel an on-going transfer
*
* @param    dmaRequestHandle - Handle of DMA_REQUEST_HANDLE_TYPE
*
* @return   TRUE if valid handle is the stream owner, FALSE otherwise
**********************************************************************/
BOOL DmaManager_CancelTransfer(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle)
{
    // Get request info from handle
    DMA_REQUEST_INFO_t* pDmaRequestInfo = dmaManagerGetInfoFromHandle(dmaRequestHandle);

    if (pDmaRequestInfo == NULL)
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_CT: Invalid pointer");
        return FALSE;
    }

    // Only current stream owner can cancel existing transfer and release stream
    if (!amIThisDmaStreamOwner(pDmaRequestInfo))
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_CT: Not stream owner");
        return FALSE;
    }

    DMA_Stream_TypeDef* DMAy_Streamx =
        dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex].DMAy_Streamx;

    const DMA_STREAM_DESCRIPTOR_t* pStreamDescriptor =
        &dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex];


    // Disabling the stream will force a transfer completion interrupt that we want to ignore
    // Disable the interrupt at the NVIC level
    Interrupt_Disable(pStreamDescriptor->irqNumber);
    DMA_Cmd(DMAy_Streamx, DISABLE);

    // Wait for the stream to shut down
    if (dmaManagerWaitDmaReady(DMAy_Streamx, DMA_MANAGER_INTERNAL_TIMEOUT_MS) == FALSE)
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMgr_CT: Timed Out!!!");
        return FALSE;
    }

    // Now clear all of the stream interrupt flags
    DMA_FLAGS_PER_STREAM_t dmaFlags = dmaStreamFlags[pDmaRequestInfo->channelInfo.streamIndex];
    DMA_ClearITPendingBit(DMAy_Streamx, (dmaFlags.DMA_IT_TCifx) | (dmaFlags.DMA_IT_TEifx) |
                          (dmaFlags.DMA_IT_HTifx) | (dmaFlags.DMA_IT_FEifx) | (dmaFlags.DMA_IT_DEifx));
    // And clear the pending interrupt in the NVIC
    Interrupt_ClearPending(pStreamDescriptor->irqNumber);
    // Finally, re-enable the interrupt in the NVIC
    Interrupt_Enable(pStreamDescriptor->irqNumber,
                     configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);

    // Make the stream available to other requests
    dmaManagerReleaseStream(pDmaRequestInfo, TRUE);

    return TRUE;
}


/*****************************************************************//**
* @brief    Get DMA request info from a handle
*
* @param    dmaRequestHandle - Handle of DMA_REQUEST_HANDLE_TYPE
*
* @return   Pointer to DMA_REQUEST_INFO_t if valid, NULL otherwise
**********************************************************************/
static DMA_REQUEST_INFO_t* dmaManagerGetInfoFromHandle(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle)
{
    DMA_REQUEST_INFO_t* pDmaRequestHandle = NULL;

    if (xSemaphoreTake(s_DmaManagerConfigMutex,
                       TIMER_MSEC_TO_TICKS(DMA_MANAGER_INTERNAL_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaManagerGetInfoFromHandle: Mutex error");
        return NULL;
    }

    for (int i = 0; i < MAX_CONFIGURED_DMA_CHANNELS; i++)
    {
        if ((dmaRequestHandle == &dmaConfiguredChannels[i]) &&
                (dmaRequestHandle == dmaConfiguredChannels[i].dmaRequestHandle))
        {
            pDmaRequestHandle = (DMA_REQUEST_INFO_t*)dmaRequestHandle;
            break;
        }
    }

    // Free mutex
    xSemaphoreGive(s_DmaManagerConfigMutex);

    return pDmaRequestHandle;
}


/*****************************************************************//**
* @brief    Get DMA request info from a handle while in interrupt
*           context
*
* @param    dmaRequestHandle - Handle of DMA_REQUEST_HANDLE_TYPE
*
* @return   Pointer to DMA_REQUEST_INFO_t if valid, NULL otherwise
**********************************************************************/
static DMA_REQUEST_INFO_t* dmaManagerGetInfoFromHandleInInterrupt(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle)
{
    DMA_REQUEST_INFO_t* pDmaRequestHandle = NULL;

    for (int i = 0; i < MAX_CONFIGURED_DMA_CHANNELS; i++)
    {
        if ((dmaRequestHandle == &dmaConfiguredChannels[i]) &&
                (dmaRequestHandle == dmaConfiguredChannels[i].dmaRequestHandle))
        {
            pDmaRequestHandle = (DMA_REQUEST_INFO_t*)dmaRequestHandle;
            break;
        }
    }

    return pDmaRequestHandle;
}


/*****************************************************************//**
* @brief    Set up a channel based on user input and DMA internals
*
* @param    pDmaRequest - Pointer to DMA_MANAGER_REQUEST_TYPE structure
* @param    pDmaRequestInfo - Pointer to a given sturct of DMA_REQUEST_INFO_t
* @param    pError        - Pointer to error code
*
* @return   TRUE if set up OK, FALSE otherwise, errors is pointed to by pError
**********************************************************************/
static BOOL dmaMangerPrimeChannel(const DMA_MANAGER_REQUEST_TYPE* pDmaRequest,
                                  DMA_REQUEST_INFO_t*  pDmaRequestInfo,
                                  DMA_MANAGER_ERROR_CODE* pError)
{
    // Stream assignement and Channel distribution
    memcpy(&pDmaRequestInfo->channelInfo, &dmaChannelInfo[pDmaRequest->selectedChannel],
           sizeof(DMA_CHANNEL_INFO_t));

    // Copy user's config
    memcpy(&pDmaRequestInfo->dmaRequest, pDmaRequest, sizeof(DMA_MANAGER_REQUEST_TYPE));

    const DMA_STREAM_DESCRIPTOR_t* pStreamDescriptor =
        &dmaStreamDescriptor[pDmaRequestInfo->channelInfo.totalIndex];

    // Dont depend on reset value, explictly define them
    // DMA is the flow controller
    DMA_FlowControllerConfig(pStreamDescriptor->DMAy_Streamx,
                             DMA_FlowCtrl_Memory);

    if (dmaManagerAcquireStream(pDmaRequestInfo, DMA_MANAGER_INTERNAL_TIMEOUT_MS) == FALSE)
    {
        *pError = DMA_MANAGER_STREAM_OCCUPIED;
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "dmaMangerPrimeChannel: Can't get stream %d",
            pDmaRequestInfo->channelInfo.totalIndex);
        return FALSE;
    }

    // Do this only once, when the stream is used for first time
    if (dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex].pPreviousChannel == NULL)
    {
        // Register the ISR
        Interrupt_RegisterISR(pStreamDescriptor->irqNumber,
                              pStreamDescriptor->dmaStreamIsr);

        // Enable interupt of DMA transfer streams from NVIC
        Interrupt_Enable(pStreamDescriptor->irqNumber,
                         configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
    }

    BOOL retVal = dmaManagerPrepareTransfer(pDmaRequestInfo, pStreamDescriptor);

    // Make the stream available to other requests
    dmaManagerReleaseStream(pDmaRequestInfo, TRUE);

    return retVal;
}


/*****************************************************************//**
* @brief    Try to acquire access to a given DMA stream
*
* @param    pDmaRequestInfo - Pointer to a given sturct of DMA_REQUEST_INFO_t
* @param    blockingTimeMs - Maximum waiting time in milli-seconds
*
* @return   TRUE if stream is acquired, FALSE otherwise
**********************************************************************/
static BOOL dmaManagerAcquireStream(DMA_REQUEST_INFO_t* pDmaRequestInfo, uint16_t blockingTimeMs)
{
    uint8_t retry = 0;
    BOOL retval = FALSE;

    // Wait for event bit to set and then clear it
    EventBits_t ebitToWait = ( 1 << pDmaRequestInfo->channelInfo.totalIndex);

    DMA_STREAM_STATUS_t* pDmaStreamStatus =
        &dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex];

    // Retry 3 times
    while (retry++ < 3)
    {
        if ( (xEventGroupWaitBits(s_dmaEventGroupHandle, ebitToWait,
                                  pdTRUE, /* ClearOnExit */
                                  pdFALSE, // WaitFor one Bit
                                  TIMER_MSEC_TO_TICKS( blockingTimeMs) )& ebitToWait) != 0)
        {
            if ((pDmaStreamStatus->pCurrentChannel == NULL) ||
                    (pDmaStreamStatus->pStreamLockOwner == pDmaRequestInfo))
            {
                pDmaStreamStatus->pCurrentChannel = pDmaRequestInfo;
                retval = TRUE; // Got the stream
                break;
            }
            else
            {
                // Set the event bit only
                dmaManagerReleaseStream(pDmaRequestInfo, FALSE);
            }
        }
    }

    return retval;
}


/*****************************************************************//**
* @brief    Release the access right to a given DMA stream
*           The caller must ensure the stream is occupied by this request.
*
* @param    pDmaRequestInfo - Pointer to a given sturct of DMA_REQUEST_INFO_t
* @param    isStreamOwner - TRUE = Caller owns the stream, FALSE = Caller is not the owner
*
**********************************************************************/
static void dmaManagerReleaseStream(DMA_REQUEST_INFO_t* pDmaRequestInfo, BOOL isStreamOwner)
{
    // Set the bit, i.e., make the stream available
    EventBits_t ebitToSet = ( 1 << pDmaRequestInfo->channelInfo.totalIndex);

    if (isStreamOwner)
    {
        DMA_STREAM_STATUS_t* pDmaStreamStatus =
            &dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex];

        pDmaStreamStatus->pPreviousChannel = pDmaStreamStatus->pCurrentChannel;

        pDmaStreamStatus->pCurrentChannel = NULL;
    }
    xEventGroupSetBits( s_dmaEventGroupHandle, ebitToSet );
}

/*****************************************************************//**
* @brief    Check if current request/peripheral is the stream owner
*
* @param    pDmaRequestInfo - Pointer to a request data structure
*
* @return   TRUE for owner, FALSE for not owner
**********************************************************************/
static inline BOOL amIThisDmaStreamOwner(DMA_REQUEST_INFO_t* pDmaRequestInfo)
{
    // Event bit to check
    EventBits_t ebitToCheck = ( 1 << pDmaRequestInfo->channelInfo.totalIndex);

    // Get current bit pattern
    EventBits_t ebitCurrentPattern = xEventGroupGetBits(s_dmaEventGroupHandle);

    if (ebitToCheck & (~ebitCurrentPattern))
    {
        if (dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex].pCurrentChannel
                == pDmaRequestInfo)
        {
            return TRUE;
        }
    }

    return FALSE;
}


/*****************************************************************//**
* @brief    Loop to wait DMA stream to be disabled - ready for next transfer
*
* @param    DMAy_Streamx - The DMA stream to wait for
* @param    timeoutInMs  - Timeout value in ms
*
* @return   TRUE if Ready before timedout, FALSE otherwise
**********************************************************************/
static BOOL dmaManagerWaitDmaReady(DMA_Stream_TypeDef* DMAy_Streamx, uint16_t timeoutInMs)
{
    uint32_t TimeoutStartTimeMs = GET_SYSTEM_UPTIME_MS();
    while (DMA_GetCmdStatus(DMAy_Streamx) == ENABLE)
    {
        if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= timeoutInMs))
        {
            return FALSE;
        }
    }
    return TRUE;
}



/*****************************************************************//**
* @brief    Prepare DMA for data transfer.
*           Memory address and buffer size will be set up later.
*
* @param    pDmaRequestInfo - Pointer to data structure to configure the  DMA
* @param    pStreamDescriptor - Pointer to stream descriptor data
*
* @return   TRUE for success, FALSE for error
**********************************************************************/
static BOOL dmaManagerPrepareTransfer(DMA_REQUEST_INFO_t* pDmaRequestInfo,
                                      const DMA_STREAM_DESCRIPTOR_t* pStreamDescriptor)
{
    // Set to default value
    DMA_InitTypeDef dmaInit;
    memcpy(&dmaInit, &defaultDmaInitCondition, sizeof(DMA_InitTypeDef));

    uint32_t dummy;
    DMA_Stream_TypeDef* DMAy_Streamx = pStreamDescriptor->DMAy_Streamx;

    // Change only if different from the default
    dmaInit.DMA_Channel = pDmaRequestInfo->channelInfo.DMA_Channel;
    dummy = pDmaRequestInfo->channelInfo.direction;
    dmaInit.DMA_DIR       = dummy << 4;
    if (pDmaRequestInfo->channelInfo.peripheralInc)
    {
        dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    }

    dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dummy = pDmaRequestInfo->dmaRequest.dmaPriority;

    dmaInit.DMA_Priority = dummy << 16;

    if (dmaInit.DMA_DIR == DMA_DIR_MemoryToMemory)
    {
        dmaInit.DMA_FIFOMode = DMA_FIFOMode_Enable;
    }

    // Reset to default
    DMA_DeInit(DMAy_Streamx);

    // As suggested by the Reference Manual, ensure DMA is disabled before configuring it
    if (dmaManagerWaitDmaReady(DMAy_Streamx, DMA_MANAGER_INTERNAL_TIMEOUT_MS) == FALSE)
    {
        return FALSE;
    }

    // configure it according to user input
    DMA_Init(DMAy_Streamx, &dmaInit);

    // Double Buffer Mode
    FunctionalState enable = pDmaRequestInfo->dmaRequest.doubleBuffer ? ENABLE : DISABLE;
    DMA_DoubleBufferModeCmd(pStreamDescriptor->DMAy_Streamx, enable);

    // Clear all possible pending interrupt bits before enable
    DMA_FLAGS_PER_STREAM_t dmaFlags = dmaStreamFlags[pDmaRequestInfo->channelInfo.streamIndex];
    DMA_ClearITPendingBit(DMAy_Streamx, (dmaFlags.DMA_IT_TCifx) | (dmaFlags.DMA_IT_TEifx) |
                          (dmaFlags.DMA_IT_HTifx) | (dmaFlags.DMA_IT_FEifx) | (dmaFlags.DMA_IT_DEifx));

    uint32_t DMA_IT = 0;

    // Enable DMA interrupts according to user input
    if (pDmaRequestInfo->dmaRequest.transferComplete)
    {
        DMA_IT |= DMA_IT_TC;
    }

    if (pDmaRequestInfo->dmaRequest.transferError)
    {
        DMA_IT |=  DMA_IT_TE;
    }

    if (pDmaRequestInfo->dmaRequest.halfTransfer)
    {
        DMA_IT |= DMA_IT_HT;
    }

    if (pDmaRequestInfo->dmaRequest.directModeError)
    {
        DMA_IT |=  DMA_IT_DME;
    }

    if (pDmaRequestInfo->dmaRequest.fifoModeError)
    {
        DMA_IT |=  DMA_IT_FE;
    }

    DMA_ITConfig(DMAy_Streamx, DMA_IT, ENABLE);

    return TRUE;
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream0
*       Because ISR defined in interrupt.h does not take any argument, each of the
*       16 DMA streams needs its own ISR handler, just for the purpose of passing
*       stream-specific arguments, since the handling algorithms are identical to
*       all streams. In below, DMA1 stream0 to stream7 use index 0-7; and
*       DMA2 stream0 to stream7 use index 8-15;
**********************************************************************/
static void dma1Stream0IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream0, dmaStreamStatus[0].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream1
*
**********************************************************************/
static void dma1Stream1IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream1, dmaStreamStatus[1].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream2
*
**********************************************************************/
static void dma1Stream2IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream2, dmaStreamStatus[2].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream3
*
**********************************************************************/
static void dma1Stream3IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream3, dmaStreamStatus[3].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream4
*
**********************************************************************/
static void dma1Stream4IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream4, dmaStreamStatus[4].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream5
*
**********************************************************************/
static void dma1Stream5IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream5, dmaStreamStatus[5].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream6
*
**********************************************************************/
static void dma1Stream6IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream6, dmaStreamStatus[6].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA1_Stream7
*
**********************************************************************/
static void dma1Stream7IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA1_Stream7, dmaStreamStatus[7].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA2_Stream0
*
**********************************************************************/
static void dma2Stream0IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream0, dmaStreamStatus[8].pCurrentChannel);
}

/*****************************************************************//**
* @brief    ISR of DMA2_Stream1
*
**********************************************************************/
static void dma2Stream1IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream1, dmaStreamStatus[9].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA2_Stream2
*
**********************************************************************/
static void dma2Stream2IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream2, dmaStreamStatus[10].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA2_Stream3
*
**********************************************************************/
static void dma2Stream3IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream3, dmaStreamStatus[11].pCurrentChannel);
}

/*****************************************************************//**
* @brief    ISR of DMA2_Stream4
*
**********************************************************************/
static void dma2Stream4IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream4, dmaStreamStatus[12].pCurrentChannel);
}

/*****************************************************************//**
* @brief    ISR of DMA2_Stream5
*
**********************************************************************/
static void dma2Stream5IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream6, dmaStreamStatus[13].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA2_Stream6
*
**********************************************************************/
static void dma2Stream6IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream6, dmaStreamStatus[14].pCurrentChannel);
}


/*****************************************************************//**
* @brief    ISR of DMA2_Stream7
*
**********************************************************************/
static void dma2Stream7IsrHandler(void)
{
    dmaManagerCommonIsrHandler(DMA2_Stream7, dmaStreamStatus[15].pCurrentChannel);
}



/*****************************************************************//**
* @brief    Interrupt service routine for all DMA transfers
*
* @param    DMAy_Streamx - Pointer to DMA y and Stream x
* @param    pActiveChannel - Pointer to current request
*
* @return   void
**********************************************************************/
static void dmaManagerCommonIsrHandler(DMA_Stream_TypeDef* DMAy_Streamx,
                                       DMA_REQUEST_INFO_t* pActiveChannel)
{
    INTERRUPT_RUN_LATENCY_BEGIN(DMA);

    DMA_FLAGS_PER_STREAM_t dmaFlags = dmaStreamFlags[pActiveChannel->channelInfo.streamIndex];

    DMA_TRANSFER_RESULT_TYPE dmaTransferResult = {0, 0, 0, 0, 0, 0};

    if (pActiveChannel->pCallbackISR)
    {
        // Prepare callback results only needed
        if (pActiveChannel->dmaRequest.transferComplete)
        {
            // Transfer complete
            if ( DMA_GetITStatus(DMAy_Streamx, dmaFlags.DMA_IT_TCifx) == SET)
            {
                dmaTransferResult.transferComplete = 1;
            }
        }

        if (pActiveChannel->dmaRequest.transferError)
        {
            // Transfer error
            if (DMA_GetITStatus(DMAy_Streamx, dmaFlags.DMA_IT_TEifx) == SET)
            {
                dmaTransferResult.transferError = 1;
            }
        }

        if (pActiveChannel->dmaRequest.halfTransfer)
        {
            // Half transfer
            if (DMA_GetITStatus(DMAy_Streamx, dmaFlags.DMA_IT_HTifx) == SET)
            {
                dmaTransferResult.halfTransfer = 1;
            }
        }

        if (pActiveChannel->dmaRequest.fifoModeError)
        {
            // FIFO Error
            if (DMA_GetITStatus(DMAy_Streamx, dmaFlags.DMA_IT_FEifx) == SET)
            {
                dmaTransferResult.fifoModeError = 1;
            }
        }

        if (pActiveChannel->dmaRequest.directModeError)
        {
            // Direct Mode Error
            if (DMA_GetITStatus(DMAy_Streamx, dmaFlags.DMA_IT_DEifx) == SET)
            {
                dmaTransferResult.directModeError = 1;
            }
        }

    }

    DMA_ClearITPendingBit(DMAy_Streamx, (dmaFlags.DMA_IT_TCifx) | (dmaFlags.DMA_IT_TEifx) |
                          (dmaFlags.DMA_IT_HTifx) | (dmaFlags.DMA_IT_FEifx) | (dmaFlags.DMA_IT_DEifx));

    if (pActiveChannel->pCallbackISR)
    {
        uint16_t remainingBytes = DMA_GetCurrDataCounter(DMAy_Streamx);
        pActiveChannel->pCallbackISR(remainingBytes, dmaTransferResult,
                                     pActiveChannel->vYourStateBlob, (pActiveChannel->channelInfo.rx == 1) ? TRUE : FALSE);
    }

    if ( pActiveChannel->channelInfo.holdStream == 0)
    {
        // Set the bit, i.e., make the stream available
        freeStreamInsideISR(pActiveChannel);
    }

    INTERRUPT_RUN_LATENCY_END(DMA, 0);
}

/*****************************************************************//**
* @brief    Helper function to release a stream inside ISR
*
* @param    pDmaRequestInfo - Pointer to current request
*
* @return   void
**********************************************************************/
static inline void freeStreamInsideISR(DMA_REQUEST_INFO_t* pDmaRequestInfo)
{
    // Define the bit to set
    EventBits_t ebitToSet = ( 1 << pDmaRequestInfo->channelInfo.totalIndex);

    DMA_STREAM_STATUS_t* pDmaStreamStatus =
        &dmaStreamStatus[pDmaRequestInfo->channelInfo.totalIndex];

    pDmaStreamStatus->pPreviousChannel = pDmaStreamStatus->pCurrentChannel;

    pDmaStreamStatus->pCurrentChannel = NULL;

    driverSetEventBitsInISR( s_dmaEventGroupHandle, ebitToSet );
}
