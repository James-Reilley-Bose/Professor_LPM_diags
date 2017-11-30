/*
    File    :   etapAssert.c
    Author  :   ja70076
    Created :   07/07/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Assert related TAP commands

===============================================================================
*/
#include "project.h"
#include "etapAssert.h"
#include "instrument.h"
#include "etap.h"
#include "nv_system.h"

uint32_t SystemReboot = 1;

TAPCommand(TAP_Assert)
{
    if (CommandLine->numArgs > 0)
    {
        if (strcmp(CommandLine->args[0], "fake") == 0)
        {
            TAP_AssertPrint(__FILE__, __LINE__);
            return;
        }
        else if (strcmp(CommandLine->args[0], "reset") == 0)
        {
            SystemReboot = 0;
            return;
        }
        else if (strcmp(CommandLine->args[0], "check") == 0)
        {
            TAP_Printf("Reboot: %d", SystemReboot);
            return;
        }
#if defined(HAS_SYSPARAMS_NVRAM)
        else if (strcmp(CommandLine->args[0], "reboot") == 0)
        {
            if (strcmp(CommandLine->args[1], "on") == 0)
            {
                NV_SetRebootOnAssert(TRUE);
                NV_CommitSystemParams();
            }
            else if (strcmp(CommandLine->args[1], "off") == 0)
            {
                NV_SetRebootOnAssert(FALSE);
                NV_CommitSystemParams();
            }
            TAP_Printf("Reboot On Assert: %s\n", NV_GetRebootOnAssert() ? "on" : "off");
            return;
        }
#endif
    }
    TAP_Printf("%s", ASSERT_HELP_TEXT);
}

