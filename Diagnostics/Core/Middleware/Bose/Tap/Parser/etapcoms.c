/*
    File    :   ETAPCOMS.C
    Title   :   Extended TAP (Test Access Port) Interface Command Set
    Author  :   Ken Lyons
    Created :   2/20/97
    Language:   C
    Copyright:  (C) 1997 Bose Corporation, Framingham, MA

    Description:
        This module contains the command table that is utilized
    by the ETAP parser. The parser will invoke the function that
    is associated with the command string.


    Note: Set tab stops every 4 characters to properly display this file.

===============================================================================
*/
#define BODY_ETAPCOMS

#include <string.h>
#include <stdio.h>
#include "project.h"
#include "configuresystem.h"
#include "etap.h"       /* for basic feature set */
#include "etapcoms.h"   /* our header */
#include "etapTasks.h"
#include "TapListenerAPI.h"
#include "etapHeap.h"
#include "SystemEventsTAP.h"
#include "etapGpio.h"
#include "etapLog.h"
#include "etapI2C.h"
#include "etapSpi.h"
#include "etapInternalFlash.h"
#include "etapAssert.h"
#include "versionlib.h"
#include "etapFlash.h"
#include "ir.h"

#ifdef NOT_NOW // TODO: dj1005472 - Handle these
#include "etapIpc.h"
#include "etapPassThrough.h"
#include "UpdateManagerBlob.h"
#include "etapPower.h"
#include "etapKeys.h"
#include "etapUpdate.h"
#include "etapNV.h"
#include "PowerTask.h"
#include "nv_system.h"
#endif

#ifdef INCLUDE_TAP_HELP
#define TAP_HELP(x) x
#else
#define TAP_HELP(x) ""
#endif

//
// TAP_CommandsShowAll
//
TAPCommand(TAP_CommandsShowAll)
{
    TAP_PrintString("--------------\r\n");
    for (int i = 0; i < (TAPNumCommands); i++)
    {
        for (int j = 0; j < (TAP_NUMBER_OF_ALIASES); j++)
        {
            TAP_PrintString(TAP_Commands[i].command[j]);
            TAP_PrintString("\r\n");
        }
        TAP_PrintString("--------------\r\n");
    }
}

//
// TAP_Help
//
TAPCommand(TAP_Help)
{
    switch (CommandLine->numArgs)
    {
        case 0:
            {
                TAP_PrintString(HELP_HELP_TEXT);
            }
            break;

        case 1:
            {
                if (CommandLine->args[0] != NULL)
                {
                    const TAP_Command* temp = TAP_FindTAPCommand(TAP_Commands, TAPNumCommands, CommandLine->args[0]);
                    if (temp == NULL)
                    {
                        TAP_Printf("\"%s\" has no help to show!", CommandLine->args[0]);
                    }
                    else
                    {
                        TAP_PrintString(temp->helpString);
                    }
                }
            }
            break;

        default:
            break;
    }
}
