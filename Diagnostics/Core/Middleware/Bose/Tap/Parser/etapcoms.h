/*
    File    :   ETAPCOMS.H
    Title   :   Extended TAP (Test Access Port) Interface Command Set Header
    Author  :   Ken Lyons
    Created :   2/20/97
    Language:   Toshiba C-Like
    Copyright:  (C) 1997 Bose Corporation, Framingham, MA

    Description:
        Header file for the tap command set.

    Note: Set tab stops every 4 characters to properly display this file.

===============================================================================
*/
#ifndef HEADER_ETAPCOMS
#define HEADER_ETAPCOMS

#include "platform_settings.h"

// This is obviously a problem we need to address with the parser. --JD71069
#define TAP_NUMBER_OF_ALIASES       2

/* TAP command data structure */

#define NUM_ARGS_SUPPORTED 15
typedef struct
{
    char* command;
    uint8_t numArgs;
    char* args[NUM_ARGS_SUPPORTED];
} CommandLine_t;

typedef struct TAP_Command
{
    const char* command[TAP_NUMBER_OF_ALIASES];   /* include byte for null terminator */
    void (*TAP_Function)(CommandLine_t*);
    const char* helpString;
} TAP_Command;

extern const struct TAP_Command TAP_Commands[];
extern const int TAPNumCommands;

/*
 * @DOCETAP
 * COMMAND:   displaycommands
 *
 * ALIAS:     dc
 *
 * DESCRIP:   Display commands used to list out all the commands recognized
 *            by the system.
 *
 * PARAM:     N/A
 *
 * REPLY:     List of all commands.
 *
 * EXAMPLE:   dc
 *
 * @DOCETAPEND
 *
 */

#define DISPLAY_COMMANDS_HELP_TEXT "* HELP_TEXT: Display commands: Shows all availible commands\n\rExample: dc"

void TAP_CommandsShowAll(CommandLine_t*);

/*
 * @DOCETAP
 * COMMAND:   help
 *
 * ALIAS:     ?
 *
 * DESCRIP:   Print the help text for a tap command.
 *
 * PARAM:     specific command
 *
 * REPLY:     Help text for the specified command
 *
 * EXAMPLE:   help dc
 *
 * @DOCETAPEND
 *
 */

#define HELP_HELP_TEXT "* HELP_TEXT: Type help followed by a command name for more info\n\rExample: help [command name]"

void TAP_Help(CommandLine_t*);

#endif

/* ======== END OF FILE ======== */
