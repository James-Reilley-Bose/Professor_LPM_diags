// etapUI.c

#include "UITask.h"
#include "etapUI.h"
#include "etap.h"
#include "unifySourceList.h"
#include "nv_mfg.h"
#include "UEI_KeyIndex.h"

#if defined(BARDEEN) || defined(GINGER)
#include "nv_system.h"
#include "ConsoleSystemBehaviors.h"
#include "AccessoryManager.h"
#include "Hospitality.h"
#endif

static void ReportSpeakerType(void);
static void ReportAccessoryStatus(void);

const struct TAP_Command UI_Commands[] =
{
    {
        {"get", "get"},
        TAP_UI_Get,
        UI_GET_HELP_TEXT,
    },
    {
        {"li", "list"},
        TAP_UI_List,
        UI_LIST_HELP_TEXT,
    },
    {
        {"set", "set"},
        TAP_UI_Set,
        UI_SET_HELP_TEXT,
    },
    {
        {"fd", "default"},
        TAP_UI_Default,
        UI_DEFAULT_HELP_TEXT,
    },
    {
        {"sp", "speaker"},
        TAP_UI_SpeakerPackage,
        UI_ACCESSORY_HELP_TEXT,
    },
    {
        {"llw", "llwstate"},
        TAP_UI_LLWState,
        UI_LLWSTATE_HELP_TEXT,
    },
    {
        {"usr", "user list"},
        TAP_UI_UserList,
        UI_USERLIST_HELP_TEXT,
    },
    {
        {"er", "error"},
        TAP_UI_Error,
        UI_ERROR_HELP_TEXT,
    },
    {
        {"ik", "ignorekey"},
        TAP_UI_IgnoreKeyDuringSourceChange,
        UI_IGNOREKEYDURINGSOURCECHANGE_HELP_TEXT,
    },
    {
        {"mb", "morebuttons"},
        TAP_UI_MoreButtons,
        UI_MOREBUTTONS_HELP_TEXT,
    },
    {
        {"di", "die"},
        TAP_UI_Die,
        UI_DIE_HELP_TEXT,
    },
    {
        {"hp", "hospitality"},
        TAP_Hosp,
        TAP_HOSPITALITY_HELP
    },
    {
        {"acc", "accessory"},
        TAP_Acc,
        TAP_ACCESSORY_HELP
    },
};
const int Num_UI_Commands = sizeof(UI_Commands) / sizeof(UI_Commands[0]);

TAPCommand(TAP_Acc)
{
    IpcAccessoryList_t* listToPrint = AccessoryManager_GetAccList();
    char SnVrBuffer[WA_SERIAL_NO_LENGTH + 1] = {0};
    for (uint32_t listIndex = ACCESSORY_POSITION_START; listIndex < ACCESSORY_POSITION_NUM_OF; listIndex++)
    {
        TAP_Printf("====Accessory: %d====\n", listIndex);
        TAP_Printf("Type: %d\n", listToPrint->accessory[listIndex].type);
        TAP_Printf("Position: %d\n", listToPrint->accessory[listIndex].position);
        TAP_Printf("Status: %d\n", listToPrint->accessory[listIndex].status);
        TAP_Printf("Active: %d\n", listToPrint->accessory[listIndex].active);

        memset(SnVrBuffer, 0, WA_SERIAL_NO_LENGTH + 1);
        strncpy(SnVrBuffer, listToPrint->accessory[listIndex].sn, WA_SERIAL_NO_LENGTH);
        TAP_Printf("Serial: %s\n", SnVrBuffer);

        memset(SnVrBuffer, 0, WA_SERIAL_NO_LENGTH + 1);
        memcpy(SnVrBuffer, listToPrint->accessory[listIndex].version, 8);
        TAP_Printf("Version: %s\n", SnVrBuffer);
    }
}

TAPCommand(TAP_Hosp)
{
    if (CommandLine->numArgs > 0)
    {
        if (CommandLine->args[0][0] == 'r')
        {
            SystemBehavior_StartHospitalityMode();
            return;
        }
        else if (CommandLine->args[0][0] == 'h')
        {
            NV_SetSystemMode(SYSTEM_MODE_HOSPITALITY);
            return;
        }
        else if (CommandLine->args[0][0] == 'n')
        {
            NV_SetSystemMode(SYSTEM_MODE_NORMAL);
            return;
        }
    }

    TAP_Printf("NV System Mode = %d\n\r", Hospitality_IsEnabled());
    for (HospitalityFieldType i = HOSPITALITY_ENABLED; i < HOSPITALITY_NUM_OF_FIELDS; i++)
    {
        TAP_Printf("%s = %d\n\r", Hospitality_GetFieldName(i), Hospitality_GetField(i));
    }
}

TAPCommand(TAP_UnifySource)
{
    if (TAP_ProcessSubCommand(UI_Commands, Num_UI_Commands, CommandLine) == FALSE)
    {
        TAP_PrintString(UI_HELP_TEXT);
        TAP_PrintSubCommands(UI_Commands, Num_UI_Commands);
    }
}

TAPCommand(TAP_UI_Get)
{
    const UnifySourceStruct* currentUI = UI_GetUnifyStructForSource(UI_GetCurrentSource());
    if (CommandLine->numArgs > 0)
    {
        if (!strcmp(CommandLine->args[0], "nm"))
        {
            TAP_Printf("Number of Available Sources: %d\n", UI_GetTotalNumSources());
        }
        else if (!strcmp(CommandLine->args[0], "mxnm"))
        {
            TAP_Printf("Maximum number of Sources: %d\n", UITask_GetMaxNumOfUnifySrcs());
        }
        else
        {
            TAP_PrintString(UI_HELP_TEXT);
            TAP_PrintSubCommands(UI_Commands, Num_UI_Commands);
        }
    }
    else
    {
        if (currentUI)
        {
            TAP_Printf("Current Unify Source: %s\n", UITask_GetUnifySrcName(currentUI->sourceID));
        }
        else
        {
            TAP_Printf("Current Unify Source: undefined\n");
        }
    }
}

TAPCommand(TAP_UI_List)
{
    for (uint8_t i = 0; i < UI_GetTotalNumSources(); i++)
    {
        TAP_Printf("Index: %d\t Name: %-22s\tSOURCE_ID:%d\r\n", i, UITask_GetUnifySrcName(UITask_GetUnifySourceStruct(i)->sourceID), UITask_GetUnifySourceStruct(i)->sourceID);
    }
}

TAPCommand(TAP_UI_Error)
{
    TAP_Printf("Current error state = %d\n", UI_GetErrorState());

}

TAPCommand(TAP_UI_Set)
{
    BOOL valid = FALSE;
    uint32_t index = TAP_DecimalArgToInt(CommandLine, 0, &valid);
    if (valid)
    {
        if (UI_SetUnifySource(index))
        {
            TAP_Printf("Changing to Unify Source: %s\n", UITask_GetUnifySrcName(UITask_GetUnifySourceStruct(index)->sourceID));
        }
        else
        {
            TAP_Printf("Invalid Index");
        }
    }
    else
    {
        TAP_Printf(UI_SET_HELP_TEXT);
    }
}


TAPCommand(TAP_UI_Default)
{
    NV_SetDefaultSourceList();
    TAP_Printf("Unify Source list set to default\n");
}

TAPCommand(TAP_UI_SpeakerPackage)
{
    BOOL valid = FALSE;
    uint8_t sp_type = TAP_DecimalArgToInt(CommandLine, 0, &valid);
    if ( (valid) && (CommandLine->numArgs == 1))
    {
        IpcAccessoryList_t override = {0};

        override.accessory[ACC_POS_SUB].active = ACCESSORY_DEACTIVATED;
        override.accessory[ACC_POS_REAR_1].active = ACCESSORY_DEACTIVATED;
        override.accessory[ACC_POS_REAR_2].active = ACCESSORY_DEACTIVATED;

        switch (sp_type)
        {
            // Report actual accessory
            case 0:
                ReportSpeakerType();
                ReportAccessoryStatus();
                break;

            // Simulate Ginger + Skipper
            case 1:
            case 5:
                override.accessory[ACC_POS_SUB].active = ACCESSORY_ACTIVATED;
                TAP_PrintString("TAP force Accessory = Bass\n");
                break;

            // Simulate Ginger + Maxwell
            case 2:
                override.accessory[ACC_POS_REAR_1].active = ACCESSORY_ACTIVATED;
                override.accessory[ACC_POS_REAR_2].active = ACCESSORY_ACTIVATED;
                TAP_PrintString("TAP force Accessory = Surround\n");
                break;

            // Simulate Ginger + Skipper + Maxwell
            case 3:
            case 6:
            case 99:
                override.accessory[ACC_POS_SUB].active = ACCESSORY_ACTIVATED;
                override.accessory[ACC_POS_REAR_1].active = ACCESSORY_ACTIVATED;
                override.accessory[ACC_POS_REAR_2].active = ACCESSORY_ACTIVATED;
                TAP_PrintString("TAP force Accessory = Bass and Surround\n");
                break;

            // Simulate only Ginger
            case 4:
                TAP_PrintString("TAP force Accessory = No external speaker\n");
                break;

            default:
                TAP_PrintString("Invalid selection.");
                return; // Return so we don't set anything
                break;
        }
        AccessoryManager_SetSpeakerActiveInformation(&override);
    }
    else
    {
        TAP_PrintString(UI_ACCESSORY_HELP_TEXT);
    }
}

TAPCommand(TAP_UI_LLWState)
{
    TAP_PrintString("LLW State: ");
    switch (WirelessAudioTask_GetState())
    {
        case WA_STATE_OFF:
            TAP_PrintString("WA_STATE_OFF");
            break;
        case WA_STATE_ON:
            TAP_PrintString("WA_STATE_ON");
            break;
        case WA_STATE_PAIRING:
            TAP_PrintString("WA_STATE_PAIRING");
            break;
        case WA_STATE_ERROR:
            TAP_PrintString("WA_STATE_ERROR");
            break;
        case WA_STATE_UPDATE_SPEAKERS:
            TAP_PrintString("WA_STATE_UPDATE_SPEAKERS");
            break;
        default:
            TAP_PrintString("unknown");
            break;
    }
    TAP_PrintString("\n");
}

/*
===============================================================================
@fn ReportSpeakerType
@brief Prints out speaker package detected from network
===============================================================================
*/
static void ReportSpeakerType(void)
{

    SPEAKER_PACKAGE_VARIANT sp_type = AccessoryManager_GetCurrentSpeakerVariant();
    {
        switch (sp_type)
        {
            case SPEAKER_PACKAGE_OMNIVO:
                TAP_PrintString("Actual Speaker Package = Bardeen Omnivo\n");
                break;
            case SPEAKER_PACKAGE_JEWELCUBES:
                TAP_PrintString("Actual Speaker Package = Bardeen Jewelcubes\n");
                break;
            case SPEAKER_PACKAGE_GINGER:
                TAP_PrintString("Actual Speaker Package = Ginger\n");
                break;
            case SPEAKER_PACKAGE_GINGER_W_BASS:
                TAP_PrintString("Actual Speaker Package = Ginger Skipper\n");
                break;
            case SPEAKER_PACKAGE_GINGER_W_SS:
                TAP_PrintString("Actual Speaker Package = Ginger Maxwell\n");
                break;
            case SPEAKER_PACKAGE_GINGER_W_BASS_SS:
                TAP_PrintString("Actual Speaker Package = Ginger Skipper + Maxwell\n");
                break;
            default:
                TAP_Printf("Invalid speaker package detected. %x\n", sp_type);
                break;
        }
    }
}

/*
===============================================================================
@fn ReportAccessoryStatus
@brief Prints out accessory status detected from network and stored in nv
===============================================================================
*/
static void ReportAccessoryStatus(void)
{
    IpcAccessoryFlags_t acc = AccessoryManager_GetCurrentAccessoryFlags();

    TAP_Printf("Accessory bit configuration\r\n");
    TAP_Printf("Bass -- %s\r\n", (IPC_BASSBOX_WIRED == (IPC_BASSBOX_WIRED & acc)) ?
               "Wired" : "Wireless or not present");
    TAP_Printf("Rear -- %s\r\n", (IPC_REAR_SPEAKERS_WIRED == (IPC_REAR_SPEAKERS_WIRED & acc)) ?
               "Wired or not present" : "Wireless");
}

TAPCommand(TAP_UI_UserList)
{
    for (uint8_t i = 0; i < UI_GetTotalNumSources(); i++)
    {
        if (UITask_GetUnifySourceStruct(i)->displayInList)
        {
            TAP_Printf("Index: %d\t Name: %s\tSOURCE_ID:%d UEI: %s\r\n",
                       i, UITask_GetUnifySrcName(UITask_GetUnifySourceStruct(i)->sourceID),
                       UITask_GetUnifySourceStruct(i)->sourceID, UITask_GetUnifySourceStruct(i)->ueiKeyRecordID);
        }
    }
}

TAPCommand(TAP_UI_IgnoreKeyDuringSourceChange)
{
    if (CommandLine->numArgs == 1)
    {
        if (strcmp("on", CommandLine->args[0]) == 0)
        {
            NV_SetIgnoreKeysDuringSourceChange(TRUE);
        }
        else if (strcmp("off", CommandLine->args[0]) == 0)
        {
            NV_SetIgnoreKeysDuringSourceChange(FALSE);
        }
        else
        {
            TAP_PrintString(UI_ACCESSORY_HELP_TEXT);
            return;
        }
    }
    TAP_Printf("IgnoreKeyDuringSourceChange: %s\n", (NV_GetIgnoreKeysDuringSourceChange()) ? "on" : "off");
}

TAPCommand(TAP_UI_MoreButtons)
{
    const UnifySourceStruct* currentUI = UI_GetUnifyStructForSource(UI_GetCurrentSource());

    if (currentUI)
    {
        TAP_Printf("moreButtonsBitMap: 0x%x  moreButtonsBitMap2: 0x%x\n", currentUI->moreButtonsBitMap, currentUI->moreButtonsBitMap2);
        if (currentUI->moreButtonsBitMap & LIVE_SOFTKEY_BIT)
        {
            TAP_Printf("\tLIVE\n");
        }
        if (currentUI->moreButtonsBitMap & FAVORITE_SOFTKEY_BIT)
        {
            TAP_Printf("\tFAVORITE\n");
        }
        if (currentUI->moreButtonsBitMap & FORMAT_SOFTKEY_BIT)
        {
            TAP_Printf("\tFORMAT\n");
        }
        if (currentUI->moreButtonsBitMap & SAP_SOFTKEY_BIT)
        {
            TAP_Printf("\tSAP\n");
        }
        if (currentUI->moreButtonsBitMap & PIP_SOFTKEY_BIT)
        {
            TAP_Printf("\tPIP\n");
        }
        if (currentUI->moreButtonsBitMap & PIPOFF_SOFTKEY_BIT)
        {
            TAP_Printf("\tPIPOFF\n");
        }
        if (currentUI->moreButtonsBitMap & PANDORA_SOFTKEY_BIT)
        {
            TAP_Printf("\tPANDORA\n");
        }
        if (currentUI->moreButtonsBitMap & YOUTUBE_SOFTKEY_BIT)
        {
            TAP_Printf("\tYOUTUBE\n");
        }
        if (currentUI->moreButtonsBitMap & ROKU_SOFTKEY_BIT)
        {
            TAP_Printf("\tROKU\n");
        }
        if (currentUI->moreButtonsBitMap & YAHOO_SOFTKEY_BIT)
        {
            TAP_Printf("\tYAHOO\n");
        }
        if (currentUI->moreButtonsBitMap & SPOTIFY_SOFTKEY_BIT)
        {
            TAP_Printf("\tSPOTIFY\n");
        }
        if (currentUI->moreButtonsBitMap & WEBINTERNET_SOFTKEY_BIT)
        {
            TAP_Printf("\tWEBINTERNET\n");
        }
        if (currentUI->moreButtonsBitMap & APPSWIDGETS_SOFTKEY_BIT)
        {
            TAP_Printf("\tAPPSWIDGETS\n");
        }
        if (currentUI->moreButtonsBitMap & PICTUREMODE_SOFTKEY_BIT)
        {
            TAP_Printf("\tPICTUREMODE\n");
        }
        if (currentUI->moreButtonsBitMap & SOUNDMODE_SOFTKEY_BIT)
        {
            TAP_Printf("\tSOUNDMODE\n");
        }
        if (currentUI->moreButtonsBitMap & CLEAR_SOFTKEY_BIT)
        {
            TAP_Printf("\tCLEAR\n");
        }
        if (currentUI->moreButtonsBitMap & PWRON_SOFTKEY_BIT)
        {
            TAP_Printf("\tPWRON\n");
        }
        if (currentUI->moreButtonsBitMap & SUBTITLE_SOFTKEY_BIT)
        {
            TAP_Printf("\tSUBTITLE\n");
        }
        if (currentUI->moreButtonsBitMap & SLOW_SOFTKEY_BIT)
        {
            TAP_Printf("\tSLOW\n");
        }
        if (currentUI->moreButtonsBitMap & PWROFF_SOFTKEY_BIT)
        {
            TAP_Printf("\tPWROFF\n");
        }
        if (currentUI->moreButtonsBitMap & DELIMITER_SOFTKEY_BIT)
        {
            TAP_Printf("\tDELIMITER\n");
        }
        if (currentUI->moreButtonsBitMap & LASTCHANNEL_SOFTKEY_BIT)
        {
            TAP_Printf("\tLASTCHANNEL\n");
        }
        if (currentUI->moreButtonsBitMap & BACK_SOFTKEY_BIT)
        {
            TAP_Printf("\tBACK\n");
        }
        if (currentUI->moreButtonsBitMap & CHANNELLIST_SOFTKEY_BIT)
        {
            TAP_Printf("\tCHANNELLIST\n");
        }
        if (currentUI->moreButtonsBitMap & EJECT_SOFTKEY_BIT)
        {
            TAP_Printf("\tEJECT\n");
        }
        if (currentUI->moreButtonsBitMap & QUICKSKIP_SOFTKEY_BIT)
        {
            TAP_Printf("\tQUICKSKIP\n");
        }
        if (currentUI->moreButtonsBitMap & RECORDINGS_SOFTKEY_BIT)
        {
            TAP_Printf("\tRECORDINGS\n");
        }
        if (currentUI->moreButtonsBitMap & NETFLIX_SOFTKEY_BIT)
        {
            TAP_Printf("\tNETFLIX\n");
        }
        if (currentUI->moreButtonsBitMap & AMAZON_SOFTKEY_BIT)
        {
            TAP_Printf("\tAMAZON\n");
        }
        if (currentUI->moreButtonsBitMap & VIDEO3D_SOFTKEY_BIT)
        {
            TAP_Printf("\tVIDEO3D\n");
        }
        if (currentUI->moreButtonsBitMap & STANDBY_SOFTKEY_BIT)
        {
            TAP_Printf("\tSTANDBY\n");
        }
        if (currentUI->moreButtonsBitMap & HULU_SOFTKEY_BIT)
        {
            TAP_Printf("\tHULU\n");
        }
        if (currentUI->moreButtonsBitMap2 & (OPTIONS_SOFTKEY_BIT >> 32))
        {
            TAP_Printf("\tOPTIONS\n");
        }
        if (currentUI->moreButtonsBitMap2 & (TOOLS_SOFTKEY_BIT >> 32))
        {
            TAP_Printf("\tTOOLS\n");
        }
        if (currentUI->moreButtonsBitMap2 & (PWRTOGGLE_SOFTKEY_BIT >> 32))
        {
            TAP_Printf("\tPWRTOGGLE\n");
        }
        if (currentUI->moreButtonsBitMap2 & (POPUPMENU_SOFTKEY_BIT >> 32))
        {
            TAP_Printf("\tPOPUPMENU\n");
        }
        if (currentUI->moreButtonsBitMap2 & (TITLEMENU_SOFTKEY_BIT >> 32))
        {
            TAP_Printf("\tTITLEMENU\n");
        }
        if (currentUI->moreButtonsBitMap2 & (HELP_SOFTKEY_BIT >> 32))
        {
            TAP_Printf("\tHELP\n");
        }
    }
}

TAPCommand(TAP_UI_Die)
{
    TAP_PrintString("Entering critical section and never returning...\n");
    taskENTER_CRITICAL();
    while (1);
}
