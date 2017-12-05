/**
  ******************************************************************************
  * @file    Diag_psoc_test.h
  *           + Diagnostic test for PSOC
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_PSOC_TEST_H
#define DIAG_PSOC_TEST_H

#include "etapcoms.h"
#include "HwMacros.h"
#include "KeyData.h"
#include "RemoteDefs.h"

void ledbar_init(void);
void psoc_usart3_send_str(char *str);
void psoc_usart3_get_str(char *str);
void build_psoc_led_num_inetsity_cmd(char *cmdarray, int lednum, int ledintensity);
void build_psoc_led_color_inetsity_cmd(char *cmdarray, char *ledcolor, int ledintensity);
int build_psoc_action(char *cmdarray, char *cmd, uint32_t *ledintensity);
int build_psoc_animation_cmd(char *cmdarray, char *cmd);
//void psoc_enable_int(void);
void psoc_reset(void);
int psoc_send_cmd(void);
//void HandlePSoCLoad(CommandLine_t* CommandLine);
void diags_ltbar_control(void);
void diags_ltbar_control_save_key(KEY_DATA_t key);

extern int diags_ltbar_control_save_key_flag;
extern int diags_rf_remote_key_press_flag;
void diag_rf_remote(uint8_t Remote_numPacketsReceived, Remote_SPIPacket_t *pkt);

#define PSOC_HELP "dt psoc,<version,clearall,allset,oneset,reset,read,int,intoff>\n\r"

#define PSOC_HELP1 "\
\tversion\t\t - get version 2 bytes\n\r\
\tclear  \t\t - clear all leds\n\r\
\tallset,intensity - set all with an intensity(0-4095)\n\r"

#define PSOC_HELP2 "\toneset,<led>,<i>,<repeat>   - set one led led=(0-24), i=(0-4095), 1 for walking repeat\r\n"

#define PSOC_HELP3 "\
\tint    \t\t - enable interrupts\n\r\
\tintoff \t\t - interrupts off\n\r"

#define PSOC_HELP4 "\
\tals    \t\t - als, #patterns, autostart, loop \n\r\
\talp    \t\t - alp, duration, 48-byte-hex-intensities\n\r"
#define PSOC_HELP5 "\
\tase    \t\t - stop animation at the end of cycle\n\r\
\tasi    \t\t - stop animation immediate\n\r\
\tas     \t\t - start animation\n\r"

#define GetVersion_cmd        0
#define GetVersion_cmd_size   1
#define LEDsClearAll_cmd      1
#define LEDsClearAll_cmd_size 1
#define LEDsSetAll_cmd        2
#define LEDsSetAll_cmd_size   50
#define LEDsSetOne_cmd        3
#define LEDsSetOne_cmd_size   4

#define ANI_LOAD_ST_cmd       4 
#define ANI_LOAD_ST_cmd_size  4
#define ANI_LOAD_cmd          5
#define ANI_LOAD_cmd_size     51  //cmd,2byte duration, 48bytes intensity
#define ANI_START_cmd         6
#define ANI_START_cmd_size    1
#define ANI_STOP_I_cmd        7
#define ANI_STOP_I_cmd_size   1
#define ANI_STOP_E_cmd        8
#define ANI_STOP_E_cmd_size   1

#endif

/* ======== END OF FILE ======== */