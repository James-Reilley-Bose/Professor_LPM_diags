//
// etapUpdate.h
//

#ifndef _ETAP_UPDATE_H
#define _ETAP_UPDATE_H

/**
 * @DOCETAP
 * COMMAND:   update
 *
 * ALIAS:     up
 *
 * DESCRIP:   Update a specific device
 *
 *
 * PARAM:     [device], [forced]
 *
 *            device: the device to update
 *              - mu (wireless speakers)
 *              - rem (rf remote)
 *              - dsp
 *              - hdmi
 *              - f0
 *              - lpm (lpm user application)
 *              - mfg (lpm mfg application)
 *              - bl (lpm bootloader)
 *              - all (all components under lpm control)
 *              - blob (print summary of blob contents)
 *              - state (gives you current update state)
 *
 *             forced: optionally force the update, even if the component is already at the correct version
 *
 * EXAMPLE:    up all
 *             up dsp
 *             up hdmi, f
 *
 * @DOCETAPEND
 */

void TAP_Update(CommandLine_t*);

TAPCommand(TAP_Update);
#define UP_HELP_TEXT "* This command updates a specific device. May not be used if an update is already running."

TAPCommand(TAP_UP_All);
#define UP_ALL_HELP_TEXT "up all - updates all components"
TAPCommand(TAP_UP_Mu);
#define UP_MU_HELP_TEXT "up mu - updates the speaker firmware"

#ifdef LPM_HAS_RF_REMOTE
TAPCommand(TAP_UP_Remote);
#define UP_REMOTE_HELP_TEXT "up rem - updates the remote firmware"
#endif

#ifdef LPM_HAS_DSP
TAPCommand(TAP_UP_Dsp);
#define UP_DSP_HELP_TEXT "up dsp - updates the dsp firmware"
#endif

#ifdef LPM_HAS_HDMI_UPDATE
TAPCommand(TAP_UP_Hdmi);
#define UP_HDMI_HELP_TEXT "up hdmi - updates the hdmi firmware"
#endif

#ifdef LPM_HAS_WIRELESS_AUDIO
TAPCommand(TAP_UP_WA);
#define UP_WA_HELP_TEXT "up wa - updates the WAM (DARR) firmware"
#endif

#ifdef LPM_HAS_STANDBY_CONTROLLER
TAPCommand(TAP_UP_F0);
#define UP_F0_HELP_TEXT "up F0 - updates the F0 firmware"
#endif

TAPCommand(TAP_UP_Lpm);
#define UP_LPM_HELP_TEXT "up lpm - updates the lpm application image"
TAPCommand(TAP_UP_Mfg);
#define UP_MFG_HELP_TEXT "up mfg - updates the lpm manufacturing image"
TAPCommand(TAP_UP_Bl);
#define UP_BL_HELP_TEXT "up bl - updates the lpm bootloader image"
TAPCommand(TAP_UP_Blob);
#define UP_BLOB_HELP_TEXT "up blob - displays contents of blob file"

TAPCommand(TAP_UP_State);
#define UP_STATE_HELP_TEXT "up state - displays the current update state"

#endif // _ETAP_UPDATE_H
