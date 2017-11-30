#ifndef ETAP_LOG_H
#define ETAP_LOG_H

#include "etapcoms.h"
#include "etap.h"

/**
 * @DOCETAP
 * COMMAND:   logdump
 *
 * ALIAS:     ld
 *
 * DESCRIP:   pushes logs from one store to another
 *
 * PARAM:     from: name of log store source
 *            to: name of log store sink
 *
 * REPLY:     empty
 *
 * EXAMPLE:   ld abc,xyz - dump log store "abc" into "xyz"
 *
 * @DOCETAPEND
 */

#define LOG_DUMP_HELP_TEXT "* Not implemented."

TAPCommand(TAP_LogDump);

/**
 * @DOCETAP
 * COMMAND:   logenable
 *
 * ALIAS:     le
 *
 * DESCRIP:   sets or gets the enabled state of a log store
 *
 * PARAM:     store: optional name of log store
 *            enable: optional enable or disable (ENABLED, DISABLED)
 *
 * REPLY:     the table of stores and their states.
 *
 * EXAMPLE:   le -- set nothing, just print stores.
 *            le abc,ENABLED -- set store "abc" enabled.
 *            le xyz,DISABLED -- set store "xyz" disabled.
 *
 * @DOCETAPEND
 */

#define LOG_ENABLE_HELP_TEXT "* Not implemented."

TAPCommand(TAP_LogStoreEnable);


/**
 * @DOCETAP
 * COMMAND:   logscribelevel
 *
 * ALIAS:     ll
 *
 * DESCRIP:   sets or gets the log level of a scribe
 *
 * PARAM:     level: optional loglevel to set (DISABLED, NORMAL, VERBOSE, INSANE)
 *            scribe: optional name of a log scribe; if missing, all.
 *
 * REPLY:     the table of scribes and their levels
 *
 * EXAMPLE:   ll -- set nothing, just print scribes
 *            ll VERBOSE -- set all scribes' log levels to verbose
 *            ll NORMAL,abc -- set scribe "abc" log level to normal
 *
 * @DOCETAPEND
 */

#define LOG_SCRIBE_LEVEL_HELP_TEXT "* Use this command to get/set log level.\n\r\tUsage(print scribes): ll\n\r\tUsage(set level): ll <level>,<scribe_name>\r\n\tLevels: disabled, normal, verbose, insane"


TAPCommand(TAP_LogScribeLevel);

/**
 * @DOCETAP
 * COMMAND:   logflush 
 *
 * ALIAS:     lf
 *
 * DESCRIP:   flushes the contents of the specified store
 *
 * PARAM:     store_name: the store to be flushed
 *
 * REPLY:     nothing if successful, otherwise prints an error message
 *
 * EXAMPLE:   lf nv  - clears the nv flash
 *            lf rb  - empties the ring buffer
 *            lf tap - does nothing
 *
 * @DOCETAPEND
 */

#define LOG_FLUSH_HELP_TEXT "* Not implemented."

TAPCommand(TAP_LogFlush);

/**
 * @DOCETAP
 * COMMAND:   logannotate
 *
 * ALIAS:     la
 *
 * DESCRIP:   Inserts a time-stamped message into the log.
 *
 * REPLY:     nothing if successful, otherwise prints an error message
 *
 * EXAMPLE:   la "This is the message." -- Prints "This is the message." to the log.
 *
 * @DOCETAPEND
 */

#define LOG_ANNOTATE_HELP_TEXT "* Use this command to log a message.\n\r\tUsage: la \"Your message here.\""

TAPCommand(TAP_AnnotateLog);

#endif
