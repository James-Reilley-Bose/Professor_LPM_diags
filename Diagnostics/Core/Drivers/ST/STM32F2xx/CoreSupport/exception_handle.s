;;////////////////////////////////////////////////////////////////////////////////
;;/// @file            exception_handle.s
;;/// @version         $Id: $
;;/// @brief           special case exception handlers written in assembly
;;/// @author          Dylan
;;/// @date            Creation Date: Wed Oct 08 2008
;;///
;;/// Copyright 2008 Bose Corporation
;;////////////////////////////////////////////////////////////////////////////////
;;  we want to pass params in to some exception handlers that we
;;   cannot get to from c code

  RSEG CODE:CODE(2)
  EXTERN __CrashHandler

; Exported functions
  EXPORT __CallHardFault

__CallHardFault
        movs r0, #4
        movs r1, lr
        tst r0, r1
        beq _MSP
        mrs r0, psp
        b _HALT
     _MSP:
        mrs r0, msp
     _HALT:
        mrs r1, ipsr
        b __CrashHandler
    END

