/*
    File    :   etapUI.h
    Author  :   dr1005920
    Created :   10/6/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Unify related TAP command

===============================================================================
*/
#ifndef ETAP_UI_H
#define ETAP_UI_H

#include "etap.h"
#include "etapcoms.h"

/*
* @DOCETAP
* COMMAND:    unify
*
* ALIAS:      ui
*
* DESCRIP:   Unify Source List Commands
*
* SUBCOMMAND: ui get, [nm|mxnm] - gets the current UI source
*             A4V>ui get
*             Current Unify Source: SOURCE_UNIFY
*             A4V>ui get, nm
*             Number of Available Sources: 18
*             A4V>ui get, mxnm
*             Maximum number of Sources: 25
*
* SUBCOMMAND: ui set, UnifyID - changes to Unify Source
*             A4V>ui set, 2
*             Changing to Unify Source: SOURCE_HDMI_2
*
* SUBCOMMAND: ui list - lists all Unify Sources
*             A4V>ui list
*             Index: 0         Name: SOURCE_STANDBY   SOURCE_ID:0
*             Index: 1         Name: SOURCE_HDMI_1    SOURCE_ID:1
*             Index: 2         Name: SOURCE_HDMI_2    SOURCE_ID:2
*             Index: 3         Name: SOURCE_HDMI_3    SOURCE_ID:3
*             Index: 4         Name: SOURCE_HDMI_4    SOURCE_ID:4
*             Index: 5         Name: SOURCE_HDMI_5    SOURCE_ID:5
*             Index: 6         Name: SOURCE_HDMI_6    SOURCE_ID:6
*             Index: 7         Name: SOURCE_TV        SOURCE_ID:8
*             Index: 8         Name: SOURCE_SHELBY    SOURCE_ID:9
*             Index: 9         Name: SOURCE_BLUETOOTH SOURCE_ID:10
*             Index: 10        Name: SOURCE_ADAPTIQ   SOURCE_ID:11
*             Index: 11        Name: SOURCE_ASOC      SOURCE_ID:23
*             Index: 12        Name: SOURCE_OPTICAL1  SOURCE_ID:20
*             Index: 13        Name: SOURCE_OPTICAL2  SOURCE_ID:21
*             Index: 14        Name: SOURCE_UNIFY     SOURCE_ID:22
*             Index: 15        Name: SOURCE_DEMO      SOURCE_ID:12
*             Index: 16        Name: SOURCE_PTS       SOURCE_ID:13
*             Index: 17        Name: SOURCE_UPDATE    SOURCE_ID:14
*
* SUBCOMMAND: ui add, DisplayName, SourceID, AVSyncDelay, TrebleLevel, BassLevel, AudioMode - adds new unify source
*             A4V>ui add, test, 19, 150, 0, 1, 2
*             Unify Source test added
*
* SUBCOMMAND: ui del, UnifyID - deletes UnifyID from Unify Sources
*             A4V>ui del, 18
*             Unify Source: 18 deleted
*
* SUBCOMMAND: ui default - factory defaults Unify Sources
*             A4V>ui default
*             Unify Source list set to default
*
* SUBCOMMAND: ui sp - Reports different type of speaker package to DSP to create a different sound effect, used by demo team in store
*             A4V>ui sp, 0
*             Usage: Reports current speaker package from network
*             A4V>ui sp, 1
*             Usage: Sets speaker package to Skipper
*             A4V>ui sp, 2
*             Usage: Sets speaker package to Maxwell Left + Maxwell Right
*             A4V>ui sp, 3
*             Usage: Sets speaker package to Maxwell Left + Maxwell Right + Skipper
*             A4V>ui sp, 4
*             Usage: Sets speaker package to Ginger only
*             A4V>ui sp, 99
*             Usage: Resets DSP to what was as detected from network
*             IMPORTANT: Remember to issue [ui sp, 99] to allow normal accessory status update after having issued any of [ui sp] command
*
* @DOCETAPEND
*/




TAPCommand(TAP_UnifySource);
#define UI_HELP_TEXT "Unify source list, accessory commands"
TAPCommand(TAP_UI_Get);
#define UI_GET_HELP_TEXT "ui get - gets the current UI source"
TAPCommand(TAP_UI_List);
#define UI_LIST_HELP_TEXT "ui list - lists all Unify Sources"
TAPCommand(TAP_UI_Set);
#define UI_SET_HELP_TEXT "ui set, UnifyID"
TAPCommand(TAP_UI_Default);
#define UI_DEFAULT_HELP_TEXT "ui default - factory defaults Unify Sources"
TAPCommand(TAP_UI_SpeakerPackage);
#define UI_ACCESSORY_HELP_TEXT "ui speaker package - Alters speaker accessory package reported to DSP"
TAPCommand(TAP_UI_LLWState);
#define UI_LLWSTATE_HELP_TEXT "ui llw - gets the llw state"
TAPCommand(TAP_UI_UserList);
#define UI_USERLIST_HELP_TEXT "ui usr - lists sources configured via Unify App"
TAPCommand(TAP_UI_Error);
#define UI_ERROR_HELP_TEXT "ui er - reports curent console error state"
TAPCommand(TAP_UI_IgnoreKeyDuringSourceChange);
#define UI_IGNOREKEYDURINGSOURCECHANGE_HELP_TEXT "ui ik, [on/off] - ignore all keys while a source change is pending"
TAPCommand(TAP_UI_MoreButtons);
#define UI_MOREBUTTONS_HELP_TEXT "ui mb - lists available more buttons menu items for the current source"
TAPCommand(TAP_UI_Die);
#define UI_DIE_HELP_TEXT "ui die - kills the lpm.  power cycle to return"
TAPCommand(TAP_Hosp);
#define TAP_HOSPITALITY_HELP "ui hp, r - requests limits file\n\rui hp, h - sets system mode to hospitality\n\rui hp, n - sets system mode to normal\n\rui hp - prints current settings"
TAPCommand(TAP_Acc);
#define TAP_ACCESSORY_HELP "Prints current accessory list."
#endif //ETAP_UI_H
