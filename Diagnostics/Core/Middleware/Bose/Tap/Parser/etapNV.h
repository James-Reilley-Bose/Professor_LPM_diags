#ifndef ETAPNV_H
#define ETAPNV_H

/*
  File   :  etapNV.h
  Title  :
  Author :  Brian White (modified by dr1005920 for bardeen)
  Created   :  6/07/13
  Language: C
  Copyright:   (C) 2006, 2009, 2010, 2013 Bose Corporation, Framingham, MA

  Description:   etap interface to NV sub-system

  ===============================================================================
*/

#include "etap.h"

#define SERIAL_NUMBER_HELP_TEXT "Get the stored serial number or set it\n\r\t Usage: 'sn' or 'sn serialnum' "
#define CONSOLE_NUMBER_HELP_TEXT "Get the stored serial number or set it\n\r\t Usage: 'sn' or 'sn serialnum' "
#define TEST_RESULT_HELP_TEXT "Gets result of mfg test\n\r\t \
Usage: mt 0 \n\r\t \
0     FUNCTIONAL_TEST_GET\n\r\t \
1     FUNCTIONAL_TEST_SET_PASS\n\r\t \
2     FUNCTIONAL_TEST_SET_FAIL\n\r\t \
3     PREBOOTH_TEST_GET\n\r\t \
4     PREBOOTH_TEST_SET_PASS\n\r\t \
5     PREBOOTH_TEST_SET_FAIL\n\r\t \
6     FINALBOOTH_TEST_GET\n\r\t \
7     FINALBOOTH_TEST_SET_PASS\n\r\t \
8     FINALBOOTH_TEST_SET_FAIL\n\r\t \
      FUNCTIONAL_TEST_SET_REMAN = 0xFF"

#define SYS_NONVOL_HELP_TEXT "Store the current system parameters to nonvol\n\rUsage: 'cvn' or 'commitnv' "

/**
 * @DOCETAP
 * COMMAND:     sysvariant 
 *
 * ALIAS:       sv
 *
 * DESCRIP:     Sets or gets the console variant.
 *
 * FORMAT:      sv [variant], [speaker_variant], [country], [region], [default language], [color]
 *
 * PARAM:       Product Variant:
 *               0=Undefined
 *               1=Bardeen
 *               2=Ginger
 *               3=Skipper
 *               4=Maxwell
 *              Speaker Package Variant:
 *               1=Omnivo
 *               2=Jewel Cubes
 *               3=Ginger
 *               4=Skipper
 *               5=Maxwell
 *               6=Skipper_Maxwell
 *              Country Variant:
 *               1=United States
 *               2=Great Britain
 *               3=Canada
 *               4=Japan
 *              Region Variant
 *               1=United States
 *               2=Great Britain
 *               3=Reserved-Canada
 *               4=Japan
 *              Default Language Selection
 *               1=English
 *               2=Spanish
 *               3=French
 *               4=Danish
 *               5=German
 *               6=Italian
 *               7=Swedish
 *               8=Polish
 *               9=Hungarian
 *               10=Finnish
 *               11=Czech
 *               12=Greek
 *               13=Norwegian
 *               14=Portuguese
 *               15=Romanian
 *               16=Russian
 *               17=Slovenian
 *               18=Turkish
 *               19=Simple Mandarin
 *               20=Thai
 *               21=Korean
 *               22=Traditional Mandarin
 *               23=Hebrew
 *               24=Arabic
 *               25=Japanese
 *               26=Dutch
 *               27=Traditional Cantonese
 *              System Color:
 *               0=White
 *               1=Black
 *
 *
 * REPLY:       Product: Product Variant
 *              Speaker package: Speaker Package Variant
 *              Country: Country Variant
 *              Region: Region Variant
 *              Default Language: Default Language Selection
 *              Color: System Color
 *
 * EXAMPLE:   sv 1,1,1,1,1,1 sets Bardeen, Omnivo, US, US, English, black
 *
 * @DOCETAPEND      
 *       
 *       
*/
#define SYS_VARIANT_HELP_TEXT "\
    \nPRODUCT_VARIANT: \
    \n\t1=BARDEEN, \
    \n\t2=GINGER, \
    \n\t3=SKIPPER, \
    \n\t4=MAXWELL, \
    \nSPEAKER_PACKAGE_VARIANT: \
    \n\t1=SPEAKER_PACKAGE_OMNIVO, \
    \n\t2=SPEAKER_PACKAGE_JEWELCUBES, \
    \n\t3=SPEAKER_PACKAGE_GINGER, \
    \n\t4=SPEAKER_PACKAGE_GINGER_W_BASS, \
    \n\t5=SPEAKER_PACKAGE_GINGER_W_SS, \
    \n\t6=SPEAKER_PACKAGE_GINGER_W_BASS_SS, \
    \n\t7=SPEAKER_PACKAGE_GINGER_W_DS, \
    \n\t8=SPEAKER_PACKAGE_GINGER_W_BASS_DS, \
    \n\t9=SPEAKER_PACKAGE_SS, \
    \n\t10=SPEAKER_PACKAGE_DS, \
    \n\t11=SPEAKER_PACKAGE_SKIPPER, \
    \n\t12=SPEAKER_PACKAGE_PBB,\
    \n\t13=SPEAKER_PACKAGE_GINGER_W_PBB,\
    \n\t14=SPEAKER_PACKAGE_GINGER_W_PBB_SS,\
    \nCOUNTRY_VARIANT: \
    \n\t1=COUNTRY_US, \
    \n\t2=COUNTRY_GB, \
    \n\t3=COUNTRY_CN, \
    \n\t4=COUNTRY_JP, \
    \nREGION_VARIANT: \
    \n\t1=REGION_US, \
    \n\t2=REGION_GB, \
    \n\t3=REGION_RESERVED_CN, \
    \n\t4=REGION_JP, \
    \nDEFAULT_LANGUAGE_SELECTION: \
    \n\t0=LANGUAGE_ENGLISH, \
    \n\tSee Tap Document for more languages \
    \nSYSTEM_COLOR: \
    \n\t0=COLOR_WHITE, \
    \n\t1=COLOR_BLACK \
    \n \
    \nUsage:sv <PRODUCT_VARIANT>, <SPEAKER_PACKAGE_VARIANT>, <COUNTRY_VARIANT>, <REGION_VARIANT>, <DEFAULT_LANGUAGE_SELECTION>, <SYSTEM_COLOR> \
    \nsv (no param) get current settings"
#define SYS_MFG_PARAM_COMMIT_HELP_TEXT "Store the current manufacturing parameters to novol\n\rUsage: 'cmf' or 'commitmfg'"
#define SYS_FACTORY_DEFAULT_HELP_TEXT "\
   Reset all of the user parameters to default/first boot state. \n\r\
   Usage: fd - resets user parameters and reboots system \r\n\
          fd, lp - resets user parameters without rebooting and sets system to low power"

#define MFG_DATA_HELP_TEXT "\
    Get/Set manufacturing data\r\n\
    Usage: mfg param - gets a value\r\n\
           mfg param,value - writes a value (hex string)\r\n\
           mfg com - commits written values\r\n\
    A commit is automatically performed after writing.\r\n\
    Params are:\r\n\
    all (read-only, will also show the sizes of the fields)\r\n\
"

void TAP_SerialNo (CommandLine_t*);
void TAP_ConsoleSerialNo (CommandLine_t*);
void TAP_CommitSysNV (CommandLine_t*);
void TAP_FunctionalTestResults (CommandLine_t*);
void TAP_SysVariant(CommandLine_t*);
void TAP_CommitMfgNV(CommandLine_t*);
void TAP_FactoryDefault(CommandLine_t*);
void TAP_ManufacturingData(CommandLine_t*);


/**
* @DOCETAP
* COMMAND:    nvram 
*
* ALIAS:      nv
*
* DESCRIP:   Commands to set/get some NV features.
*
* PARAM:     [oob|aw|ssu|dwd], [value]
*            oob: A value of 0 indicates feature is disabled.
*                 A value of 1 indicates feature is enabled.
*
*            aw: A value of 0 indicates feature is disabled
*                A value of 1 indicates feature is enabled.
*                This feature is only available on Ginger.
*
*            ssu: A value of 0 means the speaker update will be performed.
*                 A value of 1 means the speaker update will be skipped.
*
*            dwd: A value of 0 indicates feature is disabled
*                 A value of 1 indicates feature is enabled.
*                This feature is only available on Bardeen/Ginger.
*
* REPLY:     When the sub command is used without a value, it returns the current setting.
*
* EXAMPLE:   nv oob     <- returns current value for this feature
*            nv oob, 1  <- sets the feature to enabled
*
*
* @DOCETAPEND
*
*/

#define NV_HELP_TEXT  "Usage: nv <cmd> [,value]\n\r"
TAPCommand(TAP_SysNV);

#define NV_OOB_HELP_TEXT  "Out of Box: nv oob [,{0 = not out of box, 1 = out of box}]\r\n"
TAPCommand(TAP_NV_OOB);

#if defined(GINGER) || defined(PROFESSOR)
#define NV_AW_HELP_TEXT  "AutoWake Enable: nv aw [, {0 = normal standby, 1 = autowake standby}]\r\n"
TAPCommand(TAP_NV_Autowake);
#endif

#define NV_SSU_HELP_TEXT "Skip speaker update: nv ssu,<0 = don't skip, 1 = skip>\r\n"
TAPCommand(TAP_NV_SSU);

#define NV_STG_HELP_TEXT  "Prints all NV settings\r\n"
TAPCommand(TAP_NV_SETTINGS);

#ifdef SUPPORT_DSPWATCHDOG

#define NV_DWD_HELP_TEXT  "DspWD Enable: nv dwd [, {0 = WD disabled, 1 = WD reset on expiry}]\r\n"
TAPCommand(TAP_NV_DspWD);

#endif

#endif
