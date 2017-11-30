/*
    File    :   interrupt.c
    Author  :   Ported from Triode
    Created :   12/02/2014
    Language:   C
    Copyright:  (C) 2014 Bose Corporation, Framingham, MA

    Description:   Interface implementation of interrupts in Bardeen

===============================================================================
*/
#include "project.h"
#include "stm32f2xx.h"

#include "interrupt.h"
#include "etap.h"

static  INTERRUPT_FNCT_PTR  Interrupt_VectorTable[(HASH_RNG_IRQn + 1)];

/*****************************************************************//**
* @brief    Dummy interrupt handler for un-hanlded interrupts
**********************************************************************/
static void Interrupt_DummyHandler(void)
{
    IRQn_Type      src_nbr;
    uint32_t     reg_val;

    reg_val  = (REG_NVIC_ICSR & MSK_NVIC_ICSR_VECT_ACTIVE);
    src_nbr  = (IRQn_Type)(reg_val -  INT_EXT0);

    //should never get here
    TAP_AssertPrint("Unhandled Interrupt", src_nbr);
}

/*****************************************************************//**
* @brief    Disable the given interrupt
*
* @param    id - IRQ number as defined in stm32f2xx.h
*
* @return   void
**********************************************************************/
void Interrupt_Disable(IRQn_Type id)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_InitStructure.NVIC_IRQChannel =  id;
    NVIC_Init(&NVIC_InitStructure);

}

/*****************************************************************//**
* @brief    Enable the given interrupt
*
* @param    id - IRQ number as defined in stm32f2xx.h
* @param    priority - preemption priority of the interrupt, value range 0 - 15
*                       0 is the highest, and 15 is the lowest
* @param    subPriority - subpriority level of the interrupt, value range 0 - 15
*                       0 is the highest, and 15 is the lowest
*
* @return   void
**********************************************************************/
void Interrupt_Enable(IRQn_Type id, uint8_t priority, uint8_t subPriority)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel =  id;
    NVIC_Init(&NVIC_InitStructure);

}

/*****************************************************************//**
* @brief    Register an interrupt
*
* @param    id - IRQ number as defined in stm32f2xx.h
* @param    isr - Interrupt service routine
*
* @return   void
**********************************************************************/
void  Interrupt_RegisterISR(IRQn_Type  id, INTERRUPT_FNCT_PTR isr)
{
    if (id < (HASH_RNG_IRQn + 1))
    {
        portENTER_CRITICAL();
        Interrupt_VectorTable[id] = isr;
        portEXIT_CRITICAL();

    }
}


/*****************************************************************//**
* @brief    Set a dummy handler for all IRQ lines
* @return   void
**********************************************************************/
void Interrupt_Init(void)
{
    for (IRQn_Type id = WWDG_IRQn; id < (HASH_RNG_IRQn + 1); id++)
    {
        Interrupt_RegisterISR(id, Interrupt_DummyHandler);
    }
}


/*****************************************************************//**
* @brief    Generic interupt handler for IRQ lines 0 to 80
* @return   void
**********************************************************************/
void Interrupt_Handler(void)
{
    INTERRUPT_FNCT_PTR  p_int_vect;
    IRQn_Type      src_nbr;
    uint32_t     reg_val;

    reg_val  = (REG_NVIC_ICSR & MSK_NVIC_ICSR_VECT_ACTIVE);
    if (reg_val < INT_EXT0)
    {
        return;
    }
    src_nbr  = (IRQn_Type)(reg_val -  INT_EXT0);

    if (src_nbr < (HASH_RNG_IRQn + 1))
    {
        p_int_vect = Interrupt_VectorTable[src_nbr];
        if (p_int_vect != (INTERRUPT_FNCT_PTR)0)
        {
            p_int_vect();
        }
    }
}


/*****************************************************************//**
* @brief    Clear a pending interrupt
* @return   void
**********************************************************************/
void Interrupt_ClearPending(IRQn_Type id)
{
    int ofs = (id >> 5);
    int bit = id & 31;

    REG_NVIC_ICPR[ofs] = (1 << bit);
}

