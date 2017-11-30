/*
    File    :   i2cMaster_API.c
    Title   :
    Author  :   dx1007255
    Created :   12/29/2014
    Language:   C
    Copyright:  (C) 2014 Bose Corporation, Framingham, MA

    Description:   I2C bus master API implementation

===============================================================================
*/

#include "i2cBusDriver.h"
#include "SystemAnalysis.h"

SCRIBE_DECL(i2c);

static I2C_Bus_DriverInfo_t i2cMasterDriverInfo[MAX_NUMBER_I2C_BUSES] = {0};

static uint8_t writeBuffer[MAX_NUMBER_I2C_BUSES][I2C_TOTAL_BUFFER_LENGTH];

static BOOL i2cBus_isStartOrStopOrPecSet(I2C_TypeDef* i2cRegisterBase);
static BOOL i2cBus_WaitForNotBusyAndStopStartPecCleared(I2C_TypeDef* i2cRegisterBase);

void i2cMaster_SetBusNeedsInit (uint8_t theBus)
{
    debug_assert((theBus >= 1) && (theBus <= MAX_NUMBER_I2C_BUSES));
    i2cMasterDriverInfo[(theBus - 1 )].busNeedsInit = TRUE;
    LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Mark Bus %d for ReInit", theBus);
}

static void i2cMaster_SetFatalError(I2C_TypeDef* i2cBusRegister)
{
    for (int i = 0; i < MAX_NUMBER_I2C_BUSES; i++)
    {
        if (i2cBusRegister == i2cBusDescriptor[i].i2cRegisterBase)
        {
            i2cMasterDriverInfo[i].fatalError = TRUE;
        }
    }
}

BOOL i2cBus_CheckI2CRecoverBus(I2C_BUS_HANDLE_TYPE pI2cHandle)
{
    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = (I2C_Bus_DriverInfo_t*)pI2cHandle;
    I2C_Master_Config_t* pI2cMasterConfig = &pI2cMasterDriverInfo->i2cMasterConfig;
    I2C_TypeDef* i2cBusRegister = i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cRegisterBase;
    I2C_API_ERROR_CODE err;

    xSemaphoreTake(pI2cMasterDriverInfo->s_I2cDriverMutex, portMAX_DELAY);

    if (I2C_GetFlagStatus(i2cBusRegister, I2C_FLAG_BUSY) || i2cBus_isStartOrStopOrPecSet(i2cBusRegister) ||
            pI2cMasterDriverInfo->fatalError)
    {
        // something's messed up, try to repair it
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "I2C bus %d isn't ready, reinit", pI2cMasterDriverInfo->i2cBusIndex + 1);
        pI2cMasterDriverInfo->busNeedsInit = TRUE;
        pI2cMasterDriverInfo->fatalError = FALSE;
        // disable wants to take the semaphore
        xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);
        i2cMaster_DisableBus (pI2cMasterDriverInfo->i2cBusIndex + 1);
        // as does init
        xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);
        i2cMaster_Init(pI2cMasterConfig, &err);
    }

    xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);
    return TRUE;
}

/*********************************************************************
* @brief    Configure I2C pins
*
* @param    enable - TRUE sets pins for I2C  FALSE sets pins as input floating
*
* @param    busNum - bus number 0 through (MaxBusNumber - 1)
*
* @return   NONE
**********************************************************************/
static inline void i2cMaster_ConfigPins(BOOL enable, uint8_t busNum)
{
    // Do not configure busses that aren't in use
    if (i2cBusDescriptor[busNum].i2cSclGpioBank == 0)
    {
      return;
    }
    GPIO_TypeDef* sclGPIO = i2cBusDescriptor[busNum].i2cSclGpioBank;
    GPIO_TypeDef* sdaGPIO = i2cBusDescriptor[busNum].i2cSdaGpioBank;
    uint32_t sclPin = i2cBusDescriptor[busNum].i2cPinSCL;
    uint32_t sdaPin = i2cBusDescriptor[busNum].i2cPinSDA;
    uint32_t regVal = 0;

    portENTER_CRITICAL();
    // These are the same for enabled or disabled
    // SCL
    regVal = sclGPIO->AFR[sclPin >> 0x03];
    sclGPIO->AFR[sclPin >> 0x03] = (regVal & ~(((uint32_t)0x0F) << ((sclPin & 0x07) * 4)))
                                   | (GPIO_AF_I2C2 << ((sclPin & 0x07) * 4));
    regVal = sclGPIO->OTYPER;
    sclGPIO->OTYPER = (regVal & ~(GPIO_OTYPER_OT_0 << sclPin)) | (GPIO_OType_OD << sclPin);
    regVal = sclGPIO->OSPEEDR;
    sclGPIO->OSPEEDR = (regVal & ~(GPIO_OSPEEDER_OSPEEDR0 << (sclPin * 2))) | (GPIO_Speed_2MHz << (sclPin * 2));
    // SDA
    regVal = sdaGPIO->AFR[sdaPin >> 0x03];
    sdaGPIO->AFR[sdaPin >> 0x03] = (regVal & ~(((uint32_t)0x0F) << ((sdaPin & 0x07) * 4)))
                                   | (GPIO_AF_I2C2 << ((sdaPin & 0x07) * 4));
    regVal = sdaGPIO->OTYPER;
    sdaGPIO->OTYPER = (regVal & ~(GPIO_OTYPER_OT_0 << sdaPin)) | (GPIO_OType_OD << sdaPin);
    regVal = sdaGPIO->OSPEEDR;
    sdaGPIO->OSPEEDR = (regVal & ~(GPIO_OSPEEDER_OSPEEDR0 << (sdaPin * 2))) | (GPIO_Speed_2MHz << (sdaPin * 2));

    if (enable)
    {
        // we are going from floating to Alternate function, so make sure the pin is setup correctly before
        // switching to AF
        //SCL
        regVal = sclGPIO->PUPDR;
        sclGPIO->PUPDR = (regVal & ~(GPIO_PUPDR_PUPDR0 << (sclPin * 2))) | (GPIO_PuPd_UP << (sclPin * 2));
        regVal = sclGPIO->MODER;
        sclGPIO->MODER = (regVal & ~(GPIO_MODER_MODER0 << (sclPin * 2))) | (GPIO_Mode_AF << (sclPin * 2));
        //SDA
        regVal = sdaGPIO->PUPDR;
        sdaGPIO->PUPDR = (regVal & ~(GPIO_PUPDR_PUPDR0 << (sdaPin * 2))) | (GPIO_PuPd_UP << (sdaPin * 2));
        regVal = sdaGPIO->MODER;
        sdaGPIO->MODER = (regVal & ~(GPIO_MODER_MODER0 << (sdaPin * 2))) | (GPIO_Mode_AF << (sdaPin * 2));
    }
    else
    {
        // we are going from AF to Floating, order should not matter since we are open drain but to be safe, go to input mode first
        //SCL
        regVal = sclGPIO->MODER;
        sclGPIO->MODER = (regVal & ~(GPIO_MODER_MODER0 << (sclPin * 2))) | (GPIO_Mode_IN << (sclPin * 2));
        regVal = sclGPIO->PUPDR;
        sclGPIO->PUPDR = (regVal & ~(GPIO_PUPDR_PUPDR0 << (sclPin * 2))) | (GPIO_PuPd_NOPULL << (sclPin * 2));
        //SDA
        regVal = sdaGPIO->MODER;
        sdaGPIO->MODER = (regVal & ~(GPIO_MODER_MODER0 << (sdaPin * 2))) | (GPIO_Mode_IN << (sdaPin * 2));
        regVal = sdaGPIO->PUPDR;
        sdaGPIO->PUPDR = (regVal & ~(GPIO_PUPDR_PUPDR0 << (sdaPin * 2))) | (GPIO_PuPd_NOPULL << (sdaPin * 2));
    }
    portEXIT_CRITICAL();
}

/*********************************************************************
* @brief    Initial configuration of the bus status. Needs to be called
*           before any device calls i2cMaster_Init.
*
* @return   NONE
**********************************************************************/
void i2cMaster_SetInitialConditions (void)
{
    for (int bus = 0; bus < MAX_NUMBER_I2C_BUSES; bus++)
    {
        i2cMasterDriverInfo[bus].busNeedsInit = TRUE;
        i2cMasterDriverInfo[bus].busIsDisabled = TRUE;
        // This is to make sure the pins are set to floating and not alt function
        i2cMaster_ConfigPins(DISABLE, bus);
    }
}



/*********************************************************************
* @brief    Disables i2c bus. Be careful when calling this on a bus with
*           Multiple devices. This will grab the MUTEX until i2cMaster_EnableBus
*           is called.
*
* @param    busNum      - Which bus to enable
*
* @return   NONE
**********************************************************************/
void i2cMaster_DisableBus (uint8_t busNum)
{
    // I2C buses are labeled starting at 1
    if ((busNum <  1) || (busNum > MAX_NUMBER_I2C_BUSES))
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "_Disable invalid bus %d", busNum);
        debug_assert(0);
        return;
    }

    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = &i2cMasterDriverInfo[busNum - 1];

    // If no MUTEX, then we have not initialized this bus yet, just return
    if (pI2cMasterDriverInfo->s_I2cDriverMutex == NULL)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "_Disable called with NULL MUTEX.");
        debug_assert(0);
        return;
    }

    // Buses are labeled from 1, which we can't use, so get the bus index
    uint8_t adjustdBusNum = pI2cMasterDriverInfo->i2cBusIndex;

    // Hold mutex
    if (xSemaphoreTake(pI2cMasterDriverInfo->s_I2cDriverMutex, TIMER_MSEC_TO_TICKS(I2C_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "i2cMaster_Disable Unable to get mutex Bus %d ", busNum);
        return;
    }

    // Disable I2C interrupts
    I2C_ITConfig(i2cBusDescriptor[adjustdBusNum].i2cRegisterBase, (I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF), DISABLE);

    // Disable i2c Peripheral
    i2cBusDescriptor[adjustdBusNum].i2cRegisterBase->CR1 &= (~I2C_CR1_PE);

    i2cMaster_ConfigPins(DISABLE, adjustdBusNum);

    // leave a crumb
    pI2cMasterDriverInfo->busIsDisabled = TRUE;
    LOG(i2c, ROTTEN_LOGLEVEL_VERBOSE, "Disable I2C Bus %d", busNum);

}



/*********************************************************************
* @brief    Enables i2c bus for transfers.
*           !!! If you are calling this function then you are fully aware
*           that it will free the buses MUTEX.
*
* @param    busNum      - Which bus to enable
*
* @return   NONE
**********************************************************************/
void i2cMaster_EnableBus(uint8_t busNum)
{
    // I2C buses are labeled starting at 1
    if ((busNum < 1) || (busNum > MAX_NUMBER_I2C_BUSES))
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "i2cMaster_Enable called with invalid bus %d", busNum);
        debug_assert(0);
        return; // Invalid bus number input
    }

    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = &i2cMasterDriverInfo[busNum - 1];

    // If no MUTEX, then we have not initialized this bus yet, just return
    if (pI2cMasterDriverInfo->s_I2cDriverMutex == NULL)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "i2cMaster_Disable called with NULL MUTEX.");
        debug_assert(0);
        return;
    }

    // Buses are labeled from 1, which we can't use, so get the bus index
    uint8_t adjustdBusNum = pI2cMasterDriverInfo->i2cBusIndex;

    LOG(i2c, ROTTEN_LOGLEVEL_VERBOSE, "Enable I2C Bus %d", busNum);

    // enable GPIO pins
    i2cMaster_ConfigPins(ENABLE, adjustdBusNum);

    //Re-enable I2C interrupts
    I2C_ITConfig(i2cBusDescriptor[adjustdBusNum].i2cRegisterBase, (I2C_IT_ERR | I2C_IT_EVT), ENABLE);

    // Re-enable i2c Peripheral
    i2cBusDescriptor[adjustdBusNum].i2cRegisterBase->CR1 |= I2C_CR1_PE;

    if (pI2cMasterDriverInfo->busIsDisabled == FALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "A task tried to Enable I2C Bus %d and it wasn't disabled", busNum);
    }

    pI2cMasterDriverInfo->busIsDisabled = FALSE;

    // always free mutex
    xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);

}

/*********************************************************************
* @brief    Initializes i2c bus DMA channels. This should only be called once per bus
*
* @param    pI2cMasterDriverInfo     - Pointer to I2C_Bus_DriverInfo_t structure
*
* @return   DMA_MANAGER_ERROR_CODE   - DMA error code
**********************************************************************/
static DMA_MANAGER_ERROR_CODE i2cMaster_InitDMA(I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo)
{
    DMA_MANAGER_ERROR_CODE dmaErrorCode = DMA_MANAGER_NO_ERROR;

    if (pI2cMasterDriverInfo->i2cDmaRxHandle == NULL)
    {
        pI2cMasterDriverInfo->i2cDmaRxHandle =
            dmaManager_ConfigureChannel(i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].pI2cDmaRxRequest,
                                        &dmaErrorCode);
    }

    // If previous channel failed, no reason to configure this channel unless we allow DMA on RX/TX separately, which we do not currently.
    if (dmaErrorCode == DMA_MANAGER_NO_ERROR && pI2cMasterDriverInfo->i2cDmaTxHandle == NULL)
    {
        pI2cMasterDriverInfo->i2cDmaTxHandle =
            dmaManager_ConfigureChannel(i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].pI2cDmaTxRequest,
                                        &dmaErrorCode);
    }

    return dmaErrorCode;
}

/*********************************************************************
* @brief    Initializes i2c bus ISRs. This should only be called once per bus
*
* @param    pI2cMasterDriverInfo     - Pointer to I2C_Bus_DriverInfo_t structure
*
* @return   NONE
**********************************************************************/
static void i2cMaster_InitISRs(I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo)
{
    // Register and Enable interupt of I2C bus from NVIC (event and error)
    Interrupt_RegisterISR(i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cEventIrq,
                          i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cBusEventHandler);
    Interrupt_Enable(i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cEventIrq,
                     configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
    Interrupt_RegisterISR(i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cErrorIrq,
                          i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cBusErrorHandler);
    Interrupt_Enable(i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cErrorIrq,
                     configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

/*******************************************************************
* @brief    Initializes i2c bus master driver
*
* @param    pI2cMasterConfig     - Pointer to I2C_Master_Config_t structure
* @param    pError      - Pointer to API's error code
*
* @return   Handle of I2C_BUS_HANDLE_TYPE on success, error code is I2C_API_NO_ERROR;
*           NULL otherwise, error code is poitned to by pError.
**********************************************************************/
I2C_BUS_HANDLE_TYPE i2cMaster_Init(I2C_Master_Config_t* pI2cMasterConfig, I2C_API_ERROR_CODE* pError)
{
    if (pI2cMasterConfig == NULL)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Invalid input pointer");
        *pError = I2C_API_INVALID_INPUT_POINTER;
        return NULL; // Invalid input pointer
    }

    if ((pI2cMasterConfig->busNumber <  1) || (pI2cMasterConfig->busNumber > MAX_NUMBER_I2C_BUSES))
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Invalid bus number input");
        *pError = I2C_API_INVALID_BUS_NUMBER;
        return NULL; // Invalid bus number input
    }

    if ((pI2cMasterConfig->masterClockFrequency < I2C_MASTER_CLOCK_STANDARD_SPEED_HZ) ||
            (pI2cMasterConfig->masterClockFrequency > I2C_MASTER_CLOCK_FAST_SPEED_HZ) )
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d Invalid clock", pI2cMasterConfig->busNumber);
        *pError = I2C_API_INVALID_CLOCK;
        return NULL;    // Invalid clock
    }

    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo =
        &i2cMasterDriverInfo[(pI2cMasterConfig->busNumber - 1 )];

    // Create mutex per bus
    portENTER_CRITICAL();
    if (pI2cMasterDriverInfo->s_I2cDriverMutex == NULL)
    {
        pI2cMasterDriverInfo->s_I2cDriverMutex = xSemaphoreCreateMutex();
        RTOS_assert ((pI2cMasterDriverInfo->s_I2cDriverMutex != NULL), strFailI2CMutexCreate);
    }
    portEXIT_CRITICAL();

    // Hold mutex
    if (xSemaphoreTake(pI2cMasterDriverInfo->s_I2cDriverMutex, TIMER_MSEC_TO_TICKS(I2C_TIMEOUT_MS)) == pdFALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "I2CInit Bus %d Unable to get mutex",
            pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
        *pError = I2C_API_MUTEX_ERROR;
        return NULL;
    }

    *pError = I2C_API_NO_ERROR;

    if (pI2cMasterDriverInfo->busNeedsInit)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "I2CInit Bus %d", pI2cMasterConfig->busNumber);

        // Set up bus specific parameters
        i2cMasterSetupParameter(pI2cMasterConfig, pI2cMasterDriverInfo);

        I2C_TypeDef* i2cBusRegister =
            i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cRegisterBase;

        // De-Init to default state
        I2C_DeInit(i2cBusRegister);

        // Software reset; this will reset bus back to reset state.
        i2cBusRegister->CR1 |= I2C_CR1_SWRST;

        // Release software reset
        i2cBusRegister->CR1 &= (~I2C_CR1_SWRST);

        // Configure DMA
        i2cMaster_InitDMA(pI2cMasterDriverInfo);

        // Set as last DMA transfer bit
        i2cBusRegister->CR2 |= ( I2C_CR2_LAST);

        // Create an Event Group handle for synchronization
        if (pI2cMasterDriverInfo->i2cEventGroupHandle == NULL)
        {
            pI2cMasterDriverInfo->i2cEventGroupHandle = xEventGroupCreate();
            RTOS_assert(pI2cMasterDriverInfo->i2cEventGroupHandle, strFailI2CEventGroupCreate)
        }

        // Register and Enable interrupt of I2C bus from NVIC (event and error)
        i2cMaster_InitISRs(pI2cMasterDriverInfo);

        // Init I2C using STM's library
        I2C_Init(i2cBusRegister, &pI2cMasterDriverInfo->i2cBusInit);

        // I2C_Init enables the module but we do not want it enabled yet. It will be enabled later.
        i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cRegisterBase->CR1 &= (~I2C_CR1_PE);

        pI2cMasterDriverInfo->i2cBusHandle = pI2cMasterDriverInfo;

        pI2cMasterDriverInfo->busNeedsInit = FALSE;

        // Enable the bus , frees mutex
        i2cMaster_EnableBus(pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
    }
    else
    {
        // the bus is already initialized, release semaphore
        xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);
    }

    // disable client DMA use if channel configuration failed
    if ((pI2cMasterDriverInfo->i2cDmaRxHandle == NULL) ||
            (pI2cMasterDriverInfo->i2cDmaTxHandle == NULL))
    {
        pI2cMasterDriverInfo->i2cMasterConfig.disableDMA = 1;
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "I2CInit: DMA config for I2C Bus %d failed",
            pI2cMasterDriverInfo->i2cBusIndex);
        *pError = I2C_API_DMA_CONFIG_ERROR;
    }

    return pI2cMasterDriverInfo;
}


/*****************************************************************//**
* @brief    Ping i2c slave by address
*           Send START bit to a slave with the given address, wait for its ACK
*               and then send STOP bit.
*
* @param    slaveAddr  - address of component to ping
* @return   TRUE if the slave ACKed without errors, FALSE otherwise
**********************************************************************/
BOOL i2cMaster_Ping_Internal(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr)
{
    // Wait for I2C
    BOOL retval = FALSE;

    // Validate the handle
    if (isI2cHandleValid(pI2cHandle) == FALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Invalid I2C Handle");
        return FALSE;
    }

    i2cBus_CheckI2CRecoverBus(pI2cHandle);

    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = (I2C_Bus_DriverInfo_t*)pI2cHandle;
    I2C_TypeDef* i2cBusRegister = i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cRegisterBase;

    // Hold mutex
    if (xSemaphoreTake(pI2cMasterDriverInfo->s_I2cDriverMutex,
                       WAIT_FOR_SEMAPHORE_MUTEX_TICKS) == pdFALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d Unable to get mutex",
            pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
        return FALSE;
    }

    // Set up address to Ping (write)
    pI2cMasterDriverInfo->activeSlaveAddress = (slaveAddr & (~I2C_Direction_Receiver));

    // Clear up any pending flags
    xEventGroupClearBits(pI2cMasterDriverInfo->i2cEventGroupHandle, ebALL_EVENT_GROUP_BITS);

    pI2cMasterDriverInfo->bytesLeft = 0; // No data transfer

    if (i2cBus_WaitForNotBusyAndStopStartPecCleared(i2cBusRegister) == TRUE)
    {
        // Enable ACK, since it might be disabled
        i2cBusRegister->CR1 |= I2C_CR1_ACK;

        // if this function fails, it will "handle" cleanup, which does not mean it is in a good state. If we do
        // succeed, then we need to issue a stop since this function assumes we are going to continue the transaction.
        if ((retval = i2cStartingAndAddressing(i2cBusRegister, pI2cMasterDriverInfo->i2cEventGroupHandle)) == TRUE)
        {
            // we succeeded so we need to close the transfer by issuing a stop.
            I2C_GenerateSTOP (i2cBusRegister, ENABLE);
        }
    }
    else
    {
        //TODO: If we have the MUTEX, then the bus should not be busy by now. We need to analyze the bus and fix it
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "PING: Bus %d busy or CR1 is not safe to write", pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
        i2cMaster_SetFatalError(i2cBusRegister);
    }

    // Free mutex
    xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);

    return retval;
}

/*****************************************************************//**
* @brief    Ping i2c slave by address
*           Send START bit to a slave with the given address, wait for its ACK
*               and then send STOP bit.
*
* @param    slaveAddr  - address of component to ping
* @return   TRUE if the slave ACKed without errors, FALSE otherwise
**********************************************************************/
BOOL i2cMaster_Ping(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr)
{
    return i2cMaster_Ping_Internal(pI2cHandle, slaveAddr);
}



/*****************************************************************//**
* @brief    Write data to the i2c port specidied by the handle
*           Send START bit to a slave with the given address, once the slave is
*               addressed, send the data buffer, and then send STOP bit.
*
* @param    pI2cHandle  - I2C bus handle
* @param    slaveAddr  - address of component to write to
* @param    pRegAddr     - array of register address
* @param    reg_len - pRegAddr length
* @param    a_pData - pointer to the data to send
* @param    a_len   - number of data bytes to send, NOT include regAddr byte
*
* @return   Numebr of bytes written (NOT include regAddr bytes),
*           0 means either nothing written or error
**********************************************************************/
uint16_t i2cMaster_WriteRegister_Internal(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr,
        const uint8_t* pRegAddr, const uint8_t reg_len,
        const uint8_t* a_pData, const uint16_t a_len)
{
    // Ensure parameters are valid
    if ((a_pData == NULL)  || (a_len == 0) || ((a_len + reg_len) > I2C_TOTAL_BUFFER_LENGTH))
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Invalid Params");
        return 0;
    }
    // Validate the handle
    if (isI2cHandleValid(pI2cHandle) == FALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Invalid Handle");
        return 0;
    }

    i2cBus_CheckI2CRecoverBus(pI2cHandle);

    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = (I2C_Bus_DriverInfo_t*)pI2cHandle;

    // Hold mutex
    if (xSemaphoreTake(pI2cMasterDriverInfo->s_I2cDriverMutex, WAIT_FOR_SEMAPHORE_MUTEX_TICKS) == pdFALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d Unable to get mutex",
            pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
        return 0;
    }

    if ((pRegAddr != NULL) && (reg_len != 0))
    {
        // Make register byte plus the data bytes into a single DMA transfer
        memcpy(writeBuffer[pI2cMasterDriverInfo->i2cBusIndex], pRegAddr, reg_len);
        memcpy(&writeBuffer[pI2cMasterDriverInfo->i2cBusIndex][reg_len], a_pData, a_len);
    }
    else
    {
        // Data only, no registers
        memcpy(&writeBuffer[pI2cMasterDriverInfo->i2cBusIndex][0], a_pData, a_len);
    }

    uint16_t bytesWritten = i2cBusMasterTransmit(pI2cMasterDriverInfo,
                            slaveAddr, writeBuffer[pI2cMasterDriverInfo->i2cBusIndex],
                            a_len + reg_len, TRUE);

    // Free mutex
    xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);

    // Excluding the register byte
    if (bytesWritten >= reg_len)
    {
        bytesWritten -= reg_len;
    }

    return bytesWritten;
}

/*****************************************************************//**
* @brief    Write data to the i2c port specidied by the handle
*           Send START bit to a slave with the given address, once the slave is
*               addressed, send the data buffer, and then send STOP bit.
*
* @param    pI2cHandle  - I2C bus handle
* @param    slaveAddr  - address of component to write to
* @param    pRegAddr     - array of register address
* @param    reg_len - pRegAddr length
* @param    a_pData - pointer to the data to send
* @param    a_len   - number of data bytes to send, NOT include regAddr byte
*
* @return   Numebr of bytes written (NOT include regAddr bytes),
*           0 means either nothing written or error
**********************************************************************/
uint16_t i2cMaster_WriteRegister(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr,
                                 const uint8_t* pRegAddr, const uint8_t reg_len,
                                 const uint8_t* a_pData, const uint16_t a_len)
{
    return i2cMaster_WriteRegister_Internal(pI2cHandle, slaveAddr, pRegAddr, reg_len, a_pData, a_len);
}


/*****************************************************************//**
* @brief    Write data to the i2c port specidied by the handle, w/o regAddr
*
* @param    pI2cHandle  - I2C bus handle
* @param    slaveAddr  - address of component to write to
* @param    a_pData - pointer to the data to send
* @param    a_len   - number of data bytes to send.
*
* @return   Numebr of bytes written, 0 means either nothing written or error
**********************************************************************/
uint16_t i2cMaster_WriteNoRegister(I2C_BUS_HANDLE_TYPE pI2cHandle,
                                   const uint8_t slaveAddr, const uint8_t* a_pData, const uint16_t a_len)
{
    return i2cMaster_WriteRegister(pI2cHandle, slaveAddr, NULL, 0, a_pData, a_len);
}


/*****************************************************************//**
* @brief    Read data from the i2c port specidied by the handle
*           Send START bit to a slave with the given address, once the slave is
*               addressed, read the data, and then send STOP bit.
*
* @param    pI2cHandle  - I2C bus handle
* @param    slaveAddr  - address of component to read From
* @param    pRegAddr   -  array of register address to read from
* @param    reg_len - length of pRegAddr
* @param    a_pData - pointer to place to put resulting data.
* @param    a_len   - number of data bytes to get, NOT include regAddr byte
*
* @return   Number of bytes read (NOT include regAddr byte),
*               0 means either nothing read or error
**********************************************************************/
uint16_t i2cMaster_ReadRegister_Internal(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr,
        uint8_t* pRegAddr, const uint8_t reg_len, uint8_t* a_pData, const uint16_t a_len)
{
    // Ensure parameters are valid
    if ((a_pData == NULL) || (a_len == 0))
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Invalid Params");
        return 0;
    }

    // Validate the handle
    if (isI2cHandleValid(pI2cHandle) == FALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Invalid Handle");
        return 0;
    }

    i2cBus_CheckI2CRecoverBus(pI2cHandle);

    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = (I2C_Bus_DriverInfo_t*)pI2cHandle;

    // Hold mutex
    if (xSemaphoreTake(pI2cMasterDriverInfo->s_I2cDriverMutex, WAIT_FOR_SEMAPHORE_MUTEX_TICKS) == pdFALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d Unable to get mutex",
            pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
        return 0;
    }

    uint16_t bytesRead = 0;

    if ((pRegAddr != NULL) && (reg_len != 0))
    {
        // Send the register address first, don't generate STOP signal
        if (i2cBusMasterTransmit(pI2cMasterDriverInfo, slaveAddr, pRegAddr, reg_len, FALSE) == reg_len)
        {
            bytesRead = i2cBusMasterReceive(pI2cMasterDriverInfo, slaveAddr, a_pData, a_len, FALSE);
        }
    }
    else
    {
        bytesRead = i2cBusMasterReceive(pI2cMasterDriverInfo, slaveAddr, a_pData, a_len, TRUE);
    }

    //Free mutex
    xSemaphoreGive(pI2cMasterDriverInfo->s_I2cDriverMutex);

    return bytesRead;
}

/*****************************************************************//**
* @brief    Read data from the i2c port specidied by the handle
*           Send START bit to a slave with the given address, once the slave is
*               addressed, read the data, and then send STOP bit.
*
* @param    pI2cHandle  - I2C bus handle
* @param    slaveAddr  - address of component to read From
* @param    pRegAddr   -  array of register address to read from
* @param    reg_len - length of pRegAddr
* @param    a_pData - pointer to place to put resulting data.
* @param    a_len   - number of data bytes to get, NOT include regAddr byte
*
* @return   Number of bytes read (NOT include regAddr byte),
*               0 means either nothing read or error
**********************************************************************/
uint16_t i2cMaster_ReadRegister(I2C_BUS_HANDLE_TYPE pI2cHandle, const uint8_t slaveAddr,
                                uint8_t* pRegAddr, const uint8_t reg_len, uint8_t* a_pData, const uint16_t a_len)
{
    return i2cMaster_ReadRegister_Internal(pI2cHandle, slaveAddr, pRegAddr, reg_len, a_pData, a_len);
}

/*****************************************************************//**
* @brief    Read data from the i2c port specidied by the handle, w/o regAddr
*
* @param    pI2cHandle  - I2C bus handle
* @param    slaveAddr  - address of component to read From
* @param    a_pData - pointer to place to put resulting data.
* @param    a_len   - number of data bytes to get.
*
* @return   Number of bytes read, 0 means either nothing read or error
**********************************************************************/
uint16_t i2cMaster_ReadNoRegister (I2C_BUS_HANDLE_TYPE pI2cHandle,
                                   const uint8_t slaveAddr, uint8_t* a_pData, const uint16_t a_len)
{
    return i2cMaster_ReadRegister(pI2cHandle, slaveAddr, NULL, 0, a_pData, a_len);
}

/********************************************************************
* @brief    Check is STOP or START or PEC are set. If so we cannot modify
*           CR1 without bad things happening
*
* @param    i2cRegisterBase - Pointer to I2C register base
*
* @return   TRUE if STOP or START or PEC are set
**********************************************************************/
static inline BOOL i2cBus_isStartOrStopOrPecSet(I2C_TypeDef* i2cRegisterBase)
{
    return (i2cRegisterBase->CR1 & (I2C_CR1_STOP | I2C_CR1_START | I2C_CR1_PEC)) != 0;
}


/*****************************************************************//**
* @brief    Loop to wait for BUSY flag to go low ( IDLE ) and wait for
*           CR1's START/STOP/PEC to be cleared
*
* @param    i2cRegisterBase - Pointer to I2C register base
*
* @return   TRUE if Not busy and cleared before timeout,
*           FALSE if it's busy or not cleared and times out
**********************************************************************/
static BOOL i2cBus_WaitForNotBusyAndStopStartPecCleared(I2C_TypeDef* i2cRegisterBase)
{
    uint32_t TimeoutStartTimeMs = GET_SYSTEM_UPTIME_MS();
    while (I2C_GetFlagStatus(i2cRegisterBase, I2C_FLAG_BUSY) || i2cBus_isStartOrStopOrPecSet(i2cRegisterBase))
    {
        if ((GET_MILLI_SINCE(TimeoutStartTimeMs) >= I2C_TIMEOUT_MS))
        {
            return FALSE;
        }
    }
    return TRUE;
}


/*****************************************************************//**
* @brief    Validate an I2C handle
*
* @param    pI2cHandle - Handle of I2C_BUS_HANDLE_TYPE
*
* @return   TRUE if valid, FALSE otherwise
**********************************************************************/
static BOOL isI2cHandleValid(I2C_BUS_HANDLE_TYPE pI2cHandle)
{
    for (int i = 0; i < MAX_NUMBER_I2C_BUSES; i++)
    {
        if ((pI2cHandle == &i2cMasterDriverInfo[i]) &&
                (pI2cHandle == i2cMasterDriverInfo[i].i2cBusHandle))
        {
            return TRUE;
        }
    }

    return FALSE;
}


/*****************************************************************//**
* @brief    Set up I2C master parameters per bus
*
* @param    pI2cMasterConfig     - Pointer to I2C_Master_Config_t structure
* @param    pI2cMasterDriverInfo - Pointer to a structure of I2C_Bus_DriverInfo_t
* @param    i2cBusIndex  - I2C bus index: 0 = I2C1, 1 = I2C2, 2 = I2C3
*
* @return   void
**********************************************************************/
static void i2cMasterSetupParameter(I2C_Master_Config_t* pI2cMasterConfig,
                                    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo)
{
    // Save the config
    memcpy(&pI2cMasterDriverInfo->i2cMasterConfig, pI2cMasterConfig, sizeof(I2C_Master_Config_t));

    pI2cMasterDriverInfo->i2cBusIndex = (pI2cMasterConfig->busNumber - 1 );

    // Common I2C settings
    pI2cMasterDriverInfo->i2cBusInit.I2C_Mode = I2C_Mode_I2C;
    pI2cMasterDriverInfo->i2cBusInit.I2C_ClockSpeed =
        pI2cMasterConfig->masterClockFrequency;
    pI2cMasterDriverInfo->i2cBusInit.I2C_OwnAddress1 = 0x00;
    pI2cMasterDriverInfo->i2cBusInit.I2C_Ack = I2C_Ack_Enable;
    pI2cMasterDriverInfo->i2cBusInit.I2C_DutyCycle = I2C_DutyCycle_2; // not used in 100kHz mode
    pI2cMasterDriverInfo->i2cBusInit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    if (!pI2cMasterConfig->disableDMA)
    {
        pI2cMasterDriverInfo->pCallbackISR_Dma = callbackISR_DmaCommon;
    }
}


/*****************************************************************//**
* @brief    Interrupt service routine for I2C_1 events: SB, ADDR, and BTF
* @return   void
**********************************************************************/
static void i2cOne_eventHandler(void)
{
    i2cBus_eventHandler(I2C1, &i2cMasterDriverInfo[0]);
}


/*****************************************************************//**
* @brief    Interrupt service routine for I2C_1 errors
* @return   void
**********************************************************************/
static void i2cOne_errorHandler(void)
{
    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = &i2cMasterDriverInfo[0];
    i2cBus_errorHandler(I2C1, pI2cMasterDriverInfo->i2cEventGroupHandle);
}


/*****************************************************************//**
* @brief    Interrupt service routine for I2C_2 events: SB, ADDR, and BTF
* @return   void
**********************************************************************/
static void i2cTwo_eventHandler(void)
{
    i2cBus_eventHandler(I2C2, &i2cMasterDriverInfo[1]);
}


/*****************************************************************//**
* @brief    Interrupt service routine for I2C_2 errors
* @return   void
**********************************************************************/
static void i2cTwo_errorHandler(void)
{
    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = &i2cMasterDriverInfo[1];
    i2cBus_errorHandler(I2C2, pI2cMasterDriverInfo->i2cEventGroupHandle);
}


/*****************************************************************//**
* @brief    Interrupt service routine for I2C_3 events: SB, ADDR, and BTF
* @return   void
**********************************************************************/
static void i2cThree_eventHandler(void)
{
    i2cBus_eventHandler(I2C3, &i2cMasterDriverInfo[2]);
}


/*****************************************************************//**
* @brief    Interrupt service routine for I2C_3 errors
* @return   void
**********************************************************************/
static void i2cThree_errorHandler(void)
{
    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = &i2cMasterDriverInfo[2];
    i2cBus_errorHandler(I2C3, pI2cMasterDriverInfo->i2cEventGroupHandle);
}



/*****************************************************************//**
* @brief    Interrupt service routine for I2C bus events: SB, ADDR, and BTF
*
* @param    i2cRegisterBase - Pointer to I2C register base
* @param    pI2cMasterDriverInfo  - Pointer to the I2C master driver info structure
*
* @return   void
**********************************************************************/
static void i2cBus_eventHandler(I2C_TypeDef* i2cRegisterBase,
                                I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo)
{
    INTERRUPT_RUN_LATENCY_BEGIN(I2C_EVT);

    uint32_t flag1 = i2cRegisterBase->SR1;

    // handle SB interrupt
    if (flag1 & I2C_SR1_SB)
    {
        // This will clear SB interrupt
        i2cRegisterBase->DR = pI2cMasterDriverInfo->activeSlaveAddress;
    }
    else if (flag1 & I2C_SR1_ADDR) // handle ADDR interrupt
    {
        if (pI2cMasterDriverInfo->activeSlaveAddress & I2C_Direction_Receiver)
        {
            if (pI2cMasterDriverInfo->bytesLeft == 1)
            {
                // Read 1 byte only,  no ACK
                i2cRegisterBase->CR1 &= (~I2C_CR1_ACK);
            }
            else if (pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
            {
                if (pI2cMasterDriverInfo->bytesLeft == 2)
                {
                    // DONT_GENERATE_ACK();
                    i2cRegisterBase->CR1 &= (~I2C_CR1_ACK);

                    // Set POS high
                    i2cRegisterBase->CR1 |= I2C_CR1_POS;
                }
            }
        }

        // dummy read SR2 to clear ADDR interrupt
        i2cRegisterBase->SR2;

        // Addressed
        driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                ebSLAVE_ADDRESSED_BIT );

        if (pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
        {
            if (pI2cMasterDriverInfo->activeSlaveAddress & I2C_Direction_Receiver)
            {
                if (pI2cMasterDriverInfo->bytesLeft == 1)
                {
                    // Enable BUF interrupt
                    i2cRegisterBase->CR2 |= I2C_IT_BUF;
                    // Send stop bit
                    i2cRegisterBase->CR1 |= I2C_CR1_STOP;
                }
            }          // Transfer 1st byte in case of transmission
            else if (pI2cMasterDriverInfo->bytesLeft)
            {
                i2cRegisterBase->DR = *(pI2cMasterDriverInfo->pTxRxBuffer);
            }
        }
    }
    else // Other i2c event interrupts
    {
        if (pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
        {
            if (flag1 & I2C_SR1_RXNE)       // Receiving
            {
                if (pI2cMasterDriverInfo->bytesLeft > 3)
                {
                    *(pI2cMasterDriverInfo->pTxRxBuffer++) = i2cRegisterBase->DR;
                    pI2cMasterDriverInfo->bytesLeft--;
                    return;
                }
                else  if (pI2cMasterDriverInfo->bytesLeft == 3)
                {
                    // DONT_GENERATE_ACK();
                    i2cRegisterBase->CR1 &= (~I2C_CR1_ACK);
                    // Read data (N-2)
                    *(pI2cMasterDriverInfo->pTxRxBuffer++) = i2cRegisterBase->DR;
                    pI2cMasterDriverInfo->bytesLeft--;
                    return;
                }
                else  if (pI2cMasterDriverInfo->bytesLeft == 1)
                {
                    *(pI2cMasterDriverInfo->pTxRxBuffer) = i2cRegisterBase->DR;
                    // Disable BUF interrupt
                    i2cRegisterBase->CR2 &= (uint16_t)(~I2C_IT_BUF);
                    driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                            ebTRANSFER_DONE_NO_DMA_BIT);
                    return;
                }
            }
            else if (flag1 & I2C_SR1_TXE)   // Transmission
            {
                if (pI2cMasterDriverInfo->bytesLeft == 0)
                {
                    driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                            ebTRANSFER_DONE_NO_DMA_BIT);
                }
                else
                {
                    if (--pI2cMasterDriverInfo->bytesLeft == 0)
                    {
                        driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                                ebTRANSFER_DONE_NO_DMA_BIT);
                    }
                    else
                    {
                        // Transfer next byte
                        i2cRegisterBase->DR = *(++pI2cMasterDriverInfo->pTxRxBuffer);
                    }
                }
            }
        }

        if (flag1 & I2C_SR1_BTF)        //  Byte transfer finished
        {
            if ((pI2cMasterDriverInfo->i2cMasterConfig.disableDMA) &&
                    (pI2cMasterDriverInfo->activeSlaveAddress & I2C_Direction_Receiver))
            {
                if (pI2cMasterDriverInfo->bytesLeft == 2)
                {
                    // Send stop bit
                    i2cRegisterBase->CR1 |= I2C_CR1_STOP;

                    // Read data (N-1) and N
                    *(pI2cMasterDriverInfo->pTxRxBuffer++) = i2cRegisterBase->DR;
                    *(pI2cMasterDriverInfo->pTxRxBuffer) = i2cRegisterBase->DR;
                }
            }
            else
            {
                // clear BTF by reading DR
                (i2cRegisterBase->DR);
            }

            // BTF Coming
            driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                    ebBYTE_TRANSFER_FINISHED_BIT );
        }
    }

    INTERRUPT_RUN_LATENCY_END(I2C_EVT, 0);
}


/*****************************************************************//**
* @brief    Interrupt service routine for I2C bus errors
*         Only 4 errors are handled now
*
* @param    i2cRegisterBase - Pointer to I2C register base
* @param    i2cDmaEventGroupHandle - Handle to EventGroupHandle_t
*
* @return   void
**********************************************************************/
static void i2cBus_errorHandler(I2C_TypeDef* i2cRegisterBase,
                                EventGroupHandle_t i2cDmaEventGroupHandle)
{
    INTERRUPT_RUN_LATENCY_BEGIN(I2C_ERR);

    // Bus error
    if (I2C_GetITStatus(i2cRegisterBase, I2C_IT_BERR) == SET)
    {
        driverSetEventBitsInISR(i2cDmaEventGroupHandle,
                                ebI2C_BUS_ERROR_BIT );
    }

    // Arbitration error
    if (I2C_GetITStatus(i2cRegisterBase, I2C_IT_ARLO) == SET)
    {
        driverSetEventBitsInISR(i2cDmaEventGroupHandle,
                                ebI2C_ARBITRATION_ERROR_BIT );
    }

    // Acknowledge error
    if (I2C_GetITStatus(i2cRegisterBase, I2C_IT_AF) == SET)
    {
        driverSetEventBitsInISR(i2cDmaEventGroupHandle,
                                ebI2C_ACKNOWLEGE_FAIL_BIT );
    }

    // Overrun & Underrun error
    if (I2C_GetITStatus(i2cRegisterBase, I2C_IT_OVR) == SET)
    {
        driverSetEventBitsInISR(i2cDmaEventGroupHandle,
                                ebI2C_OVER_UNDER_ERROR_BIT );
    }

    // Clear all possible pending bits
    I2C_ClearITPendingBit(i2cRegisterBase, (I2C_IT_SMBALERT | I2C_IT_TIMEOUT |
                                            I2C_IT_PECERR | I2C_IT_OVR | I2C_IT_AF | I2C_IT_ARLO | I2C_IT_BERR));

    INTERRUPT_RUN_LATENCY_END(I2C_ERR, 0);
}

/*****************************************************************//**
* @brief    Write data to a particular i2c bus with DMA option
*
* @param    pI2cMasterDriverInfo  - Pointer to the I2C master driver info structure
* @param    slaveAddr  - address of component to write to
* @param    buffer - pointer to the data to send
* @param    length   - number of data bytes to send.
* @param    stop    - TURE to generate stop after write, FALSE not to generate stop
*
* @return   Numebr of bytes written, 0 means either nothing written or error
**********************************************************************/
static uint16_t i2cBusMasterTransmit(I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo,
                                     const uint8_t slaveAddr, uint8_t* buffer, uint16_t length, BOOL stop)
{
    I2C_TypeDef* i2cBusRegister =
        i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cRegisterBase;
    BOOL SendStop = stop;

    if (pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
    {
        pI2cMasterDriverInfo->pTxRxBuffer = buffer;
    }
    else
    {
        // Enable the selected I2C DMA requests
        i2cBusRegister->CR2 |= I2C_CR2_DMAEN;
    }

    // Reset synchronization for write
    xEventGroupClearBits(pI2cMasterDriverInfo->i2cEventGroupHandle,
                         ebALL_EVENT_GROUP_BITS);

    // Set up address to write
    pI2cMasterDriverInfo->activeSlaveAddress = (slaveAddr & (~I2C_Direction_Receiver));

    pI2cMasterDriverInfo->bytesLeft = length;

    // Wait for I2C
    if (i2cBus_WaitForNotBusyAndStopStartPecCleared(i2cBusRegister) == FALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "WRITE: Bus %d busy or CR1 not safe to write",
            pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
        i2cMaster_SetFatalError(i2cBusRegister);
        return 0; // nothing written
    }

    // Enable ACK, since it might be disabled
    i2cBusRegister->CR1 |= I2C_CR1_ACK;

    EventBits_t bitsToWait;
    // Also checks for error conditions
    if (pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
    {
        bitsToWait = (ebALL_I2C_ERROR_BITS | ebTRANSFER_DONE_NO_DMA_BIT);
    }
    else
    {
        bitsToWait = (ebALL_I2C_ERROR_BITS | ebALL_DMA_ERROR_BITS | ebBYTE_TRANSFER_FINISHED_BIT);

        DMA_MANAGER_ERROR_CODE dmaError;

        if (DmaManager_SubmitTransfer(pI2cMasterDriverInfo->i2cDmaTxHandle,
                                      buffer, NULL, (uint32_t)&i2cBusRegister->DR, length, I2C_TIMEOUT_MS,
                                      pI2cMasterDriverInfo->pCallbackISR_Dma, (void*)pI2cMasterDriverInfo,
                                      FALSE, &dmaError) == FALSE)
        {
            LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d dma submit Tx failed: error: %d",
                pI2cMasterDriverInfo->i2cMasterConfig.busNumber, dmaError);
            return 0;
        }
    }

    // Start to write
    if (i2cStartingAndAddressing(i2cBusRegister, pI2cMasterDriverInfo->i2cEventGroupHandle) == FALSE)
    {
        return 0;
    }

    if (i2c_waitForTransactionDone(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                   bitsToWait, I2C_TIMEOUT_MS) == FALSE)
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d Tx failed",
            pI2cMasterDriverInfo->i2cMasterConfig.busNumber);

        if (!pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
        {
            DmaManager_CancelTransfer(pI2cMasterDriverInfo->i2cDmaTxHandle);
        }

        // TODO: track contiguous failures, when threshold is reached, implement a more intelligent recovery attempt.

        // issue a STOP if the bus is still busy
        SendStop = (I2C_GetFlagStatus(i2cBusRegister, I2C_FLAG_BUSY) == SET);

    }

    // Send stop bit
    if (SendStop)
    {
        I2C_GenerateSTOP(i2cBusRegister, ENABLE);
    }

    return (length - pI2cMasterDriverInfo->bytesLeft);
}

/*****************************************************************//**
* @brief    Read data from a particular i2c bus with DMA option
*
* @param    pI2cMasterDriverInfo  - Pointer to the I2C master driver
* @param    slaveAddr  - address of component to write to
* @param    buffer - pointer to the buffer to store data
* @param    length   - number of data bytes to read.
* @param    wait    - TURE to wait for I2C bus idle, FALSE not to wait
*
* @return   Numebr of bytes read, 0 means either nothing read or error
**********************************************************************/
static uint16_t i2cBusMasterReceive(I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo,
                                    const uint8_t slaveAddr, uint8_t* buffer,
                                    const uint16_t length, BOOL wait)
{
    I2C_TypeDef* i2cBusRegister =
        i2cBusDescriptor[pI2cMasterDriverInfo->i2cBusIndex].i2cRegisterBase;

    // Reset synchronization for read
    xEventGroupClearBits(pI2cMasterDriverInfo->i2cEventGroupHandle,
                         ebALL_EVENT_GROUP_BITS);

    // Set up address to read
    pI2cMasterDriverInfo->activeSlaveAddress = (slaveAddr | I2C_Direction_Receiver);

    pI2cMasterDriverInfo->bytesLeft = length;

    // Wait for I2C
    if ((wait == TRUE) && (i2cBus_WaitForNotBusyAndStopStartPecCleared(i2cBusRegister) == FALSE))
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "READ: Bus %d busy or CR1 not safe to write",
            pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
        i2cMaster_SetFatalError(i2cBusRegister);
        return 0; // nothing read
    }

    // Enable ACK, since it might be disabled
    i2cBusRegister->CR1 |= I2C_CR1_ACK;

    if (pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
    {
        // Set POS low
        i2cBusRegister->CR1 &= (~I2C_CR1_POS);
        pI2cMasterDriverInfo->pTxRxBuffer = buffer;
    }
    else
    {
        /* Enable I2C DMA request again since ISR disabled it */
        i2cBusRegister->CR2 |= I2C_CR2_DMAEN;
    }

    if (!pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
    {
        DMA_MANAGER_ERROR_CODE dmaError;

        if (DmaManager_SubmitTransfer(pI2cMasterDriverInfo->i2cDmaRxHandle,
                                      buffer, NULL, (uint32_t)&i2cBusRegister->DR, length, I2C_TIMEOUT_MS,
                                      pI2cMasterDriverInfo->pCallbackISR_Dma, (void*)pI2cMasterDriverInfo,
                                      FALSE, &dmaError) == FALSE)
        {
            LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d dma submit Rx failed: error: %d",
                pI2cMasterDriverInfo->i2cMasterConfig.busNumber, dmaError);
            return 0;
        }
    }

    // Start to read
    if (i2cStartingAndAddressing(i2cBusRegister, pI2cMasterDriverInfo->i2cEventGroupHandle) == FALSE)
    {
        return 0;
    }

    if (pI2cMasterDriverInfo->i2cMasterConfig.disableDMA)
    {
        EventBits_t bitsToWait;
        // Also checks for error conditions
        if (length == 1)
        {
            bitsToWait = (ebALL_I2C_ERROR_BITS | ebTRANSFER_DONE_NO_DMA_BIT);
        }
        else
        {
            bitsToWait = (ebALL_I2C_ERROR_BITS | ebBYTE_TRANSFER_FINISHED_BIT);
        }

        if (i2c_waitForTransactionDone(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                       bitsToWait, I2C_TIMEOUT_MS) == FALSE)
        {
            LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d Rx Failed",
                pI2cMasterDriverInfo->i2cMasterConfig.busNumber);

            // if the bus is busy, lets try sending a STOP in case the failure caused the ISR
            // to abort before issuing it. The other possibility is the slave is hung while clock stretching or
            // holding SDA low. Trying to send a STOP will not help.
            // TODO: track contiguous failures, when threshold is reached, implement a more intelligent recovery attempt.
            if (I2C_GetFlagStatus (i2cBusRegister, I2C_FLAG_BUSY) == SET)
            {
                // Send stop bit
                I2C_GenerateSTOP (i2cBusRegister, ENABLE);
            }
        }
        else
        {
            pI2cMasterDriverInfo->bytesLeft = 0;
        }
    }
    else
    {
        // Also checks for error condition, including DMA errors
        if (i2c_waitForTransactionDone(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                       (ebALL_I2C_ERROR_BITS | ebALL_DMA_ERROR_BITS | ebDMA_TRANSFER_COMPLETE_BIT), I2C_TIMEOUT_MS) == FALSE)
        {
            LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Bus %d Rx timeout",
                pI2cMasterDriverInfo->i2cMasterConfig.busNumber);
            DmaManager_CancelTransfer(pI2cMasterDriverInfo->i2cDmaRxHandle);
        }

        // Send stop bit
        I2C_GenerateSTOP (i2cBusRegister, ENABLE);
    }

    return (length - pI2cMasterDriverInfo->bytesLeft);
}


/*****************************************************************//**
* @brief    Send Start and wait for addressing with retries
*
* @param    i2cRegisterBase - Pointer to I2C register base
* @param    i2cDmaEventHandle - Handle to EventGroupHandle_t
*
* @return   TRUE if slave device acked, FALSE otherwise
**********************************************************************/
static inline BOOL i2cStartingAndAddressing(I2C_TypeDef* i2cRegisterBase,
        EventGroupHandle_t i2cEventHandle)
{
    // We assume at this point the bus check has already been done by the calling function
    // or we are doing a restart which means the bus should be busy, so we don't need to check here.
    for (int retry = 0; retry < I2C_ADDRESSING_RETRIES; retry++)
    {
        // Send start bit
        i2cRegisterBase->CR1 |= I2C_CR1_START;

        EventBits_t retval = xEventGroupWaitBits(i2cEventHandle,
                             (ebSLAVE_ADDRESSED_BIT | ebI2C_ACKNOWLEGE_FAIL_BIT),
                             pdTRUE, // ClearOnExit,
                             pdFALSE, // WaitFor any Bit(s)
                             TIMER_MSEC_TO_TICKS(I2C_ADDRESSING_TIMEOUT_MS) );

        if (retval & ebSLAVE_ADDRESSED_BIT)
        {
            return TRUE; // slave acked
        }

        // Check if ACK is failed
        if (retval & ebI2C_ACKNOWLEGE_FAIL_BIT)
        {
            LOG(i2c, ROTTEN_LOGLEVEL_VERBOSE,  "Addressing got NACK: event bit 0x%X, retry %d", retval, retry);
        }
        else
        {
            LOG(i2c, ROTTEN_LOGLEVEL_VERBOSE,  "Addressing timeout: event bit 0x%X, retry %d", retval, retry);
        }

        // At this point we failed for some reason. There should be no bits set in the CR1, if there is, it is probably
        // the START bit since that is the last thing we tried to do. So, if a bit is set, lets try to unset it and bail
        if (i2cBus_isStartOrStopOrPecSet(i2cRegisterBase) == TRUE)
        {
            //Why is it not safe at this point? Did the START not happen? lets clear it since that is the last thing we tried
            i2cRegisterBase->CR1 &= ~I2C_CR1_START;
            LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "ADDRESS: CR1 not safe to write after START attempt");
            i2cMaster_SetFatalError(i2cRegisterBase);
            return FALSE;
        }

        // Datasheet says master must generate Stop or repeated Start condition.
        // We chose to send stop and then retry.
        // If the bus is still busy, then perform a stop, otherwise something went wrong and we are not
        // expected to do a STOP.
        if (i2cRegisterBase->SR2 & I2C_SR2_BUSY)
        {
            i2cRegisterBase->CR1 |= I2C_CR1_STOP;
        }

        // We failed to get the ACK from the slave so it is time to retry, so do the normal check
        // to make sure the STOP is done. But if this is after the final attempt, do bother with a check, it will
        // be done by the next transaction.
        if ((retry < (I2C_ADDRESSING_RETRIES - 1)) && (i2cBus_WaitForNotBusyAndStopStartPecCleared(i2cRegisterBase) == FALSE))
        {
            // we waited a long time and the bus is still busy or the STOP is still set, we should bail since something is probably wrong
            i2cRegisterBase->CR1 &= ~I2C_CR1_STOP; // we failed so we should make sure the stop bit is cleared
            LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "ADDRESS: Bus busy or CR1 not safe after STOP attempt");
            i2cMaster_SetFatalError(i2cRegisterBase);
            return FALSE;
        }
    }

    return FALSE;
}


/*****************************************************************//**
* @brief    Wait for the transaction to complete
*
* @param    i2cDmaEventHandle - Handle to event group
* @param    bitsToWait - Event bits to wait for
* @param    timeoutInMs - Time out value in milli seconds
*
* @return   TRUE if transaction done without error, FALSE if error or times out
**********************************************************************/
static BOOL i2c_waitForTransactionDone(EventGroupHandle_t i2cDmaEventHandle,
                                       EventBits_t bitsToWait, uint32_t timeoutInMs)
{
    EventBits_t retval = xEventGroupWaitBits(i2cDmaEventHandle,
                         bitsToWait,
                         pdTRUE, // ClearOnExit,
                         pdFALSE, // WaitFor any Bit(s)
                         TIMER_MSEC_TO_TICKS(timeoutInMs) );

    // Check if any I2C and DMA errors bit is set
    if (0 != (retval & (ebALL_I2C_ERROR_BITS | ebALL_DMA_ERROR_BITS)))
    {
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Got error bits: 0x%X", retval);
        return FALSE;
    }

    // There are no error bits set if got here, so looking for completion bit
    if (retval == 0)
    {
        // Nothing found in any completion bits
        LOG(i2c, ROTTEN_LOGLEVEL_NORMAL, "Tansaction TO");
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************//**
* @brief    Common Callback of all I2C buses
*
* @param    pI2cMasterDriverInfo  - Pointer to the I2C master driver
* @param    bytesRemaining - Number of bytes that have not transferred
* @param    result - DMA transfer result
* @param    vYourStateBlob - The I2C driver info passed to keep track of this transfer
* @param    read - TRUE for Rx, FALSE for Tx
*
**********************************************************************/
static void callbackISR_DmaCommon(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result,
                                  void* vYourStateBlob, BOOL read)
{
    I2C_Bus_DriverInfo_t* pI2cMasterDriverInfo = (I2C_Bus_DriverInfo_t*)vYourStateBlob;

    pI2cMasterDriverInfo->bytesLeft = bytesRemaining;

    if (result.directModeError)
    {
        driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                ebDMA_DIRECT_MODE_ERROR_BIT);
    }

    if (result.transferComplete)
    {
        driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                ebDMA_TRANSFER_COMPLETE_BIT);
    }

    if (result.transferError)
    {
        driverSetEventBitsInISR(pI2cMasterDriverInfo->i2cEventGroupHandle,
                                ebDMA_TRANSFER_ERROR_BIT);
    }
}
