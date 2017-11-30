//
// SpiSlave.h
//

#ifndef SPI_SLAVE_H
#define SPI_SLAVE_H

#include "DmaManager.h"

typedef enum
{
    BSPI1 = 0,
    BSPI2 = 1,
    BSPI3 = 2,
    BSPI_NUM_BUSSES
} BSpiSlave_ID_t;

typedef struct
{
    GPIO_TypeDef* MosiBank;
    uint16_t MosiPin;
    GPIO_TypeDef* MisoBank;
    uint16_t MisoPin;
    GPIO_TypeDef* ClockBank;
    uint16_t ClockPin;
    GPIO_TypeDef* ChipSelectBank;
    uint16_t ChipSelectPin;
} SpiSlave_Pins_t;

typedef struct
{
    SpiSlave_Pins_t Pins;
    SPI_InitTypeDef Spi;
    DmaCallback TxCallback;
    DmaCallback RxCallback;
} SpiSlave_Init_t;

typedef enum
{
    BSPI_SLAVE_RX,
    BSPI_SLAVE_TX,
    BSPI_SLAVE_NUM_CHANNELS
} BSpiSlave_Channel_t;

void SpiSlave_InitializeBus(BSpiSlave_ID_t bus, const SpiSlave_Init_t* slave);
BOOL SpiSlave_Write(BSpiSlave_ID_t bus, void* data, unsigned int length);
BOOL SpiSlave_Read(BSpiSlave_ID_t bus, void* data, unsigned int length);
unsigned int SpiSlave_GetBytesLeft(BSpiSlave_ID_t bus, BSpiSlave_Channel_t channel);
BOOL SpiSlave_CancelTransfer(BSpiSlave_ID_t bus, BSpiSlave_Channel_t channel);
void SpiSlave_DeInitBus(BSpiSlave_ID_t bus);
DMA_MANAGER_ERROR_CODE SpiSlave_GetErrorState(BSpiSlave_ID_t bus, BSpiSlave_Channel_t channel);

#endif // SPI_SLAVE_H
