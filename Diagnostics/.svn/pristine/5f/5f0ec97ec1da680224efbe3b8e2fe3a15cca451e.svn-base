/*!
* @file        DiagsDispatcher.h
* @version     $Id:
* @author
* @brief       Diags handler header
*
* Copyright 2016 Bose Corporation.
*/
#ifndef INCLUDE_DIAGSDISPATCHER_H
#define INCLUDE_DIAGSDISPATCHER_H

#include "etap.h"
#include "KeyData.h"

#define TIM_PRE_48MHZ 47
#define TIM_PRE_32MHZ 31
#define TIM_PRE_24MHZ 23
#define TIM_PRE_16MHZ 15

//PSOC CMD defines

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


typedef struct
{
    uint32_t lpm_pin;     /* pin number */
    const char* name;     /* gpio name */
    const char* desc;     /* signal name */
    GPIOMode_TypeDef mode;   /* HW defined pin configured */
} LPM_PIN_DEFS;

typedef struct
{
    uint32_t mode;
    const char* name;              /* cmd name*/
    uint32_t (*func)();            /* ptr to command fcn  */
    const char* description;       /* description of command */
} CliCmdStruct;

#define DIAG_CMD_HIST_MAX 20
#define DIAG_CMD_ARGS_MAX 15
#define DIAG_CMD_TBL_ARG_SIZE 10
typedef struct
{
    char args[DIAG_CMD_ARGS_MAX][DIAG_CMD_TBL_ARG_SIZE]; /* store cmd line history upto 15 10B args*/
    uint32_t cmdtbleindex;            /* to find the cmd easily */
    int argcnt;
} DiagsCmdHist;

typedef enum
{
    CLKSPD_RD = 0, //show current clk speeds, terse
    CLKSPD_RD_V, //show current clk speeds, verbose
    CLKSPD_96,   //set clk speed 96MHz
    CLKSPD_64,   //set clk speed 64MHz
    CLKSPD_48,   //set clk speed 48MHz
    CLKSPD_32,   //set clk speed 32MHz
    CLKSPD_24,   //set clk speed 24MHz
    CLKSPD_16,   //set clk speed 16MHz
    CLKSPD_HELP  //show help
} CLKSPD_GET_SET;

typedef struct
{
    char* adcname;
    uint8_t channel;
} ADC_CHANNELS_T;

#define SENSE1_CH 4   //GPIO A4
#define THERM1_CH 8   //GPIO B0
#define THERM2_CH 9   //GPIO B1
#define SENSE2_CH 10  //GPIO C0

#define VOLTAGE_MAX (3.3)
#define DIVISIONS_MAX (0xfff +1) // 12bit
#define N2_DECIMAL_POINTS_PRECISION (100)
#define N3_DECIMAL_POINTS_PRECISION (1000)
#define STM16_CH 16
#define STM17_CH 17

#ifdef PROFESSOR
#define THERM_SERIES_OHMS          (2740)      // OHMs
#define THERM_NOMIAL_OHMS          (1000)
#define V25                        (.76)
#define AVG_SLOPE                  (.0025)       //volts/C
#else

#define THERM_SERIES_OHMS          (1020)      // OHMs
#define THERM_NOMIAL_OHMS          (3000)
#define ff             (25)
#define V25                        (.76)
#define AVG_SLOPE                  (.025)      //volts/C
#endif

#define TEMP_C_AT_76MV             (25)

typedef struct
{
    int16_t cval;
    float tscale;
} TCONVERT_T;

#define IOD_HELP "iod <bank> - dump io pins in bank"



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


#define IOS_HELP "ios <bank>,<pin>,<mode>,<pup>,<type>,<spd>,<val> - set an io pin\n\r"
#define IOS_BANK "\tbank - a,b,c,d,e,f,g,h,i\n\r"
#define IOS_PIN  "\tpin  - 0-15\n\r"
#define IOS_FULL_HELP IOS_HELP IOS_BANK IOS_PIN

#define IOS_MODE "\tmode - in,out,af,an\n\r"
#define IOS_PUP  "\tpup  - none, up, down\n\r"
#define IOS_TYPE "\ttype - pp, od\n\r"
#define IOS_SPD  "\tspd  - low,med,fast,high\n\r"
#define IOS_LVL  "\tval  - hi,lo,toggle\n\r"
#define IOS_FULL_HELP1 IOS_MODE IOS_PUP IOS_TYPE IOS_SPD IOS_LVL

#define LED_HELP1 "led <x>, <on, off, t>  - set led state"
#define LED_HELP2 "\t    <x> = 1, 2, 3, 4, 5 - all"
#define LED_HELP LED_HELP1 LED_HELP2
#define CLK_SPD_HELP "show / set LPM clocks"
#define I2CSCAN_HELP "scan i2c bus <1, 2, 3>"
#define SNAP_HELP "dsnap <fboot,init,off,on,reboot,qfil,show,help>"
#define LPM_PINLIST_HELP "list LPM pin allocation"
#define ADC_HELP "dadc <verb level>  - display 4 adc connection values"
#define LPM_OPT_BYTES_HELP "dopt <> <> -option bytes read / write"
#define DIAGPRESS_HELP "diag tests for more info type,  dt help"
#define FO_HELP "FO commands, for more info type,  f0 help"
#define DIAGS_HELP "diags for list of commands"
#define CLKPCS 24000000
#define LCD_BCKLIGHT_PSC 120 //pre-scaler for max freq = 400KHz
#define T1MAXFREQ  0xffff


typedef enum
{
    DIAG_0,
    DIAG_1,
    DIAG_2,
    DIAG_3,
    DIAG_4,
    NUM_DIAG_TESTS
} DIAG_VALUE;

extern int diags_ltbar_control_save_key_flag;

void TAP_di2cscan(CommandLine_t*);
void TAP_dget_clk_speed_cmd(CommandLine_t*);
void TAP_dled(CommandLine_t*);
void TAP_dgpio_set(CommandLine_t*);
void TAP_dgpio_get(CommandLine_t*);
void TAP_dsnap(CommandLine_t*);
void TAP_dadc(CommandLine_t*);
void TAP_dlpmplist(CommandLine_t*);
void TAP_optbyter(CommandLine_t*);
void TAP_Diags_Display(CommandLine_t*);
void TAP_DiagPress(CommandLine_t*);
void led_gpio_config(void);
void amp_init(void);
void darr_init(void);
void nfc_init(void);
void ledbar_init(void);

uint32_t cmd_adc(int index, int lastindex, uint32_t repeat, int verb, int delay);
uint32_t dled(int argc, char* argv[]);
void HandleDiagKey(DIAG_VALUE test_key, uint32_t testarg);
void dadc_config(void);
uint32_t cmd_i2cscan(int argc, char* argv[]);
uint32_t dgpio_get(int argc, char* argv[]);
void bb_init_all_gpio_pins(char verb);
int d_check_button_press(uint32_t bmask);
void bb_show_all_gpio_pins(void);
uint32_t bb_check_pon_reset_state(void);
uint32_t check_pon_int_state(void);
void snap_off(void);
int snap_force_reboot(int phase);
float get_pwm_percent(void);
void set_pwm_percent(char* inptstr);
void led_ctrl(int led_cmd, int mode);
int psoc_read_responce_chk(void);
int psoc_send_cmd(void);
void snap_on(void);
void diags_ltbar_control(void);
void diags_ltbar_control_save_key(KEY_DATA_t key);
void diags_opt_ltbar_control(void);
#endif
