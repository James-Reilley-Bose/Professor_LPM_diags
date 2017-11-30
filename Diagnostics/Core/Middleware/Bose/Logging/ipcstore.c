/*
 * IpcStore implements Rotten_Store as a
 * connection over IPC.
 * Brought to you from the keyboard of Derek "The Darr" Richardson
 */
#include <stdio.h>
#include "ipcstore.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "IPCRouterTask.h"

#define IPC_LOG_MIN_BUFFERS 10

/*
 * Procedure prototypes
 */
static void ipc_store_write(IpcStore *this, Rotten_Record *r);
static void ipc_store_dump(IpcStore *from, Rotten_Store *to);
static void ipc_store_flush(IpcStore *this);
static char ipc_scratch[IPC_NUM_DATA_BYTES];
static uint32_t logDropCount = 0;

/*
 * Public interface
 */
void ipc_store_init(IpcStore *this)
{
    this->base.write = (void(*)(Rotten_Store*,Rotten_Record*))ipc_store_write;
    this->base.dump = (void(*)(Rotten_Store*,Rotten_Store*))ipc_store_dump;
    this->base.flush = (void(*)(Rotten_Store*))ipc_store_flush;
}

/*
 * Implementation
 */
static void ipc_store_write(IpcStore *this, Rotten_Record *r)
{
    
    if(IPCRouter_BuffersTxRemaining() > IPC_LOG_MIN_BUFFERS)
    {
        Rotten_Logger *l = rotten();
        snprintf(ipc_scratch, IPC_NUM_DATA_BYTES, r->msg->text, r->params[0], r->params[1], r->params[2], r->params[3], r->params[4], r->params[5], r->params[6], r->params[7], r->params[8], r->params[9], r->params[10], r->params[11]);
        ipc_scratch[IPC_NUM_DATA_BYTES - 1] = '\0';
        IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_LOG_EVT, NULL, &ipc_scratch, IPC_NUM_DATA_BYTES);
        memset(ipc_scratch, 0x00, IPC_NUM_DATA_BYTES);
        if(logDropCount > 0)
        {
            snprintf(ipc_scratch, IPC_NUM_DATA_BYTES, "Dropped %d logs - out of buffers", logDropCount);
            logDropCount = 0;
            IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_LOG_EVT, NULL, &ipc_scratch, IPC_NUM_DATA_BYTES);
            memset(ipc_scratch, 0x00, IPC_NUM_DATA_BYTES);
        }
    }
    else
    {
        logDropCount++;
    }
}

static void ipc_store_dump(IpcStore *from, Rotten_Store *to)
{
    //No-op.  What did you *think* could happen here?
}

static void ipc_store_flush(IpcStore *this)
{
    //NOP
}