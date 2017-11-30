/*
    File    :   DmaManager.h
    Title   :
    Author  :   dx1007255
    Created :   01/29/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Interface headers of DMA Manager for STM32F2xx

===============================================================================
*/
#ifndef DMA_MANAGER_H
#define DMA_MANAGER_H

#include "platform_settings.h"


// Possible DMA channels (in alphabetic order).
// Maximum 128 channels, right now only the following are implemented.
typedef enum
{
    INTERNAL_FLASH_RX = 0,
    I2C1_RX,
    I2C1_TX,
    I2C2_RX,
    I2C2_TX,
    I2C3_RX,
    I2C3_TX,
    SPI1_RX,
    SPI1_TX,
    SPI2_RX,
    SPI2_TX,
    SPI3_RX,
    SPI3_TX,
    USART1_RX,
    USART1_TX,
    USART2_RX,
    USART2_TX,
    USART3_RX,
    USART3_TX,
    UART4_RX,
    UART4_TX,
    UART5_RX,
    UART5_TX,
    USART6_RX,
    USART6_TX,
    MAX_REQUESTS_SUPPORTED
} DMA_CHANNEL_SELECTION;

// Maximum DMA channels are 128, we have to modify it if
// number of supported peripherals exceeds memory capability
#define MAX_CONFIGURED_DMA_CHANNELS           MAX_REQUESTS_SUPPORTED

typedef enum
{
    DMA_MANAGER_NO_ERROR = 0,
    DMA_MANAGER_INVALID_CHANNEL_SELECTION,
    DMA_MANAGER_MAX_CHANNELS_EXCEEDED,
    DMA_MANAGER_INVALID_REQUEST_HANDLE,
    DMA_MANAGER_INVALID_ARGUMENT,
    DMA_MANAGER_CONFIGUE_FAILURE,
    DMA_MANAGER_STREAM_OCCUPIED,
    DMA_MANAGER_SUBMISSION_FAILURE,
    DMA_MANAGER_TRANSFER_TIMEOUT,
    DMA_MANAGER_MUTEX_ERROR,
    DMA_MANAGER_EVENT_GROUP_ERROR,
    DMA_MANAGER_STREAM_LOCKING_ERROR,
    DMA_MANAGER_STREAM_UNLOCKING_ERROR,
    DMA_MANAGER_STREAM_LOCKED_ERROR,
    DMA_MANAGER_DMA_NOT_READY_ERROR
} DMA_MANAGER_ERROR_CODE;

#pragma pack(1)

// Data structure used to request DMA
// Tips: directModeError is used when read from Peripheral;
//       fifoModeError is used in Memory-to-Memory transfer
typedef struct
{
    DMA_CHANNEL_SELECTION   selectedChannel;
    // Bit fields to select which status to care of
    //    1 = care, 0 = Don't care
    uint8_t  transferComplete : 1;        // Transfer Complete
    uint8_t  transferError : 1;           // Transfer Error
    uint8_t  halfTransfer : 1;            // Half Transfer Done
    uint8_t  directModeError : 1;         // Direct Mode Error
    uint8_t  fifoModeError : 1;           // FIFO Mode Error
    uint8_t  dmaPriority : 2;             // 0 = low; 1 = medium; 2 = high; 3 = very high
    uint8_t  doubleBuffer : 1;            // 0 = single; 1 = double
} DMA_MANAGER_REQUEST_TYPE;


// Bit fields of transfer result
//    1 = TRUE, 0 = FALSE
typedef struct
{
    uint8_t  transferComplete : 1;        // Transfer Complete
    uint8_t  transferError : 1;           // Transfer Error
    uint8_t  halfTransfer : 1;            // Half Transfer Done
    uint8_t  directModeError : 1;         // Direct Mode Error
    uint8_t  fifoModeError : 1;           // FIFO Mode Error
    uint8_t  reserved : 3;

} DMA_TRANSFER_RESULT_TYPE;

#pragma pack()


typedef void*   DMA_REQUEST_HANDLE_TYPE;

typedef void (*DmaCallback)(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                            void* vYourStateBlob, BOOL read);

DMA_REQUEST_HANDLE_TYPE dmaManager_ConfigureChannel(const DMA_MANAGER_REQUEST_TYPE* pDmaRequest,
        DMA_MANAGER_ERROR_CODE* pError);

BOOL dmaManager_DeleteChannel(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                              DMA_MANAGER_ERROR_CODE* pError);

BOOL DmaManager_SubmitTransfer(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                               uint8_t* pBuffer0, uint8_t* pBuffer1, uint32_t peripheralAddress,
                               const uint16_t length, uint16_t blockingTimeMs,
                               DmaCallback pCallbackISR,
                               void* vYourStateBlob, BOOL holdStream,
                               DMA_MANAGER_ERROR_CODE* pError);

BOOL DmaManager_LockStream(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                           DMA_MANAGER_ERROR_CODE* pError);

BOOL DmaManager_UnlockStream(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                             DMA_MANAGER_ERROR_CODE* pError);

BOOL DmaManager_SubmitTransferFromISR(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                                      uint8_t* pBuffer, const uint16_t length,
                                      DmaCallback pCallbackISR);

void DmaManager_ReleaseStreamFromISR(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle);

BOOL dmaManager_UserAssignStreamChannel(DMA_CHANNEL_SELECTION dmaPeripheral,
                                        uint8_t dmaController, uint8_t dmaStream, uint8_t dmaChannel);

BOOL DmaManager_CancelTransfer(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle);
BOOL DmaManager_GetCurrentBuffer(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                                 uint32_t* addr);
BOOL DmaManager_GetBytesRemaining(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle,
                                  uint32_t* left);

#endif // DMA_MANAGER_H
