////////////////////////////////////////////////////////////////////////////////
/// @file          HwMacros.h
/// @brief         Define some basic macros for interacting with the hardware
///                   on the LPM board.
/// @author        Dylan Reid
/// Copyright      2006-2010
////////////////////////////////////////////////////////////////////////////////
#ifndef _HWMACROS_H_
#define _HWMACROS_H_

#include "project.h"

#define INCREMENT_BUFFER_INDEX(idx, length) if(++idx == length) idx = 0

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define __arm_irq__
#define __arm_fiq__
#define __arm_swi__
#define __arm_abort__

#else

#define likely(x) (x)
#define unlikely(x) (x)

#define __arm_irq__
#define __arm_fiq__
#define __arm_swi__
#define __arm_abort__
#endif

#define EXTI_IMR_ADDR ((unsigned)EXTI + 0x00)
#define EXTI_EMR_ADDR ((unsigned)EXTI + 0x04)
#define EXTI_RTSR_ADDR ((unsigned)EXTI + 0x08)
#define EXTI_FTSR_ADDR ((unsigned)EXTI + 0x0c)
#define EXTI_SWIER_ADDR ((unsigned)EXTI + 0x10)
#define EXTI_PR_ADDR ((unsigned)EXTI + 0x14)

#define TIGGER_SW_INTERRUPT() EXTIT->SWIER = SW_INT_TRIGGER_LINE;

#define interrupt_disable() __disable_irq()
#define interrupt_enable() __enable_irq()

#define EXTI_INTERRUPT_DISABLE(X) PERIPH_BIT_BAND_CLEAR(EXTI_IMR_ADDR, X)
#define EXTI_INTERRUPT_ENABLE(X) PERIPH_BIT_BAND_SET(EXTI_IMR_ADDR, X)
#define EXTI_CHECK_INTERRUPT_ENABLED(X) PERIPH_BIT_BAND_TEST(EXTI_IMR_ADDR, X)

#define EXTI_ENABLE_FALLING_EDGE(X)   PERIPH_BIT_BAND_SET(EXTI_FTSR_ADDR, X)
#define EXTI_ENABLE_RISING_EDGE(X)    PERIPH_BIT_BAND_SET(EXTI_RTSR_ADDR, X)

#define EXTI_DISABLE_FALLING_EDGE(X)   PERIPH_BIT_BAND_CLEAR(EXTI_FTSR_ADDR, X)
#define EXTI_DISABLE_RISING_EDGE(X)    PERIPH_BIT_BAND_CLEAR(EXTI_RTSR_ADDR, X)

// not bit-banded as it seems to wipe out the entire PR register.
//  maybe it reads and writes back a modified register value?
//  this would clear all the pending interrupts.
#define EXTI_INTERRUPT_PENDING(X) (EXTI->PR & (1 << X))
#define EXTI_CLEAR_INTERRUPT_PENDING(X) (EXTI->PR = (1 << X))

// interupt functions need to save the mask off before setting the pin

/* These macros take a MASK */
#define ASSERT_GPIO_PIN_FG(x, y) GPIO_WriteBit(x, (0x0001 << y), Bit_SET)
#define DEASSERT_GPIO_PIN_FG(x, y) GPIO_WriteBit(x, (0x0001 << y), Bit_RESET)
#define ASSERT_GPIO_PIN_INT(x, y) GPIO_WriteBit(x, (0x0001 << y), Bit_SET)
#define DEASSERT_GPIO_PIN_INT(x, y) GPIO_WriteBit(x, (0x0001 << y), Bit_RESET)

/* y is pin MASK value */
#define GPIO_PIN_ASSERTED(x, y) GPIO_ReadInputDataBit(x,(0x0001 << y))

#define WORD_BYTE_REVERSE(x) ( (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24) )

#define ASSERT_TEST1_GPIO() ASSERT_GPIO_PIN_INT(SW_DBG_1_GPIO_BANK, SW_DBG_1_GPIO_PIN)
#define DEASSERT_TEST1_GPIO() DEASSERT_GPIO_PIN_INT(SW_DBG_1_GPIO_BANK, SW_DBG_1_GPIO_PIN)
#define ASSERT_TEST2_GPIO() ASSERT_GPIO_PIN_INT(SW_DBG_2_GPIO_BANK, SW_DBG_2_GPIO_PIN)
#define DEASSERT_TEST2_GPIO() DEASSERT_GPIO_PIN_INT(SW_DBG_2_GPIO_BANK, SW_DBG_2_GPIO_PIN)

#define TOGGLE_TEST1_GPIO()  \
{   static BOOL flip = TRUE; \
    if (flip) \
    { \
        ASSERT_TEST1_GPIO(); \
    } \
    else \
    { \
        DEASSERT_TEST1_GPIO(); \
    } \
    flip = !flip; \
}

#define TOGGLE_TEST2_GPIO()  \
{   static BOOL flip = TRUE; \
    if (flip) \
    { \
        ASSERT_TEST2_GPIO(); \
    } \
    else \
    { \
        DEASSERT_TEST2_GPIO(); \
    } \
    flip = !flip; \
}

typedef unsigned system_time_t;

/* Variable implemented in main.c */
extern volatile system_time_t g_SystemUpTime_ms;

#define GET_SYSTEM_UPTIME_MS() g_SystemUpTime_ms
#define GET_MILLI_SINCE(x) (GET_SYSTEM_UPTIME_MS() - (x))


/************** STM32 specific stuff **************/


#if 1
#ifdef __GNUC__
#define __ram_function__ __attribute__((section(".ramfunc")))
#define __ram_function_called_from_flash__ __attribute__((section(".ramfunc"))) \
   __attribute__((__long_call__))
#endif
#else
#define __ram_function__
#define __ram_function_called_from_flash__
#endif

#if defined(__GNUC__)
#define PLACE_IN_SECTION(name) __attribute__ ((section(#name)))
#elif defined(__ICCARM__)
#define PLACE_IN_SECTION(name) @ #name
#else
#define PLACE_IN_SECTION(name)
#endif

#endif //_HWMACROS_H_
