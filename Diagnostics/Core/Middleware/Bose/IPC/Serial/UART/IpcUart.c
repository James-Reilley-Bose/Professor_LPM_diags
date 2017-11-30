//
// IpcUart.c
//

#include "IpcUart.h"
#include "IpcRx.h"
#include "IpcTxTask.h"

SCRIBE_DECL(ipc_router);

void Ipc_InitializeUart(IpcInterface_t* device)
{
    debug_assert(device && (device->type == IPC_INTERFACE_UART));
    //set up rxconfig
    IpcRxConfig_t* rxConfig = &device->uart.rx;

    DMA_MANAGER_ERROR_CODE pError;
    rxConfig->DmaRxHandle = dmaManager_ConfigureChannel(&rxConfig->DmaRxRequest, &pError);
    debug_assert(pError == DMA_MANAGER_NO_ERROR);
    IPC_RX_StartService(device);

    /* Configure the GPIOs... */
    ConfigureGpioPin(device->uart.RxBank, device->uart.RxPin, device->uart.RxPadConfig, device->uart.AF);
    ConfigureGpioPin(device->uart.TxBank, device->uart.TxPin, device->uart.TxPadConfig, device->uart.AF);

    /* Configure the UART peripheral */
    BUSART_DeInit(device->uart.BUART);
    BUSART_Init(device->uart.BUART, &device->uart.buartInit);
    BUSART_ITConfig(device->uart.BUART, IRQ_RXNE, PS_ENABLE);
    BUSART_ITConfig(device->uart.BUART, IRQ_TXE, PS_DISABLE);

    IpcTxTask_Init(&device->uart.tx);

    LOG(ipc_router, ROTTEN_LOGLEVEL_VERBOSE, "IPC UART configured: %s", IpcTaskNames[device->DeviceID]);
}
