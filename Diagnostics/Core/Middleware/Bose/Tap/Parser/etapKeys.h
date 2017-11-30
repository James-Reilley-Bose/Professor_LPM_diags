#ifndef _ETAPKEYS_H_
#define _ETAPKEYS_H_

#include "etapcoms.h"

#define TAP_KEYS_MAX_HOLD_TIME 15000 // 15 seconds

void InitializeTAPKeys (void);
void TapKeyHandlePressAndHold (void);
void GiveKeyListSemaphore (void);
void TAP_KeyPressInit(void);

/**
 * @DOCETAP
 *
 * COMMAND:   keylist
 *
 * ALIAS:     kl
 *
 * DESCRIP:   Print/clear list of recently pressed keys.
 *            in the form: Value: <hex value>,Key: <name>, Producer: <producer ID>
 *            <hex value> is defined in A4V_KeyValues.h
 *            <name> is defined in KeyValues.c
 *            <producer> is defined in A4V_KeyVAlues.h
 *
 * PARAM:     clear: set to "clear" to reset the list
 *
 * REPLY:     empty
 *
 * EXAMPLE:   kl
 *
 * @DOCETAPEND
 */

#define KEY_LIST_HELP_TEXT "Usage (print): kl\r\nUsage (clear): kl clear"

void TAP_KeyList(CommandLine_t*);

/**
 * @DOCETAP
 *
 * COMMAND:   keypress
 *
 * ALIAS:     kp
 *
 * DESCRIP:   inject a key (hex value)
 *
 * PARAM:     key: hex value of a bose code
 *
 * REPLY:     empty
 *
 * EXAMPLE:   kp mute,1000
 *
 * @DOCETAPEND
 */

#define KEY_PRESS_HELP_TEXT "Usage: kp <key>, <hold_time_in_ms>, <producer_num>\r\n\tex: kp mute,1000"

void TAP_KeyPress(CommandLine_t*);

/**
 * @DOCETAP
 * COMMAND:   keyinfo
 *
 * ALIAS:     ki
 *
 * DESCRIP:   prints all the keys and metadata
 *
 * PARAM:     n/a
 *
 * REPLY:     empty
 *
 * EXAMPLE:   ki
 *
 * @DOCETAPEND
 */

#define KEY_INFO_HELP_TEXT "Usage (print all keys): ki\r\nUsage (search): ki <search_string>\r\n\tex: ki vol"

void TAP_KeyInfo(CommandLine_t*);

/**
 * @DOCETAP
 * COMMAND:   irtoggle
 *
 * ALIAS:     ir
 *
 * DESCRIP:   Toggles the ability to use ir
 *
 * PARAM:     ir - get current state
 *            ir <0,1> - sets enabled (1) or disabled
 *
 * REPLY:     IR Disabled or IR Enabled
 *
 * EXAMPLE:   ir 1
 *
 * @DOCETAPEND
 */

#define TAP_IR_HELP_TEXT "IR Toggle : Enter ir to see current value\n Enter ir <0,1> to enable or disable\n"

void TAP_IR_Command(CommandLine_t*);

/**
 * @DOCETAP
 * COMMAND:   keyhook
 *
 * ALIAS:     kh
 *
 * DESCRIP:   turns on/off echo for, or ignores/stops ignoring all or specific keys, or returns key echo/ignore state
 *
 * PARAM:     echo,all - echo all
 *            echo,none - echo none
 *            echo,X - echo key X (in hex)
 *            echo,!,X - stop echoing key X (in hex)
 *            ignore,all - ignore all keys
 *            ignore,none - stop ignoring any keys
 *            ignore,X - ignore key X (in hex)
 *            ignore,!,X - stop ignoring key X (in hex)
 *            note: all params can be abbreviated to one char, e.g. e = echo, a = all, i = ignore, n = none
 *
 * REPLY:     with no params, the bitmask of keys currently being echoed/ignored
 *
 * EXAMPLE:   kh echo
 *            kh echo,all
 *            kh echo,none
 *            kh echo,A0
 *            kh echo,!,A0
 *            kh ignore
 *            kh ignore,all
 *            kh ignore,none
 *            kh ignore,A0
 *            kh ignore,!,A0
 *            kh e,a
 *
 * @DOCETAPEND
 */

#define KEY_HOOK_HELP_TEXT "Usage: \n\r" \
    "\tkh echo - get the bitmap of keys being echoed\n\r" \
    "\tkh echo,all - echo all keys\n\r" \
    "\tkh echo,none - stop echoing keys\n\r" \
    "\tkh echo,X - echo key X (hex key value)\n\r" \
    "\tkh echo,!,X - stop echoing key X (hex key value)\n\r" \
    "\tkh ignore - get the bitmap of keys being ignored\n\r" \
    "\tkh ignore,all - ignore all keys\n\r" \
    "\tkh ignore,none - stop ignoring keys\n\r" \
    "\tkh ignore,X - ignore key X (hex key value)\n\r" \
    "\tkh ignore,!,X - stop ignoring key X (hex key value)\n\r" \
    "\tnote: all params can be abbreviated to one char, e.g. e = echo, a = all, i = ignore, n = none\n\r"

void TAP_KeyHook(CommandLine_t*);

#ifdef SUPPORT_ASSIGN_KEY_TO_HDMI_INPUT
#define KEY_ASSIGN_HELP_TEXT "Usage: \n\r" \
    "\tkh ak ff -- remove input assignment and restore default behavior \r\n" \
    "\tkh ak xx -- xx is a valid key id in hex \n\r"

/**
 * @DOCETAP
 * COMMAND:   assignkey
 *
 * ALIAS:     ak
 *
 * DESCRIP:   Assign a source key to activate the HDMI input
 *
 * PARAM:     ak ff -- remove input assignment and restore default behavior
 *            ak is only valid with source keys:
 *                           BOSE_BD_DVD_SOURCE
 *                           BOSE_GAME_SOURCE
 *                           BOSE_AUX_SOURCE
 * REPLY:     OK or Invalid Argument[s]!
 *
 * @DOCETAPEND
 *
 */


void TAP_AssignHDMIInputKey(CommandLine_t*);
#endif

#endif // _ETAPKEYS_H_
