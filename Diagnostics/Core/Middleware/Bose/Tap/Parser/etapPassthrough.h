#ifndef ETAPPASSTHOUGH_H
#define ETAPPASSTHOUGH_H

/**
 * @DOCETAP
 * COMMAND:   passthrough
 *
 * ALIAS:     pt
 *
 * DESCRIP:   Send passthrough tap command to a specific device
 *
 * PARAM:     destination, command
 *
 * REPLY:     Depends
 *
 * EXAMPLE:   pt
 *
 * @DOCETAPEND
 */

#define PASS_THROUGH_HELP_TEXT "Send pass through tap command to dsp, asoc, or st\r\n\tUsage: pt dest, \"command\""
void TAP_PtCommand(CommandLine_t*);

#endif // ETAPPASSTHOUGH_H