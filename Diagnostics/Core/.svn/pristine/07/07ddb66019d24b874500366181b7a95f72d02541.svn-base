//
// etapPower.c
//
#include "etap.h"
#include "TapListenerAPI.h"
#include "TapParserAPI.h"
#include "etapSource.h"
#include "HwMacros.h"
#include "sourceInterface.h"
#include "SourceChangeTask.h"

/*
 * @func TAP_CurrentSource
 *
 * @brief Source switch
 */

TAPCommand(TAP_CurrentSource)
{
    BOOL valid = FALSE; // valid flag for parsing various arguments

    if (CommandLine->numArgs == 0)
    {
        SOURCE_ID source_id = SourceChangeAPI_GetCurrentSource();
        SOURCE_ID desired_source_id = SourceChangeAPI_GetDesiredSource();
        TAP_Printf("Desired: %d Actual: %d\n", desired_source_id, source_id);
    }
    else if (CommandLine->numArgs == 1 && strcmp(CommandLine->args[0], "list") == 0)
    {
        TAP_PrintString("\nSource ID list\n");
        for (SOURCE_ID id = SOURCE_STANDBY; id < NUM_SOURCES; id++)
        {
            if (IsValidForVariant(id))
            {
                const UnifySourceStruct* pUSS = UI_GetUnifyStructForSource(id);
                if (pUSS)
                {
                    TAP_Printf("%2d %s\r\n", id, UITask_GetUnifySrcName(pUSS->sourceID));
                }
                else
                {
                    TAP_Printf("%2d Does not exist in Unify Source list.\r\n", id);
                }
            }
        }
    }
    else if (CommandLine->numArgs == 2 && strcmp(CommandLine->args[0], "set") == 0)
    {
        SOURCE_ID source_id = (SOURCE_ID)TAP_DecimalArgToInt(CommandLine, 1, &valid);
        if (!valid)
        {
            TAP_PrintString("Source ID must be an integer\n");
        }
        else if (source_id >= NUM_SOURCES || !IsValidForVariant(source_id))
        {
            TAP_PrintString("Source ID invalid for this product\n");
        }
        else
        {
            SourceChangeAPI_SetCurrentSource( source_id);
            TAP_PrintString("Source set command sent\n");
        }
    }
    else
    {
        TAP_PrintString(CURRENT_SRC_HELP_TEXT);
    }
}




