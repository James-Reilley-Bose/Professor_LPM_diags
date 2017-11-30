//
// etapPower.h
//

#ifndef _ETAP_POWER_H
#define _ETAP_POWER_H

/**
 * @DOCETAP
 * COMMAND:   reboot
 *
 * ALIAS:     rb
 *
 * DESCRIP:   Reboot the system
 *
 * PARAM:     n/a
 *
 * REPLY:     empty
 *
 * EXAMPLE:   reboot
 *
 * @DOCETAPEND
 */

#define REBOOT_HELP_TEXT "* Reboots the system. \n\rEnter 'rb 0' to reboot to user \
                                                \n\rEnter 'rb 1' to reboot to diags \
                                                \n\rEnter 'rb 2' to reboot to bootloader"

void TAP_RebootCommand(CommandLine_t*);

/**
 * @DOCETAP
 * COMMAND:     power
 *
 * ALIAS:       po
 *
 * DESCRIP:     Power related TAP utility.
 *
 * SUBCOMMAND:  po get  -or-  po set
 *
 * PARAM:       PowerStates:
 *               cb - cold boot
 *               lp - low power
 *               ns - network standby
 *               aw - auto wake
 *               fp - full power
 *              PowerFail (toggle):
 *               pf1 - power fail 1
 *               pf2 - power fail 2
 *               pf3 - power fail 3
 *
 * REPLY:       CurrentPowerState, Power Fail On/Off
 *
 *
 * EXAMPLE:     po get - Returns current Power State
 *
 * @DOCETAPEND
 *
 */

#define POWER_HELP_TEXT "Power related TAP commands\n\r  \
   \tUsage : po get  -or-  po set, newPowerState or toggle Power Fail\n\r \
   \tPowerStates: cb, lp, ns, aw, fp\n\r"



void TAP_Power(CommandLine_t*);


void TAP_IdleTimeout(CommandLine_t*);

/**
 * @DOCETAP
 * COMMAND:    idleset
 *
 * ALIAS:      gs
 *
 * DESCRIP:    Get/Set Idle Time properties.
 *
 * SUBCOMMAND: gs [e|d] to enable/disable gs [a time |u time | s time] to set Audio/User Interaction/Screensaver timeout duration.
 *
 *
 * REPLY:      idle  time: Enabled
 *             User  time(minutes): 1440 Count 0
 *             Audio time(minutes): 240 count 0
 *             Screensaver time(minutes): 5 count 0
 *
 * EXAMPLE:    gs - Get/Set Idle Time properties
 *
 * @DOCETAPEND
 *
 */

#define IDLE_SET_TEXT "Get/set Idle time out" \
                             "\n\r 'gs e' to enable timeouts" \
                             "\n\r 'gs d' to disable timeouts" \
                             "\n\r 'a,seconds' to set audio timeout" \
                             "\n\r 'u,seconds' to set user interaction timeout" \
                             "\n\r 's,seconds' to set screensaver timeout" \
                             "\n"

#endif // _ETAP_POWER_H
