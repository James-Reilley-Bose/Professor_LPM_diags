/*
    File    :   DmaManagerInternal.h
    Title   :
    Author  :   dx1007255
    Created :   01/20/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Interface headers of DMA manager internals for STM32F2xx

===============================================================================
*/
#ifndef DMA_MANAGER_INTERNAL_H
#define DMA_MANAGER_INTERNAL_H

#include "platform_settings.h"
#include "DmaManager.h"
#include "stm32f2xx_dma.h"
#include "driverLibCommon.h"

#define NUMBER_DMA_CONTROLLERS             2
#define DMA_STREAMS_PER_CONTROLLER         8
#define DMA_CHANNELS_PER_STREAM            8

#define TOTAL_DMA_STREAMS  (NUMBER_DMA_CONTROLLERS * DMA_STREAMS_PER_CONTROLLER)

#define DMA_MANAGER_INTERNAL_TIMEOUT_MS    10   // ms

#define DMA_ALL_INTERRUPT_BITS (DMA_IT_TC | DMA_IT_TE | DMA_IT_HT | DMA_IT_FE | DMA_IT_DME)

#define DMA_EVENT_BITS_ALL_STREAMS      0xFFFF  // Total 16 streams     

#define MIN_REQUESTS_SUPPORTED          INTERNAL_FLASH_RX


// Due to the fact that the system has relatively more flash but less SRAM,
//      the design tries to save memory rather than save code size.
// Also use statically allocated data structures and arrays instead of using malloc.

typedef struct
{
    uint32_t DMA_IT_TCifx;      // Transfer Completion bit in interrupt
    uint32_t DMA_IT_TEifx;      // Transfer Error bit in interrupt
    uint32_t DMA_IT_HTifx;      // Transfer Error bit in interrupt
    uint32_t DMA_IT_DEifx;      // Direct Mode Error bit in interrupt
    uint32_t DMA_IT_FEifx;      // Frame Error Error bit in interrupt

    // Corresponding 5 polling flags
    uint32_t DMA_FLAG_TCIFx;
    uint32_t DMA_FLAG_TEIFx;
    uint32_t DMA_FLAG_HTIFx;
    uint32_t DMA_FLAG_DEIFx;
    uint32_t DMA_FLAG_FEIFx;

} DMA_FLAGS_PER_STREAM_t;


typedef struct
{
    DMA_Stream_TypeDef* DMAy_Streamx;   // DMA and Stream selection
    IRQn_Type irqNumber;                // IRQ numebr of the selected DMAy_Streamx
    INTERRUPT_FNCT_PTR dmaStreamIsr;    // ISR of this stream
} DMA_STREAM_DESCRIPTOR_t;


#pragma pack(1)

typedef struct
{
    uint32_t DMA_Channel;                // Channel selection
    uint16_t streamIndex : 3;            // DMA Stream number : 0 - 7
    uint16_t totalIndex :  4;            // 0 - 15, see note below
    uint16_t peripheralInc : 1;          // Peripheral address increment: 0 = No increment; 1 = increment
    uint16_t direction : 4;              // Transfer direction: 0 = P-M; 4 = M-P; 8 = M-M;
    uint16_t rx : 1;                     // 0 = TX transfer, 1 = RX transfer
    uint16_t holdStream : 1;             // 0 = Not hold Stream, 1 = Transfer hold stream
    uint16_t reserved : 2;               // Reserved
    // totalIndex is an indication of which DMA controller the channel belongs to
    // DMA1: totalIndex = streamIndex = 0-7
    // DMA2: totalIndex = streamIndex + DMA_STREAMS_PER_CONTROLLER = 8-15;
} DMA_CHANNEL_INFO_t;


// Data structure used to configure DMA request
// Refer to DMA datasheet of STM32F2xx
typedef struct
{
    DMA_REQUEST_HANDLE_TYPE dmaRequestHandle;
    DMA_MANAGER_REQUEST_TYPE dmaRequest;// Copy of user request
    void (*pCallbackISR)(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                         void* vYourStateBlob, BOOL read);
    DMA_CHANNEL_INFO_t channelInfo;
    void* vYourStateBlob;
} DMA_REQUEST_INFO_t;

#pragma pack()

typedef struct
{
    DMA_REQUEST_INFO_t* pPreviousChannel;
    DMA_REQUEST_INFO_t* pCurrentChannel;
    DMA_REQUEST_INFO_t* pStreamLockOwner;
} DMA_STREAM_STATUS_t;


// Total 8 streams per DMA controller
static const DMA_FLAGS_PER_STREAM_t dmaStreamFlags[DMA_STREAMS_PER_CONTROLLER] =
{
    // Stream0
    {
        DMA_IT_TCIF0, DMA_IT_TEIF0, DMA_IT_HTIF0, DMA_IT_DMEIF0, DMA_IT_FEIF0,
        DMA_FLAG_TCIF0, DMA_FLAG_TEIF0, DMA_FLAG_HTIF0, DMA_FLAG_DMEIF0, DMA_FLAG_FEIF0
    },

    // Stream1
    {
        DMA_IT_TCIF1, DMA_IT_TEIF1, DMA_IT_HTIF1, DMA_IT_DMEIF1, DMA_IT_FEIF1,
        DMA_FLAG_TCIF1, DMA_FLAG_TEIF1, DMA_FLAG_HTIF1, DMA_FLAG_DMEIF1, DMA_FLAG_FEIF1
    },

    // Stream2
    {
        DMA_IT_TCIF2, DMA_IT_TEIF2, DMA_IT_HTIF2, DMA_IT_DMEIF2, DMA_IT_FEIF2,
        DMA_FLAG_TCIF2, DMA_FLAG_TEIF2, DMA_FLAG_HTIF2, DMA_FLAG_DMEIF2, DMA_FLAG_FEIF2
    },

    // Stream3
    {
        DMA_IT_TCIF3, DMA_IT_TEIF3, DMA_IT_HTIF3, DMA_IT_DMEIF3, DMA_IT_FEIF3,
        DMA_FLAG_TCIF3, DMA_FLAG_TEIF3, DMA_FLAG_HTIF3, DMA_FLAG_DMEIF3, DMA_FLAG_FEIF3
    },

    // Stream4
    {
        DMA_IT_TCIF4, DMA_IT_TEIF4, DMA_IT_HTIF4, DMA_IT_DMEIF4, DMA_IT_FEIF4,
        DMA_FLAG_TCIF4, DMA_FLAG_TEIF4, DMA_FLAG_HTIF4, DMA_FLAG_DMEIF4, DMA_FLAG_FEIF4
    },

    // Stream5
    {
        DMA_IT_TCIF5, DMA_IT_TEIF5, DMA_IT_HTIF5, DMA_IT_DMEIF5, DMA_IT_FEIF5,
        DMA_FLAG_TCIF5, DMA_FLAG_TEIF5, DMA_FLAG_HTIF5, DMA_FLAG_DMEIF5, DMA_FLAG_FEIF5
    },

    // Stream6
    {
        DMA_IT_TCIF6, DMA_IT_TEIF6, DMA_IT_HTIF6, DMA_IT_DMEIF6, DMA_IT_FEIF6,
        DMA_FLAG_TCIF6, DMA_FLAG_TEIF6, DMA_FLAG_HTIF6, DMA_FLAG_DMEIF6, DMA_FLAG_FEIF6
    },

    // Stream7
    {
        DMA_IT_TCIF7, DMA_IT_TEIF7, DMA_IT_HTIF7, DMA_IT_DMEIF7, DMA_IT_FEIF7,
        DMA_FLAG_TCIF7, DMA_FLAG_TEIF7, DMA_FLAG_HTIF7, DMA_FLAG_DMEIF7, DMA_FLAG_FEIF7
    }
};

static DMA_REQUEST_INFO_t* dmaManagerGetInfoFromHandle(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle);
static DMA_REQUEST_INFO_t* dmaManagerGetInfoFromHandleInInterrupt(DMA_REQUEST_HANDLE_TYPE dmaRequestHandle);

static BOOL dmaMangerPrimeChannel(const DMA_MANAGER_REQUEST_TYPE* pDmaRequest,
                                  DMA_REQUEST_INFO_t*  pDmaRequestInfo, DMA_MANAGER_ERROR_CODE* pError);

static BOOL dmaManagerPrepareTransfer(DMA_REQUEST_INFO_t* pDmaRequestInfo,
                                      const DMA_STREAM_DESCRIPTOR_t* pStreamDescriptor);

static BOOL dmaManagerWaitDmaReady(DMA_Stream_TypeDef* DMAy_Streamx, uint16_t timeoutInMs);

static BOOL dmaManagerAcquireStream(DMA_REQUEST_INFO_t* pDmaRequestInfo, uint16_t blockingTimeMs);

static void dmaManagerReleaseStream(DMA_REQUEST_INFO_t* pDmaRequestInfo, BOOL isStreamOwner);

static inline void freeStreamInsideISR(DMA_REQUEST_INFO_t* pDmaRequestInfo);

static inline BOOL amIThisDmaStreamOwner(DMA_REQUEST_INFO_t* pDmaRequestInfo);

// Each stream has its ISR handler
static void dma1Stream0IsrHandler(void);
static void dma1Stream1IsrHandler(void);
static void dma1Stream2IsrHandler(void);
static void dma1Stream3IsrHandler(void);
static void dma1Stream4IsrHandler(void);
static void dma1Stream5IsrHandler(void);
static void dma1Stream6IsrHandler(void);
static void dma1Stream7IsrHandler(void);
static void dma2Stream0IsrHandler(void);
static void dma2Stream1IsrHandler(void);
static void dma2Stream2IsrHandler(void);
static void dma2Stream3IsrHandler(void);
static void dma2Stream4IsrHandler(void);
static void dma2Stream5IsrHandler(void);
static void dma2Stream6IsrHandler(void);
static void dma2Stream7IsrHandler(void);

// Common to all ISRs
static void dmaManagerCommonIsrHandler(DMA_Stream_TypeDef* DMAy_Streamx,
                                       DMA_REQUEST_INFO_t* pActiveChannel);

static const DMA_InitTypeDef defaultDmaInitCondition =
{
    0,                              // DMA_Channel
    0,                              // DMA_PeripheralBaseAddr
    0,                              // DMA_Memory0BaseAddr
    DMA_DIR_PeripheralToMemory,     // DMA_DIR
    0,                              // DMA_BufferSize
    DMA_PeripheralInc_Disable,      // DMA_PeripheralInc
    DMA_MemoryInc_Disable,          // DMA_MemoryInc
    DMA_PeripheralDataSize_Byte,    // DMA_PeripheralDataSize
    DMA_MemoryDataSize_Byte,        // DMA_MemoryDataSize
    DMA_Mode_Normal,                // DMA_Mode
    DMA_Priority_Low,               // DMA_Priority
    DMA_FIFOMode_Disable,           // DMA_FIFOMode
    DMA_FIFOThreshold_1QuarterFull, // DMA_FIFOThreshold
    DMA_MemoryBurst_Single,         // DMA_MemoryBurst
    DMA_PeripheralBurst_Single      // DMA_PeripheralBurst
};

// Constant per stream data structure
// Total 16 streams: 0-7 for DMA1, 8-15 for DMA2
static const DMA_STREAM_DESCRIPTOR_t dmaStreamDescriptor[TOTAL_DMA_STREAMS] =
{
    {DMA1_Stream0, DMA1_Stream0_IRQn, dma1Stream0IsrHandler}, // DMA1_Stream0

    {DMA1_Stream1, DMA1_Stream1_IRQn, dma1Stream1IsrHandler}, // DMA1_Stream1

    {DMA1_Stream2, DMA1_Stream2_IRQn, dma1Stream2IsrHandler}, // DMA1_Stream2

    {DMA1_Stream3, DMA1_Stream3_IRQn, dma1Stream3IsrHandler}, // DMA1_Stream3

    {DMA1_Stream4, DMA1_Stream4_IRQn, dma1Stream4IsrHandler}, // DMA1_Stream4

    {DMA1_Stream5, DMA1_Stream5_IRQn, dma1Stream5IsrHandler}, // DMA1_Stream5

    {DMA1_Stream6, DMA1_Stream6_IRQn, dma1Stream6IsrHandler}, // DMA1_Stream6

    {DMA1_Stream7, DMA1_Stream7_IRQn, dma1Stream7IsrHandler}, // DMA1_Stream7


    {DMA2_Stream0, DMA2_Stream0_IRQn, dma2Stream0IsrHandler}, // DMA2_Stream0

    {DMA2_Stream1, DMA2_Stream1_IRQn, dma2Stream1IsrHandler}, // DMA2_Stream1

    {DMA2_Stream2, DMA2_Stream2_IRQn, dma2Stream2IsrHandler}, // DMA2_Stream2

    {DMA2_Stream3, DMA2_Stream3_IRQn, dma2Stream3IsrHandler}, // DMA2_Stream3

    {DMA2_Stream4, DMA2_Stream4_IRQn, dma2Stream4IsrHandler}, // DMA2_Stream4

    {DMA2_Stream5, DMA2_Stream5_IRQn, dma2Stream5IsrHandler}, // DMA2_Stream5

    {DMA2_Stream6, DMA2_Stream6_IRQn, dma2Stream6IsrHandler}, // DMA2_Stream6

    {DMA2_Stream7, DMA2_Stream7_IRQn, dma2Stream7IsrHandler}  // DMA2_Stream7

};

// Constant channel properties of periperals
static DMA_CHANNEL_INFO_t dmaChannelInfo[MAX_REQUESTS_SUPPORTED] =
{
    // channel, stream, totalIndex, peripheralIncrement, direction, rx, holdStream
    { DMA_Channel_1, 4, 12,  1, 8, 1, 0},  // Internal Flash Rx
    { DMA_Channel_1, 5,  5,  0, 0, 1, 0},  // I2C1 RX
    { DMA_Channel_1, 6,  6,  0, 4, 0, 0},  // I2C1 TX
    { DMA_Channel_7, 3,  3,  0, 0, 1, 0},  // I2C2 RX
    { DMA_Channel_7, 7,  7,  0, 4, 0, 0},  // I2C2 TX
    { DMA_Channel_3, 2,  2,  0, 0, 1, 0},  // I2C3 RX
    { DMA_Channel_3, 4,  4,  0, 4, 0, 0},  // I2C3 TX

    { DMA_Channel_3, 0,  8,  0, 0, 1, 0},  // SPI1 RX
    { DMA_Channel_3, 3, 11,  0, 4, 0, 0},  // SPI1 TX
    { DMA_Channel_0, 3,  3,  0, 0, 1, 0},  // SPI2 RX
    { DMA_Channel_0, 4,  4,  0, 4, 0, 0},  // SPI2 TX
    { DMA_Channel_0, 2,  2,  0, 0, 1, 0},  // SPI3 RX
    { DMA_Channel_0, 5,  5,  0, 4, 0, 0},  // SPI3 TX

    { DMA_Channel_4, 2, 10,  0, 0, 1, 0},  // USART1 RX
    { DMA_Channel_4, 7, 15,  0, 4, 0, 0},  // USART1 TX
    { DMA_Channel_4, 5,  5,  0, 0, 1, 0},  // USART2 RX
    { DMA_Channel_4, 6,  6,  0, 4, 0, 0},  // USART2 TX
    { DMA_Channel_4, 1,  1,  0, 0, 1, 0},  // USART3 RX
    { DMA_Channel_4, 3,  3,  0, 4, 0, 0},  // USART3 TX

    { DMA_Channel_4, 2,  2,  0, 0, 1, 0},  // UART4 RX
    { DMA_Channel_4, 4,  4,  0, 4, 0, 0},  // UART4 TX
    { DMA_Channel_4, 0,  0,  0, 0, 1, 0},  // UART5 RX
    { DMA_Channel_4, 7,  7,  0, 4, 0, 0},  // UART5 TX
    { DMA_Channel_5, 1,  9,  0, 0, 1, 0},  // USART6 RX
    { DMA_Channel_5, 6, 14,  0, 4, 0, 0}   // USART6 TX

};



#ifdef USE_DEBUG_ASSERT
static const char strFailDMAMutexCreate[] = {"DMA  Mutex create failed"};
static const char strFailDMAEventGroupCreate[] = {"DMA  Event Group create failed"};
static const char strFailDMAEventGroupSet[] = {"Set Event Group bits failed"};
#endif

#endif // DMA_MANAGER_INTERNAL_H
