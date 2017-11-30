/*
    File    :   SpiBusDriver.h
    Title   :
    Author  :   dx1007255
    Created :   01/12/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   SPI bus driver header for STM32F2xx

===============================================================================
*/
#ifndef SPI_BUS_DRIVER_H
#define SPI_BUS_DRIVER_H

#include "SpiBus_API.h"
#include "DmaManager.h"
#include "event_groups.h"
#include "driverLibCommon.h"


#define DMA_CHANNELS_PER_SPI_BUS     2

#define TOTAL_SPI_DMA_REQUESTS  (DMA_CHANNELS_PER_SPI_BUS * MAX_NUMBER_SPI_BUSES)

static const DMA_MANAGER_REQUEST_TYPE spiDmaRequests[TOTAL_SPI_DMA_REQUESTS] =
{
    {SPI1_RX, 1, 1, 0, 0, 0, 1, 0},
    {SPI1_TX, 1, 1, 0, 0, 0, 1, 0},
    {SPI2_RX, 1, 1, 0, 0, 0, 1, 0},
    {SPI2_TX, 1, 1, 0, 0, 0, 1, 0},
    {SPI3_RX, 1, 1, 0, 0, 0, 1, 0},
    {SPI3_TX, 1, 1, 0, 0, 0, 1, 0}
};

#pragma pack(1)
typedef struct
{
    GPIO_TypeDef* mosiGpioBank;   // Pointer to GPIO bank for MOSI of this SPI bus

    GPIO_TypeDef* misoGpioBank;   // Pointer to GPIO bank for MISO of this SPI bus

    GPIO_TypeDef* clockGpioBank;  // Pointer to clock GPIO bank used by this SPI bus

    uint16_t spiPinMOSI : 4;  // GPIO MOSI pin number

    uint16_t spiPinMISO : 4;  // GPIO MISO pin number

    uint16_t spiPinCLK : 4;  // GPIO Clock pin number

    uint16_t dummy : 4;      // reserved

    SPI_TypeDef* pSpiBus;

    uint32_t spiPeripheralClock;

    void (*RCC_SetPeripheralClock)(uint32_t RCC_APB2Periph, FunctionalState NewState);

    void (*spiBusTxeRxneHandler)(void);

    IRQn_Type spiIrqNumber;

    uint8_t spiGpioAlternateFunction;   // SPI GPIO alternate function
} SPI_Bus_Descriptor_t;
#pragma pack()

#ifndef SPI1_MISO_GPIO_BANK
#define SPI1_MISO_GPIO_BANK                     NULL
#endif

#ifndef SPI1_CLK_GPIO_BANK
#define SPI1_CLK_GPIO_BANK                      NULL
#endif

#ifndef SPI1_MOSI_GPIO_BANK
#define SPI1_MOSI_GPIO_BANK                     NULL
#endif

#ifndef SPI1_MOSI_GPIO_PIN
#define SPI1_MOSI_GPIO_PIN                      0xFF
#endif

#ifndef SPI1_MISO_GPIO_PIN
#define SPI1_MISO_GPIO_PIN                      0xFF
#endif

#ifndef SPI1_CLK_GPIO_PIN
#define SPI1_CLK_GPIO_PIN                       0xFF
#endif

#ifndef SPI2_MISO_GPIO_BANK
#define SPI2_MISO_GPIO_BANK                     NULL
#endif

#ifndef SPI2_CLK_GPIO_BANK
#define SPI2_CLK_GPIO_BANK                      NULL
#endif

#ifndef SPI2_MOSI_GPIO_BANK
#define SPI2_MOSI_GPIO_BANK                     NULL
#endif

#ifndef SPI2_MOSI_GPIO_PIN
#define SPI2_MOSI_GPIO_PIN                      0xFF
#endif

#ifndef SPI2_MISO_GPIO_PIN
#define SPI2_MISO_GPIO_PIN                      0xFF
#endif

#ifndef SPI2_CLK_GPIO_PIN
#define SPI2_CLK_GPIO_PIN                       0xFF
#endif

#ifndef SPI3_MISO_GPIO_BANK
#define SPI3_MISO_GPIO_BANK                     NULL
#endif

#ifndef SPI3_CLK_GPIO_BANK
#define SPI3_CLK_GPIO_BANK                      NULL
#endif

#ifndef SPI3_MOSI_GPIO_BANK
#define SPI3_MOSI_GPIO_BANK                     NULL
#endif

#ifndef SPI3_MOSI_GPIO_PIN
#define SPI3_MOSI_GPIO_PIN                      0xFF
#endif

#ifndef SPI3_MISO_GPIO_PIN
#define SPI3_MISO_GPIO_PIN                      0xFF
#endif

#ifndef SPI3_CLK_GPIO_PIN
#define SPI3_CLK_GPIO_PIN                       0xFF
#endif


static void spi1Bus_RxneHandler(void);
static void spi2Bus_RxneHandler(void);
static void spi3Bus_RxneHandler(void);

static const SPI_Bus_Descriptor_t spiBusDescriptor[MAX_NUMBER_SPI_BUSES] =
{
    // bus 1
    {
        SPI1_MOSI_GPIO_BANK,    // mosi gpio bank
        SPI1_MISO_GPIO_BANK,    // miso gpio bank
        SPI1_CLK_GPIO_BANK,     // clock gpio bank
        SPI1_MOSI_GPIO_PIN,     // 3 gpio pin numbers
        SPI1_MISO_GPIO_PIN,
        SPI1_CLK_GPIO_PIN,
        0,                      // dummy
        SPI1,
        RCC_APB2Periph_SPI1,
        RCC_APB2PeriphClockCmd,
        spi1Bus_RxneHandler,
        SPI1_IRQn,
        GPIO_AF_SPI1
    },
    // bus 2
    {
        SPI2_MOSI_GPIO_BANK,    // mosi gpio bank
        SPI2_MISO_GPIO_BANK,    // miso gpio bank
        SPI2_CLK_GPIO_BANK,     // clock gpio bank
        SPI2_MOSI_GPIO_PIN,     // 3 gpio pin numbers
        SPI2_MISO_GPIO_PIN,
        SPI2_CLK_GPIO_PIN,
        0,                      // dummy
        SPI2,
        RCC_APB1Periph_SPI2,
        RCC_APB1PeriphClockCmd,
        spi2Bus_RxneHandler,
        SPI2_IRQn,
        GPIO_AF_SPI2
    },
    // bus 3
    {
        SPI3_MOSI_GPIO_BANK,    // mosi gpio bank
        SPI3_MISO_GPIO_BANK,     // miso gpio bank
        SPI3_CLK_GPIO_BANK,      // clock gpio bank
        SPI3_MOSI_GPIO_PIN,      // 3 gpio pin numbers
        SPI3_MISO_GPIO_PIN,
        SPI3_CLK_GPIO_PIN,
        0,                       // dummy
        SPI3,
        RCC_APB1Periph_SPI3,
        RCC_APB1PeriphClockCmd,
        spi3Bus_RxneHandler,
        SPI3_IRQn,
        GPIO_AF_SPI3
    }
};


#pragma pack(1)
typedef struct
{
    SPI_BUS_HANDLE_TYPE spiBusHandle;   // SPI bus handle

    SPI_InitTypeDef spiBusInitStruct;   // from STM's library

    SPI_Bus_Config_t spiBusConfig;      // Copy of user config

    xSemaphoreHandle s_SpiDriverMutex;  // Mutex to protect the bus

    // DMA
    const DMA_MANAGER_REQUEST_TYPE* pSpiDmaRxRequest;
    const DMA_MANAGER_REQUEST_TYPE* pSpiDmaTxRequest;
    DMA_REQUEST_HANDLE_TYPE spiDmaRxHandle;
    DMA_REQUEST_HANDLE_TYPE spiDmaTxHandle;

    void (*pCallbackISR_Spi)(uint16_t numberBytesTransferred, SPI_API_ERROR_CODE errorCode);
    void (*pCallbackISR_DmaRxTx)(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                                 void* vYourStateBlob, BOOL read);

    uint8_t* pBufGet;
    uint8_t* pBufSend;
    uint16_t transferLength;
    uint16_t bytesSent;
    uint16_t bytesGot;

    uint8_t spiBusIndex : 2;        // 0, 1, 2, for "pointer"

    uint8_t reserved : 6;

} SPI_Bus_DriverInfo_t;
#pragma pack()

#define WAIT_FOR_SEMAPHORE_MUTEX_TICKS    100
#define SPI_DRIVER_INTERNAL_WAIT_TIME_MS        2

#define TOTAL_EVENT_GROUP_BITS_PER_BUS          3

#define EVENT_GROUP_BIT_MASK_PER_BUS            0x07

// Definitions for bits used in Event_Group
#define ebDMA_TRANSFER_DONE_BIT     ( 0x01UL )      //  BIT_0
#define ebDMA_TRANSFER_ERROR_BIT    ( 0x02UL )      //  BIT_1

#define ebSPI_TRANSACTION_DONE_BIT  ( 0x04UL )      //  BIT_2

#ifdef USE_DEBUG_ASSERT
static const char strFailSPIMutexCreate[] = {"SPI  Mutex create failed"};
static const char strFailSPIEventGroupCreate[] = {"SPI  Event Group create failed"};
#endif

#define SPI_MAX_NUMBER_PRESCALERS       8
static const uint16_t baudRateScalers[SPI_MAX_NUMBER_PRESCALERS] =
{
    SPI_BaudRatePrescaler_2, SPI_BaudRatePrescaler_4, SPI_BaudRatePrescaler_8,
    SPI_BaudRatePrescaler_16, SPI_BaudRatePrescaler_32, SPI_BaudRatePrescaler_64,
    SPI_BaudRatePrescaler_128, SPI_BaudRatePrescaler_256
};


// Prototypes
static SPI_Bus_DriverInfo_t* spiGetDriverPointerFromHandle(SPI_BUS_HANDLE_TYPE spiBusHandle);

static void spiBusSetupParameter(SPI_Bus_Config_t* pSpiBusConfig,
                                 SPI_Bus_DriverInfo_t* pSpiBusDriverInfo);

static BOOL WaitForStatusRegisterBit(SPI_TypeDef* SPIx, uint16_t BitsToTest,
                                     uint16_t blockingTimeMs, BOOL clear );

static uint16_t spiTransferInterrupt( SPI_Bus_DriverInfo_t* pSpiBusDriverInfo,
                                      uint8_t* pDataToSend, uint8_t* pDataReceived, const uint16_t dataLength,
                                      uint16_t blockingTimeMs, SPI_API_ERROR_CODE* pError );

static BOOL spiSubmit_DmaTransfer(SPI_Bus_DriverInfo_t* pSpiBusDriverInfo,
                                  uint8_t* pDataToSend, uint8_t* pDataReceived, const uint16_t dataLength,
                                  uint16_t blockingTimeMs, SPI_API_ERROR_CODE* pError );

static uint16_t spiConcludeTransfer(SPI_Bus_DriverInfo_t* pSpiBusDriverInfo,
                                    uint8_t* pSendBuf, uint8_t* pReceiveBuf, uint16_t length,
                                    uint16_t waitingTimeMs, SPI_API_ERROR_CODE* pError );

static void callbackISR_DmaCommon(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                                  void* vYourStateBlob, BOOL read);

static void spiCommon_RxneHandler(SPI_Bus_DriverInfo_t* pSpiBusDriverInfo );

static inline BOOL isArgumentValid(SPI_Bus_Config_t* pSpiBusConfig);
static inline void spiAssertSlaveSelect(SPI_TypeDef* SPIx, SPI_Bus_Config_t* pSpiBusConfig);
static inline void spiDeAssertSlaveSelect(SPI_TypeDef* SPIx, SPI_Bus_Config_t* pSpiBusConfig);

#endif // SPI_BUS_DRIVER_H



