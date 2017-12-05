
/**
  ******************************************************************************
  * @file    Diag_gpio_test.h
  *           + Diagnostic test for GPIO
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_GPIO_TEST_H
#define DIAG_GPIO_TEST_H

typedef struct {
   uint32_t lpm_pin;     /* pin number */
   const char *name;     /* gpio name */
   const char *desc;     /* signal name */
   GPIOMode_TypeDef mode;   /* HW defined pin configured */
} LPM_PIN_DEFS;

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

void bb_show_all_gpio_pins(void);
uint32_t dgpio_get(int argc, char *argv[]);
uint32_t dgpio_set(int argc, char *argv[]);

#endif

/* ======== END OF FILE ======== */
