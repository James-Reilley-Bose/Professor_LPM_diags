
/**
  ******************************************************************************
  * @file    Diag_clock_test.h
  *           + Diagnostic test for clock
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_CLOCK_TEST_H
#define DIAG_CLOCK_TEST_H

typedef enum{
  CLKSPD_RD=0, //show current clk speeds, terse 
  CLKSPD_RD_V, //show current clk speeds, verbose
  CLKSPD_96,   //set clk speed 96MHz
  CLKSPD_64,   //set clk speed 64MHz
  CLKSPD_48,   //set clk speed 48MHz
  CLKSPD_32,   //set clk speed 32MHz
  CLKSPD_24,   //set clk speed 24MHz
  CLKSPD_16,   //set clk speed 16MHz
  CLKSPD_HELP  //show help
}CLKSPD_GET_SET;

uint32_t get_set_clk_speed_cmd(CLKSPD_GET_SET mode);


#endif

/* ======== END OF FILE ======== */
