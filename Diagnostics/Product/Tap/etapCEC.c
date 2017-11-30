//
// etapHDMI.c - Print version info
//

#include "etap.h"
#include "CECTask.h"
#include "CEC_ControlInterface.h"
#include "etapCEC.h"
#include <stdlib.h>

SCRIBE_DECL(cec);

const struct TAP_Command CEC_Commands[] =
{
    {
        {"initarc", "initiatearc"},
        TAP_InitiateArc,
        InitiateArc_HELP_TEXT
    },

    {
        {"addr", "spoofhdmiaddr"},
        TAP_SPOOF_PHYSICAL_ADDR_Command,
        TAP_SPOOF_CEC_ADDR_HELP_TEXT
    },

    {
        {"sysa", "systemaudio"},
        TAP_CEC_SYS_AUDIO_Command,
        TAP_CEC_SYS_AUDIO_HELP_TEXT
    },
};
const int Num_CEC_Commands = sizeof(CEC_Commands) / sizeof(CEC_Commands[0]);

TAPCommand(CEC_Command)
{
    if (TAP_ProcessSubCommand(CEC_Commands, Num_CEC_Commands, CommandLine) == FALSE)
    {
        TAP_PrintString(CEC_HELP_TEXT);
        TAP_PrintSubCommands(CEC_Commands, Num_CEC_Commands);
    }
}

TAPCommand(TAP_InitiateArc)
{
    LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "Attempting to initiate ARC");
    CECPostMsg(CEC_MESSAGE_ID_InitiateARC, NOP_CALLBACK, 0);
}

TAPCommand(TAP_CEC_SYS_AUDIO_Command)
{
    if (1 != CommandLine->numArgs)
    {
        TAP_PrintString(TAP_CEC_SYS_AUDIO_HELP_TEXT);
        return;
    }
    LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "Sending SystemAudio request");
    if (strncmp(CommandLine->args[0], "on", 2) == 0)
    {
        CECPostMsg(CEC_MESSAGE_ID_SendSystemAudioRequest, NOP_CALLBACK, TRUE);
    }
    else if (strncmp(CommandLine->args[0], "off", 3) == 0)
    {
        CECPostMsg(CEC_MESSAGE_ID_SendSystemAudioRequest, NOP_CALLBACK, FALSE);
    }
    else if (strncmp(CommandLine->args[0], "enable", 6) == 0)
    {
        CECPostMsg(CEC_MESSAGE_ID_EnableSystemAudio, NOP_CALLBACK, TRUE);
    }
    else if (strncmp(CommandLine->args[0], "disable", 7) == 0)
    {
        CECPostMsg(CEC_MESSAGE_ID_SendSystemAudioRequest, NOP_CALLBACK, FALSE);
        CECPostMsg(CEC_MESSAGE_ID_EnableSystemAudio, NOP_CALLBACK, FALSE);
    }
    else
    {
        if (TRUE == CEC_GetSystemAudioPermitted())
        {
            TAP_PrintString("System Audio is ENABLED\n");
            if (TRUE == CEC_SystemAudioModeStatus())
            {
                TAP_PrintString("System Audio is ON\n");
            }
            else
            {
                TAP_PrintString("System Audio is OFF\n");
            }
        }
        else
        {
            TAP_PrintString("System Audio is DISABLED\n");
        }
    }
}

TAPCommand(TAP_SPOOF_PHYSICAL_ADDR_Command)
{
    LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "Spoofing HDMI physical address to 0x3000");
    CECPostMsg(CEC_MESSAGE_ID_NewPhysAddrFromEDID, NOP_CALLBACK, 0);
}