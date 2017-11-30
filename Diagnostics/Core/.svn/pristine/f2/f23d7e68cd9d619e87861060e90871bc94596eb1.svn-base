/*
    File    :   interrupt.h
    Author  :   Ported from Triode
    Created :   12/02/2014
    Language:   C
    Copyright:  (C) 2014 Bose Corporation, Framingham, MA

    Description:   Interface headers of interrupts in Bardeen

===============================================================================
*/
#ifndef INTERRUPT_H
#define INTERRUPT_H

#define  REG_NVIC_ICSR           (*((volatile unsigned int *)(0xE000ED04)))
#define  REG_NVIC_ICPR           ((volatile unsigned int *)(0xE000E280))
#define  INT_SYSTICK                                  15u
#define  INT_EXT0                                     16u
#define  MSK_NVIC_ICSR_VECT_ACTIVE            0x000001FF

typedef            void      (*INTERRUPT_FNCT_PTR )(void );




void Interrupt_Init(void);
void Interrupt_RegisterISR(IRQn_Type  id, INTERRUPT_FNCT_PTR isr);
void Interrupt_Enable(IRQn_Type id, uint8_t priority, uint8_t subPriority);
void Interrupt_Disable(IRQn_Type id);
void Interrupt_ClearPending(IRQn_Type id);

#endif
