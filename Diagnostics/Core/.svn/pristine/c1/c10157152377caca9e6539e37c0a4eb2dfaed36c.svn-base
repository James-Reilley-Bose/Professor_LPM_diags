#ifndef _IPC_INTERFACE_H_
#define _IPC_INTERFACE_H_

#include "IpcTx.h"
#include "BoseUSART_API.h"
#include "i2cMaster_API.h"

typedef uint8_t IpcDeviceId_t;

typedef struct
{
    DMA_MANAGER_REQUEST_TYPE DmaRxRequest;
    DMA_REQUEST_HANDLE_TYPE DmaRxHandle;
    DMA_MANAGER_ERROR_CODE DmaRxError;
    USART_TypeDef* RegisterBase;
    BOOL _bServiceStarted;
    IpcPacket_t* packet;
    IpcPacket_t* nextPacket;
    void (*_RXDoneCallback)(void* pThis);
    uint32_t _RXDMAResetCount;
} IpcRxConfig_t;

typedef struct
{
    BU_Id_t BUART;
    BU_InitTypeDef buartInit;
    GPIO_TypeDef* RxBank;
    uint8_t RxPin;
    GPIO_MODE_t RxPadConfig;
    GPIO_TypeDef* TxBank;
    uint8_t TxPin;
    GPIO_MODE_t TxPadConfig;
    uint8_t AF;
    BOOL valid;
    IpcRxConfig_t rx;
    IpcTxConfig_t tx;
} IpcUartConfig_t;

typedef struct
{
    BOOL valid;
    I2C_BUS_HANDLE_TYPE handle;
    I2C_Master_Config_t config;
    I2C_API_ERROR_CODE error;
    uint8_t slaveAddress;
    uint8_t slaveRegWrite;
    uint8_t slaveRegRead;
    IpcPacket_t* packet;
    BOOL recoverOnError;
} IpcI2CConfig_t;

typedef struct
{
    unsigned int totalLatency;
    unsigned int worstLatency;
} IpcSpiStat_t;

typedef struct
{
    IpcPacket_t* packet;
    IpcSpiStat_t stat;
} IpcSpiConfig_t;

typedef enum
{
    IPC_INTERFACE_INVALID,
    IPC_INTERFACE_UART,
    IPC_INTERFACE_I2C,
    IPC_INTERFACE_SPI,
    IPC_INTERFACE_LOOPBACK,

    IPC_INTERFACE_NUM
} IpcInterfaceType_t;

typedef struct
{
    unsigned int messagesRxd;
    unsigned int errors;
    unsigned int dmaErrors;
    unsigned int messagesForwarded;
} IpcStats_t;

typedef struct
{
    BOOL echoActive;
    unsigned int echoCount;
} IpcTest_t;

typedef struct
{
    uint8_t DeviceID;
    IpcInterfaceType_t type;
    union
    {
        IpcUartConfig_t uart;
        IpcI2CConfig_t i2c;
        IpcSpiConfig_t spi;
    };
    IpcStats_t stat;
    IpcTest_t test;
} IpcInterface_t;

uint32_t IpcInterface_GetIndexFromIpcDevice(ipc_uint8_t dev);
#define IPC_INTERFACE_INVALID 0xFFFFFFFF

#endif // _IPC_INTERFACE_H_
