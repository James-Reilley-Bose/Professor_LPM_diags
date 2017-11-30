//
// etapPower.c
//
#include "etap.h"
#include "etapPower.h"
#include "HwMacros.h"
#include "PowerTask.h"
#include "nv_mfg.h"

/*
 * @func TAP_RebootCommand
 *
 * @brief Reboots the system
 */
TAPCommand(TAP_RebootCommand)
{
    BL_STATE bootState = BL_STATE_BOOT_USER;
    BOOL valid = TRUE;

    if (CommandLine->numArgs >= 1)
    {
        uint32_t var = TAP_DecimalArgToInt(CommandLine, 0, &valid);
        if (valid)
        {
            switch (var)
            {
                case 0: // boot user
                    bootState = BL_STATE_BOOT_USER;
                    break;
                case 1: // boot diags
                    bootState = BL_STATE_BOOT_DIAG;
                    break;
                case 2: // boot to bl cli
                    bootState = BL_STATE_CLI;
                    break;
                default:
                    bootState = BL_STATE_BOOT_USER;
                    break;
            }
            NV_SetBootState(bootState);
        }
    }

    if (valid)
    {
        system_reboot();
    }
    else
    {
        TAP_PrintString("Invalid boot state.");
    }
}

TAPCommand(TAP_Power)
{
    if (CommandLine->numArgs < 1)
    {
        TAP_Printf("%s", POWER_HELP_TEXT);
        return;
    }
    if (strcmp(CommandLine->args[0], "set") == 0)
    {
        if (CommandLine->numArgs < 2)
        {
            TAP_Printf("%s", POWER_HELP_TEXT);
            return;
        }
        if ((strcmp(CommandLine->args[1], "cb") == 0) ||
                (strcmp(CommandLine->args[1], "0") == 0)  ||
                (strncmp(CommandLine->args[1], "cold", 4) == 0))
        {
            PowerAPI_SetPowerState(POWER_STATE_COLD_BOOTED);
        }
        else if ((strcmp(CommandLine->args[1], "lp") == 0) ||
                 (strcmp(CommandLine->args[1], "1") == 0)  ||
                 (strncmp(CommandLine->args[1], "low", 3) == 0))
        {
            PowerAPI_SetPowerState(POWER_STATE_LOW_POWER);
            while (PowerAPI_GetCurrentPowerState() != POWER_STATE_LOW_POWER);
        }
        else if ((strcmp(CommandLine->args[1], "ns") == 0) ||
                 (strcmp(CommandLine->args[1], "2") == 0)  ||
                 (strncmp(CommandLine->args[1], "net", 3) == 0))
        {
            PowerAPI_SetPowerState(POWER_STATE_NETWORK_STANDBY);
            while (PowerAPI_GetCurrentPowerState() != POWER_STATE_NETWORK_STANDBY);
        }
        else if ((strcmp(CommandLine->args[1], "aw") == 0) ||
                 (strcmp(CommandLine->args[1], "3") == 0)  ||
                 (strncmp(CommandLine->args[1], "wake", 4) == 0))
        {
            PowerAPI_SetPowerState(POWER_STATE_AUTO_WAKE_STANDBY);
            while (PowerAPI_GetCurrentPowerState() != POWER_STATE_AUTO_WAKE_STANDBY);
        }
        else if ((strcmp(CommandLine->args[1], "fp") == 0) ||
                 (strcmp(CommandLine->args[1], "4") == 0)  ||
                 (strncmp(CommandLine->args[1], "full", 4) == 0))
        {
            PowerAPI_SetPowerState(POWER_STATE_FULL_POWER);
            while (PowerAPI_GetCurrentPowerState() != POWER_STATE_FULL_POWER);
        }
        else
        {
            TAP_Printf("Unknown PowerState setting %s - [try cb, lp, ns, aw, or fp]", CommandLine->args[1]);
        }
    }
    if ((strcmp(CommandLine->args[0], "set") == 0) ||
            (strcmp(CommandLine->args[0], "get") == 0))
    {
        switch (PowerAPI_GetCurrentPowerState())
        {
            case POWER_STATE_COLD_BOOTED:
            case POWER_STATE_LOW_POWER:
            case POWER_STATE_NETWORK_STANDBY:
            case POWER_STATE_AUTO_WAKE_STANDBY:
            case POWER_STATE_FULL_POWER:
                TAP_Printf("CurrentPowerState: %s\n\r", PowerStateStrings[PowerAPI_GetCurrentPowerState()]);
                break;
            default:
                TAP_Printf("CurrentPowerState: Unknown\n\r");
                break;
        }

    }
    else
    {
        TAP_Printf("%s", POWER_HELP_TEXT);
    }
}
