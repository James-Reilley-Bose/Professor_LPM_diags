//
// etapHDMI.h
//

#ifndef ETAP_HDMI_H
#define ETAP_HDMI_H

/*
 * @DOCETAP
 * COMMAND:   hdmi
 *
 * ALIAS:     hdmi
 *
 * DESCRIP:   Top level command for HDMI related commands
 *
 * PARAM:     sub command
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   hdmi earcinit
 *            Instantiates the driver for the Sii9437
 *
 * @DOCETAPEND
 *
 */
void HDMI_Command(CommandLine_t*);
#define HDMI_HELP_TEXT "Top level command for all HDMI related commands"

/*
 * @DOCETAP
 * COMMAND:   earcinit
 *
 * ALIAS:     ea
 *
 * DESCRIP:   Initializes eARC Sii9437 Driver
 *
 * SUBCOMMAND: hdmi
 *
 * PARAM:     N/A
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   hdmi ea
 *            Instantiates the driver for the Sii9437
 *
 * @DOCETAPEND
 *
 */
#define EARC_INIT_HELP_TEXT "Initialize the eARC driver"
void TAP_EarcInit(CommandLine_t*);

/*
 * @DOCETAP
 * COMMAND:   earcsetpower
 *
 * ALIAS:     earcsp
 *
 * DESCRIP:   Sets the power state of the earc chip
 *
 * SUBCOMMAND: hdmi
 *
 * PARAM:     0 (full power) 1 (standby)
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   hdmi earcsp, 1
 *            Sets the Sii9437 chip to standby
 *
 * @DOCETAPEND
 *
 */
#define EARC_SET_POWER_STATE_HELP_TEXT "Set the power state of the eARC. 1 (standby) 0 (full power)"
void TAP_EarcSetPowerState(CommandLine_t*);

 /*
 * @DOCETAP
 * COMMAND:   earcgetpower
 *
 * ALIAS:     earcgp
 *
 * DESCRIP:   Displays the power state of the earc chip
 *
 * SUBCOMMAND: hdmi
 *
 * PARAM:     N/A
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   hdmi earcgp
 *            Get the current power state of the eARC chip
 *
 * @DOCETAPEND
 *
 */
#define EARC_GET_POWER_STATE_HELP_TEXT "Displays the power state of the earc chip"
void TAP_EarcGetPowerState(CommandLine_t*);

 /*
 * @DOCETAP
 * COMMAND:   earchotplugdetected
 *
 * ALIAS:     earchpd
 *
 * DESCRIP:   Displays/sets the hot plug detected pin
 *
 * SUBCOMMAND: hdmi
 *
 * PARAM:     1 (Hi) 0 (Lo)
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   hdmi earchpd, 1
 *            Sets the HPD pin high
 *
 * @DOCETAPEND
 *
 */
#define EARC_HPD_HELP_TEXT "Displays/set HPD"
void TAP_EarcHPD(CommandLine_t*);

 /*
 * @DOCETAP
 * COMMAND:   earcarcmode
 *
 * ALIAS:     earcarc
 *
 * DESCRIP:   Displays/sets arc mode (None, ARC, eARC)
 *
 * SUBCOMMAND: hdmi
 *
 * PARAM:     0 (None) 1 (Arc) 2 (eARC)
 *
 * REPLY:     N/A
 *
 * EXAMPLE:   hdmi earcarc, 2
 *            Sets the arc mode of the Sii9437 to eARC
 *
 * @DOCETAPEND
 *
 */
#define EARC_ARC_MODE_HELP_TEXT "Displays/set ARC Mode"
void TAP_EarcArcMode(CommandLine_t*);

#endif // ETAP_HDMI_H
