//
// DeviceModel_DSP_Standin.c - This is just a standin to initialize the IPC interface
//                             until the real device model is brought in.
//

#include "project.h"
#include "IPCRouterTask.h"
#include "IpcInterface.h"
#include "DeviceModel_DSP_Standin.h"
#include "IpcTxTask.h"
#include "IpcUart.h"

static void DeviceModel_DSP_Standin_EnablePower(BOOL enable);
static void DeviceModel_DSP_Standin_TurnOn(void);
static void DeviceModel_DSP_Standin_TurnOff(void);

static IpcInterface_t IpcInterface =
{
    .DeviceID = IPC_DEVICE_DSP,
    .type = IPC_INTERFACE_UART,
    .uart =
    {
        .BUART = DSP_BUART,
        .buartInit =
        {
            BR_921600, // 921600
            WL_8BITS,  // 8 bit
            SB_1,      // 1 stop bit
            P_NONE,    // No parity
            M_RX_TX,   // RX and TX
            FC_NONE    // No Flow Control
        },
        .RxBank = DSP_RX_GPIO_BANK,
        .RxPin = DSP_RX_PIN,
        .RxPadConfig = GPIO_MODE_AF_IN_FLOATING,
        .TxBank = DSP_TX_GPIO_BANK,
        .TxPin = DSP_TX_PIN,
        .TxPadConfig = GPIO_MODE_AF_OUT_PP,
        .AF = DSP_GPIO_AF,
        .valid = TRUE,
        .tx =
        {
            .DeviceID = IPC_DEVICE_DSP,
            .DmaTxRequest =
            {
                .selectedChannel = USART6_TX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 0,
            },
            .RegisterBase = DSP_UART,
        },
        .rx =
        {
            .DmaRxRequest =
            {
                .selectedChannel = USART6_RX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 1,
            },
            .RegisterBase = DSP_UART,
            ._bServiceStarted = FALSE,
            .packet = NULL,
            .nextPacket = NULL,
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
            ._RXDMAResetCount = 0,
        },
    },
    .stat = {0},
    .test = {0},
};

void DeviceModel_DSP_Standin_Init(void* p)
{
    ASSERT_GPIO_PIN_FG(DSP_LEVEL_SHIFT_L_BANK, DSP_LEVEL_SHIFT_L_PIN);
    ConfigureGpioPin(DSP_LEVEL_SHIFT_L_BANK, DSP_LEVEL_SHIFT_L_PIN, GPIO_MODE_OUT_PP, 0);

    Ipc_InitializeUart(&IpcInterface);
    IPCRouter_RegisterDevice(&IpcInterface, GetManagedTaskPointer(IpcTaskNames[IpcInterface.DeviceID]));
}

void DeviceModel_DSP_Standin_Run(void* pvParameters)
{
    DEASSERT_GPIO_PIN_FG(DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN);
    ConfigureGpioPin(DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN, GPIO_MODE_OUT_PP, 0);
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));

    for (;;)
    {
        TaskManagerPollQueue(ManagedIpcTxTasks[IpcInterface.DeviceID]);
    }
}

void DeviceModel_DSP_Standin_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case DSP_DM_Msg_Power:
            DeviceModel_DSP_Standin_EnablePower(msg->params[0]);
            break;
        default:
            break;
    }
}

static void DeviceModel_DSP_Standin_EnablePower(BOOL enable)
{
    if (enable)
    {
        DeviceModel_DSP_Standin_TurnOn();
    }
    else
    {
        DeviceModel_DSP_Standin_TurnOff();
    }
}

static void DeviceModel_DSP_Standin_TurnOn(void)
{
    vTaskDelay(TIMER_MSEC_TO_TICKS(200));
    ASSERT_GPIO_PIN_FG(DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN);
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));
    BUSART_Cmd(DSP_BUART, PS_ENABLE);
    BUSART_DMACmd(DSP_BUART, DMA_TX_RX, PS_ENABLE);
    DEASSERT_GPIO_PIN_FG(DSP_LEVEL_SHIFT_L_BANK, DSP_LEVEL_SHIFT_L_PIN);
}

static void DeviceModel_DSP_Standin_TurnOff(void)
{
    ASSERT_GPIO_PIN_FG(DSP_LEVEL_SHIFT_L_BANK, DSP_LEVEL_SHIFT_L_PIN);
    BUSART_Cmd(DSP_BUART, PS_DISABLE);
    BUSART_DMACmd(DSP_BUART, DMA_TX_RX, PS_DISABLE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(50));
    DEASSERT_GPIO_PIN_FG(DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN);
}
