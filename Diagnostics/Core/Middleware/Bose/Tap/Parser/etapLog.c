#include "rottenlog.h"
#include "etapLog.h"
#include "etap.h"

SCRIBE_DECL(annotate);

//
// @func TAP_LogDump
// @brief TAP command to dump log to a store.
//
// Args: args[0] = from
//       args[1] = to
//
// Note: unimplemented in etapcoms.c
//
TAPCommand(TAP_LogDump)
{
    if(CommandLine->numArgs != 2) {
        TAP_PrintString("Usage: ld <from_store>,<to_store>\r\n");
        return;
    }
    Rotten_Logger *l = rotten();
    if(l == NULL) {
      TAP_PrintString("Could not initialize logger.\r\n");
      return;
    }

    l->store_dump(l, CommandLine->args[0], CommandLine->args[1]);
}

//
// @func TAP_LogStoreEnable
// @brief TAP command to enable store.
//
// Args: args[0] = store
//       args[1] = enabled/disabled
//
// Note: unimplemented in etapcoms.c
//
TAPCommand(TAP_LogStoreEnable)
{
    Rotten_Logger *l = rotten();
    if(l == NULL) {
      TAP_PrintString("Could not initialize logger.\r\n");
      return;
    }

    switch(CommandLine->numArgs) {
        case 0:
            l->print_stores(l);
            break;

        case 1:
            TAP_PrintString("Usage: le <store_name>,{ENABLED|DISABLED|FRAMED}\r\n");
            break;

        case 2:
        default:
        {
            Rotten_StoreEnable enable = ROTTEN_STORE_DISABLED;
            if(rotten_storeenable_parse(CommandLine->args[1], &enable) == ROTTEN_SUCCESS) {
                l->store_enable(l, CommandLine->args[0], enable);
            } else {
                TAP_PrintString("Invalid value for enable\r\n");
            }
        }
        break;
    }
}

//
// @func TAP_LogScribeLevel
// @brief TAP command to change scribe level.
//
// Args: args[0] = level
//       args[1] = scribe
//
TAPCommand(TAP_LogScribeLevel)
{
    Rotten_Logger *l = rotten();
    if(l == NULL) {
      TAP_PrintString("Could not initialize logger.\r\n");
      return;
    }

    switch(CommandLine->numArgs) {
        case 0:
            l->print_scribes(l);
            TAP_PrintString("\n");
            break;

        case 1:
            TAP_PrintString("Missing argument.\r\n");
            TAP_PrintString(LOG_SCRIBE_LEVEL_HELP_TEXT);
            break;

        case 2:
        default:
        {
            Rotten_LogLevel level = ROTTEN_LOGLEVEL_NORMAL;
            if(rotten_loglevel_parse(CommandLine->args[0], &level) == ROTTEN_SUCCESS)
            {
                if(l->scribe_set_level(l, CommandLine->args[1], level) != ROTTEN_LOGLEVEL_INVALID)
                {
                    TAP_PrintString("Log level set.\r\n");
                    return;
                }
            }
            // try again with opposite order
            if(rotten_loglevel_parse(CommandLine->args[1], &level) == ROTTEN_SUCCESS)
            {
                if(l->scribe_set_level(l, CommandLine->args[0], level) != ROTTEN_LOGLEVEL_INVALID)
                {
                    TAP_PrintString("Log level set.\r\n");
                    return;
                }
            }
            TAP_PrintString("Invalid value for level/scribe\r\n");
        }
        break;
    }
}

//
// @func TAP_LogFlush
// @brief TAP command to flush a log store.
//
// Args: args[0] = store
//
// Note: unimplemented in etapcoms.c
//
TAPCommand(TAP_LogFlush)
{
    Rotten_Logger *l = rotten();
    if(l == NULL) {
        TAP_PrintString("Could not initialize logger.\r\n");
        return;
    }

    switch(CommandLine->numArgs) {
        case 1:
            if (l->store_flush(l, CommandLine->args[0]) == ROTTEN_FAILURE) {
                TAP_PrintString("Invalid store name.\r\n");
            }
            break;
        default:
            TAP_PrintString("Usage: lf <store_name>\r\n");
            break;
    }
}

//
// @func TAP_LogStoreAnnotate
// @brief TAP command to log a message.
//
// Args: args[0] = store
//
TAPCommand(TAP_AnnotateLog)
{
    switch(CommandLine->numArgs) {
        case 0:
            TAP_PrintString("No message found.\r\n");
            TAP_PrintString(LOG_ANNOTATE_HELP_TEXT);
            break;
        default:
            LOG(annotate, ROTTEN_LOGLEVEL_NORMAL, "\t==== %s ====", CommandLine->args[0]);
            TAP_Printf("==== %s ====\n\r", CommandLine->args[0]);
            break;
    }
}
