
#include "IpcInterface.h"
#include "IpcProtocolLpm.h"
#include "IPCRouterTask.h"

#if 0
IpcInterface_t IpcInterfaceTable[IPC_NUM_DEVICES] =
{
    // Invalid
    {
        .DeviceID = IPC_DEVICE_INVALID,
        .valid = FALSE,
        .uart =
        {
            .valid = FALSE,
        },
        .i2c =
        {
            .valid = FALSE,
        },
    },

    // DSP
    {
        .DeviceID = IPC_DEVICE_DSP,
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
            .DeviceID = IPC_DEVICE_DSP,
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
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
            ._RXMsgCount = 0,
            ._RXErrCount = 0,
            ._RXDmaErrCount = 0,
            ._RTWOn = FALSE,
        },
        .uart =
        {
            .valid = TRUE,
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
        },
        .i2c =
        {
            .valid = FALSE,
        }
    },

    // LPM - the rx_config is used by IPC loopback via TAP
    {
        .DeviceID = IPC_DEVICE_LPM,
        .valid = TRUE,
        .rx =
        {
            .DeviceID = IPC_DEVICE_LPM,
            .packet = NULL,
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
            ._RXMsgCount = 0,
            ._RXErrCount = 0,
            ._RXDmaErrCount = 0,
        },
        .i2c =
        {
            .valid = FALSE,
        },
        .uart =
        {
            .valid = FALSE,
        },
    },
#ifdef BARDEEN
    // AP
    {
        .DeviceID = IPC_DEVICE_AP,
        .valid = TRUE,
        .tx =
        {
            .DeviceID = IPC_DEVICE_AP,
            .DmaTxRequest =
            {
                .selectedChannel = USART1_TX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 0,
            },
            .RegisterBase = AP_UART,
        },
        .rx =
        {
            .DeviceID = IPC_DEVICE_AP,
            .RegisterBase = AP_UART,
            ._bServiceStarted = FALSE,
            .packet = NULL,
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
            ._RXMsgCount = 0,
            ._RXErrCount = 0,
            ._RXDmaErrCount = 0,
            ._RTWOn = FALSE,
            .DmaRxRequest =
            {
                .selectedChannel = USART1_RX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 1,
            },
        },
        .uart =
        {
            .valid = TRUE,
            .DeviceID = IPC_DEVICE_AP,
            .BUART = AP_BUART,
            .buartInit =
            {
                BR_115200, // 115200
                WL_8BITS,  // 8 bit
                SB_1,      // 1 stop bit
                P_NONE,    // No parity
                M_RX_TX,   // RX and TX
                FC_NONE    // No Flow Control
            },
            .RxBank = AP_RX_GPIO_BANK,
            .RxPin = AP_RX_PIN,
            .RxPadConfig = GPIO_MODE_AF_IN_PULLDOWN,
            .TxBank = AP_TX_GPIO_BANK,
            .TxPin = AP_TX_PIN,
            .TxPadConfig = GPIO_MODE_AF_OUT_PP,
            .AF = AP_GPIO_AF,
        },
        .i2c =
        {
            .valid = FALSE,
        },
    },
#else
    // AP
    {
        .DeviceID = IPC_DEVICE_AP,
        .valid = FALSE,
        .i2c =
        {
            .valid = FALSE,
        },
        .uart =
        {
            .valid = FALSE,
        },
    },
#endif
    // REMOTE
    {
        .DeviceID = IPC_DEVICE_REMOTE,
        .valid = FALSE,
        .i2c =
        {
            .valid = FALSE,
        },
        .uart =
        {
            .valid = FALSE,
        },
    },

    // F0
    {

        .DeviceID = IPC_DEVICE_F0,
        .valid = TRUE,
        .tx =
        {
            .DeviceID = IPC_DEVICE_F0,
        },
        .rx =
        {
            .DeviceID = IPC_DEVICE_F0,
            ._bServiceStarted = FALSE,
            .packet = NULL,
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
            ._RXMsgCount = 0,
            ._RXErrCount = 0,
            ._RXDmaErrCount = 0,
            ._RTWOn = FALSE,
        },
        .i2c =
        {
            .valid = TRUE,
            .config =
            {
                .busNumber = F0_I2C_BUS,
                .masterClockFrequency = F0_I2C_SPEED,
                .disableDMA = 1,
            },
            .slaveAddress = 0xA4,
            .slaveRegWrite = 0x01,
            .slaveRegRead = 0x02,
            .recoverOnError = TRUE,
        },
        .uart =
        {
            .valid = FALSE,
        },
    },

    // SOUNDTOUCH
    {
        .DeviceID = IPC_DEVICE_SOUNDTOUCH,
        .valid = TRUE,
        .tx =
        {
            .DeviceID = IPC_DEVICE_SOUNDTOUCH,
            .DmaTxRequest =
            {
                .selectedChannel = SOUNDTOUCH_DMA_TX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 0,
            },
            .RegisterBase = SOUNDTOUCH_UART,
        },
        .rx =
        {
            .DeviceID = IPC_DEVICE_SOUNDTOUCH,
            .DmaRxRequest =
            {
                .selectedChannel = SOUNDTOUCH_DMA_RX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 1,
            },
            .RegisterBase = SOUNDTOUCH_UART,
            ._bServiceStarted = FALSE,
            .packet = NULL,
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
            ._RXMsgCount = 0,
            ._RXErrCount = 0,
            ._RXDmaErrCount = 0,
            ._RTWOn = FALSE,
        },
        .uart =
        {
            .valid = TRUE,
            .BUART = SOUNDTOUCH_BUART,
            .buartInit =
            {
                BR_921600, // 921600
                WL_8BITS,  // 8 bit
                SB_1,      // 1 stop bit
                P_NONE,    // No parity
                M_RX_TX,   // RX and TX
                FC_NONE    // No Flow Control
            },
            .RxBank = SOUNDTOUCH_RX_GPIO_BANK,
            .RxPin = SOUNDTOUCH_RX_PIN,
            .RxPadConfig = GPIO_MODE_AF_IN_FLOATING,
            .TxBank = SOUNDTOUCH_TX_GPIO_BANK,
            .TxPin = SOUNDTOUCH_TX_PIN,
            .TxPadConfig = GPIO_MODE_AF_OUT_PP,
            .AF = SOUNDTOUCH_GPIO_AF,
        },
        .i2c =
        {
            .valid = FALSE,
        },
    },
};
#endif
