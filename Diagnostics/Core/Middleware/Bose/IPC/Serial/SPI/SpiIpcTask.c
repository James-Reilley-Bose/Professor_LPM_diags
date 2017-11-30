//
// SpiIpcTask.c
//

#include "project.h"
#include "TaskDefs.h"
#include "SpiIpcTask.h"
#include "SpiSlave.h"
#include "IPCRouterTask.h"

SCRIBE_DECL(ipc);

#define RIVIERA_SPI_DMA_BLOCK_TIME_MS 5
#define RIVIERA_CTS_TIMEOUT TIMER_MSEC_TO_TICKS(50)

static void SpiIpcTask_StartSpi(void);
static void SpiIpcTask_ConfigureGPIO(void);
static void SpiIpcTask_ConfigureEXTI(void);
static void SpiIpcTask_EnableEXTI(BOOL enable);
static void SpiIpcTask_PacketRxCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read);
//static void SpiIpcTask_LogPacket(uint8_t* packet);
static void SpiIpcTask_SendPacket(IpcPacket_t* p);
static void SpiIpcTask_RequestToSend(BOOL request);
//static BOOL SpiIpcTask_IsTransmitInProgress(void);
static void SpiIpcTask_HangOutRead(void);
static void SpiIpcTask_DeInitSpi(void);
static void SpiIpcTask_CancelTransmit(void);
static void SpiIpcTask_ProcessReceivedPacket(IpcPacket_t* p);

ManagedTask* SpiIpcTaskHandle = NULL;

static const SpiSlave_Init_t SpiInit =
{
    .Pins = {
        .MosiBank = RIVIERA_SPI_MOSI_BANK,
        .MosiPin = RIVIERA_SPI_MOSI_PIN,
        .MisoBank = RIVIERA_SPI_MISO_BANK,
        .MisoPin = RIVIERA_SPI_MISO_PIN,
        .ClockBank = RIVIERA_SPI_CLK_BANK,
        .ClockPin = RIVIERA_SPI_CLK_PIN,
        .ChipSelectBank = RIVIERA_SPI_CS_BANK,
        .ChipSelectPin = RIVIERA_SPI_CS_PIN,
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
    .TxCallback = NULL,
    .RxCallback = SpiIpcTask_PacketRxCallback,
};

static SemaphoreHandle_t CtsSemaphore = NULL;
static IpcInterface_t IpcInterface =
{
    .DeviceID = IPC_DEVICE_SOUNDTOUCH,
    .type = IPC_INTERFACE_SPI,
    .spi = {0},
    .stat = {0},
    .test = {0},
};

void SpiIpcTask_Init(void* p)
{
    SpiIpcTaskHandle = GetManagedTaskPointer(SPI_IPC_TASK_NAME);
    debug_assert(SpiIpcTaskHandle);

    CtsSemaphore = xSemaphoreCreateBinary();
    debug_assert(CtsSemaphore);

    IPCRouter_RegisterDevice(&IpcInterface, SpiIpcTaskHandle);
}

void SpiIpcTask(void* pvParameters)
{
    for (;;)
    {
        TaskManagerPollQueue(SpiIpcTaskHandle);
    }
}

void SpiIpcTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case SpiIpc_Msg_ID_InitializeSpi:
            SpiIpcTask_StartSpi();
            break;
        case SpiIpc_Msg_ID_DeInitSpi:
            SpiIpcTask_DeInitSpi();
            break;
        case SpiIpc_Msg_ID_CancelTransmit:
            SpiIpcTask_CancelTransmit();
            break;
        case IPC_TX_MESSAGE_ID_Send:
            SpiIpcTask_SendPacket((IpcPacket_t*) msg->params[0]);
            break;
        case IPCROUTER_MESSAGE_ID_RxMessage:
            SpiIpcTask_ProcessReceivedPacket((IpcPacket_t*) msg->params[0]);
            break;
        default:
            LOG(ipc, ROTTEN_LOGLEVEL_NORMAL, "Unhandled message %d in %s", __func__);
            break;
    }
}

static void SpiIpcTask_StartSpi(void)
{
    SpiSlave_InitializeBus(RIVIERA_SPI_BID, &SpiInit);
    SpiIpcTask_ConfigureGPIO();
    SpiIpcTask_ConfigureEXTI();
    SpiIpcTask_EnableEXTI(TRUE);
    SpiIpcTask_HangOutRead();
}

static void SpiIpcTask_ConfigureGPIO(void)
{
    SpiIpcTask_RequestToSend(FALSE);
    ConfigureGpioPin(RIVIERA_RTS_GPIO_BANK,
                     RIVIERA_RTS_GPIO_PIN,
                     GPIO_MODE_OUT_PP,
                     NO_ALT_FUNC);
}

static void SpiIpcTask_ConfigureEXTI(void)
{
    ConfigureGpioPin(RIVIERA_CTS_GPIO_BANK,
                     RIVIERA_CTS_GPIO_PIN,
                     GPIO_MODE_IN_FLOATING,
                     NO_ALT_FUNC);

    SYSCFG_EXTILineConfig(RIVIERA_CTS_IRQ_PORT_SOURCE, RIVIERA_CTS_IRQ_LINE_NUM);

    SpiIpcTask_EnableEXTI(FALSE);
    EXTI_ClearITPendingBit(RIVIERA_CTS_IRQ_LINE);

    Interrupt_RegisterISR(RIVIERA_CTS_IRQ_CHANNEL, RIVIERA_CTS_IRQ_HANDLER);
    Interrupt_Enable(RIVIERA_CTS_IRQ_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

static void SpiIpcTask_EnableEXTI(BOOL enable)
{
    EXTI_InitTypeDef EXTI_InitStructure =
    {
        .EXTI_Mode    = EXTI_Mode_Interrupt,
        .EXTI_Line    = RIVIERA_CTS_IRQ_PIN_MASK,
        .EXTI_Trigger = (RIVIERA_CTS_IRQ_ACTIVE_STATE == GPIO_HIGH) ? EXTI_Trigger_Rising : EXTI_Trigger_Falling,
    };
    EXTI_InitStructure.EXTI_LineCmd = enable ? ENABLE : DISABLE;
    EXTI_Init(&EXTI_InitStructure);
}

static void SpiIpcTask_PacketRxCallback(uint16_t bytesRemaining, DMA_TRANSFER_RESULT_TYPE result, void* vYourStateBlob, BOOL read)
{
    SpiIpcPostMsg(IPCROUTER_MESSAGE_ID_RxMessage, (uint32_t) IpcInterface.spi.packet, 0);
    SpiIpcTask_HangOutRead();
}


static void SpiIpcTask_SendPacket(IpcPacket_t* p)
{
    unsigned int time = GET_SYSTEM_UPTIME_MS();
    SpiIpcTask_RequestToSend(TRUE);
    if (xSemaphoreTake(CtsSemaphore, RIVIERA_CTS_TIMEOUT) == pdFALSE)
    {
        LOG(ipc, ROTTEN_LOGLEVEL_NORMAL, "CTS timed out.");
        IpcInterface.stat.errors++;
    }
    else
    {
        time = GET_MILLI_SINCE(time);
        IpcInterface.spi.stat.totalLatency += time;
        IpcInterface.spi.stat.worstLatency = MAX(time, IpcInterface.spi.stat.worstLatency);
        BOOL success = SpiSlave_Write(RIVIERA_SPI_BID, p, sizeof(IpcPacket_t));
        if(!success)
        {
          LOG(ipc, ROTTEN_LOGLEVEL_NORMAL, "SpiIpcTask_SendPacket Failed!");
        }
        else
        {
            IpcLogPacket(p);
        }
    }
    SpiIpcTask_RequestToSend(FALSE);
    vTaskDelay(5); //TODO - lb1032816 - Remove this when 'packet overlap' problem is fixed
}

static void SpiIpcTask_RequestToSend(BOOL request)
{
    if (request)
    {
        ASSERT_GPIO_PIN_FG(RIVIERA_RTS_GPIO_BANK, RIVIERA_RTS_GPIO_PIN);
    }
    else
    {
        DEASSERT_GPIO_PIN_FG(RIVIERA_RTS_GPIO_BANK, RIVIERA_RTS_GPIO_PIN);
    }
}

#if 0
static BOOL SpiIpcTask_IsTransmitInProgress(void)
{
    uint32_t rxBytesLeft = SpiSlave_GetBytesLeft(RIVIERA_SPI_BID, BSPI_SLAVE_RX);
    uint32_t txBytesLeft = SpiSlave_GetBytesLeft(RIVIERA_SPI_BID, BSPI_SLAVE_TX);
    return (((rxBytesLeft != 0) && (rxBytesLeft != sizeof(IpcPacket_t))) ||
            (txBytesLeft > 0));
}
#endif

static void SpiIpcTask_HangOutRead(void)
{
    IpcInterface.spi.packet = IPCRouter_GetNextRxBuffer();
    SpiSlave_Read(RIVIERA_SPI_BID, IpcInterface.spi.packet, sizeof(IpcPacket_t));
}

static void SpiIpcTask_DeInitSpi(void)
{
    SpiSlave_CancelTransfer(RIVIERA_SPI_BID, BSPI_SLAVE_RX);
    SpiIpcTask_CancelTransmit();
    SpiSlave_DeInitBus(RIVIERA_SPI_BID);
}

static void SpiIpcTask_CancelTransmit(void)
{
    if (SpiSlave_GetBytesLeft(RIVIERA_SPI_BID, BSPI_SLAVE_TX) != 0)
    {
        SpiSlave_CancelTransfer(RIVIERA_SPI_BID, BSPI_SLAVE_TX);
    }
}

SemaphoreHandle_t SpiIpcTask_GetCtsSemHandle(void)
{
    return CtsSemaphore;
}

// Validate the packet. Since this is SPI, we always clock data in.
static const uint8_t emptyHeader[4] = {0};
static void SpiIpcTask_ProcessReceivedPacket(IpcPacket_t* p)
{
    if((p->s.checkXOR == 0) && (memcmp(p, emptyHeader, sizeof(emptyHeader)) == 0))
    {
        IPCRouter_ReturnRxBuffer(p);
    }
    else
    {
        IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_RxMessage, (uint32_t) &IpcInterface, (uint32_t) p);
    }
}

