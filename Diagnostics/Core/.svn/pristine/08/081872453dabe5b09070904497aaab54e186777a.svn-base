//
// etapWirelessAudio.h
//

#ifndef ETAP_WIRELESS_AUDIO_H
#define ETAP_WIRELESS_AUDIO_H

/*
 * @DOCETAP
 * COMMAND:   WirelessAudio
 *
 * ALIAS:     wa
 *
 * DESCRIP:   Work with wireless audio registers and the DARR
 *
 * PARAM:     <action>, <params...>
 *            b - set band: wa b, <band>
 *            c - program coco: wa c, <c0>, <c1>, <c2>
 *            e - erase: wa e, <0|1>
 *            j - pairing window: wa j
 *            p - start auto pairing: wa p
 *            q - toggle data dump: wa q
 *            t - send test message: wa t
 *            u - update DARR: wa u
 *            x - command extension: wa x, <command>
 *            s - status: wa x, s
 *            r - read: wa r, <dev>, <len>, <RegOffset>
 *            w - write: wa w, <dev>, <len>, <RegOffset>, <Bytes [0,1,...len]>
 *
 * REPLY:        <success> or <failure>
 *            b - <nothing> or <error message>
 *            c - <"Programming coco..."> or <error message>
 *            e - <"FD...", "Erasing coco..."> or <error message>
 *            j - nothing
 *            p - nothing
 *            q - nothing
 *            t - nothing
 *            u - nothing
 *            x - <status> or <error message>
 *            s - <state, status of coco, and bands>
 *            r - <bytes at the RegOffset> or <error message>
 *            w - <nothing> or <error message>
 *
 * EXAMPLE:   wa w, 0, 3, 1, 5, 1, 2
 *            wa r, 0, 3, 1
 *            wa e, 0
 *            wa x, s
 *
 * @DOCETAPEND
 *
 */

#define WIRELESS_AUDIO_HELP_TEXT "Command format: wa <command>" \
    "\nAvailable commands: " \
    "\n\tb - set band: wa b, <band>" \
    "\n\tc - program coco: wa c, <c0>, <c1>, <c2>" \
    "\n\te - erase: wa e, <0|1>" \
    "\n\tj - pairing window: wa j" \
    "\n\tp - start auto pairing: wa p" \
    "\n\tq - toggle data dump: wa q" \
    "\n\tt - send test message: wa t" \
    "\n\tu - update DARR: wa u" \
    "\n\tx - command extension: wa x, <command>" \
    "\n\ts - status: wa x, s" \
    "\n\tr - read: wa r, <dev>, <len>, <RegOffset>" \
    "\n\tw - write: wa w, <dev>, <len>, <RegOffset>, <Bytes [0,1,...len]>\n" \

void TAP_WACommand(CommandLine_t*);

#endif // ETAP_WIRELESS_AUDIO_H
