//
// SpiSlaveTestTask.c
//

#include "project.h"
#include "TaskDefs.h"
#include "SpiSlaveTestTask.h"
#include "SpiSlave.h"

SCRIBE_DECL(system);

#define SPI_SLAVE_DMA_BLOCK_TIME_MS 5
#define PACKET_SIZE 128
#define NUM_RX_BUFS 8
#define SPI_SLAVE_CTS_TIMEOUT TIMER_MSEC_TO_TICKS(1000)

static void SpiSlaveTask_StartSpi(void);
static void SpiSlaveTask_ConfigureGPIO(void);
static void SpiSlaveTask_ConfigureEXTI(void);
static void SpiSlaveTask_EnableEXTI(BOOL enable);
static void SpiSlaveTask_PacketRxCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read);
static void SpiSlaveTask_PacketTxCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read);
static void SpiSlaveTask_HandleReceivedPacket(uint8_t* p);
static void SpiSlaveTask_LogReceivedPacket(uint8_t* p);
static void SpiSlaveTask_SendPacket(void);
static void SpiSlaveTask_RequestToSend(BOOL request);
static void SpiSlave_HangOutRead(void);
static void SpiSlaveTask_DeInitSpi(void);
static void SpiSlaveTask_CancelTransmit(void);

ManagedTask* SpiSlaveTestTask = NULL;

static const SpiSlave_Init_t SpiInit =
{
    .Pins = {
        .MosiBank = SPI_SLAVE_MOSI_BANK,
        .MosiPin = SPI_SLAVE_MOSI_PIN,
        .MisoBank = SPI_SLAVE_MISO_BANK,
        .MisoPin = SPI_SLAVE_MISO_PIN,
        .ClockBank = SPI_SLAVE_SCK_BANK,
        .ClockPin = SPI_SLAVE_SCK_PIN,
        .ChipSelectBank = SPI_SLAVE_CS_BANK,
        .ChipSelectPin = SPI_SLAVE_CS_PIN,
    },
    .Spi = {
        .SPI_Direction = SPI_Direction_2Lines_FullDuplex,
        .SPI_Mode = SPI_Mode_Slave,
        .SPI_DataSize = SPI_DataSize_8b,
        .SPI_CPOL = SPI_CPOL_Low, // clock is low when idle
        .SPI_CPHA = SPI_CPHA_1Edge, // data sampled at first edge
        .SPI_NSS = SPI_NSS_Hard,
        .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2,
        .SPI_FirstBit = SPI_FirstBit_MSB,
    },
    .TxCallback = SpiSlaveTask_PacketTxCallback,
    .RxCallback = SpiSlaveTask_PacketRxCallback,
};

static uint8_t txBuffer[PACKET_SIZE] = {0};
static uint8_t rxBuffer[NUM_RX_BUFS][PACKET_SIZE] = {0};
static unsigned int RxBufSelect = 0;
static SemaphoreHandle_t CtsSemaphore = NULL;

void SpiSlaveTask_Init(void* p)
{
    SpiSlaveTestTask = GetManagedTaskPointer("SpiSlaveTestTask");
    debug_assert(SpiSlaveTestTask);

    CtsSemaphore = xSemaphoreCreateBinary();
    debug_assert(CtsSemaphore);

    for (uint_fast8_t i = 0; i < PACKET_SIZE; i++)
    {
        txBuffer[i] = i;
    }
}

void SpiSlaveTask(void* pvParameters)
{
    for (;;)
    {
        TaskManagerPollQueue(SpiSlaveTestTask);
    }
}

void SpiSlaveTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case SpiSlaveTest_Msg_ID_InitializeSpi:
            SpiSlaveTask_StartSpi();
            break;
        case SpiSlaveTest_Msg_ID_ReceivedPacket:
            SpiSlaveTask_HandleReceivedPacket((uint8_t*) msg->params[0]);
            break;
        case SpiSlaveTest_Msg_ID_SendPacket:
            SpiSlaveTask_SendPacket();
            break;
        case SpiSlaveTest_Msg_ID_DeInitSpi:
            SpiSlaveTask_DeInitSpi();
            break;
        case SpiSlaveTest_Msg_ID_CancelTransmit:
            SpiSlaveTask_CancelTransmit();
            break;
        case SpiSlaveTest_Msg_ID_SendPacketComplete:
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Tx complete: %d bytes remaining, result = %d", msg->params[0], msg->params[1]);
            break;
        default:
            LOG(system, ROTTEN_LOGLEVEL_NORMAL, "Unhandled message %d in %s", __func__);
            break;
    }
}

static void SpiSlaveTask_StartSpi(void)
{
    SpiSlave_InitializeBus(SPI_SLAVE_BID, &SpiInit);
    SpiSlaveTask_ConfigureGPIO();
    SpiSlaveTask_ConfigureEXTI();
    SpiSlaveTask_EnableEXTI(TRUE);
    SpiSlave_HangOutRead();
}

static void SpiSlaveTask_ConfigureGPIO(void)
{
    SpiSlaveTask_RequestToSend(FALSE);
    ConfigureGpioPin(SPI_SLAVE_EXTI_BANK,
                     SPI_SLAVE_EXTI_PIN,
                     GPIO_MODE_OUT_PP,
                     NO_ALT_FUNC);
}

static void SpiSlaveTask_ConfigureEXTI(void)
{
    ConfigureGpioPin(SPI_SLAVE_CTS_GPIO_BANK,
                     SPI_SLAVE_CTS_GPIO_PIN,
                     GPIO_MODE_IN_PULLDN,
                     NO_ALT_FUNC);

    SYSCFG_EXTILineConfig(SPI_SLAVE_CTS_IRQ_PORT_SOURCE, SPI_SLAVE_CTS_IRQ_LINE_NUM);

    SpiSlaveTask_EnableEXTI(FALSE);
    EXTI_ClearITPendingBit(SPI_SLAVE_CTS_IRQ_LINE);

    Interrupt_RegisterISR(SPI_SLAVE_CTS_IRQ_CHANNEL, SPI_SLAVE_CTS_IRQ_HANDLER);
    Interrupt_Enable(SPI_SLAVE_CTS_IRQ_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

static void SpiSlaveTask_EnableEXTI(BOOL enable)
{
    EXTI_InitTypeDef EXTI_InitStructure =
    {
        .EXTI_Mode    = EXTI_Mode_Interrupt,
        .EXTI_Line    = SPI_SLAVE_CTS_IRQ_PIN_MASK,
        .EXTI_Trigger = EXTI_Trigger_Rising,
    };
    EXTI_InitStructure.EXTI_LineCmd = enable ? ENABLE : DISABLE;
    EXTI_Init(&EXTI_InitStructure);
}

static void SpiSlaveTask_PacketRxCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read)
{
    SpiSlaveTestPostMsg(SpiSlaveTest_Msg_ID_ReceivedPacket, (uint32_t) rxBuffer[RxBufSelect], 0);
    RxBufSelect = (RxBufSelect + 1) % NUM_RX_BUFS;
    SpiSlave_HangOutRead();
}

static void SpiSlaveTask_PacketTxCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read)
{
    uint8_t r = 0;
    memcpy(&r, &result, sizeof(r));
    SpiSlaveTestPostMsg(SpiSlaveTest_Msg_ID_SendPacketComplete, bytesRemaining, r);
}

static const uint8_t EmptyPacket[PACKET_SIZE] = {0};

static void SpiSlaveTask_HandleReceivedPacket(uint8_t* p)
{
    // Ignore empty packets
    if (memcmp(p, EmptyPacket, PACKET_SIZE) == 0)
    {
        return;
    }

    if (p[0] == 0xFF)
    {
        SpiSlaveTestPostMsg(SpiSlaveTest_Msg_ID_SendPacket, 0, 0);
    }
    SpiSlaveTask_LogReceivedPacket(p);
}

static void SpiSlaveTask_LogReceivedPacket(uint8_t* p)
{
    uint32_t* pWord = (uint32_t*) p;
    for (uint_fast8_t i = 0; i < 4; i++)
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "[%2d - %2d]: %08X %08X %08X %08X %08X %08X %08X %08X", (i * 8 + 0), (i * 8 + 7),
            __REV(pWord[i * 8 + 0]),  __REV(pWord[i * 8 + 1]),  __REV(pWord[i * 8 + 2]),  __REV(pWord[i * 8 + 3]),
            __REV(pWord[i * 8 + 4]),  __REV(pWord[i * 8 + 5]),  __REV(pWord[i * 8 + 6]),  __REV(pWord[i * 8 + 7]));
    }
}

static void SpiSlaveTask_SendPacket(void)
{
    SpiSlaveTask_RequestToSend(TRUE);
    if (xSemaphoreTake(CtsSemaphore, SPI_SLAVE_CTS_TIMEOUT) == pdFALSE)
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "CTS timed out.");
    }
    else
    {
        BOOL success = SpiSlave_Write(SPI_SLAVE_BID, txBuffer, PACKET_SIZE);
    }
    SpiSlaveTask_RequestToSend(FALSE);
}

static void SpiSlaveTask_RequestToSend(BOOL request)
{
    if (request)
    {
        ASSERT_GPIO_PIN_FG(SPI_SLAVE_EXTI_BANK, SPI_SLAVE_EXTI_PIN);
    }
    else
    {
        DEASSERT_GPIO_PIN_FG(SPI_SLAVE_EXTI_BANK, SPI_SLAVE_EXTI_PIN);
    }
}

static void SpiSlave_HangOutRead(void)
{
    SpiSlave_Read(SPI_SLAVE_BID, rxBuffer[RxBufSelect], PACKET_SIZE);
}

static void SpiSlaveTask_DeInitSpi(void)
{
    SpiSlave_CancelTransfer(SPI_SLAVE_BID, BSPI_SLAVE_RX);
    SpiSlaveTask_CancelTransmit();
    SpiSlave_DeInitBus(SPI_SLAVE_BID);
}

static void SpiSlaveTask_CancelTransmit(void)
{
    if (SpiSlave_GetBytesLeft(SPI_SLAVE_BID, BSPI_SLAVE_TX) != 0)
    {
        SpiSlave_CancelTransfer(SPI_SLAVE_BID, BSPI_SLAVE_TX);
    }
}

SemaphoreHandle_t SpiSlaveTask_GetCtsSemHandle(void)
{
    return CtsSemaphore;
}
