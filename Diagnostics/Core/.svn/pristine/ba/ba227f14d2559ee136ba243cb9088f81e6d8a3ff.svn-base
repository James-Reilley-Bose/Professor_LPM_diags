/*
    File    :   SpiBus_API.h
    Title   :
    Author  :   dx1007255
    Created :   01/12/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   SPI bus API header for STM32F2xx

===============================================================================
*/

#ifndef SPI_BUS_API_H
#define SPI_BUS_API_H

#include "platform_settings.h"

#define MAX_NUMBER_SPI_BUSES         3

#define CHIP_SELECT_CONTROL_API_DRIVER           0      // Controlled by this driver
#define CHIP_SELECT_CONTROL_HARDWARE             1      // Controlled by SPI engine
// The following allows users to control chip select by toggling GPIO pins,
// its intention is to support multiple slaves on the same bus with identical configuration
// except for multiple different chip select pins respectively.
// If CHIP_SELECT_CONTROL_API_USERS is selected, the pointer csGpioBank inside the
// structure SPI_Bus_Config_t must be set to NULL. The recommended usage is as below:
//      a. toggle CS pin to low (the CS pin needs to be configured properly at init time),
//      b. call SPIBus_Communicate
//      c. toggle the CS pin to high.
#define CHIP_SELECT_CONTROL_API_USERS          2

//#define SUPPORT_SPI_STAND_BY_MODE

// See usage example in eTapSpi.c
typedef struct
{
    GPIO_TypeDef* csGpioBank;     // Pointer to chip select GPIO bank used by this SPI bus
    // Must set to NULL if not used

    uint16_t spiPinCS : 4;   // GPIO Chip Select pin number

    uint8_t busNumber : 2;      // 1, 2, 3 for SPI1, SPI2, SPI3 respectively

    uint16_t clockPhase : 1;     // 0 = Data latched on 1st clock transition;
    // 1 = Data latched on 2nd clock transition

    uint16_t clockPolarity : 1;  // 0 = SCK is low-level idle;
    // 1 = SCK is high-level idel

    uint16_t chipSelectControl : 2;  // Chip Select control: 0 = Software control,
    //                      1 = Hardware control,
    //                      2 = API User control

    uint16_t baudRateExponent : 3;   // Value n = 0 - 7
    // baudRateDivider will be 2^(n+1), and are respectively
    //  2, 4, 8, 16, 32, 64, 128, 256
    // Peripheral clock will be divided by the baudRateDivider.
    uint16_t disableDMA : 1;         // 1 = No DMA, 0 = Using DMA
    uint16_t dummy : 2;              // Reserved

} SPI_Bus_Config_t;


#ifndef SPI1_CS_GPIO_BANK
#define SPI1_CS_GPIO_BANK                     NULL
#endif

#ifndef SPI1_CS_GPIO_PIN
#define SPI1_CS_GPIO_PIN                      0xFF
#endif


#ifndef SPI2_CS_GPIO_BANK
#define SPI2_CS_GPIO_BANK                     NULL
#endif

#ifndef SPI2_CS_GPIO_PIN
#define SPI2_CS_GPIO_PIN                      0xFF
#endif


#ifndef SPI3_CS_GPIO_BANK
#define SPI3_CS_GPIO_BANK                     NULL
#endif

#ifndef SPI3_CS_GPIO_PIN
#define SPI3_CS_GPIO_PIN                      0xFF
#endif

static const SPI_Bus_Config_t spiDefaultConfig[MAX_NUMBER_SPI_BUSES] =
{
    // bus 1
    {
        SPI1_CS_GPIO_BANK,      // chip select gpio bank
        SPI1_CS_GPIO_PIN,
        1,                      // bus number
        0,                      // clock phase default
        0,                      // clock polarity default
        CHIP_SELECT_CONTROL_API_DRIVER, // chip select control
        0,                      // baud rate exponent
        0                      // dummy
    },
    // bus 2
    {
        SPI2_CS_GPIO_BANK,      // chip select gpio bank
        SPI2_CS_GPIO_PIN,
        2,                      // bus number
        0,                      // clock phase default
        0,                      // clock polarity default
        CHIP_SELECT_CONTROL_API_DRIVER, // chip select control
        0,                      // baud rate exponent
        0                      // dummy
    },
    // bus 3
    {
        SPI3_CS_GPIO_BANK,       // chip select gpio bank
        SPI3_CS_GPIO_PIN,
        3,                       // bus number
        0,                       // clock phase default
        0,                       // clock polarity default
        CHIP_SELECT_CONTROL_API_DRIVER,  // chip select control
        0,                       // baud rate exponent
        0                       // dummy
    }
};

typedef void*   SPI_BUS_HANDLE_TYPE;

typedef enum
{
    SPI_API_NO_ERROR = 0,
    SPI_API_INVALID_BUS_HANDLE,
    SPI_API_INVALID_CONFIG_PARAMETER,
    SPI_API_MUTEX_ERROR,
    SPI_API_BUS_BUSY,
    SPI_API_CANNOT_NON_BLOCKING,
    SPI_API_NO_ACTIVE_TRANSFER,
    SPI_API_NOT_READY_FOR_TRANSFER,
    SPI_API_DMA_CONFIG_ERROR,
    SPI_API_DMA_TRANSFER_ERROR,
    SPI_API_DMA_DIRECT_MODE_ERROR,
    SPI_API_INVALID_ARGUMENT,
    SPI_API_EVENT_GROUP_ERROR,
    SPI_API_DMA_SUBMIT_ERROR,
    SPI_API_DMA_HANDLE_ERROR,
    SPI_API_TRANSFER_TIMEOUT,
    SPI_API_TRANSFER_INCOMPLETE,
} SPI_API_ERROR_CODE;

SPI_BUS_HANDLE_TYPE SPIBus_Initialize(SPI_Bus_Config_t* pSpiBusConfig, SPI_API_ERROR_CODE* pError);


uint16_t SPIBus_Communicate(
    SPI_BUS_HANDLE_TYPE spiBusHandle,
    const uint16_t dataLength,
    uint8_t* pDataToSend,
    uint8_t* pDataReceived,
    void (*pCallBackFunction)(uint16_t numberBytesTransferred, SPI_API_ERROR_CODE errorCode),
    uint16_t blockingTimeMs,
    SPI_API_ERROR_CODE* pError
);


BOOL SPIBus_CommunicateComplete(SPI_BUS_HANDLE_TYPE spiBusHandle, SPI_API_ERROR_CODE* pError);

BOOL SPIBus_Standby(SPI_BUS_HANDLE_TYPE spiBusHandle, BOOL enter, SPI_API_ERROR_CODE* pError);

#endif // SPI_BUS_API_H



