/*
    File    :   TapCommands.c
    Author  :   Ken Lyons
    Created :   2/20/97
    Language:   C
    Copyright:  (C) 1997 Bose Corporation, Framingham, MA

    Description:
        This module contains the command table that is utilized
    by the ETAP parser. The parser will invoke the function that
    is associated with the command string.
===============================================================================
*/

#include <string.h>
#include <stdio.h>
#include "project.h"
#include "configuresystem.h"
#include "etap.h"       /* for basic feature set */
#include "etapcoms.h"   /* our header */
#include "etapTasks.h"
#include "TapListenerAPI.h"
#include "etapHeap.h"
#include "SystemEventsTAP.h"
#include "etapGpio.h"
#include "etapLog.h"
#include "etapI2C.h"
#include "etapSpi.h"
#include "etapInternalFlash.h"
#include "etapAssert.h"
#include "etapFlash.h"
#include "ir.h"
#include "etapVersion.h"
#include "etapPower.h"
#include "etapKeys.h"
#include "etapNV.h"
#include "etapUpdate.h"
#include "etapIpc.h"
#include "etapPassThrough.h"
#include "etapHDMI.h"
#include "etapCEC.h"
#include "etapEDID.h"
#include "etapuei.h"
#include "etapWirelessAudio.h"
#include "etapNV.h"
#ifdef LPM_HAS_LIGHTBARCAPSENSE
#include "etapLBCS.h"
#endif
#include "DiagsDispatcher.h"
#include "RemoteTap.h"

#ifdef NOT_NOW // TODO: dj1005472 - Handle these

#include "UpdateManagerBlob.h"
#include "PowerTask.h"
#include "nv_system.h"
#endif

#ifdef INCLUDE_TAP_HELP
#define TAP_HELP(x) x
#else
#define TAP_HELP(x) ""
#endif

/*******************************************************************************
    Data Structure: TAP_Commands[]

    Description:
        The table of commands interpreted by the TAP command parser. It is
    an evenly filled table consisting of a character string in all upper case
    that is the command identifier, and a pointer to the function that is
    invoked when the user types that particular command into the TAP port.

********************************************************************************
*/
/* Command List - **PLEASE** keep this alphabetized */
const struct TAP_Command TAP_Commands[] =
{
#ifdef NOT_YET
    {
        {"as", "assert"},
        TAP_Assert,
        TAP_HELP(ASSERT_HELP_TEXT)
    },
#endif
#if 0
#ifdef INCLUDE_EVENT_STRINGS
    {
        {"be", "block"},
        TAP_BlockOnEvent,
        TAP_HELP(BLOCK_ON_EVENT_HELP_TEXT)
    },
#endif
#endif

    {
        {"cec", "cec_commands"},
        CEC_Command,
        TAP_HELP(CEC_HELP_TEXT)
    },

    
    {
        {"cnv", "commitnv"},
        TAP_CommitSysNV,
        TAP_HELP(SYS_NONVOL_HELP_TEXT)
    },

    
    {
        {"dc", "displaycommands"},
        TAP_CommandsShowAll,
        TAP_HELP(DISPLAY_COMMANDS_HELP_TEXT)
    },

    {
        {"edid", "edid_commands"},
        TAP_EDIDCommand,
        TAP_HELP(EDID_HELP_TEXT)
    },
    
    {
        {"ft", "internalflashtest"},
        TAP_TestInternalFlash,
        TAP_HELP(FLASH_TEST_HELP_TEXT)
    },


    {
        {"hdmi", "hdmi_commands"},
        HDMI_Command,
        TAP_HELP(HDMI_HELP_TEXT)
    },


    {
        {"he", "heap"},
        TAP_Heap,
        TAP_HELP(HEAP_HELP_TEXT)
    },

    {
        {"help", "?"},
        TAP_Help,
        TAP_HELP(HELP_HELP_TEXT)
    },

    {
        {"ic", "i2c"},
        TAP_IICCommand,
        TAP_HELP(I2C_HELP_TEXT)
    },

    {
        {"io", "gpio"},
        TAP_GpioCommand,
        TAP_HELP(GPIO_HELP_TEXT)
    },

    {
        {"ip", "ipc"},
        TAP_IpcCommand,
        TAP_HELP(IPC_HELP_TEXT)
    },

    {
        {"ki", "keyinfo"},
        TAP_KeyInfo,
        TAP_HELP(KEY_INFO_HELP_TEXT)
    },

    {
        {"kl", "keylist"},
        TAP_KeyList,
        TAP_HELP(KEY_LIST_HELP_TEXT)
    },

    {
        {"kp", "keypress"},
        TAP_KeyPress,
        TAP_HELP(KEY_PRESS_HELP_TEXT)
    },

    {
        {"la", "logannotate"},
        TAP_AnnotateLog,
        TAP_HELP(LOG_ANNOTATE_HELP_TEXT)
    },

#ifdef LPM_HAS_LIGHTBARCAPSENSE
    {
        {"lb", "lbcs" },
        TAP_LBCS,
        TAP_HELP(LBCS_HELP_TEXT)
    },
#endif

    {
        {"ld", "logdump"},
        TAP_LogDump,
        TAP_HELP(LOG_DUMP_HELP_TEXT)
    },

    {
        {"le", "logenable"},
        TAP_LogStoreEnable,
        TAP_HELP(LOG_ENABLE_HELP_TEXT)
    },

    {
        {"lf", "logflush"},
        TAP_LogFlush,
        TAP_HELP(LOG_FLUSH_HELP_TEXT)
    },

    {
        {"ll", "logscribelevel"},
        TAP_LogScribeLevel,
        TAP_HELP(LOG_SCRIBE_LEVEL_HELP_TEXT)
    },


    {
        {"mfg", "manuf"},
        TAP_ManufacturingData,
        TAP_HELP(MFG_DATA_HELP_TEXT)
    },
#if 0 // TODO - dj1005472, fix this
    {
        {"nv", "nvram"},
        TAP_SysNV,
        TAP_HELP(SYS_NONVOL_HELP_TEXT)
    },
#endif
#if 0
#ifdef INCLUDE_EVENT_STRINGS
    {
        {"pe", "postevent"},
        TAP_PostEvent,
        TAP_HELP(POST_EVENT_HELP_TEXT)
    },
#endif
#endif

    {
        {"po", "power"},
        TAP_Power,
        TAP_HELP(POWER_HELP_TEXT)
    },

    {
        {"pt", "passthrough"},
        TAP_PtCommand,
        TAP_HELP(PASS_THROUGH_HELP_TEXT)
    },

    {
        {"rb", "reboot"},
        TAP_RebootCommand,
        TAP_HELP(REBOOT_HELP_TEXT)
    },

#ifdef LPM_HAS_RF_REMOTE
    {
        {"re", "remote"},
        TAP_Remote,
        TAP_HELP(REMOTE_HELP_TEXT)
    },
#endif

    {
        {"sa", "system"},
        TAP_HandleFREtapMsg,
        TAP_HELP(SYSTEM_ANALYSIS_HELP_TEXT)
    },

    {
        {"sf", "flash"},
        TAP_Flash,
        TAP_HELP(FLASH_HELP_TEXT)
    },

    {
        {"sfd", "flashdump"},
        TAP_FlashDump,
        TAP_HELP(FLASH_DUMP_HELP_TEXT)
    },

    {
        {"sfe", "flasherase"},
        TAP_FlashErase,
        TAP_HELP(FLASH_ERASE_HELP_TEXT)
    },

    {
        {"sp", "spi"},
        TAP_SPICommand,
        TAP_HELP(SPI_HELP_TEXT)
    },

    {
        {"sv", "sysvariant"},
        TAP_SysVariant,
        TAP_HELP(SYS_VARIANT_HELP_TEXT)
    },

    {
        {"up", "update"},
        TAP_Update,
        TAP_HELP(UP_HELP_TEXT)
    },

    {
        {"ut", "ueiblaster"},
        TAP_UEIBlasterCommand,
        TAP_HELP(DISPLAY_UEI_HELP_TEXT)
    },

    {
        {"vr", "version"},
        TAP_Version,
        TAP_HELP(VERSION_HELP_TEXT)
    },

    {
        {"wa", "wirelessaudio" },
        TAP_WACommand,
        TAP_HELP(WIRELESS_AUDIO_HELP_TEXT)
    },
    
     {
        {"diag", "diags"},
        TAP_Diags_Display,
        TAP_HELP(DIAGS_HELP)
    },

    {
        {"dadc", "dadc"},
        TAP_dadc,
        TAP_HELP(ADC_HELP)
    },

    {
        {"dclk", "dclkget"},
        TAP_dget_clk_speed_cmd,
        TAP_HELP(CLK_SPD_HELP)
    },

    {
        {"dgpiog", "dgpioget"},
        TAP_dgpio_get,
        TAP_HELP(IOD_HELP)
    },

    {
        {"dgpios", "dgpioset"},
        TAP_dgpio_set,
        TAP_HELP(IOS_HELP)
    },

    {
        {"di2c", "di2cscan"},
        TAP_di2cscan,
        TAP_HELP(I2CSCAN_HELP)
    },

    {
        {"dt", "dtask"},
        TAP_DiagPress,
        TAP_HELP(DIAGPRESS_HELP)
    },

    {
        {"dlpm", "dlpmlist"},
        TAP_dlpmplist,
        TAP_HELP(LPM_PINLIST_HELP)
    },

    {
        {"dopt", "dopt"},
        TAP_optbyter,
        TAP_HELP(LPM_OPT_BYTES_HELP)
    },

    {
        {"ds", "dsnap"},
        TAP_dsnap,
        TAP_HELP(SNAP_HELP)
    },


};

/* this should get put into ROM, since it is a constant */
const int TAPNumCommands = sizeof(TAP_Commands) / sizeof(TAP_Commands[0]);
