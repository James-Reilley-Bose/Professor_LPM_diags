/*
  File  : EDID_Transfer.c
  Title : EDID Bulk Transfer
  Author  : DR
  Created : 11/23/2016
  Language: C
  Copyright:  (C) 2016 Bose Corporation, Framingham, MA

  Description:
        API for sending the EDID to another processor over IPC
===============================================================================
*/
#include "project.h"
#include "buffermanager.h"
#include "TaskDefs.H"
#include "EDID_Transfer.h"
#include "IpcInterface.h"
#include "IPCBulk.h"
#include "edidapi.h"

static uint16_t edidTransferID = 0;
static edid_t* tvEdid;

SCRIBE_DECL(log_edid);

static void EDID_Transfer_HandleBulkTransferComplete(uint16_t transferID);

void EDID_Transfer_Start(Ipc_Device_t device)
{
    if(edidTransferID != 0)
    {
        // transfer already in progress so no
        return;
    }

    // Freed when the bulk transfer is complete.
    tvEdid = BufferManagerGetBuffer(sizeof(edid_t));
    debug_assert(tvEdid);

    tvEdid->edidData = NULL;

    if (FALSE == EDID_GetCurrentSinkEdid(tvEdid))
    {
        LOG(log_edid, ROTTEN_LOGLEVEL_NORMAL, "Could not get sink EDID, sending one block of 0s.");
        tvEdid->edidData = BufferManagerGetBuffer(EDID_BLOCK_SIZE);
        debug_assert(tvEdid->edidData != NULL);
        tvEdid->numBlocks = 1;
        memset(tvEdid->edidData, 0, EDID_BLOCK_SIZE);
    }

    edidTransferID = IPCBulk_BulkTransfer(device, tvEdid->edidData, tvEdid->numBlocks * EDID_BLOCK_SIZE,
                               BULK_TYPE_EDID, NULL, "TV EDID", &(GetManagedTaskPointer("HDMITask")->Queue));

    if (0 == edidTransferID)
    {
        EDID_Destroy(tvEdid);
        BufferManagerFreeBuffer(tvEdid);
        LOG(log_edid, ROTTEN_LOGLEVEL_NORMAL, "EDID bulk transfer to %d failed to start.", device);
    }
    else
    {
        LOG(log_edid, ROTTEN_LOGLEVEL_NORMAL, "EDID bulk transfer to %d started.", device);
    }
}

void EDID_Transfer_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
            EDID_Transfer_HandleBulkTransferComplete(msg->params[0]);
            break;
    }
}

/*
 * @func DeviceModel_ASOC_HandleBulkTransferComplete
 *
 * @brief When the EDID transfer completes or fails, free the buffer.
 *
 * @param uint16_t transferID -- Make sure we're looking at the right transfer.
 * @return n/a
 */
static void EDID_Transfer_HandleBulkTransferComplete(uint16_t transferID)
{
    if (transferID == edidTransferID)
    {
        IPCBulk_DeactivateSocket(edidTransferID);
        EDID_Destroy(tvEdid);
        BufferManagerFreeBuffer(tvEdid);
        edidTransferID = 0;
    }
}
