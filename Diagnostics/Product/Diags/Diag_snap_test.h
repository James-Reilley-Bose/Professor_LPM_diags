
/**
  ******************************************************************************
  * @file    Diag_snap_test.h
  *           + Diagnostic test for snap
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_SNAP_TEST_H
#define DIAG_SNAP_TEST_H

#define SNAP_HELP "dsnap <init,on,off,show,qfil>"

uint32_t bb_check_pon_reset_state(void);
uint32_t check_pon_int_state(void);
uint32_t cmd_snap(int argc, char *argv[]);
void snap_off(void);
int snap_force_reboot(int phase);

#endif

/* ======== END OF FILE ======== */
