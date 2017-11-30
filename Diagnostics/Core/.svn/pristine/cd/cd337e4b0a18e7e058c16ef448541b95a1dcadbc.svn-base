#include "project.h"
#include "TaskDefs.h"
#include "IpcI2CRxTxTask.h"
#include "i2cMaster_API.h"
#include "IpcTx.h"
#include "IpcRx.h"
#include "IPCRouterTask.h"
#include "IpcInterface.h"

#define NUM_I2C_TRIES 4
#define I2C_RETRY_INTERVAL 500
#define REBOOT_SYSTEM_ON_ERROR 1

/* Logs */
SCRIBE_DECL(sys_events);
SCRIBE_DECL(ipc_tx);
SCRIBE_DECL(ipc_rx);

/* Globals */

/* Local Functions */
static void IpcI2CRxTxTask_Send(GENERIC_MSG_t* msg);
static void IpcI2CRxTxTask_Receive(GENERIC_MSG_t* msg);
static void IpcI2CRxTxTask_Recover(void);

/* Locals */
static IpcInterface_t IpcInterface =
{
    .DeviceID = IPC_DEVICE_F0,
    .type = IPC_INTERFACE_I2C,
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
    .stat = {0},
    .test = {0},
};

void IpcI2CRxTxTask_Init(void* p)
{
    IpcInterface.i2c.handle = i2cMaster_Init(&IpcInterface.i2c.config, &IpcInterface.i2c.error);
    debug_assert(I2C_API_NO_ERROR == IpcInterface.i2c.error);
    debug_assert(NULL != IpcInterface.i2c.handle);

    IPCRouter_RegisterDevice(&IpcInterface, GetManagedTaskPointer(IpcTaskNames[IpcInterface.DeviceID]));

    LOG(ipc_tx, ROTTEN_LOGLEVEL_VERBOSE, "IPC Tx service started: %s", IpcTaskNames[IpcInterface.DeviceID]);
    LOG(ipc_rx, ROTTEN_LOGLEVEL_VERBOSE, "IPC Rx service started: %s", IpcTaskNames[IpcInterface.DeviceID]);
}

IpcInterface_t* IpcI2CRxTxTask_GetIpcInterface(void)
{
    return &IpcInterface;
}

void IpcI2CRxTxTask(void* pvParameters)
{
    IpcI2CTask_ConfigureEXTI();
    IpcI2CRxTxPostMsg(IPC_I2C_MESSAGE_ID_Receive, IPC_DEVICE_F0, NOP_CALLBACK);
    for (;;)
    {
        TaskManagerPollQueue(ManagedIpcTxTasks[IpcInterface.DeviceID]);
    }
}

void IpcI2CRxTxTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case IPC_TX_MESSAGE_ID_Send:
            IpcI2CRxTxTask_Send(msg);
            break;
        case IPC_I2C_MESSAGE_ID_Receive:
            IpcI2CRxTxTask_Receive(msg);
            break;
        default:
            LOG(sys_events, ROTTEN_LOGLEVEL_NORMAL, "\tIpcI2CTask\t%s was invalid message", GetEventString(msg->msgID));
            break;
    }
}

void IpcI2CRxTxTask_Send(GENERIC_MSG_t* msg)
{
    IpcPacket_t* packet = (IpcPacket_t*) msg->params[0];
    uint16_t dest = msg->params[1];
    uint16_t size = msg->params[2];
    int tries;

    for (tries = 0; tries < NUM_I2C_TRIES; tries++)
    {
        uint16_t len = i2cMaster_WriteRegister(IpcInterface.i2c.handle, IpcInterface.i2c.slaveAddress,
                                               &IpcInterface.i2c.slaveRegWrite, 1, packet->b, size);
        if (len == sizeof(IpcPacket_t))
        {
            LOG(ipc_tx, ROTTEN_LOGLEVEL_VERBOSE, "IPC sent: %s", IpcTaskNames[dest]);
            IpcLogPacket((IpcPacket_t*) packet);
            break;
        }
        else
        {
            LOG(ipc_tx, ROTTEN_LOGLEVEL_NORMAL, "IPC error: only wrote %d instead of %d: %s", len, size, IpcTaskNames[dest]);
            vTaskDelay(I2C_RETRY_INTERVAL);
        }
    }

    if ((tries >= NUM_I2C_TRIES) && IpcInterface.i2c.recoverOnError)
    {
        IpcI2CRxTxTask_Recover();
    }
}

void IpcI2CRxTxTask_Receive(GENERIC_MSG_t* msg)
{
    IpcInterface.i2c.packet = IPCRouter_GetNextRxBuffer();
    int tries;

    for (tries = 0; tries < NUM_I2C_TRIES; tries++)
    {
        i2cMaster_WriteNoRegister(IpcInterface.i2c.handle, IpcInterface.i2c.slaveAddress, (uint8_t[])
        {
            IpcInterface.i2c.slaveRegRead
        }, 1);

        vTaskDelay(TIMER_MSEC_TO_TICKS(1));

        memset(IpcInterface.i2c.packet->b, 0x00, sizeof(IpcPacket_t));
        uint16_t len = i2cMaster_ReadNoRegister(IpcInterface.i2c.handle, IpcInterface.i2c.slaveAddress, IpcInterface.i2c.packet->b, sizeof(IpcPacket_t));

        if (len == sizeof(IpcPacket_t))
        {
            IPCRouterPostMsg(IPCROUTER_MESSAGE_ID_RxMessage, (uint32_t) &IpcInterface, (uint32_t) IpcInterface.i2c.packet);
            break;
        }
        else
        {
            LOG(ipc_tx, ROTTEN_LOGLEVEL_NORMAL, "IPC error: only read %d instead of %d: %s", len, sizeof(IpcPacket_t), IpcTaskNames[IpcInterface.DeviceID]);
            vTaskDelay(I2C_RETRY_INTERVAL);
        }
    }

    if (tries >= NUM_I2C_TRIES)
    {
        IPCRouter_ReturnRxBuffer(IpcInterface.i2c.packet);
        if (IpcInterface.i2c.recoverOnError)
        {
            IpcI2CRxTxTask_Recover();
        }
    }
}

static void IpcI2CRxTxTask_Recover(void)
{
    static BOOL updateStarted = FALSE;

    if (!updateStarted)
    {
        LOG(ipc_tx, ROTTEN_LOGLEVEL_NORMAL, "I2C IPC comm failure!");
    }
}
