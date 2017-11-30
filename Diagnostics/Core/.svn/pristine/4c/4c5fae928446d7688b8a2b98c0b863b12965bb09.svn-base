/*
    File    :   i2cBusDriver.h
    Title   :
    Author  :   dx1007255
    Created :   12/26/2014
    Language:   C
    Copyright:  (C) 2014 Bose Corporation, Framingham, MA

    Description:   I2C bus driver internals

===============================================================================
*/
#ifndef I2C_BUS_DRIVER_H
#define i2C_BUS_DRIVER_H

#include "i2cMaster_API.h"
#include "DmaManager.h"
#include "event_groups.h"
#include "driverLibCommon.h"


#define MAX_NUMBER_I2C_BUSES            3
#define DMA_CHANNELS_PER_I2C_BUS        2

#define TOTAL_I2C_DMA_REQUESTS  (DMA_CHANNELS_PER_I2C_BUS * MAX_NUMBER_I2C_BUSES)

static const DMA_MANAGER_REQUEST_TYPE i2cDmaRequests[TOTAL_I2C_DMA_REQUESTS] =
{
    {I2C1_RX, 1, 1, 0, 1, 0, 1, 0},
    {I2C1_TX, 1, 1, 0, 0, 0, 1, 0},
    {I2C2_RX, 1, 1, 0, 1, 0, 1, 0},
    {I2C2_TX, 1, 1, 0, 0, 0, 1, 0},
    {I2C3_RX, 1, 1, 0, 1, 0, 1, 0},
    {I2C3_TX, 1, 1, 0, 0, 0, 1, 0}
};

static void i2cOne_eventHandler(void);
static void i2cOne_errorHandler(void);

static void i2cTwo_eventHandler(void);
static void i2cTwo_errorHandler(void);

static void i2cThree_eventHandler(void);
static void i2cThree_errorHandler(void);

#pragma pack(1)
typedef struct
{
    I2C_BUS_HANDLE_TYPE i2cBusHandle;   // I2C bus handle
    xSemaphoreHandle s_I2cDriverMutex;  // Mutex to protect the bus

    // Bus init/configuration
    I2C_InitTypeDef i2cBusInit;         // As defined in STM's library

    // Event Group handle for synchronization
    EventGroupHandle_t i2cEventGroupHandle;

    // DMA
    DMA_REQUEST_HANDLE_TYPE i2cDmaRxHandle;
    DMA_REQUEST_HANDLE_TYPE i2cDmaTxHandle;

    // Completion callback in DMA ISR context
    void (*pCallbackISR_Dma)(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                             void* vYourStateBlob, BOOL read);

    I2C_Master_Config_t i2cMasterConfig;   // Copy of configuration

    uint8_t* pTxRxBuffer;                  // Used by Non-DMA transfers

    uint16_t bytesLeft;                   // Bytes left in a transfer

    uint8_t i2cBusIndex;

    uint8_t activeSlaveAddress;         // Currently active slave address

    BOOL busNeedsInit;         // tracks whether the peripheral needs initialization
    BOOL busIsDisabled;        // tracks whether the Bus is in power save mode
    BOOL fatalError;           // used for deferred reinit
} I2C_Bus_DriverInfo_t;


typedef struct
{
    GPIO_TypeDef* i2cSclGpioBank;  // Pointer to SCL GPIO bank

    GPIO_TypeDef* i2cSdaGpioBank;  // Pointer to SDA GPIO bank

    uint8_t i2cPinSCL;  // GPIO SCL pin number

    uint8_t i2cPinSDA;  // GPIO SDA pin number

    uint8_t dummy;              // Reserved

    uint8_t i2cGpioAlternateFunction;   // I2C GPIO alternate function

    I2C_TypeDef* i2cRegisterBase;       // I2C register base

    IRQn_Type i2cEventIrq;              // I2C interrupt Event IRQ
    IRQn_Type i2cErrorIrq;              // I2C interrupt Error IRQ

    INTERRUPT_FNCT_PTR i2cBusEventHandler;
    INTERRUPT_FNCT_PTR i2cBusErrorHandler;

    const DMA_MANAGER_REQUEST_TYPE* pI2cDmaRxRequest;
    const DMA_MANAGER_REQUEST_TYPE* pI2cDmaTxRequest;
} I2C_Bus_Descriptor_t;
#pragma pack()

static const I2C_Bus_Descriptor_t i2cBusDescriptor[MAX_NUMBER_I2C_BUSES] =
{
    {
        I2C_ONE_SCL_GPIO_BANK, I2C_ONE_SDA_GPIO_BANK, I2C_ONE_GPIO_SCL_PIN, I2C_ONE_GPIO_SDA_PIN,
        0, GPIO_AF_I2C1, I2C1, I2C1_EV_IRQn, I2C1_ER_IRQn,
        i2cOne_eventHandler, i2cOne_errorHandler,
        &i2cDmaRequests[0], &i2cDmaRequests[1]
    },
    {
        I2C_TWO_SCL_GPIO_BANK, I2C_TWO_SDA_GPIO_BANK, I2C_TWO_GPIO_SCL_PIN, I2C_TWO_GPIO_SDA_PIN,
        0, GPIO_AF_I2C2, I2C2, I2C2_EV_IRQn, I2C2_ER_IRQn,
        i2cTwo_eventHandler, i2cTwo_errorHandler,
        &i2cDmaRequests[2], &i2cDmaRequests[3]
    },
    {
        I2C_THREE_SCL_GPIO_BANK, I2C_THREE_SDA_GPIO_BANK, I2C_THREE_GPIO_SCL_PIN, I2C_THREE_GPIO_SDA_PIN,
        0, GPIO_AF_I2C3, I2C3, I2C3_EV_IRQn, I2C3_ER_IRQn,
        i2cThree_eventHandler, i2cThree_errorHandler,
        &i2cDmaRequests[4], &i2cDmaRequests[5]
    }
};


#ifdef USE_DEBUG_ASSERT
static const char strFailI2CMutexCreate[] = {"I2C  Mutex create failed"};
static const char strFailI2CEventGroupCreate[] = {"I2C  Event Group create failed"};
#endif

#define I2C_TIMEOUT_MS 50       // in milli seconds
#define I2C_ADDRESSING_TIMEOUT_MS  3       // in milli seconds
#define I2C_ADDRESSING_RETRIES     1

#define WAIT_FOR_SEMAPHORE_MUTEX_TICKS    100

// BUSY, MSL, TXE and TRA flags
#define I2C_CR1_BUSY_MSL_TXE_TRA_MASK        ((uint32_t)0x00070080)


// For synchronization between ISRs and tasks

// Definitions for bits used in Event_Group
// This depends on configUSE_16_BIT_TICKS is 0 (it is indeed now).
// In case configUSE_16_BIT_TICKS is 1, which only has 8 event bits, more
// than one Event Group must be used to support this synchronization mechanism
// I2C bits
#define ebSLAVE_ADDRESSED_BIT       ( 0x01UL )      //  BIT_0
#define ebBYTE_TRANSFER_FINISHED_BIT    ( 0x02UL )      //  BIT_1

#define ebI2C_BUS_ERROR_BIT         ( 0x04UL )      //  BIT_2
#define ebI2C_ARBITRATION_ERROR_BIT     ( 0x08UL )      //  BIT_3

#define ebI2C_ACKNOWLEGE_FAIL_BIT       ( 0x10UL )      //  BIT_4
#define ebI2C_OVER_UNDER_ERROR_BIT  ( 0x20UL )      //  BIT_5

#define ebTRANSFER_DONE_NO_DMA_BIT      ( 0x40UL )      //  BIT_6

// DMA bits
#define ebDMA_TRANSFER_COMPLETE_BIT ( 0x80UL )      //  BIT_7
#define ebDMA_TRANSFER_ERROR_BIT    ( 0x100UL )     //  BIT_8
#define ebDMA_DIRECT_MODE_ERROR_BIT     ( 0x200UL )     //  BIT_9

#define ebALL_I2C_ERROR_BITS  (ebI2C_BUS_ERROR_BIT | ebI2C_ARBITRATION_ERROR_BIT |      \
                               ebI2C_ACKNOWLEGE_FAIL_BIT | ebI2C_OVER_UNDER_ERROR_BIT)

#define ebALL_DMA_ERROR_BITS  (ebDMA_TRANSFER_ERROR_BIT | ebDMA_DIRECT_MODE_ERROR_BIT)

#define ebALL_EVENT_GROUP_BITS  0x0003FF    // For 24-bit maximal, only 10 bits are used now


// Prototypes
static BOOL isI2cHandleValid(I2C_BUS_HANDLE_TYPE pI2cHandle);

static void i2cMasterSetupParameter(I2C_Master_Config_t* pI2cMasterConfig,
                                    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo);

static uint16_t i2cBusMasterTransmit(I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo,
                                     const uint8_t slaveAddr, uint8_t* buffer, uint16_t length, BOOL stop);

static uint16_t i2cBusMasterReceive(I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo,
                                    const uint8_t slaveAddr, uint8_t* buffer, const uint16_t length, BOOL wait);

static BOOL i2c_waitForTransactionDone(EventGroupHandle_t i2cDmaEventHandle,
                                       EventBits_t bitsToWait, uint32_t timeoutInMs);

static inline BOOL i2cStartingAndAddressing(I2C_TypeDef* i2cRegisterBase,
        EventGroupHandle_t i2cDmaEventHandle);

// ISRs
static void i2cBus_eventHandler(I2C_TypeDef* i2cRegisterBase,
                                I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo);

static void i2cBus_errorHandler(I2C_TypeDef* i2cRegisterBase,
                                EventGroupHandle_t i2cDmaEventGroupHandle);

static void callbackISR_DmaCommon(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                                  void* vYourStateBlob, BOOL read);

#endif // 
