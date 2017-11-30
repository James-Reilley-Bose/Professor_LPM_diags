/*
    File    :   SpiBus_API.c
    Title   :
    Author  :   dx1007255
    Created :   01/15/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   SPI bus master API implementation using interrupt for STM32F2xx

===============================================================================
*/
#include "project.h"
#include "SpiBusDriver.h"
#include "SystemAnalysis.h"

// Per bus driver info
static SPI_Bus_DriverInfo_t spiBusDriverInfo[MAX_NUMBER_SPI_BUSES];

// Event Group handle for synchronization, a single Event Group is used for all buses
// to save resources.
static EventGroupHandle_t s_spiEventGroupHandle;


/*****************************************************************//**
* @brief    Initializes spi bus driver
*
* @param    pSpiBusConfig - Pointer to SPI_Bus_Config_t structure
* @param    pError        - Pointer to API's error code
*
* @return   Handle of SPI_BUS_HANDLE_TYPE on success;
*           NULL otherwise, error code is poitned to by pError.
**********************************************************************/
SPI_BUS_HANDLE_TYPE SPIBus_Initialize(SPI_Bus_Config_t* pSpiBusConfig, SPI_API_ERROR_CODE* pError)
{
    if (isArgumentValid(pSpiBusConfig) == FALSE)
    {
        *pError = SPI_API_INVALID_CONFIG_PARAMETER;
        return NULL; // Invalid argument
    }

    SPI_Bus_DriverInfo_t* pSpiBusDriverInfo = &spiBusDriverInfo[(pSpiBusConfig->busNumber - 1 )];

    // Set up bus specific parameters
    spiBusSetupParameter(pSpiBusConfig, pSpiBusDriverInfo);

    const SPI_Bus_Descriptor_t* pSpiBusDescriptor =
        &spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex];

    SPI_TypeDef* SPIx = pSpiBusDescriptor->pSpiBus;

    // Reset SPI to default state
    SPI_I2S_DeInit( SPIx );

    // Configure master pins
    ConfigureGpioPin(pSpiBusDescriptor->mosiGpioBank, pSpiBusDescriptor->spiPinMOSI,
                     GPIO_MODE_AF_OUT_PP_100MHZ, pSpiBusDescriptor->spiGpioAlternateFunction);

    ConfigureGpioPin(pSpiBusDescriptor->misoGpioBank, pSpiBusDescriptor->spiPinMISO,
                     GPIO_MODE_AF_IN_FLOATING, pSpiBusDescriptor->spiGpioAlternateFunction );

    // Clock is output
    ConfigureGpioPin(pSpiBusDescriptor->clockGpioBank, pSpiBusDescriptor->spiPinCLK,
                     GPIO_MODE_AF_OUT_PP_100MHZ, pSpiBusDescriptor->spiGpioAlternateFunction );


    uint8_t alternateFunction = 0;

    if (pSpiBusConfig->chipSelectControl == CHIP_SELECT_CONTROL_HARDWARE)
    {
        // Alternate function is SPI
        alternateFunction =
            pSpiBusDescriptor->spiGpioAlternateFunction;
    }

    if (pSpiBusConfig->csGpioBank != NULL)
    {
        ConfigureGpioPin(pSpiBusConfig->csGpioBank, pSpiBusConfig->spiPinCS,
                         GPIO_MODE_OUT_PP, alternateFunction );

        // Pull it high
        GPIO_SetBits(pSpiBusConfig->csGpioBank, (0x0001) << pSpiBusConfig->spiPinCS);
    }

    SPI_Cmd(SPIx, DISABLE );

    // Initializes the SPI communication
    SPI_Init( SPIx, &pSpiBusDriverInfo->spiBusInitStruct);

    if (pSpiBusConfig->disableDMA)
    {
        // Regisetr and Enable interrupt of SPIbus from NVIC
        Interrupt_RegisterISR(pSpiBusDescriptor->spiIrqNumber,
                              pSpiBusDescriptor->spiBusTxeRxneHandler);
        Interrupt_Enable(pSpiBusDescriptor->spiIrqNumber,
                         configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
    }
    else
    {
        // Configure DMA channels
        DMA_MANAGER_ERROR_CODE dmaErrorCode;

        if ( pSpiBusDriverInfo->spiDmaRxHandle == NULL)
        {
            pSpiBusDriverInfo->spiDmaRxHandle = dmaManager_ConfigureChannel(
                                                    pSpiBusDriverInfo->pSpiDmaRxRequest, &dmaErrorCode);
        }

        if ( pSpiBusDriverInfo->spiDmaTxHandle == NULL)
        {
            pSpiBusDriverInfo->spiDmaTxHandle = dmaManager_ConfigureChannel(
                                                    pSpiBusDriverInfo->pSpiDmaTxRequest, &dmaErrorCode);
        }

        if ( (pSpiBusDriverInfo->spiDmaRxHandle == NULL) ||
                (pSpiBusDriverInfo->spiDmaTxHandle == NULL))
        {
            memset(pSpiBusDriverInfo, 0, sizeof(SPI_Bus_DriverInfo_t));
            *pError = SPI_API_DMA_CONFIG_ERROR;
            return NULL;
        }
    }

    portENTER_CRITICAL();
    // Create event group
    if (s_spiEventGroupHandle == NULL)
    {
        s_spiEventGroupHandle = xEventGroupCreate();
        RTOS_assert(s_spiEventGroupHandle, strFailSPIEventGroupCreate);
    }

    // Create mutex if it doesn't exist
    if (NULL == pSpiBusDriverInfo->s_SpiDriverMutex)
    {
        pSpiBusDriverInfo->s_SpiDriverMutex = xSemaphoreCreateMutex();
        RTOS_assert ((pSpiBusDriverInfo->s_SpiDriverMutex != NULL), strFailSPIMutexCreate);
    }
    portEXIT_CRITICAL();

    // Enable SPI bus
    SPI_Cmd(SPIx, ENABLE );

    pSpiBusDriverInfo->spiBusHandle = pSpiBusDriverInfo;

    *pError = SPI_API_NO_ERROR;
    return pSpiBusDriverInfo;
}

/*****************************************************************//**
* @brief    Transmit/receive, SPI hardware always read and write at the same time.
*           If you want to read without writting, set the write data pointer to zero.
*           If you want to write without reading, set the read data pointer to zero.
*           If you want a non-blocking transfer, DMA and interrupt must be enabled.
*           Non-blocking means to submit a transfer request without waiting result,
*               but the caller must use callback to know when the transfer is done;
*               Note that the callback is in ISR context, and thus should be as short as
*               possible, such as post a semaphore, and then the caller must call
*               SPIBus_CommunicateComplete to end the previously submitted request.
*           In all blocking requests, the callback function can be set to NULL.
*
* @param    pSpiBusHandle - Handle of SPI_BUS_HANDLE_TYPE
* @param    dataLength - Length of data to transfer (must be non-zero)
* @param    pDataToSend - Point to the data buffer to send
* @param    pDataReceived - Point to the data buffer to receive
* @param    pCallBackFunction - Pointer to callback function
* @param    blockingTimeMs - Maximum waiting time in milli-seconds, 0 means non-blocking
* @param    pError       - Pointer to API's error code
*
* @return   Number of bytes transferred on success,
*           0 otherwise, error code is poitned to by pError.
**********************************************************************/
uint16_t SPIBus_Communicate(SPI_BUS_HANDLE_TYPE spiBusHandle,
                            const uint16_t dataLength, uint8_t* pDataToSend, uint8_t* pDataReceived,
                            void (*pCallBackFunction)(uint16_t numberBytesTransferred, SPI_API_ERROR_CODE errorCode),
                            uint16_t blockingTimeMs, SPI_API_ERROR_CODE* pError )
{
    // Validate the handle
    SPI_Bus_DriverInfo_t* pSpiBusDriverInfo = spiGetDriverPointerFromHandle(spiBusHandle);

    if (pSpiBusDriverInfo == NULL)
    {
        *pError = SPI_API_INVALID_BUS_HANDLE;
        return 0;
    }

    // Validate input arguments
    if ( (dataLength == 0) || ((pDataToSend == NULL) && (pDataReceived == NULL)))
    {
        *pError = SPI_API_INVALID_ARGUMENT;
        return 0;
    }

    // Hold mutex
    if (xSemaphoreTake(pSpiBusDriverInfo->s_SpiDriverMutex,
                       TIMER_MSEC_TO_TICKS(blockingTimeMs)) == pdFALSE)
    {
        *pError = SPI_API_MUTEX_ERROR;
        return 0;
    }

    pSpiBusDriverInfo->transferLength = dataLength;
    pSpiBusDriverInfo->pBufSend = pDataToSend;
    pSpiBusDriverInfo->bytesSent = 0;
    pSpiBusDriverInfo->pBufGet = pDataReceived;
    pSpiBusDriverInfo->bytesGot = 0;
    pSpiBusDriverInfo->pCallbackISR_Spi = pCallBackFunction;

    uint8_t numberBitsToShift =
        pSpiBusDriverInfo->spiBusIndex * TOTAL_EVENT_GROUP_BITS_PER_BUS;

    // Reset synchronization for write
    xEventGroupClearBits(s_spiEventGroupHandle,
                         (EVENT_GROUP_BIT_MASK_PER_BUS << numberBitsToShift));

    *pError = SPI_API_NO_ERROR;

    // Assert chip select
    spiAssertSlaveSelect(spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus,
                         &pSpiBusDriverInfo->spiBusConfig);

    uint16_t retval = spiTransferInterrupt(pSpiBusDriverInfo, pDataToSend, pDataReceived,
                                           dataLength, blockingTimeMs, pError);

    if (blockingTimeMs) // blocking transfers
    {
        // De-assert chip select
        spiDeAssertSlaveSelect(spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus,
                               &pSpiBusDriverInfo->spiBusConfig);
        // Free mutex
        xSemaphoreGive(pSpiBusDriverInfo->s_SpiDriverMutex);
    }

    return retval;
}


/*****************************************************************//**
* @brief    Performs the post processing needed by Non-Blocking SPIBus_Communicate().
*
* @param    pSpiBusHandle - Handle of SPI_BUS_HANDLE_TYPE
* @param    pError       - Pointer to API's error code
*
* @return   TRUE on success, FALSE otherwise
**********************************************************************/
BOOL SPIBus_CommunicateComplete(SPI_BUS_HANDLE_TYPE spiBusHandle, SPI_API_ERROR_CODE* pError)
{
    // Validate the handle
    SPI_Bus_DriverInfo_t* pSpiBusDriverInfo = spiGetDriverPointerFromHandle(spiBusHandle);

    debug_assert(pSpiBusDriverInfo != NULL);

    *pError = SPI_API_NO_ERROR;

    spiConcludeTransfer(pSpiBusDriverInfo, pSpiBusDriverInfo->pBufSend,
                        pSpiBusDriverInfo->pBufGet, pSpiBusDriverInfo->transferLength,
                        SPI_DRIVER_INTERNAL_WAIT_TIME_MS, pError );

    // De-assert chip select
    spiDeAssertSlaveSelect(spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus,
                           &pSpiBusDriverInfo->spiBusConfig);

    // Free mutex
    xSemaphoreGive(pSpiBusDriverInfo->s_SpiDriverMutex);

    if (*pError == SPI_API_NO_ERROR)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


#ifdef SUPPORT_SPI_STAND_BY_MODE
/*****************************************************************//**
* @brief    Enter or Exit Stand_by mode -- Turning off clock to save power
*
* @param    pSpiBusHandle - Handle of SPI_BUS_HANDLE_TYPE
* @param    enter - TRUE to Enter Stand_by, FALSE to Exit (wake up)
* @param    pError        - Pointer to API's error code
*
* @return   TRUE on success, FALSE otherwise, error code is poitned to by pError.
**********************************************************************/
BOOL SPIBus_Standby(SPI_BUS_HANDLE_TYPE spiBusHandle, BOOL enter, SPI_API_ERROR_CODE* pError)
{
    // Validate the handle
    SPI_Bus_DriverInfo_t* pSpiBusDriverInfo = spiGetDriverPointerFromHandle(spiBusHandle);

    if (pSpiBusDriverInfo == NULL)
    {
        *pError = SPI_API_INVALID_BUS_HANDLE;
        return FALSE;
    }

    if (TRUE == enter)
    {
        // Hold mutex
        if (xSemaphoreTake(pSpiBusDriverInfo->s_SpiDriverMutex,
                           WAIT_FOR_SEMAPHORE_MUTEX_TICKS) == pdFALSE)
        {
            *pError = SPI_API_MUTEX_ERROR;
            return FALSE;
        }

        // Go into standby
        SPI_I2S_DeInit( spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus );

        spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].RCC_SetPeripheralClock(
            spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].spiPeripheralClock, DISABLE);
    }
    else
    {
        // Wake up by turning on clock first
        spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].RCC_SetPeripheralClock(
            spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].spiPeripheralClock, ENABLE);

        // Perform "light-weight" init
        SPI_I2S_DeInit( spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus );
        SPI_Cmd( spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus, DISABLE );
        SPI_Init( spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus,
                  &pSpiBusDriverInfo->spiBusInitStruct);
        SPI_Cmd( spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus, ENABLE );

        // Free mutex
        xSemaphoreGive(pSpiBusDriverInfo->s_SpiDriverMutex);
    }

    return TRUE;
}
#endif


/*****************************************************************//**
* @brief    Wait for status register bits to set
*
* @param    SPIx - Pointer to SPI_TypeDef
* @param    BitsToTest - Bit mask to test
* @param    waitingTimeMs - Waiting time in milli-seconds
* @param    clear - TRUE to wait for bits clear, FALSE to wait for bits to set
*
* @return   TRUE on success, FALSE otherwise
**********************************************************************/
static BOOL WaitForStatusRegisterBit(SPI_TypeDef* SPIx, uint16_t BitsToTest,
                                     uint16_t waitingTimeMs, BOOL clear)
{
    uint32_t TimeoutStartTimeMs = GET_SYSTEM_UPTIME_MS();
    uint16_t temp;
    BOOL retVal = TRUE;

    while ( 1 )
    {
        temp = SPIx->SR;

        if (clear)
        {
            // Test clear
            if (( temp & BitsToTest ) == 0)
            {
                break;
            }
        }
        else
        {
            // Test set
            if ( temp & BitsToTest )
            {
                break;
            }
        }

        if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= waitingTimeMs))
        {
            retVal = FALSE;
            break;
        }
    }

    return retVal;
}


/*****************************************************************//**
* @brief    Set up parameters for each SPI bus
*
* @param    pSpiBusConfig - Pointer to SPI_Bus_Config_t structure
* @param    pSpiBusDriverInfo - Pointer to a given sturct of SPI_Bus_DriverInfo_t
*
**********************************************************************/
static void spiBusSetupParameter(SPI_Bus_Config_t* pSpiBusConfig,
                                 SPI_Bus_DriverInfo_t* pSpiBusDriverInfo)
{
    // Save user config
    memcpy(&pSpiBusDriverInfo->spiBusConfig, pSpiBusConfig, sizeof(SPI_Bus_Config_t));

    pSpiBusDriverInfo->spiBusIndex = pSpiBusConfig->busNumber - 1;

    // Common settings depending on user's configuration
    pSpiBusDriverInfo->spiBusInitStruct.SPI_Mode = SPI_Mode_Master;
    pSpiBusDriverInfo->spiBusInitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    pSpiBusDriverInfo->spiBusInitStruct.SPI_DataSize = SPI_DataSize_8b;
    pSpiBusDriverInfo->spiBusInitStruct.SPI_CPOL =
        ((pSpiBusConfig->clockPolarity == 0) ? SPI_CPOL_Low : SPI_CPOL_High);

    pSpiBusDriverInfo->spiBusInitStruct.SPI_CPHA =
        ((pSpiBusConfig->clockPhase == 0) ? SPI_CPHA_1Edge : SPI_CPHA_2Edge);

    pSpiBusDriverInfo->spiBusInitStruct.SPI_BaudRatePrescaler =
        baudRateScalers[pSpiBusConfig->baudRateExponent];

    pSpiBusDriverInfo->spiBusInitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

    if (!pSpiBusConfig->disableDMA) // Using DMA
    {
        uint8_t offset = pSpiBusDriverInfo->spiBusIndex * DMA_CHANNELS_PER_SPI_BUS;

        pSpiBusDriverInfo->pSpiDmaRxRequest = &spiDmaRequests[offset + 0];
        pSpiBusDriverInfo->pSpiDmaTxRequest = &spiDmaRequests[offset + 1];

        // DMA callbacks
        pSpiBusDriverInfo->pCallbackISR_DmaRxTx = callbackISR_DmaCommon;
    }

    // Enable clock
    spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].RCC_SetPeripheralClock(
        spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].spiPeripheralClock, ENABLE);


    // Common settings depending on SPI bus number
    if (pSpiBusConfig->chipSelectControl == CHIP_SELECT_CONTROL_HARDWARE)
    {
        pSpiBusDriverInfo->spiBusInitStruct.SPI_NSS = SPI_NSS_Hard;
        SPI_SSOutputCmd(spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus, ENABLE);
    }
    else
    {
        pSpiBusDriverInfo->spiBusInitStruct.SPI_NSS = SPI_NSS_Soft;
    }
}


/*****************************************************************//**
* @brief    Validate a SPI bus handle
*
* @param    pSpiBusHandle - Handle of SPI_BUS_HANDLE_TYPE
*
* @return   Pointer to SPI_Bus_DriverInfo_t if valid, NULL otherwise
**********************************************************************/
static SPI_Bus_DriverInfo_t* spiGetDriverPointerFromHandle(SPI_BUS_HANDLE_TYPE spiBusHandle)
{
    if (spiBusHandle == NULL)
    {
        return NULL;
    }

    SPI_Bus_DriverInfo_t* pSpiBusDriverInfo = (SPI_Bus_DriverInfo_t*)spiBusHandle;

    if (spiBusHandle == spiBusDriverInfo[pSpiBusDriverInfo->spiBusIndex].spiBusHandle)
    {
        return pSpiBusDriverInfo;
    }

    return NULL;
}


/*****************************************************************//**
* @brief    Transmit/receive using interrupts.
*
* @param    pSpiBusDriverInfo - Pointer to SPI_Bus_DriverInfo_t
* @param    pDataToSend - Point to the data buffer to send
* @param    pDataReceived - Point to the data buffer to receive
* @param    dataLength - Length of data to transfer (must be non-zero)
* @param    blockingTimeMs - Maximum waiting time in milli-seconds, 0 means non-blocking
* @param    pError       - Pointer to API's error code
*
* @return   Number of bytes transferred on success,
*           0 otherwise, error code is poitned to by pError.
**********************************************************************/
static uint16_t spiTransferInterrupt(SPI_Bus_DriverInfo_t* pSpiBusDriverInfo,
                                     uint8_t* pDataToSend, uint8_t* pDataReceived,
                                     const uint16_t dataLength, uint16_t blockingTimeMs,
                                     SPI_API_ERROR_CODE* pError )
{
    SPI_TypeDef* SPIx = spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus;

    // Flush data register and clear any potential overrun error by a dummy read
    SPIx->DR;

    if (pSpiBusDriverInfo->spiBusConfig.disableDMA)
    {
        // Depend upon RXNE interrupt in full duplex mode
        SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);

        // Master transfers first byte, Wait for TXE flag to set
        if (WaitForStatusRegisterBit(SPIx, SPI_FLAG_TXE,
                                     SPI_DRIVER_INTERNAL_WAIT_TIME_MS, FALSE) == FALSE)
        {
            *pError = SPI_API_NOT_READY_FOR_TRANSFER;
            return 0;
        }
        if (pSpiBusDriverInfo->pBufSend)
        {
            SPIx->DR = *pSpiBusDriverInfo->pBufSend++;
        }
        else
        {
            SPIx->DR = 0;
        }
    }
    else
    {
        if (!spiSubmit_DmaTransfer(pSpiBusDriverInfo,
                                   pDataToSend, pDataReceived, dataLength, blockingTimeMs, pError))
        {
            return 0;
        }
    }

    if (blockingTimeMs == 0)
    {
        // Non-blocking transfer, return immediately
        return dataLength;
    }

    EventBits_t ebBitsToWait = (pSpiBusDriverInfo->spiBusConfig.disableDMA ?
                                (ebSPI_TRANSACTION_DONE_BIT <<
                                 (pSpiBusDriverInfo->spiBusIndex * TOTAL_EVENT_GROUP_BITS_PER_BUS)) :
                                ( ebDMA_TRANSFER_DONE_BIT <<
                                  (pSpiBusDriverInfo->spiBusIndex * TOTAL_EVENT_GROUP_BITS_PER_BUS)));

    // Wait for transfer done
    if ((xEventGroupWaitBits(s_spiEventGroupHandle,
                             ebBitsToWait,
                             pdTRUE, // ClearOnExit,
                             pdFALSE, // WaitFor 1 Bit only
                             TIMER_MSEC_TO_TICKS(blockingTimeMs)) & ebBitsToWait) != ebBitsToWait )
    {
        if (!pSpiBusDriverInfo->spiBusConfig.disableDMA)
        {
            // Always cancel Tx
            DmaManager_CancelTransfer(pSpiBusDriverInfo->spiDmaTxHandle);

            if (pDataReceived != NULL) // Both read/write
            {
                DmaManager_CancelTransfer(pSpiBusDriverInfo->spiDmaRxHandle);
            }
        }

        *pError = SPI_API_TRANSFER_TIMEOUT;
    }

    return spiConcludeTransfer(pSpiBusDriverInfo, pDataToSend, pDataReceived,
                               dataLength, SPI_DRIVER_INTERNAL_WAIT_TIME_MS, pError);

}

/*****************************************************************//**
* @brief    Submit a DMA transfer
*
* @param    pSpiBusDriverInfo - Pointer to SPI_Bus_DriverInfo_t
* @param    pDataToSend - Point to the data buffer to send
* @param    pDataReceived - Point to the data buffer to receive
* @param    dataLength - Length of data to transfer (must be non-zero)
* @param    blockingTimeMs - Maximum waiting time in milli-seconds, minimal 1
* @param    pError       - Pointer to API's error code
*
* @return   Number of bytes transferred on success,
*           0 otherwise, error code is poitned to by pError.
*
**********************************************************************/
static BOOL spiSubmit_DmaTransfer(SPI_Bus_DriverInfo_t* pSpiBusDriverInfo,
                                  uint8_t* pDataToSend, uint8_t* pDataReceived, const uint16_t dataLength,
                                  uint16_t blockingTimeMs, SPI_API_ERROR_CODE* pError )
{
    SPI_TypeDef* SPIx = spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus;
    uint16_t SPI_I2S_DMAReq;

    DMA_MANAGER_ERROR_CODE dmaError;

    // Master always submit Tx
    if (pDataReceived == NULL)
    {
        // Write only
        if (DmaManager_SubmitTransfer(pSpiBusDriverInfo->spiDmaTxHandle,
                                      pDataToSend, NULL, (uint32_t)&SPIx->DR, dataLength, blockingTimeMs,
                                      pSpiBusDriverInfo->pCallbackISR_DmaRxTx, (void*)pSpiBusDriverInfo,
                                      FALSE, &dmaError) == TRUE)
        {
            SPI_I2S_DMAReq = SPI_I2S_DMAReq_Tx;
        }
        else
        {
            *pError = SPI_API_DMA_SUBMIT_ERROR;
            return FALSE;
        }
    }
    else
    {
        // Submit both write and read transfer, but write has no callback
        if ((DmaManager_SubmitTransfer(pSpiBusDriverInfo->spiDmaTxHandle,
                                       pDataToSend, NULL, (uint32_t)&SPIx->DR, dataLength, blockingTimeMs,
                                       NULL, NULL,
                                       FALSE, &dmaError) == TRUE) &&
                (DmaManager_SubmitTransfer(pSpiBusDriverInfo->spiDmaRxHandle,
                                           pDataReceived, NULL, (uint32_t)&SPIx->DR, dataLength, blockingTimeMs,
                                           pSpiBusDriverInfo->pCallbackISR_DmaRxTx, (void*)pSpiBusDriverInfo,
                                           FALSE, &dmaError) == TRUE))
        {
            SPI_I2S_DMAReq = (SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx);
        }
        else
        {
            *pError = SPI_API_DMA_SUBMIT_ERROR;
            return FALSE;
        }
    }

    // Start transfer
    SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq, ENABLE);

    return TRUE;
}


/*****************************************************************//**
* @brief    Conclude a SPI transfer
*
* @param    pSpiBusDriverInfo - Pointer to SPI_Bus_DriverInfo_t
* @param    pSendBuf - pointer to the data to send
* @param    pReceiveBuf - pointer to the buffer to receive
* @param    length   - expected number of data bytes to transfer.
* @param    waitingTimeMs - Maximum waiting time in milli-seconds
* @param    pError       - Pointer to API's error code
*
* @return   Number of bytes actually transferred, error code is poitned to by pError.
**********************************************************************/
static uint16_t spiConcludeTransfer(SPI_Bus_DriverInfo_t* pSpiBusDriverInfo,
                                    uint8_t* pSendBuf, uint8_t* pReceiveBuf, uint16_t length,
                                    uint16_t waitingTimeMs, SPI_API_ERROR_CODE* pError)
{
    SPI_TypeDef* SPIx = spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus;

    if (pSpiBusDriverInfo->spiBusConfig.disableDMA)
    {
        // Non-DMA, disable SPI interrupt
        SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
    }
    else // Used DMA
    {
        // Wait for TXE flag to set, according to SPI data sheet when using DMA
        if (WaitForStatusRegisterBit(SPIx, SPI_FLAG_TXE, waitingTimeMs, FALSE) == FALSE)
        {
            *pError = SPI_API_TRANSFER_INCOMPLETE;
        }
    }

    // Wait for the busy flag to clear
    if (WaitForStatusRegisterBit(SPIx, SPI_FLAG_BSY, waitingTimeMs, TRUE) == FALSE)
    {
        *pError = SPI_API_BUS_BUSY;
    }

    if (!pSpiBusDriverInfo->spiBusConfig.disableDMA) // used DMA
    {
        // Disable DMA request
        SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, DISABLE);

        // Error handling
        if ((pSpiBusDriverInfo->pSpiDmaRxRequest->transferError) ||
                (pSpiBusDriverInfo->pSpiDmaTxRequest->transferError) )
        {
            EventBits_t ebBitsToCheck = (ebDMA_TRANSFER_ERROR_BIT <<
                                         (pSpiBusDriverInfo->spiBusIndex * TOTAL_EVENT_GROUP_BITS_PER_BUS));

            if ((xEventGroupGetBits(s_spiEventGroupHandle) & ebBitsToCheck) == ebBitsToCheck )
            {
                *pError = SPI_API_DMA_TRANSFER_ERROR;
            }
        }
    }

    if (*pError == SPI_API_NO_ERROR)
    {
        return length;
    }

    if (pReceiveBuf == NULL)
    {
        // Write-only
        return pSpiBusDriverInfo->bytesSent;
    }
    else
    {
        return pSpiBusDriverInfo->bytesGot;
    }
}


/*****************************************************************//**
* @brief    ISR for SPI1 RXNE
**********************************************************************/
static void spi1Bus_RxneHandler(void)
{
    spiCommon_RxneHandler(&spiBusDriverInfo[0]);
}


/*****************************************************************//**
* @brief    ISR for SPI2 RXNE
**********************************************************************/
static void spi2Bus_RxneHandler(void)
{
    spiCommon_RxneHandler(&spiBusDriverInfo[1]);
}


/*****************************************************************//**
* @brief    ISR for SPI3 RXNE
**********************************************************************/
static void spi3Bus_RxneHandler(void)
{
    spiCommon_RxneHandler(&spiBusDriverInfo[2]);
}



/*****************************************************************//**
* @brief    Common interrupt service routine for SPI RXNE
*
* @param    pSpiBusDriverInfo - Pointer to SPI_Bus_DriverInfo_t
*
* @return   void
**********************************************************************/
static void spiCommon_RxneHandler(SPI_Bus_DriverInfo_t* pSpiBusDriverInfo )
{
    INTERRUPT_RUN_LATENCY_BEGIN(SPI_RXNE);
    BOOL error = FALSE;

    SPI_TypeDef* SPIx = spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus;

    if (pSpiBusDriverInfo->pBufGet)
    {
        *pSpiBusDriverInfo->pBufGet++ = SPIx->DR;
    }
    else
    {
        SPIx->DR;    // dummy read
    }

    if (++pSpiBusDriverInfo->bytesSent < pSpiBusDriverInfo->transferLength)
    {
        if (SPIx->SR & SPI_FLAG_TXE)
        {
            if (pSpiBusDriverInfo->pBufSend)
            {
                SPIx->DR = *pSpiBusDriverInfo->pBufSend++;
            }
            else
            {
                SPIx->DR = 0;
            }
        }
        else
        {
            error = TRUE;
        }
    }

    if ((++pSpiBusDriverInfo->bytesGot >= pSpiBusDriverInfo->transferLength) || (error == TRUE))
    {
        SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);

        if (pSpiBusDriverInfo->pCallbackISR_Spi) // Non-blocking transfer
        {
            SPI_API_ERROR_CODE errorCode = SPI_API_NO_ERROR;
            if ((pSpiBusDriverInfo->bytesSent < pSpiBusDriverInfo->transferLength)
                    || (error == TRUE))
            {
                errorCode = SPI_API_TRANSFER_INCOMPLETE;
            }

            pSpiBusDriverInfo->pCallbackISR_Spi(pSpiBusDriverInfo->bytesSent, errorCode);
        }
        else
        {
            driverSetEventBitsInISR(s_spiEventGroupHandle, (ebSPI_TRANSACTION_DONE_BIT <<
                                    (pSpiBusDriverInfo->spiBusIndex * TOTAL_EVENT_GROUP_BITS_PER_BUS)));
        }
    }

    INTERRUPT_RUN_LATENCY_END(SPI_RXNE, 0);
}

/*****************************************************************//**
* @brief    Common DMA callback function used in interrupt mode
*
* @param    pSpiBusDriverInfo - Pointer to SPI_Bus_DriverInfo_t
* @param    bytesRemaining - Number of bytes that were not transferred
* @param    result - Result of transfer
* @param    vYourStateBlob - The spi driver info passed to DMA to keep track of state
* @param    read - TRUE for read, FALSE for write
*
* @return   None
**********************************************************************/
static void callbackISR_DmaCommon(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                                  void* vYourStateBlob, BOOL read)
{
    INTERRUPT_RUN_LATENCY_BEGIN(SPI_DMA);

    SPI_Bus_DriverInfo_t* pSpiBusDriverInfo = (SPI_Bus_DriverInfo_t*) vYourStateBlob;

    uint8_t numberBitsToShift = pSpiBusDriverInfo->spiBusIndex * TOTAL_EVENT_GROUP_BITS_PER_BUS;
    SPI_TypeDef* SPIx = spiBusDescriptor[pSpiBusDriverInfo->spiBusIndex].pSpiBus;

    SPI_API_ERROR_CODE errorCode = SPI_API_NO_ERROR;

    if (read)
    {
        pSpiBusDriverInfo->bytesGot = ( pSpiBusDriverInfo->transferLength - bytesRemaining);
        SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, DISABLE);
    }
    else
    {
        pSpiBusDriverInfo->bytesSent = ( pSpiBusDriverInfo->transferLength - bytesRemaining);
    }

    if (result.transferError)
    {
        if (bytesRemaining)
        {
            driverSetEventBitsInISR(s_spiEventGroupHandle,
                                    (ebDMA_TRANSFER_ERROR_BIT << numberBitsToShift));
            errorCode = SPI_API_DMA_TRANSFER_ERROR;
        }
    }

    if (pSpiBusDriverInfo->pCallbackISR_Spi)
    {
        pSpiBusDriverInfo->pCallbackISR_Spi(
            pSpiBusDriverInfo->transferLength - bytesRemaining, errorCode);
    }
    else if (result.transferComplete)
    {
        driverSetEventBitsInISR(s_spiEventGroupHandle,
                                (ebDMA_TRANSFER_DONE_BIT << numberBitsToShift));
    }

    INTERRUPT_RUN_LATENCY_END(SPI_DMA, 0);
}


/*****************************************************************//**
* @brief    Validate user config arguments
*
* @param    pSpiBusConfig - Pointer to SPI_Bus_Config_t structure
*
* @return   TRUE if valid FALSE otherwise
**********************************************************************/
static inline BOOL isArgumentValid(SPI_Bus_Config_t* pSpiBusConfig)
{
    // Ensure pointer is valid
    if (pSpiBusConfig == NULL)
    {
        return FALSE;
    }

    // Check bus number
    if ((pSpiBusConfig->busNumber < 1) || (pSpiBusConfig->busNumber > MAX_NUMBER_SPI_BUSES))
    {
        return FALSE;
    }

    // ChipSelect pin is needed when non-software control
    if ((pSpiBusConfig->chipSelectControl == CHIP_SELECT_CONTROL_HARDWARE) &&
            (pSpiBusConfig->csGpioBank == NULL))
    {
        return FALSE;
    }

    // ChipSelect pin must be NULL when API_User control
    if ((pSpiBusConfig->chipSelectControl == CHIP_SELECT_CONTROL_API_USERS) &&
            (pSpiBusConfig->csGpioBank != NULL))
    {
        return FALSE;
    }

    if (pSpiBusConfig->baudRateExponent >= SPI_MAX_NUMBER_PRESCALERS)
    {
        return FALSE;
    }

    return TRUE;
}



/*****************************************************************//**
* @brief    Assert Chip Select
*
* @param    SPIx - Pointer to SPI_Bus
* @param    pSpiBusConfig - Pointer to SPI_Bus_Config_t structure
*
* @return   None
**********************************************************************/
static inline void spiAssertSlaveSelect(SPI_TypeDef* SPIx, SPI_Bus_Config_t* pSpiBusConfig)
{
    if (pSpiBusConfig->chipSelectControl == CHIP_SELECT_CONTROL_API_DRIVER)
    {
        if (pSpiBusConfig->csGpioBank == NULL)
        {
            // Clear SSI bit
            SPI_NSSInternalSoftwareConfig(SPIx, SPI_NSSInternalSoft_Reset);
        }
        else
        {
            // Assert GPIO pin for CS
            GPIO_ResetBits(pSpiBusConfig->csGpioBank, (0x0001) << pSpiBusConfig->spiPinCS);
        }
    }
    // Do nothing if SPI hardware controll CS (NSS) or API user control
}


/*****************************************************************//**
* @brief    De-Assert Chip Select
*
* @param    SPIx - Pointer to SPI_Bus
* @param    pSpiBusConfig - Pointer to SPI_Bus_Config_t structure
*
* @return   None
**********************************************************************/
static inline void spiDeAssertSlaveSelect(SPI_TypeDef* SPIx, SPI_Bus_Config_t* pSpiBusConfig)
{
    if (pSpiBusConfig->chipSelectControl == CHIP_SELECT_CONTROL_API_DRIVER)
    {
        if (pSpiBusConfig->csGpioBank == NULL)
        {
            // Raise SSI bit
            SPI_NSSInternalSoftwareConfig(SPIx, SPI_NSSInternalSoft_Set);
        }
        else
        {
            // De Assert GPIO pin for CS
            GPIO_SetBits(pSpiBusConfig->csGpioBank, (0x0001) << pSpiBusConfig->spiPinCS);
        }
    }
    // Do nothing if SPI hardware controll CS (NSS) or API user control
}
