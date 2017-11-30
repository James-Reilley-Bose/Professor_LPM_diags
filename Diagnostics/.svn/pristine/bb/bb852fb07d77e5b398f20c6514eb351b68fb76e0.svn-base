//
// etapCEC.h
//

#ifndef ETAP_CEC_H
#define ETAP_CEC_H

/*
 * @DOCETAP
 * COMMAND:   cec
 *
 * ALIAS:     cec
 *
 * DESCRIP:   Top level command for CEC related commands
 *
 * PARAM:     sub command
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   cec initarc
 *            Attempts to initiate/complete and ARC handshake
 *
 * @DOCETAPEND
 *
 */
#define CEC_HELP_TEXT "Top level command for all CEC related commands"
void CEC_Command(CommandLine_t*);

/*
 * @DOCETAP
 * COMMAND:   systemaudio
 *
 * ALIAS:     sysa
 *
 * SUBCOMMAND: cec
 *
 * DESCRIP:   Sends either a system audio request or enable system audio cec request
 *
 * PARAM:     on/off enable/disable
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   cec sysa, on
 *            Sends a cec request to initiate system audio
 *
 * @DOCETAPEND
 *
 */
#define TAP_CEC_SYS_AUDIO_HELP_TEXT "Sends either a system audio request or a enable system audio request"
void TAP_CEC_SYS_AUDIO_Command(CommandLine_t*);

/*
 * @DOCETAP
 * COMMAND:   intiatearc
 *
 * ALIAS:     initarc
 *
 * DESCRIP:   Sends an initiate arc message
 *
 * SUBCOMMAND: cec initarc
 *
 * PARAM:     N/A
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   initarc
 *            Attempts to initiate/complete and ARC handshake
 *
 * @DOCETAPEND
 *
 */
#define InitiateArc_HELP_TEXT "Sends an InitiateArc cec message"
void TAP_InitiateArc(CommandLine_t*);

/*
 * @DOCETAP
 * COMMAND:   spoofaddr
 *
 * ALIAS:     addr
 *
 * SUBCOMMAND: cec
 *
 * DESCRIP:   Spoofs the hdmi physical address to 0x3000 and attempts to acquire logical address 5
 *
 * PARAM:     N/A
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   cec addr
 *
 * @DOCETAPEND
 *
 */
#define TAP_SPOOF_CEC_ADDR_HELP_TEXT "Hard codes hdmi physical addr to 0x3000"
void TAP_SPOOF_PHYSICAL_ADDR_Command(CommandLine_t*);

#endif