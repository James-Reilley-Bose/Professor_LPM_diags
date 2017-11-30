//
// UpdateVariant.c
//

#include "UpdateVariant.h"
#include "UpdateModuleBase.h"
#include "UpdateModuleInactive.h"
#include "UpdateModuleMicro.h"
#include "UpdateModuleWaitDownload.h"
#include "UpdateModuleWaitExtPeriph.h"
#include "UpdateModuleWaitInstall.h"
#include "UpdateModuleDownloading.h"
#include "UpdateModuleAuthenticate.h"
#include "UpdateModuleF0.h"
#include "UpdateModuleDsp.h"
#include "UpdateModuleRadio.h"
#include "UpdateModuleBleRadio.h"

#include "nvram.h"

const UpdateStateHandler_t UpdateStateHandlers[] =
{
    // UPDATE_INACTIVE
    {
        .MessageHandler = UpdateModuleInactive_HandleMessage,
        .Init = UpdateModuleInactive_Init,
        .EnterState = UpdateModuleInactive_Enter,
        .ExitState = UpdateModuleInactive_Exit,
        .ignoreProgress = FALSE,
    },
    // UPDATE_WAIT_DOWNLOAD
    {
        .MessageHandler = UpdateModule_WaitDownload_HandleMessage,
        .Init = NULL,
        .EnterState = UpdateModule_WaitDownload_Enter,
        .ExitState = UpdateModule_WaitDownload_Exit,
        .ignoreProgress = FALSE,
    },
    // UPDATE_DOWNLOADING
    {
        .MessageHandler = UpdateModule_Downloading_HandleMessage,
        .Init = NULL,
        .EnterState = UpdateModule_Downloading_Enter,
        .ExitState = UpdateModule_Downloading_Exit,
        .ignoreProgress = FALSE,
    },
    // UPDATE_AUTHENTICATE
    {
        .MessageHandler = UpdateModule_Authenticate_HandleMessage,
        .Init = NULL,
        .EnterState = UpdateModule_Authenticate_Enter,
        .ExitState = UpdateModule_Authenticate_Exit,
        .ignoreProgress = FALSE,
    },
    // UPDATE_WAIT_INSTALL
    {
        .MessageHandler = UpdateModule_WaitInstall_HandleMessage,
        .Init = NULL,
        .EnterState = UpdateModule_WaitInstall_Enter,
        .ExitState = UpdateModule_WaitInstall_Exit,
        .ignoreProgress = FALSE,
    },
    // UPDATE_BEGIN
    {
        .MessageHandler = NULL,
        .Init = NULL,
        .EnterState = UpdateModuleBase_Begin_Enter,
        .ExitState = NULL,
        .ignoreProgress = FALSE,
    },
    // UPDATE_DO_DSP
    {
        .MessageHandler = UpdateModuleDsp_HandleMessage,
        .Init = UpdateModuleDsp_Init,
        .EnterState = UpdateModuleDsp_EnterDoDsp,
        .ExitState = UpdateModuleDsp_ExitDoDsp,
    },
    // UPDATE_DO_WAM
    {
        .MessageHandler = UpdateModuleWAM_HandleMessage,
        .Init = UpdateModuleWAM_Init,
        .EnterState = UpdateModuleWAM_EnterDoWAM,
        .ExitState = UpdateModuleWAM_ExitDoWAM,
    },
    // UPDATE_DO_F0
    {
        .MessageHandler = UpdateModuleF0_HandleMessage,
        .Init = UpdateModuleF0_Init,
        .EnterState = UpdateModuleF0_EnterDoF0,
        .ExitState = UpdateModuleF0_ExitDoF0,
    },
#ifdef LPM_HAS_RF_REMOTE
    // UPDATE_DO_BLE_RADIO
    {
        .MessageHandler = UpdateModuleBleRadio_HandleMessage,
        .Init = NULL,
        .EnterState = UpdateModuleBleRadio_Enter,
        .ExitState = UpdateModuleBleRadio_Exit,
        .ignoreProgress = FALSE,
    },
#endif
    // UPDATE_DO_MICRO
    {
        .MessageHandler = NULL,
        .Init = UpdateModuleMicro_Init,
        .EnterState = UpdateModuleMicro_EnterDoMicro,
        .ExitState = NULL,
        .ignoreProgress = FALSE,
    },
    {
        .MessageHandler = UpdateModule_WaitExtPeriph_HandleMessage,
        .Init = NULL,
        .EnterState = UpdateModule_WaitExtPeriph_Enter,
        .ExitState = UpdateModule_WaitExtPeriph_Exit,
        .ignoreProgress = TRUE,
    },
    // UPDATE_REBOOT
    {
        .MessageHandler = NULL,
        .Init = NULL,
        .EnterState = UpdateModuleBase_Reboot_Enter,
        .ExitState = NULL,
        .ignoreProgress = FALSE,
    },
};

// Generate a compile time error if the state table is the wrong size
extern const uint8_t UpdateStateCheck[(sizeof(UpdateStateHandlers) / sizeof(UpdateStateHandler_t) == UPDATE_NUM_STATES) ? 1 : -1];

void UpdateVariant_Init(void)
{

}

BOOL UpdateVariant_IsUpdateAllowed(void)
{
    return TRUE;
}

void UpdateVariant_FlashWrite(unsigned int address, const void* data, unsigned int length)
{
    nvram_write(address, length, data);
}

void UpdateVariant_FlashRead(unsigned int address, void* data, unsigned int length)
{
    nvram_read(address, length, data);
}

void UpdateVariant_FlashErase(unsigned int startAddress, unsigned int endAddress)
{
    nvram_erase(startAddress, endAddress);
}
