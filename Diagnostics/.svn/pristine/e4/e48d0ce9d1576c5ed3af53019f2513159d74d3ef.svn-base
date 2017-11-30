/*
  File  : etapF0.h
  Title :
  Language: C
  Copyright:  (C) 2016 Bose Corporation, Framingham, MA

  Description:
                i2c F0 TAP communications
===============================================================================
*/

#ifndef _ETAP_F0_H_
#define _ETAP_F0_H_

#include "f0_cmd_interface.h"
#include "etapcoms.h"
#include "etap.h"



#if defined(PROFESSOR)
#define F0_HELP_TEXT_1_OF_4 "\r\n\tUsage: where 0=clear 1=set st=read state\r\n"\
                            "\tf0 hdmipwr,<0|1|st>   ## F0_HDMI_PWR_EN        (A1)\n"
#define F0_HELP_TEXT_2_OF_4 "\tf0 hdmi5v,<0|1|st>    ## F0_HDMI_5V_EN         (A12)\n"
#define F0_HELP_TEXT_3_OF_4 "\tf0 pwr,<en|dis|st>    ## F0 Power Init Sequence\n"
#define F0_HELP_TEXT_4_OF_4 "\tf0 vr                 ## F0 Version Command\n"\
                            "\tf0 reset              ## F0 Reset Command\n"
#endif

#define F0_ERROR_ARGS "\n\tError: Incorrect or missing args!"
#define F0_MISSING_ARGS "\n\tError: Missing args!\n"
#define F0_INVALID_ARGS "\n\tError: Invalid args!\n"

/**
 * @DOCETAP
 * ID:        F0 i2c TAP command
 *
 * DESCRIP:   set F0 GPIO pins via i2c
 * PARAM:
 * REPLY:     empty
 */

/* EXAMPLE:   f0 apbsel0
 *
 * @DOCETAPEND
 */
TAPCommand(TAP_F0);

/* additional function prototypes */
#if defined(BARDEEN)
void f0_ap_boot_sel_tap(char* arg);
#endif
BOOL F0_GPIO_Pin_cmd( uint8_t F0GpioPinCmd,
                      const char* F0GpioPinName,
                      uint8_t setToState,
                      uint8_t* readState);
I2C_BUS_HANDLE_TYPE init_i2c_bus_3(I2C_API_ERROR_CODE* i2cError);
uint8_t calc_pkt_checksum(uint8_t* pkt, uint16_t size);
uint8_t check_for_CECCtrl_Int(void);
BOOL has_cmd_pkt_errors(uint8_t* respPkt, uint16_t size);
BOOL display_F0_gpio_pin_config(const char* F0GpioPinName, F0_GPIO_PIN_struct* gpio);
void f0_gpio_pin_tap( char* arg,
                      const char* pinName,
                      uint8_t cmd_write,
                      uint8_t cmd_read );
void f0_pwr_tap( char* arg );
BOOL f0_pwr_seq_cmd( uint8_t operation );
BOOL f0_variant_cmd( PROFESSOR_F0_CMDS );
BOOL send_cmd_and_get_response(uint8_t* pkt, uint16_t pktSize);
void f0_reset(void);
void f0_diags(uint32_t loop_count);
BOOL f0_mfg_params_tap(CommandLine_t* CommandLine);
void print_mfg_params_help(void);
void build_f0_packet(uint8_t* pkt, uint16_t pktSize, uint8_t cmd, uint8_t* data, uint16_t dataSize);
ErrorStatus f0_mfg_params_check_args(char* arg1, char* arg2, F0_MFG_PARAMS_struct* F0_mfg, uint8_t action);
ErrorStatus f0_mfg_params_send(F0_MFG_PARAMS_struct* F0_mfg);

#endif // _ETAP_F0_H_
