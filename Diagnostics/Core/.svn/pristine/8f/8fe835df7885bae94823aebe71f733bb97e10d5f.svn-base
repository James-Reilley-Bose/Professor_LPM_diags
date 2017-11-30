/*
    File    :   TIMx.c
    Title   :
    Author  :   Dylan Reid
    Created :   12-7-7
    Language:   C
    Copyright:  (C) 2007 Bose Corporation, Framingham, MA

    Description:  TIMx peripheral defines and macros for STM32

===============================================================================
*/
#ifndef TIMX_H
#define TIMX_H

//BasicTIMx.h
/*===========================================================================*/
//TIMER Defines
/* TIM Counter value */
#define TIM_COUNTER_Start          0x0001
#define TIM_COUNTER_Stop           0xFFFE

#define TIM_COUNTER_Update_Wrap    0x0004

/* TIM Input/Output Compare mode Enable value */
#define TIM_CC1_Enable             0x0001
#define TIM_CC2_Enable             0x0010
#define TIM_CC3_Enable             0x0100
#define TIM_CC4_Enable             0x1000

#define TIM_CC1_Disable            (~TIM_CC1_Enable)
#define TIM_CC2_Disable            (~TIM_CC2_Enable)
#define TIM_CC3_Disable            (~TIM_CC3_Enable)
#define TIM_CC4_Disable            (~TIM_CC4_Enable)

/* TIM Output Compare Polarity Set/Reset value */
#define TIM_CC1P_Set               0x0002
#define TIM_CC2P_Set               0x0020
#define TIM_CC3P_Set               0x0200
#define TIM_CC4P_Set               0x2000

#define TIM_CC1P_Reset             (~TIM_CC1P_Set)
#define TIM_CC2P_Reset             (~TIM_CC2P_Set)
#define TIM_CC3P_Reset             (~TIM_CC3P_Set)
#define TIM_CC4P_Reset             (~TIM_CC4P_Set)

/* TIM Trigger Selection value */
#define TIM_Trigger_Clear_Mask     0xFF8F
#define TIM_Trigger_TI1FP1         0x0050
#define TIM_Trigger_TI2FP2         0x0060
#define TIM_SlaveDisable_Mask      0xFFF0
#define TIM_Slave_ResetMode        0x0004

/* TIM Capture Compare mode Mask value */
#define TIM_CC13_Mask              0xFF00
#define TIM_CC24_Mask              0x00FF

/* TIM Capture Compare selection values */
#define TIM_OC_Select         0x0000
#define TIM_ICTI13_Select     0x0001
#define TIM_ICTI24_Select     0x0002

/* TIM Output Compare control mode constant */
#define TIM_OCMODE_Frozen        0x0000
#define TIM_OCMODE_SetHigh       0x0010
#define TIM_OCMODE_SetLow        0x0020
#define TIM_OCMODE_Toggle        0x0030
#define TIM_OCMODE_ForceLow      0x0040
#define TIM_OCMODE_ForceHigh     0x0050
#define TIM_OCMODE_PWM1          0x0060
#define TIM_OCMODE_PWM2          0x0070

/* TIM Capture Compare ISR Config */
#define TIM_ISR_Update             0x0001
#define TIM_ISR_CC1                0x0002
#define TIM_ISR_CC2                0x0004
#define TIM_ISR_CC3                0x0005
#define TIM_ISR_CC4                0x0010

#define TIM_ISR_Update_Disable     (~TIM_ISR_UIF)
#define TIM_ISR_CC1_Disable        (~TIM_ISR_CC1)
#define TIM_ISR_CC2_Disable        (~TIM_ISR_CC2)
#define TIM_ISR_CC3_Disable        (~TIM_ISR_CC3)
#define TIM_ISR_CC4_Disable        (~TIM_ISR_CC4)

#define BASIC_TIM_CR1_ADDR(base)   ((unsigned)base)
#define BASIC_TIM_CR2_ADDR(base)   ((unsigned)base + 0x04)
#define BASIC_TIM_DIER_ADDR(base)  ((unsigned)base + 0x0c)
#define BASIC_TIM_SR_ADDR(base)    ((unsigned)base + 0x10)
#define BASIC_TIM_EGR_ADDR(base)   ((unsigned)base + 0x14)

#define BASIC_TIM_GET_CNT(tim)   ((TIM_TypeDef*)tim)->CNT
#define BASIC_TIM_GET_PSC(tim)   ((TIM_TypeDef*)tim)->PSC
#define BASIC_TIM_GET_ARR(tim)   ((TIM_TypeDef*)tim)->ARR

#define BASIC_TIM_SET_CNT(tim, x)   ((TIM_TypeDef*)tim)->CNT = x
#define BASIC_TIM_SET_PSC(tim, x)   ((TIM_TypeDef*)tim)->PSC = x
#define BASIC_TIM_SET_ARR(tim, x)   ((TIM_TypeDef*)tim)->ARR = x

#define BASIC_TIM_FORCE_UPDATE_EVENT(tim) ((TIM_TypeDef*)tim)->EGR |= 0x01

#define BASIC_TIM_CLEAR_CNT(tim) ((TIM_TypeDef*)tim)->CNT &= 0x0;
#define BASIC_TIM_SET_PRESCALAR(tim,val) ((TIM_TypeDef*)tim)->PSC = val
#define BASIC_TIM_SET_AUTO_RELOAD(tim,val) ((TIM_TypeDef*)tim)->ARR = val

//Bit Band Set/Clears MACROS

//CR1
#define BASIC_TIM_ENABLE(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_CR1_ADDR(tim),0)

#define BASIC_TIM_DISABLE(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_CR1_ADDR(tim),0x00)

#define BASIC_TIM_ENABLE_UPDATE_EVENT(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_CR1_ADDR(tim),0x01)

#define BASIC_TIM_DISABLE_UPDATE_EVENT(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_CR1_ADDR(tim),0x01)

#define BASIC_TIM_GEN_UPDATE_ANY_SOURCE(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_CR1_ADDR(tim),0x02)

#define BASIC_TIM_GEN_UPDATE_UNDER_OVER_ONLY(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_CR1_ADDR(tim),0x02)

#define BASIC_TIM_ENABLE_ONE_PULSE(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_CR1_ADDR(tim),0x03)

#define BASIC_TIM_DISABLE_ONE_PULSE(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_CR1_ADDR(tim),0x03)

#define BASIC_TIM_ENABLE_AUTO_RELOAD_BUFFER(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_CR1_ADDR(tim),0x100)

#define BASIC_TIM_DISABLE_AUTO_RELOAD_BUFFER(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_CR1_ADDR(tim),0x100)

//CR2
#define BASIC_TIM_SET_MASTER_MODE(tim, mode) ((TIM_TypeDef*)tim)->CR2 &= 0xff80; \
                                             ((TIM_TypeDef*)tim)->CR2 &= mode

//DIER
#define BASIC_TIM_ENABLE_UPDATE_INTERUPT(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_DIER_ADDR(tim),0)

#define BASIC_TIM_DISABLE_UPDATE_INTERUPT(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_DIER_ADDR(tim),0)

#define BASIC_TIM_ENABLE_UPDATE_DMA(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_DIER_ADDR(tim),0x08)

#define BASIC_TIM_DISABLE_UPDATE_DMA(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_DIER_ADDR(tim),0x08)

#define BASIC_TIM_ENABLE_CC_DMA(tim,channel) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_DIER_ADDR(tim),channel + 8)

#define BASIC_TIM_DISABLE_CC_DMA(tim,channel) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_DIER_ADDR(tim),channel + 8)

//SR
#define BASIC_TIM_CHECK_UPDATE_INT(tim) (((TIM_TypeDef*)tim)->SR & 0x01)

#define BASIC_TIM_CLEAR_UPDATE_INT(tim) \
   PERIPH_BIT_BAND_CLEAR(BASIC_TIM_SR_ADDR(tim),0)

//EGR
#define BASIC_TIM_GENERATE_UPDATE_EVENT(tim) \
   PERIPH_BIT_BAND_SET(BASIC_TIM_EGR_ADDR(tim),0)


//TIMx.h
/*===========================================================================*/

#define TIM_CR1_ADDR(base)           BASIC_TIM_CR1_ADDR(base)
#define TIM_CR2_ADDR(base)           BASIC_TIM_CR2_ADDR(base)
#define TIM_DIER_ADDR(base)          BASIC_TIM_DIER_ADDR(base)
#define TIM_SR_ADDR(base)            BASIC_TIM_SR_ADDR(base)
#define TIM_EGR_ADDR(base)           BASIC_TIM_EGR_ADDR(base)
#define TIM_CCER_ADDR(base)          ((unsigned)base + 0x20)
#define TIM_CCR_ADDR(base,cc_num)   ((unsigned)base + 0x34 + ((cc_num -1)*0x04))

#define TIM_GET_CNT(tim)             BASIC_TIM_GET_CNT(tim)
#define TIM_GET_PSC(tim)             BASIC_TIM_GET_PSC(tim)
#define TIM_GET_ARR(tim)             BASIC_TIM_GET_ARR(tim)

#define TIM_SET_CNT(tim, x)          BASIC_TIM_SET_CNT(tim, x)
#define TIM_SET_PSC(tim, x)          BASIC_TIM_SET_PSC(tim, x)
#define TIM_SET_ARR(tim, x)          BASIC_TIM_SET_ARR(tim, x)

#define TIM_CLEAR_CNT(tim)           BASIC_TIM_CLEAR_CNT(tim)
#define TIM_SET_PRESCALAR(tim,val)   BASIC_TIM_SET_PRESCALAR(tim,val)
#define TIM_SET_AUTO_RELOAD(tim,val) BASIC_TIM_SET_AUTO_RELOAD(tim,val)

#define TIM_SET_COMPARE_REG(tim, ccr, x) ((TIM_TypeDef*)tim)->ccr = x
#define TIM_GET_CAPTURE_REG(tim, ccr)    ((TIM_TypeDef*)tim)->ccr
#define TIM_INC_CAPTURE_REG(tim, ccr, x) ((TIM_TypeDef*)tim)->ccr += x

#define TIM_DIRECT_SET_COMPARE_REG(ccrAddr, x) ((*(uint32_t*)ccrAddr) = x)
#define TIM_DIRECT_GET_CAPTURE_REG(ccrAddr)    ((*(uint32_t*)ccrAddr))
#define TIM_DIRECT_INC_CAPTURE_REG(ccrAddr, x) ((*(uint32_t*)ccrAddr += x))

#define TIM_SET_CAPTURE_REG(tim, ccr, x) TIM_SET_COMPARE_REG(tim, ccr, x)
#define TIM_INC_COMPARE_REG(tim, ccr, x) TIM_INC_CAPTURE_REG(tim, ccr, x)

//CR1
#define TIM_ENABLE(tim)                     BASIC_TIM_ENABLE(tim)
#define TIM_DISABLE(tim)                    BASIC_TIM_DISABLE(tim)
#define TIM_ENABLE_UPDATE_EVENT(tim)        BASIC_TIM_ENABLE_UPDATE_EVENT(tim)
#define TIM_DISABLE_UPDATE_EVENT(tim)       BASIC_TIM_DISABLE_UPDATE_EVENT(tim)
#define TIM_GEN_UPDATE_ANY_SOURCE(tim)      BASIC_TIM_GEN_UPDATE_ANY_SOURCE(tim)
#define TIM_GEN_UPDATE_UNDER_OVER_ONLY(tim) BASIC_TIM_GEN_UPDATE_UNDER_OVER_ONLY(tim)
#define TIM_ENABLE_ONE_PULSE(tim)           BASIC_TIM_ENABLE_ONE_PULSE(tim)
#define TIM_DISABLE_ONE_PULSE(tim)          BASIC_TIM_DISABLE_ONE_PULSE(tim)

#define TIM_COUNT_UP(tim) \
   PERIPH_BIT_BAND_CLEAR(TIM_CR1_ADDR(tim), 0x04)

#define TIM_COUNT_DOWN(tim) \
   PERIPH_BIT_BAND_SET(TIM_CR1_ADDR(tim), 0x04)

#define TIM_ENABLE_AUTO_RELOAD_BUFFER(tim)  BASIC_TIM_ENABLE_AUTO_RELOAD_BUFFER(tim)
#define TIM_DISABLE_AUTO_RELOAD_BUFFER(tim) BASIC_TIM_DISABLE_AUTO_RELOAD_BUFFER(tim)

//CR2
#define TIM_SET_MASTER_MODE(tim, mode)      BASIC_TIM_SET_MASTER_MODE(tim, mode)

//SMCR
#define TIM_DISABLE_SLAVE_MODE(tim)         ((TIM_TypeDef*)tim)->SMCR &= 0xFFF8
#define TIM_SET_SLAVE_MODE(tim, mode)       TIM_DISABLE_SLAVE_MODE(tim); \
                                            ((TIM_TypeDef*)tim)->SMCR |= mode

#define TIM_SET_TRIGGER_SELECTION(tim,trig) ((TIM_TypeDef*)tim)->SMCR &= TIM_Trigger_Clear_Mask; \
                                            ((TIM_TypeDef*)tim)->SMCR |= trig

//DIER
#define TIM_ENABLE_UPDATE_INTERUPT(tim)  BASIC_TIM_ENABLE_UPDATE_INTERUPT(tim)
#define TIM_DISABLE_UPDATE_INTERUPT(tim) BASIC_TIM_DISABLE_UPDATE_INTERUPT(tim)
#define TIM_ENABLE_UPDATE_DMA(tim)       BASIC_TIM_ENABLE_UPDATE_DMA(tim)
#define TIM_DISABLE_UPDATE_DMA(tim)      BASIC_TIM_DISABLE_UPDATE_DMA(tim)
#define TIM_ENABLE_CC_DMA(tim,channel)   BASIC_TIM_ENABLE_CC_DMA(tim,channel)
#define TIM_DISABLE_CC_DMA(tim,channel)  BASIC_TIM_DISABLE_CC_DMA(tim,channel)

#define TIM_ENABLE_CAPTURE_COMPARE_INTERUPT(tim, cc)  \
   PERIPH_BIT_BAND_SET(TIM_DIER_ADDR(tim), cc)

#define TIM_DISABLE_CAPTURE_COMPARE_INTERUPT(tim, cc) \
   PERIPH_BIT_BAND_CLEAR(TIM_DIER_ADDR(tim), cc)

#define TIM_CHECK_CC_INT_ENABLED(tim, cc)  (((TIM_TypeDef*)tim)->DIER & (0x02 << (cc-1)))
#define TIM_CHECK_UPDATE_INT_ENABLED(tim)  (((TIM_TypeDef*)tim)->DIER & 0x01)

//SR
#define TIM_CHECK_UPDATE_INT(tim)        BASIC_TIM_CHECK_UPDATE_INT(tim)
#define TIM_CLEAR_UPDATE_INT(tim)        BASIC_TIM_CLEAR_UPDATE_INT(tim)

#define TIM_CHECK_COMPARE_INT(tim, cc) (((TIM_TypeDef*)tim)->SR & (0x02 << (cc-1)))

#define TIM_CLEAR_COMPARE_INT(tim, cc)                \
   PERIPH_BIT_BAND_CLEAR(TIM_SR_ADDR(tim), cc)

//CCMR1
#define TIM_CC12_CLEAR_CONFIG(tim, cc)       ((TIM_TypeDef*)tim)->CCMR1 &= ~(0x00FF << ((cc - 1) * 8))

#define TIM_CC12_SELECTION(tim, cc, sel)     ((TIM_TypeDef*)tim)->CCMR1 |= (sel << ((cc - 1) * 8))

#define TIM_CC12_OUTPUT_MODE(tim, cc, mode)  TIM_CC12_CLEAR_CONFIG(tim,cc); \
                                             ((TIM_TypeDef*)tim)->CCMR1 |= (mode << ((cc - 1) *8))

#define TIM_CC12_INPUT_MODE(tim, cc, input)  TIM_CC12_CLEAR_CONFIG(tim,cc);         \
                                             TIM_CC12_SELECTION(tim, cc, input)

#define TIM_STANDARD_INPUT_PAD  0x01
#define TIM_ALTERNATE_INPUT_PAD 0x02

//CCMR2
#define TIM_CC34_CLEAR_CONFIG(tim, cc)        ((TIM_TypeDef*)tim)->CCMR2 &= ~(0x00FF << (((cc - 1)%2) * 8))

#define TIM_CC34_SELECTION(tim, cc, sel)     ((TIM_TypeDef*)tim)->CCMR2 &= ~(0x03 << (((cc - 1)%2) * 8)); \
                                             ((TIM_TypeDef*)tim)->CCMR2 |= (sel << (((cc - 1)%2) * 8))

#define TIM_CC34_OUTPUT_MODE(tim, cc, mode)  TIM_CC34_CLEAR_CONFIG(tim,cc);          \
                                             ((TIM_TypeDef*)tim)->CCMR2 |= (mode << (((cc - 1)%2) *8))

#define TIM_CC34_INPUT_MODE(tim, cc, input)  TIM_CC34_CLEAR_CONFIG(tim,cc);         \
                                             TIM_CC34_SELECTION(tim, cc, input)

//CCER*
#define TIM_ENABLE_CAPTURE_COMPARE(tim, cc)                     \
   PERIPH_BIT_BAND_SET(TIM_CCER_ADDR(tim), ((cc-1)*4))
#define TIM_DISABLE_CAPTURE_COMPARE(tim, cc)                    \
   PERIPH_BIT_BAND_CLEAR(TIM_CCER_ADDR(tim), ((cc-1)*4))
#define TIM_SET_CAPTURE_COMPARE_POLARITY_LOW(tim, cc)          \
   PERIPH_BIT_BAND_SET(TIM_CCER_ADDR(tim), (((cc-1)*4)+1))
#define TIM_SET_CAPTURE_COMPARE_POLARITY_HIGH(tim, cc)         \
   PERIPH_BIT_BAND_CLEAR(TIM_CCER_ADDR(tim), (((cc-1)*4)+1))


#define TIM_CC_OUTPUT_MODE(tim,cc, mode)           \
{                                                  \
   if(cc < 3)                                      \
   {                                               \
      TIM_CC12_OUTPUT_MODE(tim,cc,mode);           \
   }                                               \
   else                                            \
   {                                               \
      TIM_CC34_OUTPUT_MODE(tim,cc,mode);           \
   }                                               \
}                                                  \

#define TIM_CC_INPUT_MODE(tim,cc, mode)            \
{                                                  \
   if(cc < 3)                                      \
   {                                               \
      TIM_CC12_INPUT_MODE(tim,cc,mode);            \
   }                                               \
   else                                            \
   {                                               \
      TIM_CC34_INPUT_MODE(tim,cc,mode);            \
   }                                               \
}                                                  \

#endif // TIMX_H
