//
// etapVersion.c - Print version info
//

#include "etap.h"
#include "etapVersion.h"
#include "versionlib.h"
#include "RivieraLPM_IpcProtocol.h"
#include "DeviceModel_DSP.h"
#include "AccessoryManager.h"
#include "WiredIDConsoleTask.h"
#include "WirelessAudioTask.h"
#include "WirelessAudioAPI.h"
#include "RemoteDefs.h"
#include "RemoteDriver.h"
#include "ProductSystemParams.h"

#define VERSION_BUF_SIZE 64

extern IpcF0HealthStatusPayload_t F0HS;
__no_init char bl_version[48] @ 0x20000000;

static void PrintAccessoryVersions(void);

TAPCommand(TAP_Version)
{
    char versionBuffer[VERSION_BUF_SIZE];
    memset(versionBuffer, 0, sizeof(versionBuffer));
    NV_GetBlobVersion(versionBuffer);

    TAP_Printf("%s\n", VARIANT_NAME);
    TAP_Printf("BLOB Version: %s\n", versionBuffer);
    TAP_Printf("LPM Bootloader: %s\n", bl_version);
    TAP_Printf("LPM: %s\n", VersionGetVersionLong());
    TAP_Printf("F0 Bootloader: %s\n", F0HS.swBootloaderVersion);
    TAP_Printf("F0: %s\n", F0HS.swLongVersion);

    memset(versionBuffer, 0, sizeof(versionBuffer));
    DeviceModel_DSP_Version_Get(versionBuffer);
    TAP_Printf("DSP: %s\n", versionBuffer);

    if ((WirelessAudioTask_GetState() != WA_STATE_OFF) && !WirelessAudioUtilities_WasDarrQuickBooted())
    {
        TAP_Printf("LLW: %d\n", WirelessAudio_GetVersion());
    }

    PrintAccessoryVersions();

//    const char* ver = Remote_GetVersion();
//    TAP_Printf("BLEBridge: %s\n", ver);
}

static void PrintAccessoryVersions(void)
{
    if ((WirelessAudioTask_GetState() != WA_STATE_OFF) && !WirelessAudioUtilities_WasDarrQuickBooted())
    {
        IpcAccessoryList_t* accList = AccessoryManager_GetAccList();
        AccessoryVersion* accVersions = AccessoryManager_GetAccVersion();
        if (IS_ACCESSORY_WIRELESS(accList->accessory[ACC_POS_REAR_1]))
        {
            TAP_Printf("Maxwell 1: LLW-%d, LPM-%d.%d.%d.%d, Left\n", accVersions[ACC_POS_REAR_1].darr_ver, accVersions[ACC_POS_REAR_1].lpm_major,
                       accVersions[ACC_POS_REAR_1].lpm_minor, accVersions[ACC_POS_REAR_1].lpm_patch, accVersions[ACC_POS_REAR_1].lpm_build);
        }
        if (IS_ACCESSORY_WIRELESS(accList->accessory[ACC_POS_REAR_2]))
        {

            TAP_Printf("Maxwell 2: LLW-%d, LPM-%d.%d.%d.%d, Right\n", accVersions[ACC_POS_REAR_2].darr_ver, accVersions[ACC_POS_REAR_2].lpm_major,
                       accVersions[ACC_POS_REAR_2].lpm_minor, accVersions[ACC_POS_REAR_2].lpm_patch, accVersions[ACC_POS_REAR_2].lpm_build);
        }
        //if both wire and wireless connection, don't show version
        if (!IsWiredBassConnected() && (IS_ACCESSORY_WIRELESS(accList->accessory[ACC_POS_SUB])))
        {
            if(accList->accessory[ACC_POS_SUB].type == ACCESSORY_SKIPPER)
            {
                TAP_Printf("Skipper: LLW-%d, LPM-%d.%d.%d.%d\n", accVersions[ACC_POS_SUB].darr_ver, accVersions[ACC_POS_SUB].lpm_major,
                           accVersions[ACC_POS_SUB].lpm_minor, accVersions[ACC_POS_SUB].lpm_patch, accVersions[ACC_POS_SUB].lpm_build);
            }
            else if (accList->accessory[ACC_POS_SUB].type == ACCESSORY_LOVEY)
            {
                TAP_Printf("PBB: LLW-%d, LPM-%d.%d.%d.%d\n", accVersions[ACC_POS_SUB].darr_ver, accVersions[ACC_POS_SUB].lpm_major,
                           accVersions[ACC_POS_SUB].lpm_minor, accVersions[ACC_POS_SUB].lpm_patch, accVersions[ACC_POS_SUB].lpm_build);
            }
            else
            {
                TAP_Printf("Unknown bassbox: LLW-%d, LPM-%d.%d.%d.%d\n", accVersions[ACC_POS_SUB].darr_ver, accVersions[ACC_POS_SUB].lpm_major,
                           accVersions[ACC_POS_SUB].lpm_minor, accVersions[ACC_POS_SUB].lpm_patch, accVersions[ACC_POS_SUB].lpm_build);
            }
        }
        if (IS_ACCESSORY_WIRELESS(accList->accessory[ACC_POS_SUB_2]))
        {
            if(accList->accessory[ACC_POS_SUB_2].type == ACCESSORY_SKIPPER)
            {
                TAP_Printf("Skipper: LLW-%d, LPM-%d.%d.%d.%d\n", accVersions[ACC_POS_SUB_2].darr_ver, accVersions[ACC_POS_SUB_2].lpm_major,
                           accVersions[ACC_POS_SUB_2].lpm_minor, accVersions[ACC_POS_SUB_2].lpm_patch, accVersions[ACC_POS_SUB_2].lpm_build);
            }
            else if (accList->accessory[ACC_POS_SUB_2].type == ACCESSORY_LOVEY)
            {
                TAP_Printf("PBB: LLW-%d, LPM-%d.%d.%d.%d\n", accVersions[ACC_POS_SUB_2].darr_ver, accVersions[ACC_POS_SUB_2].lpm_major,
                           accVersions[ACC_POS_SUB_2].lpm_minor, accVersions[ACC_POS_SUB_2].lpm_patch, accVersions[ACC_POS_SUB_2].lpm_build);
            }
            else
            {
                TAP_Printf("Unknown bassbox: LLW-%d, LPM-%d.%d.%d.%d\n", accVersions[ACC_POS_SUB_2].darr_ver, accVersions[ACC_POS_SUB_2].lpm_major,
                           accVersions[ACC_POS_SUB_2].lpm_minor, accVersions[ACC_POS_SUB_2].lpm_patch, accVersions[ACC_POS_SUB_2].lpm_build);
            }
        }
    }

    if (IsWiredBassConnected())
    {
        IpcAccessoryList_t* accList = AccessoryManager_GetAccList();

        //Demo SW needs both outputs on Skipper to work with both legacy and new system
        if (accList->accessory[ACC_POS_SUB].type == ACCESSORY_SKIPPER)
        {
            TAP_Printf("Bass: Wired Bose Bass\n");
            TAP_Printf("Bass Box: Wired Skipper\n");
        }
        else if (accList->accessory[ACC_POS_SUB].type == ACCESSORY_LOVEY)
        {
            TAP_Printf("Bass Box: Wired PBB\n");
        }
        else
        {
            TAP_Printf("Bass Box: Unknown\n");
        }
    }
}