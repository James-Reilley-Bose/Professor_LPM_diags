#include "project.h"
#include "edidapi.h"
#include "etap.h"
#include "etapEDID.h"
#include <stdlib.h>
#include "BufferManager.h"
#include "EventDefinitions.h"
#include <stdio.h>
#include "sii_datatypes.h"
#include "EDID_Transfer.h"
#include "etapIpc.h"

const struct TAP_Command EDID_Commands[] =
{
  {
    {"ge", "get"},
    TAP_EDID_Get_Command,
    EDID_GET_HELP_TEXT,
  },
  {
    {"sd", "send"},
    TAP_EDID_SendCommand,
    EDID_SEND_HELP_TEXT,
  },
};
const int Num_EDID_Commands = sizeof(EDID_Commands) / sizeof(EDID_Commands[0]);

/*
===============================================================================
@fn TAP_EDIDCommand
@brief Forward to sub commands.
===============================================================================
*/
TAPCommand(TAP_EDIDCommand)
{
  if (TAP_ProcessSubCommand(EDID_Commands, Num_EDID_Commands, CommandLine) == FALSE)
  {
    TAP_PrintString(EDID_HELP_TEXT);
    TAP_PrintSubCommands(EDID_Commands, Num_EDID_Commands);
  }
}

/*
===============================================================================
@fn TAP_EDID_SendCommand
@brief Send edid to who ever you want
===============================================================================
*/
TAPCommand(TAP_EDID_SendCommand)
{
  if(CommandLine->numArgs > 0)
  {
    uint8_t dev = 0;
    if(stringToDestination(CommandLine->args[0], &dev))
    {
      EDID_Transfer_Start((Ipc_Device_t)dev);
      return;
    }
  }
    
  TAP_PrintString(EDID_SEND_HELP_TEXT);
}

/*
===============================================================================
@fn TAP_EDID_Get_Command
@brief Print out the EDID (ours or the TVs) to the logger.
===============================================================================
*/
TAPCommand(TAP_EDID_Get_Command)
{
  edid_t* edid = NULL;
  edid_t tvEdid = { 0 };
  tvEdid.edidData = NULL;
  if (2 != CommandLine->numArgs)
  {
    TAP_PrintString(EDID_GET_HELP_TEXT);
    return;
  }
  // Which one?
  if (strncmp(CommandLine->args[0],  "unmod", 5) == 0)
  {
    edid = EDID_GetA4VEdid();
  }
  else if (strncmp(CommandLine->args[0],  "mod", 3) == 0)
  {
    edid = EDID_GetCurrentModifiedEdid();
  }
  else if (strncmp(CommandLine->args[0],  "tv", 2) == 0)
  {
    if (TRUE == EDID_GetCurrentSinkEdid(&tvEdid))
    {
      edid = &tvEdid;
    }
    else
    {      
      TAP_PrintString("Could not get sink EDID, Mutex failed");
    }
  }
  else
  {
    TAP_PrintString(EDID_GET_HELP_TEXT);
  }

  if (TRUE == EDID_IsValid(edid) && edid->edidData != NULL)
  {
    //Where to?
    if (strncmp(CommandLine->args[1],  "log", 3) == 0)
    {
       EDID_PrintToLog(edid);
    }
    else if (strncmp(CommandLine->args[1], "tap", 3) == 0)
    {
       EDID_PrintToTAP(edid);
    }
    else
    {
      TAP_PrintString(EDID_GET_HELP_TEXT);
    }
  }
  else
  {
    TAP_PrintString("EDID invalid");
  }
  if (tvEdid.edidData != NULL)
  {
    EDID_Destroy(&tvEdid);    
  }
}
