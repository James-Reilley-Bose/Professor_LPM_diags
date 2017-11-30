/*
 File  : UpdateModuleF0.c
 Author  : Tony Palazzolo(Cardinal Peak)
 Created : December 16, 2015
 Copyright:  (C) 2015 Bose Corporation, Framingham, MA

 Description:
 Updates the F0, this must be last since it will pull
 power from most devices
 ===============================================================================
 */

#include "UpdateModuleF0.h"
#include "UpdateManagerTask.h"
#include "UpdateManagerBlob.h"
#include "IpcInterface.h"
#include "nvram.h"
#include "i2cMaster_API.h"
#include "genTimer.h"
#include "versionlib.h"
#include "UITask.h"
#include "IpcI2CRxTxTask.h"

SCRIBE_DECL(update);

extern IpcF0HealthStatusPayload_t F0HS;

/* Private functions */
static BOOL updateModuleF0_Program(uint8_t* packetBuf);
static BOOL UpdateModuleF0_HandleWatchdog(void);

/* Globals */
static UpdateState_F0_t Update_F0;
IpcInterface_t* F0_IpcInterface = NULL;

#define F0_NUM_UPDATE_FILES 1
UpdateFile_t gUpdateFiles_F0[F0_NUM_UPDATE_FILES] =
{
    { 0, 0, "f0.bin", FALSE },
};

static BOOL gIsF0Ready = FALSE; // this is the flag to indicate the F0 is ready for the next step
static uint8_t gI2CBuffer[IAP_HDR_SIZE + I2C_MAX_DATA_LENGTH]; // hdr + data
static I2C_BUS_HANDLE_TYPE* gI2cHandle; // we will borrow the original F0 handle to spoof the I2C master
static uint32_t gAppStartingFlashAddr = INVALID_STARTING_APP_ADDR; // We ask the F0 for the address, default to a bogus value
static const uint16_t gSizeDataBytes = I2C_MAX_DATA_LENGTH;

/*
 * @func updateModuleF0_HandleI2CError
 *
 * @brief A simple place to centralize error debugging/handling
 *
 * @param None
 *
 * @return Always returns False
 */
inline BOOL UpdateModuleF0_HandleI2CError()
{
    //TODO: This can become a real error handler or removed
    //asm("bkpt #0");
    return FALSE;
}

void UpdateModuleF0_RST_L_SetState( FunctionalState state )
{
    if(state == ENABLE)
    {
        ConfigureGpioPin( F0_CTRL_RST_L_GPIO_BANK, F0_CTRL_RST_L_GPIO_PIN, GPIO_MODE_IN_FLOATING, NO_ALT_FUNC );
    }
    else
    {
        DEASSERT_GPIO_PIN_FG( F0_CTRL_RST_L_GPIO_BANK, F0_CTRL_RST_L_GPIO_PIN);
        ConfigureGpioPin( F0_CTRL_RST_L_GPIO_BANK, F0_CTRL_RST_L_GPIO_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC );
    }
}

/*
 * @func UpdateModuleF0_IRQHandler
 *
 * @brief
 *
 * @param None
 *
 * @return None
 */
void UpdateModuleF0_IRQHandler(void)
{
    if (EXTI_GetITStatus(F0_I2C_INTERRUPT_LINE) != RESET)
    {
        EXTI_ClearITPendingBit( F0_I2C_INTERRUPT_LINE );
        gIsF0Ready = TRUE;
    }
}

/*
 * @func UpdateModuleF0_ClearF0ReadyFlag
 *
 * @brief Clear the flag for the next kick
 *
 * @param None
 *
 * @return None
 */
inline void UpdateModuleF0_ClearF0ReadyFlag(void)
{
    portENTER_CRITICAL();
    gIsF0Ready = FALSE;
    portEXIT_CRITICAL();
}

/*
 * @func UpdateModuleF0_WaitForKickFromF0
 *
 * @brief Waits for the F0 to kick us or we timeout waiting
 *
 * @param None
 *
 * @return None
 */
inline void UpdateModuleF0_WaitForKickFromF0(void)
{
    uint32_t TimeoutStartTimeMs = GET_SYSTEM_UPTIME_MS();

    // for now, just break free and then it fail elsewhere and then it will eventually reset the F0 again
    // TODO: Do we need a better timeout plan?
    while (!gIsF0Ready && (GET_SYSTEM_UPTIME_MS() - TimeoutStartTimeMs) < F0_WAIT_FOR_KICK_TIMEOUT_MS)
    {
    }
}


/*
 * @func UpdateModuleF0_Init
 *
 * @brief Initializes the F0 update module.
 *
 * @param F0UpdateState_t* F0 - pointer to F0 update state
 *
 * @return n/a
 */
void UpdateModuleF0_Init(void)
{
    Update_F0.State = UPDATE_F0_NOT_ACTIVE;
}

/*
 * @func UpdateModuleF0_EnterDoF0
 *
 * @brief Called when the UPDATE_DO_F0 state is entered. This
 *        initializes the F0 update state.
 *
 * @param n/a
 *
 * @return n/a
 */
void UpdateModuleF0_EnterDoF0(void)
{
    UpdateBlobHeader upd;
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Starting F0 Update");
        timerStart(UpdateManagerTask_GetWDT(), 0, &ManagedUpdateTask->Queue);
        UpdateModuleF0_Start(&upd);
    }
    else
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unable to read update blob file (F0)!");
        UpdateManagerTask_CompleteState(UPDATE_BLOB_ERROR);
    }
}

void UpdateModuleF0_ExitDoF0(void)
{
    LOG(update, ROTTEN_LOGLEVEL_NORMAL, "F0 Update Complete");
    timerStop(UpdateManagerTask_GetWDT(), 0);
}

/*
 * @func UpdateModuleF0_Reset(void)
 *
 * @brief Performs a reset of the F0
 *
 * @param None
 *
 * @return None
 */
void UpdateModuleF0_Reset(void)
{
    LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "Resetting F0...");
    portENTER_CRITICAL();
    UpdateModuleF0_ClearF0ReadyFlag();
    UpdateModuleF0_RST_L_SetState(DISABLE); // power off the F0, this will be fun.
    // when we bring the F0 back up, we only have a small window to send a command to it.
    Delay_us(50);
    UpdateModuleF0_RST_L_SetState(ENABLE); // Power up the F0

    //reinit i2c
    i2cMaster_SetBusNeedsInit(F0_IpcInterface->i2c.config.busNumber);
    i2cMaster_Init(&F0_IpcInterface->i2c.config, &F0_IpcInterface->i2c.error);
    debug_assert(I2C_API_NO_ERROR == F0_IpcInterface->i2c.error);
    debug_assert(F0_IpcInterface->i2c.handle);

    portEXIT_CRITICAL();
}

/*
 * @func UpdateModuleF0_GetAppStartAddr
 *
 * @brief   Request the starting address for the App
 *
 * @param uint32_t* address - will be loaded with the received address
 *
 * @return  True if we were successful
 */
BOOL UpdateModuleF0_GetAppStartAddr(uint32_t* address) //TODO: turn this into get start app address
{
    uint8_t buf[IAP_HDR_SIZE] = { OPC_APP_ADDR };
    uint16_t count;

    UpdateModuleF0_WaitForKickFromF0();
    UpdateModuleF0_ClearF0ReadyFlag();

    // TODO: to make this more dynamic, ask the F0 what is the min size flash page.
    count = i2cMaster_WriteNoRegister(*gI2cHandle, F0_SLAVE_ADDR, buf, IAP_HDR_SIZE);
    if (count != IAP_HDR_SIZE)
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Failed to get app address from F0... count = %d.", count);
        return UpdateModuleF0_HandleI2CError();
    }

    UpdateModuleF0_WaitForKickFromF0();
    UpdateModuleF0_ClearF0ReadyFlag();
    // now read the response
    count = i2cMaster_ReadNoRegister(*gI2cHandle, F0_SLAVE_ADDR, (uint8_t*) address, sizeof(uint32_t));
    if (count != sizeof(uint32_t))
    {
        //TODO: what do we do?
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Failed to get app address from F0... count = %d", count);
        return UpdateModuleF0_HandleI2CError();
    }
    LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "F0: count = %d \n We received and address of %x", count, buf[0]);
    return TRUE;
}

/*
 * @func UpdateModuleF0_Start
 *
 * @brief This function is the start of the F0 update. It hijacks the
 *        F0 interrupt to help synchronize when it is ready for the next I2C command.
 *        We will loop the whole process over and over until we update successfully. Something else needs to
 *        Tell us to give up. Once we successfully update the F0, we will eventually reboot.
 *
 * @param UpdateBlobHeader* blob
 *
 * @return n/a
 */
void UpdateModuleF0_Start(UpdateBlobHeader* blob)
{
    debug_assert(gSizeDataBytes % sizeof(uint32_t) == 0); //must be a multiple of size of uint32_t

    BOOL bUpdateDone = FALSE;
    Update_F0.FilesSent = 0;
    Update_F0.TransferAttempts = 0;

    F0_IpcInterface = IpcI2CRxTxTask_GetIpcInterface();
    debug_assert(F0_IpcInterface != NULL);

    gI2cHandle = F0_IpcInterface->i2c.handle; // grab the F0 handle but we will be using a different slave address.

    ManifestEntry F0Manifest;
    if (!GetUpdateFileInfoFromManifest(blob, BLOB_ADDRESS, gUpdateFiles_F0, F0_NUM_UPDATE_FILES) ||
            !GetManifestEntryByName(blob, BLOB_ADDRESS, &F0Manifest, gUpdateFiles_F0[0].filename))
    {
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Error reading F0 update files!");
        UpdateManagerTask_CompleteState(UPDATE_FILE_NOT_FOUND);
        return;
    }

    if (!UpdateState.forced &&
            (VERSION_COMPARE_SAME_VERSION == VersionCompareVersions(F0HS.swVersion, F0Manifest.ImageVersion)))
    {
        UpdateModuleF0_Reset(); // for TVs that do not support CEC, we still want the SM2 and ASOC to shutdown.
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "F0 already at version %s", F0Manifest.ImageVersion);
        //UpdateManagerTask_PublishVersionSkip(F0HS.swVersion);
        UpdateManagerTask_CompleteState(UPDATE_SKIPPED_VERSION);
        return;
    }
    
    // We want to get feedback from the F0, so we are hijacking the F0 int, which includes ints EXTI9-EXTI5
    Interrupt_Disable(F0_I2C_INTERRUPT_CHANNEL);
    Interrupt_RegisterISR(F0_I2C_INTERRUPT_CHANNEL, UpdateModuleF0_IRQHandler);
    Interrupt_Enable(F0_I2C_INTERRUPT_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);

    // This loop will try forever to update the F0.
    //TODO: decide who and how this ends. A failed update probably cripples the system, so we keep trying.
    do
    {
        Update_F0.State = UPDATE_F0_POWER_OFF;
        UpdateModuleF0_Reset();

        if (!UpdateModuleF0_GetAppStartAddr(&gAppStartingFlashAddr))
        {
            continue; // try all over again
        }
        // We got the starting address from the F0, let's go!

        Update_F0.State = UPDATE_F0_DOING_TRANSFER;
        bUpdateDone = updateModuleF0_Program(gI2CBuffer);
    }
    while (bUpdateDone == FALSE);   // loop until we succeed or until something else stops us
    // we are done, so complete and let it reboot
    Update_F0.State = UPDATE_F0_DONE;
}

/*
 * @func updateModuleF0_BuildPacket
 *
 * @brief Builds I2C packet including header and pulling data from nvram
 *
 * @param uint32_t whichPacket - Packet number we are building
 *
 * @param uint32_t lastPacket - True if this is the last packet
 *
 * @param uint8_t* packetBuf - packet buffer, holds hdr + data + checksum
 *
 * @return The number data bytes in this packet
 */
uint32_t updateModuleF0_BuildPacket(uint32_t whichPacket, BOOL lastPacket, uint8_t* packetBuf)
{
    uint32_t bytesInThisPage = 0;

    uint8_t checksum = 0;
    uint32_t index = 0;
    uint32_t flashAddr = gAppStartingFlashAddr + (whichPacket * gSizeDataBytes); //Calc where in flash we need to write this packet
    uint32_t fileAddr = gUpdateFiles_F0[0].fileAddress + (whichPacket * gSizeDataBytes); // Calc where in the blob we get the next packet data

    packetBuf[index++] = OPC_WREN;
    packetBuf[index++] = flashAddr >> 24;
    packetBuf[index++] = (flashAddr >> 16) & 0xFF;
    packetBuf[index++] = (flashAddr >> 8) & 0xFF;
    packetBuf[index++] = flashAddr & 0xFF;
    packetBuf[index++] = (gSizeDataBytes >> 8) & 0xFF;
    packetBuf[index++] = gSizeDataBytes & 0xFF;

    // if we are on the last packet, set data size to the actual data bytes
    bytesInThisPage = (lastPacket ? gUpdateFiles_F0[0].size % gSizeDataBytes : gSizeDataBytes);

    memset(&packetBuf[IAP_CHECKSUM], 0, gSizeDataBytes); // zero fill from checksum byte so it does not get added to the checksum and data section for padding

    //Copy data from nvram to our buffer
    nvram_read(fileAddr, bytesInThisPage, &packetBuf[IAP_DATA]);

    //calculate checksum
    for (uint32_t i = 0; i < (gSizeDataBytes + IAP_HDR_SIZE); i++)
    {
        checksum += packetBuf[i];
    }
    packetBuf[IAP_CHECKSUM] = ~checksum + 1; /* two's complement of checksum */

    LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "Packet contains %d  bytes from address %x to flash addr  %x", bytesInThisPage, fileAddr, flashAddr);

    return bytesInThisPage;
}

/*
 * @func updateModuleF0_Program
 *
 * @brief sequence for programming the F0
 *
 * @param uint8_t* packetBuf - packet buffer, holds hdr + data + checksum
 *
 * @return True if all packers were successfully written to the F0
 */
BOOL updateModuleF0_Program(uint8_t* packetBuf)
{
    // Calculate how many packets we need
    uint32_t lastPacketDataSizeBytes = gUpdateFiles_F0[0].size % gSizeDataBytes;
    uint32_t totalPackets = gUpdateFiles_F0[0].size / gSizeDataBytes + (lastPacketDataSizeBytes ? 1 : 0);
    uint16_t bytesSent;

    // cycle through each packet
    for (uint32_t packet = 0; packet < totalPackets; packet++)
    {

        //load packet with page
        updateModuleF0_BuildPacket(packet, packet == (totalPackets - 1), packetBuf);

        UpdateModuleF0_WaitForKickFromF0();
        UpdateModuleF0_ClearF0ReadyFlag();
        //send header
        bytesSent = i2cMaster_WriteNoRegister(*gI2cHandle, F0_SLAVE_ADDR, packetBuf, IAP_HDR_SIZE);
        if (bytesSent != IAP_HDR_SIZE)
        {
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Failed to send hdr to F0.");
            return UpdateModuleF0_HandleI2CError();
        }
        LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "sent hdr to F0.");

        UpdateModuleF0_WaitForKickFromF0();
        UpdateModuleF0_ClearF0ReadyFlag();

        //send payload plus checksum
        bytesSent = i2cMaster_WriteNoRegister(*gI2cHandle, F0_SLAVE_ADDR, &packetBuf[IAP_DATA], gSizeDataBytes);
        if (bytesSent != (gSizeDataBytes))
        {
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Failed to send data to F0.");
            return UpdateModuleF0_HandleI2CError();
        }
        LOG(update, ROTTEN_LOGLEVEL_VERBOSE, "Wrote data successfully.");

        // The first time through can take over a sec to erase flash
        // TODO: move erasing out of write command and just call the erase before doing any writes???
        UpdateModuleF0_WaitForKickFromF0();

        /* check for ACK */
        if (i2cMaster_ReadNoRegister(*gI2cHandle, F0_SLAVE_ADDR, packetBuf, 1) != 1)
        {
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Read failed from F0.");
            return UpdateModuleF0_HandleI2CError();
        }
        else if (packetBuf[0] != OPC_ACK)
        {
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Command failure. Expected ACK.");
            return UpdateModuleF0_HandleI2CError();
        }
        LOG(update, ROTTEN_LOGLEVEL_NORMAL, "F0 Packet Sent: %d / %d", packet, totalPackets);
    }

    return TRUE;
}

/*
 * @func UpdateModuleF0_HandleMessage
 *
 * @brief Handles messages while in UPDATE_DO_F0 state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
BOOL UpdateModuleF0_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = FALSE;

    switch (msg->msgID)
    {
        case UPDATE_MESSAGE_ID_IPCPacketAvailable:
            handled = FALSE;
            break;
        case UPDATE_MESSAGE_ID_WatchdogExpired:
            handled = UpdateModuleF0_HandleWatchdog();
            break;
        case UPDATE_MESSAGE_ID_SetUpdateFileName:
            gUpdateFiles_F0[0].filename = (char*) msg->params[0];
            handled = TRUE;
            break;
        case UPDATE_MESSAGE_ID_DoEnterState:
            handled = FALSE;
            break;
        default:
            LOG(update, ROTTEN_LOGLEVEL_NORMAL, "Unhandled message ID - %s (%s)", GetEventString(msg->msgID), __func__)
            break;
    }

    return handled;
}

/*
 * @func UpdateModuleF0_HandleWatchdog
 *
 * @brief Handles the watchdog while in UPDATE_DO_F0 state.
 *
 * @param GENERIC_MSG_t* msg - the message to handle
 *
 * @return TRUE if the message was handled
 */
static BOOL UpdateModuleF0_HandleWatchdog(void)
{
    if (Update_F0.State == UPDATE_F0_DONE)
    {
        UpdateManagerTask_CompleteState(UPDATE_COMPLETED);
    }
    return TRUE; // handled
}

