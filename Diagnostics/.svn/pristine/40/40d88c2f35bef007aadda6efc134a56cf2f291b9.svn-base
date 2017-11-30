//
// etapHDMI.c - Print version info
//

#include "etap.h"
#include "etapHDMI.h"
#include "Sii9437Task.h"
#include "sii_lib_data.h"
#include <stdlib.h>

SCRIBE_DECL(sii9437);

const struct TAP_Command HDMI_Commands[] =
{
    {
        {"earcarc", "earcarcmode"},
        TAP_EarcArcMode,
        EARC_ARC_MODE_HELP_TEXT
    },

    {
        {"earchpd", "earchotplugddetected"},
        TAP_EarcHPD,
        EARC_HPD_HELP_TEXT
    },

    {
        {"earcgp", "earcgetpower"},
        TAP_EarcGetPowerState,
        EARC_GET_POWER_STATE_HELP_TEXT
    },

    {
        {"earcsp", "earcsetpower"},
        TAP_EarcSetPowerState,
        EARC_SET_POWER_STATE_HELP_TEXT
    },

    {
        {"ei", "earcinit"},
        TAP_EarcInit,
        EARC_INIT_HELP_TEXT
    },
};
const int Num_HDMI_Commands = sizeof(HDMI_Commands) / sizeof(HDMI_Commands[0]);

TAPCommand(HDMI_Command)
{
    if (TAP_ProcessSubCommand(HDMI_Commands, Num_HDMI_Commands, CommandLine) == FALSE)
    {
        TAP_PrintString(HDMI_HELP_TEXT);
        TAP_PrintSubCommands(HDMI_Commands, Num_HDMI_Commands);
    }
}

TAPCommand(TAP_EarcInit)
{
    LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Initiating eARC driver..");
    Sii9437PostMsg(SII9437_MESSAGE_ID_Init, NOP_CALLBACK, 0);
}

TAPCommand(TAP_EarcSetPowerState)
{
    uint8_t power_state = atoi(CommandLine->args[0]);
    if (power_state <= TRUE)
    {
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Setting eARC power state to %s", power_state ? "Standby" : "Full Power");
        Sii9437PostMsg(SII9437_MESSAGE_ID_SetPowerState, NOP_CALLBACK, power_state);
    }
    else
    {
        TAP_PrintString(EARC_SET_POWER_STATE_HELP_TEXT);
    }
}

TAPCommand(TAP_EarcGetPowerState)
{
    Sii9437PostMsg(SII9437_MESSAGE_ID_GetPowerState, NOP_CALLBACK, 0);
}

TAPCommand(TAP_EarcHPD)
{
    uint32_t hpd_state = atoi(CommandLine->args[0]);
    if (CommandLine->numArgs > 0)
    {
        if (hpd_state <= TRUE)
        {
            LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Setting HPD %s", hpd_state ? "Hi" : "Lo");
            Sii9437PostMsg(SII9437_MESSAGE_ID_SetHPD, NOP_CALLBACK, hpd_state);
        }
    }
    else
    {
        Sii9437PostMsg(SII9437_MESSAGE_ID_GetHPD, NOP_CALLBACK, 0);
    }
}

TAPCommand(TAP_EarcArcMode)
{
    uint8_t arc_state = atoi(CommandLine->args[0]);
    if (CommandLine->numArgs > 0)
    {
        if (arc_state <= SII_ARC_MODE__EARC)
        {
            LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Setting Sii9437 arc mode to %d", arc_state);
            Sii9437PostMsg(SII9437_MESSAGE_ID_SetArcMode, NOP_CALLBACK, arc_state);
        }
    }
    else
    {
        Sii9437PostMsg(SII9437_MESSAGE_ID_GetArcMode, NOP_CALLBACK, 0);
    }
}