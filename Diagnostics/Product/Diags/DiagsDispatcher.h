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

#define IOD_HELP "iod <bank> - dump io pins in bank"
#define CLK_SPD_HELP "show/set LPM clocks"
#define LPM_PINLIST_HELP "list LPM pin allocation"
#define ADC_HELP "dadc <verb level>  - display 4 adc connection values"
#define LPM_OPT_BYTES_HELP "dopt <> <> -option bytes read / write"
#define DIAGPRESS_HELP "diag tests for more info type,  dt help"
#define FO_HELP "FO commands, for more info type,  f0 help"
#define DIAGS_HELP "diags for list of commands"

#define LCD_BCKLIGHT_PSC 120 //pre-scaler for max freq = 400KHz
#define T1MAXFREQ  0xffff

#include "TaskDefs.h"
extern ManagedTask* ManagedDiagHandlerTask;
//#define DiagHandlerPostMsg(_theMsg, _p1, _p2, _p3) QueueManagerPostCallbackMsg(&ManagedDiagHandlerTask->Queue, _theMsg, (uint32_t []){_p1,_p2, _p3},NOP_CALLBACK, NO_BLOCK_TIME);

#define DiagHandlerPostMsg(_theMsg, _p1) QueueManagerPostCallbackMsg(&ManagedDiagHandlerTask->Queue, _theMsg, _p1, NOP_CALLBACK, NO_BLOCK_TIME);

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
void diags_ltbar_control(void);
void diags_ltbar_control_save_key(KEY_DATA_t key);


#endif
