#ifndef _IPC_TX_H_
#define _IPC_TX_H_

#include "DmaManager.h"
#include "event_groups.h"
#include "IpcProtocolLpm.h"

/* Wait Times */
#define IPC_WAIT_FOR_MUTEX_MS               250
#define IPC_WAIT_FOR_DMA_MS                 50
#define IPC_WAIT_TRANSACTION_COMPLETE_MS    500

/* Definitions for bits used in Event_Group */
#define ebDMA_TRANSFER_COMPLETE_BIT ( 0x01UL )  //  BIT_0
#define ebDMA_TRANSFER_ERROR_BIT    ( 0x02UL )  //  BIT_1

/* Types */
typedef struct
{
    uint8_t DeviceID;
    DMA_MANAGER_REQUEST_TYPE DmaTxRequest;
    DMA_REQUEST_HANDLE_TYPE DmaTxHandle;
    DMA_MANAGER_ERROR_CODE DmaTxError;
    USART_TypeDef* RegisterBase;
    EventGroupHandle_t TxDmaEventGroupHandle;
} IpcTxConfig_t;

/* Functions */
void SendIpc(uint8_t dest, uint8_t* buffer, uint8_t length);
void CancelIpc(Ipc_Device_t dest);

#endif // _IPC_TX_H_
