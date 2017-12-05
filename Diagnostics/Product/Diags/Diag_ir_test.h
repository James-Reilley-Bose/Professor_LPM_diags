/**
  ******************************************************************************
  * @file    Diag_ir_test.h
  *           + Diagnostic test for IR
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_IR_TEST_H
#define DIAG_IR_TEST_H

#include "etapcoms.h"
#include "HwMacros.h"

typedef enum{
  DIAG_IR_HELP,
  DIAG_IR_RECEIVE_START,
  DIAG_IR_RECEIVE_STOP,
  DIAG_IR_SOUND
}DIAG_IR_COMMANDS;

void DIAG_IRCommand_Handle(DIAG_IR_COMMANDS commandType);
void Diag_IR_Blink_Led (BOOL pressed);

extern BOOL printIR;

#endif

/* ======== END OF FILE ======== */