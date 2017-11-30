/*
    File    :   i2cMaster_API.h
    Title   :
    Author  :   dx1007255
    Created :   12/26/2014
    Language:   C
    Copyright:  (C) 2014 Bose Corporation, Framingham, MA

    Description:   Headers for I2C Master APIs.

===============================================================================
*/
#ifndef I2C_MASTER_API_H
#define I2C_MASTER_API_H

#include "platform_settings.h"

// The maximum buffer size of write.
#define I2C_MAX_REGISTER_LENGTH         4
#define I2C_MAX_DATA_LENGTH    256
#define I2C_TOTAL_BUFFER_LENGTH     (I2C_MAX_REGISTER_LENGTH + I2C_MAX_DATA_LENGTH)


#define I2C_MASTER_CLOCK_STANDARD_SPEED_HZ         100000
#define I2C_MASTER_CLOCK_FAST_SPEED_HZ             400000

typedef enum
{
    I2C_API_NO_ERROR = 0,
    I2C_API_INVALID_INPUT_POINTER,
    I2C_API_INVALID_BUS_NUMBER,
    I2C_API_INVALID_CLOCK,
    I2C_API_MUTEX_ERROR,
    I2C_API_DMA_CONFIG_ERROR,
    I2C_API_DMA_SUBMISSION_ERROR,
    I2C_API_DMA_TRANSFER_ERROR,
} I2C_API_ERROR_CODE;


// See eTapI2C.c for usage example
typedef struct
{
    uint32_t busNumber : 2;  // 1, 2, 3 for I2C1, I2C2, I2C3 respectively
    uint32_t dummy1 :    2;   // Reserved
    uint32_t masterClockFrequency : 20 ;   // Master clock frequency in Hz
    uint32_t disableDMA : 1;         // 1 = No DMA, 0 = Using DMA
    uint32_t dummy2 :    7;   // Reserved
} I2C_Master_Config_t;


typedef void*   I2C_BUS_HANDLE_TYPE;

void i2cMaster_SetInitialConditions (void);
void i2cMaster_DisableBus (uint8_t theBus);
void i2cMaster_EnableBus (uint8_t theBus);
void i2cMaster_SetBusNeedsInit (uint8_t theBus);

I2C_BUS_HANDLE_TYPE i2cMaster_Init(I2C_Master_Config_t*  pI2cMasterConfig, I2C_API_ERROR_CODE* pError);


BOOL i2cMaster_Ping(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr);


uint16_t i2cMaster_WriteRegister(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr, const uint8_t* pRegAddr, const uint8_t reg_len, const uint8_t* a_pData, const uint16_t a_len);


uint16_t i2cMaster_WriteNoRegister(I2C_BUS_HANDLE_TYPE pI2cHandle,
                                   const uint8_t slaveAddr, const uint8_t* a_pData, const uint16_t a_len);


uint16_t i2cMaster_ReadRegister(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr, uint8_t* pRegAddr, const uint8_t reg_len, uint8_t* a_pData, const uint16_t a_len);


uint16_t i2cMaster_ReadNoRegister (I2C_BUS_HANDLE_TYPE pI2cHandle,
                                   const uint8_t slaveAddr,
                                   uint8_t* a_pData, const uint16_t a_len);

#endif // i2C_MASTER_API_H
