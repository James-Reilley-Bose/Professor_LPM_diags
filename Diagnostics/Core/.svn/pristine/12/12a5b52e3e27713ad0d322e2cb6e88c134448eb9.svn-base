#include "etap.h"
#include "etapUpdate.h"
#include "UpdateManagerTask.h"
#include "UpdateManagerBlob.h"
#include "UpdateVariant.h"
#include "UpdateModuleMicro.h"
#include "BufferManager.h"

static void BlockAndPrintState(void);
static BOOL GetUpdateForced(CommandLine_t* CommandLine);

const struct TAP_Command UP_Commands[] =
{
    {
        {"all", "all"},
        TAP_UP_All,
        UP_ALL_HELP_TEXT,
    },
#ifdef LPM_HAS_WIRELESS_SPEAKERS
    {
        {"mu", "mu"},
        TAP_UP_Mu,
        UP_MU_HELP_TEXT,
    },
#endif

#ifdef LPM_HAS_RF_REMOTE
    {
        {"rem", "remote"},
        TAP_UP_Remote,
        UP_REMOTE_HELP_TEXT,
    },
#endif

#ifdef LPM_HAS_DSP
    {
        {"dsp", "dsp"},
        TAP_UP_Dsp,
        UP_DSP_HELP_TEXT,
    },
#endif

#ifdef LPM_HAS_HDMI_UPDATE
    {
        {"hdmi", "hdmi"},
        TAP_UP_Hdmi,
        UP_HDMI_HELP_TEXT,
    },
#endif
#ifdef LPM_HAS_WIRELESS_AUDIO
    {
        {"wa", "wa"},
        TAP_UP_WA,
        UP_WA_HELP_TEXT,
    },
#endif
#ifdef LPM_HAS_STANDBY_CONTROLLER
    {
        {"f0", "f0"},
        TAP_UP_F0,
        UP_F0_HELP_TEXT,
    },
#endif
    {
        {"lpm", "lpm"},
        TAP_UP_Lpm,
        UP_LPM_HELP_TEXT,
    },
    {
        {"mfg", "manufacturing"},
        TAP_UP_Mfg,
        UP_MFG_HELP_TEXT,
    },
    {
        {"bl", "bootloader"},
        TAP_UP_Bl,
        UP_BL_HELP_TEXT,
    },
    {
        {"blob", "blob"},
        TAP_UP_Blob,
        UP_BLOB_HELP_TEXT,
    },
    {
        {"state", "state"},
        TAP_UP_State,
        UP_STATE_HELP_TEXT,
    },
};
const int Num_UP_Commands = sizeof(UP_Commands) / sizeof(UP_Commands[0]);
/*
 * @func TAP_Update
 *
 * @brief Allows you to excecute various update functions
 */
TAPCommand(TAP_Update)
{
    if (UpdateManagerTask_GetState() != UPDATE_INACTIVE)
    {
        TAP_PrintString("Update already in progress.\n");
        TAP_Printf("Current state: %s\n", GetUpdateManagerStateString(UpdateManagerTask_GetState()));
    }
    else if (TAP_ProcessSubCommand(UP_Commands, Num_UP_Commands, CommandLine) == FALSE)
    {
        TAP_PrintString(UP_HELP_TEXT);
        TAP_PrintSubCommands(UP_Commands, Num_UP_Commands);
    }
}

TAPCommand(TAP_UP_All)
{
    // TODO - dj1005472 - Is this needed?
    // UI_SetUnifySource(UI_GetUnifySourceIndex(SOURCE_UPDATE));

    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_SetForceUpdate, NOP_CALLBACK, forced);
    UpdatePostMsg(UPDATE_MESSAGE_ID_SetAutoUpdateMode, NOP_CALLBACK, TRUE);
    UpdatePostMsg(UPDATE_MESSAGE_ID_SetWaitForExtPeriphMode, NOP_CALLBACK, FALSE);
    UpdatePostMsg(UPDATE_MESSAGE_ID_SetState, NOP_CALLBACK, UPDATE_BEGIN);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoEnterState, NOP_CALLBACK, NULL);
    BlockAndPrintState();
}

#if 0
TAPCommand(TAP_UP_Mu)
{
    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoSingleUpdate, NOP_CALLBACK, UPDATE_DO_SPEAKERS, forced);
    BlockAndPrintState();
}
#endif

#ifdef LPM_HAS_RF_REMOTE
TAPCommand(TAP_UP_Remote)
{
    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoSingleUpdate, NOP_CALLBACK, UPDATE_DO_BLE_RADIO, forced);
}
#endif

#ifdef LPM_HAS_DSP
TAPCommand(TAP_UP_Dsp)
{
    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoSingleUpdate, NOP_CALLBACK, UPDATE_DO_DSP, forced);
    BlockAndPrintState();
}
#endif

#ifdef LPM_HAS_HDMI_UPDATE
TAPCommand(TAP_UP_Hdmi)
{
    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoSingleUpdate, NOP_CALLBACK, UPDATE_DO_HDMI, forced);
    BlockAndPrintState();
}
#endif

#ifdef LPM_HAS_WIRELESS_AUDIO
TAPCommand(TAP_UP_WA)
{
    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoSingleUpdate, NOP_CALLBACK, UPDATE_DO_WAM, forced);
    BlockAndPrintState();
}
#endif

#ifdef LPM_HAS_STANDBY_CONTROLLER
TAPCommand(TAP_UP_F0)
{
    if (CommandLine->numArgs == 2)
    {
        UpdatePostMsg(UPDATE_MESSAGE_ID_SetUpdateFileName, NOP_CALLBACK, (uint32_t) CommandLine->args[1]);
    }

    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoSingleUpdate, NOP_CALLBACK, UPDATE_DO_F0, forced);
    BlockAndPrintState();
}
#endif

TAPCommand(TAP_UP_Lpm)
{
    BOOL forced = GetUpdateForced(CommandLine);
    UpdatePostMsg(UPDATE_MESSAGE_ID_DoSingleUpdate, NOP_CALLBACK, UPDATE_DO_MICRO, forced);
    BlockAndPrintState();
}

TAPCommand(TAP_UP_Mfg)
{
    UpdateModuleMicro_EnterDoMicroMfg();
}

TAPCommand(TAP_UP_Bl)
{
    UpdateModuleMicro_EnterDoMicroBL();
}

TAPCommand(TAP_UP_Blob)
{
    if ((CommandLine->numArgs == 1) && (!strcmp(CommandLine->args[0], "erase")))
    {
        uint32_t time = GET_SYSTEM_UPTIME_MS();
        TAP_Printf("Erasing Blob...\n");
        UpdateVariant_FlashErase(BLOB_ADDRESS, BLOB_ADDRESS + MAX_BLOB_LENGTH - 1);
        TAP_Printf("Blob erased! (%d seconds)\n", GET_MILLI_SINCE(time) / 1000);
        return;
    }
    UpdateBlobHeader upd = {0};
    if (ReadBlobHeaderFromFlash(&upd, BLOB_ADDRESS))
    {
        //Print blob header
        TAP_Printf("\n\rBlob:\n\rSize: 0x%08x \n\rMaster Version: %s \n\rNumber of Images: %u\n\r", upd.TotalLength, upd.MasterVersion, upd.NumberOfImages);

        //Print manifests
        ManifestEntry* manifestBuffer = BufferManagerGetBuffer(sizeof(ManifestEntry) * upd.NumberOfImages);
        debug_assert(manifestBuffer);
        if (GetManifest(&upd, BLOB_ADDRESS, (uint8_t*)manifestBuffer))
        {
            for (int i = 0; i < upd.NumberOfImages; i++)
            {
                TAP_Printf("\n\rManifest Entry:\n\rImage Name: %s \n\rImage Version: %s\n\rImage Offset: 0x%08x\n\r",
                           manifestBuffer[i].ImageName, manifestBuffer[i].ImageVersion, manifestBuffer[i].ImageOffset);
                if (manifestBuffer[i].Flags)
                {
                    TAP_Printf("Flags: 0x%04X\n\r", manifestBuffer[i].Flags);
                }
                if (manifestBuffer[i].HeatshrinkWindow)
                {
                    TAP_Printf("Compression: %d, %d\n\r", manifestBuffer[i].HeatshrinkWindow, manifestBuffer[i].HeatshrinkLength);
                }
            }
        }
        BufferManagerFreeBuffer(manifestBuffer);
        TAP_Printf(TAP_NEW_LINE);
    }
    else
    {
        TAP_PrintString("No valid blob found!\n\r");
    }
}

TAPCommand(TAP_UP_State)
{
    TAP_Printf("Current state: %s", GetUpdateManagerStateString(UpdateManagerTask_GetState()));
}

static void BlockAndPrintState(void)
{
    UpdateBaseState_t state = INVALID_UPDATE_STATE;
    while (1)
    {
        if (state != UpdateManagerTask_GetState())
        {
            state = UpdateManagerTask_GetState();
            TAP_Printf("Update State: %s\n\r", GetUpdateManagerStateString(state));
            vTaskDelay(TIMER_MSEC_TO_TICKS(1000)); // Sleep for a second so we don't busy wait
        }
    }
}

static BOOL GetUpdateForced(CommandLine_t* CommandLine)
{
    BOOL forced = FALSE;
    if ((CommandLine->numArgs > 0) && ('f' == CommandLine->args[0][0]))
    {
        forced = TRUE;
    }

    return forced;
}
