////////////////////////////////////////////////////////////////////////////////
/// @file            cc2500_platform.c
/// @brief           This file implements the platform specific interfacing
///                   of the cc2500 driver, this is the Bardeen LPM version.
/// @author          dx1007255
/// @date            Creation Date: Thu March 24 2015
///
/// Copyright 2015 Bose Corporation
/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "SpiBus_API.h"
#include "cc2500_platform.h"
#include "RFTask.h"

#define CHIPCONSPI_CallBack_Success 0
#define CHIPCONSPI_CallBack_Fail -1

static SPI_BUS_HANDLE_TYPE ChipconSpiHandle;

static chipconFuncptr pChipconCallback = NULL;

// ISR context
static void spiIsrDmaCallback(uint16_t numberBytesTransferred, SPI_API_ERROR_CODE errorCode)
{
    RFPostMsg(RFTASK_MESSAGE_ID_ChipconSpiTransferComplete, numberBytesTransferred, errorCode);
}

void ChipconSPI_Initialize()
{
    SPI_Bus_Config_t mySpiConfig;

    memcpy(&mySpiConfig, &spiDefaultConfig[CHIPCON_SPI_BUS_NUMBER - 1], sizeof(mySpiConfig));

    // Change my config only when it differs from the default
    mySpiConfig.baudRateExponent = 3; // divider_16 for SPI2/3
    // If SPI1 is used (on PHB2), this value should be 4 to get divider_32.
    // This is to keep the clock the same as Marconi.

    // CS is controlled by SPI APIs
    mySpiConfig.chipSelectControl = CHIP_SELECT_CONTROL_API_DRIVER;

    mySpiConfig.csGpioBank = CHIPCON_SELECT_GPIO_BANK;
    mySpiConfig.spiPinCS = CHIPCON_SELECT_GPIO_PIN;
    mySpiConfig.disableDMA = 1;

    SPI_API_ERROR_CODE spiError = SPI_API_NO_ERROR;

    ChipconSpiHandle = SPIBus_Initialize(&mySpiConfig, &spiError);

    debug_assert(ChipconSpiHandle != NULL);

}


// This is a non-blocking call
uint16_t ChipconSpi_Communicate( const unsigned DataLength,
                                 const uint8_t* pDataToSend,
                                 uint8_t* pDataReceived,
                                 void (*pCallBackFunction)(int))
{
    SPI_API_ERROR_CODE spiError;

    pChipconCallback = pCallBackFunction;

    uint16_t ret = SPIBus_Communicate(ChipconSpiHandle, DataLength,
                                      (uint8_t*)pDataToSend, pDataReceived,
                                      &spiIsrDmaCallback, 0, &spiError );

    return ret;
}


// This is a blocking call
uint16_t ChipconSpi_Communicate_Blocking(const unsigned DataLength,
        const uint8_t* pDataToSend,
        uint8_t* pDataReceived)
{
    SPI_API_ERROR_CODE spiError;

    uint16_t ret = SPIBus_Communicate(ChipconSpiHandle, DataLength,
                                      (uint8_t*)pDataToSend, pDataReceived,
                                      NULL, CHIPCON_SPI_DEFAULT_TIMEOUT_MS, &spiError );

    return ret;
}


void ChipconSpi_CommunicateComplete()
{
    SPI_API_ERROR_CODE spiError;

    SPIBus_CommunicateComplete(ChipconSpiHandle, &spiError );

    if (pChipconCallback != NULL)
    {
        pChipconCallback(CHIPCONSPI_CallBack_Success);
    }

}

