//
// SpiSlave.c
//

#include "project.h"
#include "SpiSlave.h"

#define SPI_SLAVE_DMA_BLOCK_TIME_MS 5

typedef struct
{
    DMA_REQUEST_HANDLE_TYPE Handle[BSPI_SLAVE_NUM_CHANNELS];
    DmaCallback Callback[BSPI_SLAVE_NUM_CHANNELS];
    DMA_MANAGER_ERROR_CODE Error[BSPI_SLAVE_NUM_CHANNELS];
} SpiSlave_DMA_t;

static void SpiSlave_InitializeGPIO(BSpiSlave_ID_t bus, const SpiSlave_Pins_t* pins);
static void SpiSlave_InitializeDMA(BSpiSlave_ID_t bus, DmaCallback txCb, DmaCallback rxCb);
static void SpiSlave_InitializeSPI(BSpiSlave_ID_t bus, const SPI_InitTypeDef* slave);

static SpiSlave_DMA_t SpiSlaveDmaInfo[BSPI_NUM_BUSSES];

static const uint8_t SpiSlave_GpioAf[BSPI_NUM_BUSSES] =
{
    GPIO_AF_SPI1,
    GPIO_AF_SPI2,
    GPIO_AF_SPI3,
};

static const DMA_CHANNEL_SELECTION SpiSlave_DmaChannel[BSPI_NUM_BUSSES][BSPI_SLAVE_NUM_CHANNELS] =
{
    {SPI1_RX, SPI1_TX},
    {SPI2_RX, SPI2_TX},
    {SPI3_RX, SPI3_TX},
};

static SPI_TypeDef* SpiSlave_Busses[BSPI_NUM_BUSSES] =
{
    SPI1,
    SPI2,
    SPI3,
};

void SpiSlave_InitializeBus(BSpiSlave_ID_t bus, const SpiSlave_Init_t* slave)
{
    debug_assert(bus < BSPI_NUM_BUSSES);
    debug_assert(slave);

    SpiSlave_InitializeGPIO(bus, &slave->Pins);
    SpiSlave_InitializeDMA(bus, slave->TxCallback, slave->RxCallback);
    SpiSlave_InitializeSPI(bus, &slave->Spi);
}

BOOL SpiSlave_Write(BSpiSlave_ID_t bus, void* data, unsigned int length)
{
    debug_assert(bus < BSPI_NUM_BUSSES);
    debug_assert(data);
    debug_assert(length > 0);

    BOOL success = FALSE;
    SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_TX] = DMA_MANAGER_NO_ERROR;
    if (InInterrupt())
    {
        success = DmaManager_SubmitTransferFromISR(SpiSlaveDmaInfo[bus].Handle[BSPI_SLAVE_TX],
                  data,
                  length,
                  SpiSlaveDmaInfo[bus].Callback[BSPI_SLAVE_TX]);
    }
    else
    {
        success = DmaManager_SubmitTransfer(SpiSlaveDmaInfo[bus].Handle[BSPI_SLAVE_TX], data, NULL, (uint32_t)&SpiSlave_Busses[bus]->DR, length,
                                            SPI_SLAVE_DMA_BLOCK_TIME_MS, SpiSlaveDmaInfo[bus].Callback[BSPI_SLAVE_TX],
                                            NULL, FALSE, &SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_TX]);
    }
    return (success && (SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_TX] == DMA_MANAGER_NO_ERROR));
}

BOOL SpiSlave_Read(BSpiSlave_ID_t bus, void* data, unsigned int length)
{
    debug_assert(bus < BSPI_NUM_BUSSES);
    debug_assert(data);
    debug_assert(length > 0);

    BOOL success = FALSE;
    SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_RX] = DMA_MANAGER_NO_ERROR;
    if (InInterrupt())
    {
        success = DmaManager_SubmitTransferFromISR(SpiSlaveDmaInfo[bus].Handle[BSPI_SLAVE_RX],
                  data,
                  length,
                  SpiSlaveDmaInfo[bus].Callback[BSPI_SLAVE_RX]);
    }
    else
    {
        success = DmaManager_SubmitTransfer(SpiSlaveDmaInfo[bus].Handle[BSPI_SLAVE_RX], data, NULL, (uint32_t)&SpiSlave_Busses[bus]->DR,
                                            length, SPI_SLAVE_DMA_BLOCK_TIME_MS,
                                            SpiSlaveDmaInfo[bus].Callback[BSPI_SLAVE_RX],
                                            NULL, TRUE, &SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_RX]);
    }
    return (success && (SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_RX] == DMA_MANAGER_NO_ERROR));
}

unsigned int SpiSlave_GetBytesLeft(BSpiSlave_ID_t bus, BSpiSlave_Channel_t channel)
{
    debug_assert(bus < BSPI_NUM_BUSSES);
    debug_assert(channel < BSPI_SLAVE_NUM_CHANNELS);
    uint32_t bytesLeft = 0;
    DmaManager_GetBytesRemaining(SpiSlaveDmaInfo[bus].Handle[channel], &bytesLeft);
    return bytesLeft;
}

BOOL SpiSlave_CancelTransfer(BSpiSlave_ID_t bus, BSpiSlave_Channel_t channel)
{
    debug_assert(bus < BSPI_NUM_BUSSES);
    debug_assert(channel < BSPI_SLAVE_NUM_CHANNELS);
    return DmaManager_CancelTransfer(SpiSlaveDmaInfo[bus].Handle[channel]);
}

void SpiSlave_DeInitBus(BSpiSlave_ID_t bus)
{
    debug_assert(bus < BSPI_NUM_BUSSES);
    SPI_I2S_DeInit(SpiSlave_Busses[bus]);
}

DMA_MANAGER_ERROR_CODE SpiSlave_GetErrorState(BSpiSlave_ID_t bus, BSpiSlave_Channel_t channel)
{
    debug_assert(bus < BSPI_NUM_BUSSES);
    debug_assert(channel < BSPI_SLAVE_NUM_CHANNELS);
    return SpiSlaveDmaInfo[bus].Error[channel];
}

static void SpiSlave_InitializeGPIO(BSpiSlave_ID_t bus, const SpiSlave_Pins_t* pins)
{
    GPIO_InitTypeDef gpio;

    // common settings
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

    // MOSI
    gpio.GPIO_Pin = 1 << pins->MosiPin;
    GPIO_Init(pins->MosiBank, &gpio);
    GPIO_PinAFConfig(pins->MosiBank, pins->MosiPin, SpiSlave_GpioAf[bus]);

    // MISO
    gpio.GPIO_Pin = 1 << pins->MisoPin;
    GPIO_Init(pins->MisoBank, &gpio);
    GPIO_PinAFConfig(pins->MisoBank, pins->MisoPin, SpiSlave_GpioAf[bus]);

    // CLK
    gpio.GPIO_Pin = 1 << pins->ClockPin;
    GPIO_Init(pins->ClockBank, &gpio);
    GPIO_PinAFConfig(pins->ClockBank, pins->ClockPin, SpiSlave_GpioAf[bus]);

    // Chip Select
    gpio.GPIO_Pin = 1 << pins->ChipSelectPin;
    GPIO_Init(pins->ChipSelectBank, &gpio);
    GPIO_PinAFConfig(pins->ChipSelectBank, pins->ChipSelectPin, SpiSlave_GpioAf[bus]);
}

static void SpiSlave_InitializeDMA(BSpiSlave_ID_t bus, DmaCallback txCb, DmaCallback rxCb)
{
    DMA_MANAGER_REQUEST_TYPE DmaRequest =
    {
        .selectedChannel = SpiSlave_DmaChannel[bus][BSPI_SLAVE_TX],
        .transferComplete = 1,
        .transferError = 1,
        .halfTransfer = 0,
        .directModeError = 0,
        .fifoModeError = 0,
        .dmaPriority = 1,
        .doubleBuffer = 0,
    };

    SpiSlaveDmaInfo[bus].Handle[BSPI_SLAVE_TX] = dmaManager_ConfigureChannel(&DmaRequest, &SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_TX]);
    debug_assert(SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_TX] == DMA_MANAGER_NO_ERROR);

    DmaRequest.selectedChannel = SpiSlave_DmaChannel[bus][BSPI_SLAVE_RX];
    SpiSlaveDmaInfo[bus].Handle[BSPI_SLAVE_RX] = dmaManager_ConfigureChannel(&DmaRequest, &SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_RX]);
    debug_assert(SpiSlaveDmaInfo[bus].Error[BSPI_SLAVE_RX] == DMA_MANAGER_NO_ERROR);

    SPI_I2S_DMACmd(SpiSlave_Busses[bus], SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_I2S_DMACmd(SpiSlave_Busses[bus], SPI_I2S_DMAReq_Rx, ENABLE);

    SpiSlaveDmaInfo[bus].Callback[BSPI_SLAVE_TX] = txCb;
    SpiSlaveDmaInfo[bus].Callback[BSPI_SLAVE_RX] = rxCb;
}

static void SpiSlave_InitializeSPI(BSpiSlave_ID_t bus, const SPI_InitTypeDef* slave)
{
    SPI_Init(SpiSlave_Busses[bus], (SPI_InitTypeDef*) slave);
    SPI_I2S_ITConfig(SpiSlave_Busses[bus], SPI_I2S_IT_RXNE, ENABLE);
    SPI_Cmd(SpiSlave_Busses[bus], ENABLE);
}
