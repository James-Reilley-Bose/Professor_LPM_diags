/*
 * @file    PWMTimer.c
 * @author  sl930193
 * @brief   PWM timer is used to control the LED intensity
 */

#include "defs.h"
#include "stm32f2xx_tim.h"
#include "project.h"
#include "PWMTimerConfig.h"

typedef struct
{
    TIM_TypeDef* timerNum;
    BOOL IsAPB1Peripheral;
} timerClock;

static const timerClock TIMER_CLOCK_TBL [] =
{
    {TIM1,  FALSE   },
    {TIM2,  TRUE    },
    {TIM3,  TRUE    },
    {TIM4,  TRUE    },
    {TIM5,  TRUE    },
    {TIM8,  FALSE   },
    {TIM9,  FALSE   },
    {TIM10, FALSE   },
    {TIM11, FALSE   },
    {TIM12, TRUE    },
    {TIM13, TRUE    },
    {TIM14, TRUE    },
};

static const uint8_t timerClockTblSize = sizeof(TIMER_CLOCK_TBL) / sizeof(TIMER_CLOCK_TBL[0]);

///////////////////////////////////////////////////////////////////////////////
//
// @func    prescalerGet
// @brief   compute the prescaler value
// @param   timerNum - timer number
// @param   cntClk - counter clock frequency
// @return  prescaler value
//
//////////////////////////////////////////////////////////////////////////////
uint16_t prescalerGet ( TIM_TypeDef* timerNum, uint32_t cntClk )
{
    /* ---------------------------------------------------------------------------

      For TIM2~7 and TIM12~14, the input clock (TIMxCLK) is set to 2 * APB1 clock (PCLK1),
      since APB1 prescaler is different from 1.
        TIMxCLK = 2 * PCLK1
        PCLK1 = HCLK / 4
        => TIMxCLK = HCLK / 2 = SystemCoreClock /2

      The prescaler is computed as follows:
         Prescaler = (TIMxCLK / TIMx counter clock) - 1
         Prescaler = ((SystemCoreClock /2) /TIMx counter clock) - 1

    ------------------------------------------------------------------------------

      For TIM1, TIM8 and TIM9~11, the input clock (TIMxCLK) is set to 2 * APB2 clock (PCLK2),
      since APB2 prescaler is different from 1.
        TIMxCLK = 2 * PCLK2
        PCLK2 = HCLK / 2
        => TIMxCLK = HCLK = SystemCoreClock

      The prescaler is computed as follows:
         Prescaler = (TIMxCLK / TIMx counter clock) - 1
         Prescaler = (SystemCoreClock /TIMx counter clock) - 1

    --------------------------------------------------------------------------- */
    uint8_t i;
    uint16_t PrescalerValue = 0;

    for (i = 0; i < timerClockTblSize; i++)
    {
        /* Compute the prescaler value */
        if (timerNum == TIMER_CLOCK_TBL[i].timerNum)
        {
            if (TIMER_CLOCK_TBL[i].IsAPB1Peripheral == TRUE)
            {
                PrescalerValue = (uint16_t)(APB1_TIMER_PERIPHERAL_CLK / cntClk) - 1;
            }
            else
            {
                PrescalerValue = (uint16_t)(APB2_TIMER_PERIPHERAL_CLK / cntClk) - 1;
            }
            break;
        }
    }

    return PrescalerValue;
}

///////////////////////////////////////////////////////////////////////////////
//
// @func    initPWMTimer
// @brief   Configure the PWM Timer
// @param   PWMtimerId - PWM timer ID
// @return  n/a
//
//////////////////////////////////////////////////////////////////////////////
void initPWMTimer ( PWMTimerId_t PWMtimerId )
{
    switch (PWM_TIMER_TBL[PWMtimerId].timerChannel)
    {
        case TIM_CH1:
            TIM_OC1PreloadConfig(PWM_TIMER_TBL[PWMtimerId].timerNum, PWM_OCPreloadConfig[PWMtimerId]);
            break;
        case TIM_CH2:
            TIM_OC2PreloadConfig(PWM_TIMER_TBL[PWMtimerId].timerNum, PWM_OCPreloadConfig[PWMtimerId]);
            break;
        case TIM_CH3:
            TIM_OC3PreloadConfig(PWM_TIMER_TBL[PWMtimerId].timerNum, PWM_OCPreloadConfig[PWMtimerId]);
            break;
        case TIM_CH4:
            TIM_OC4PreloadConfig(PWM_TIMER_TBL[PWMtimerId].timerNum, PWM_OCPreloadConfig[PWMtimerId]);
            break;
        default:
            return;
    }

    TIM_ARRPreloadConfig(PWM_TIMER_TBL[PWMtimerId].timerNum, PWM_ARRPreloadConfig[PWMtimerId]);
}

///////////////////////////////////////////////////////////////////////////////
//
// @func    startPWMTimer
// @brief   Enable the PWM Timer
// @param   PWMtimerId - PWM timer ID
// @param   dutyCycle - duty cycle in %
// @param   outputFreq - PWM output frequency in Hz
// @return  n/a
//
//////////////////////////////////////////////////////////////////////////////
void startPWMTimer ( PWMTimerId_t PWMtimerId, uint32_t dutyCycle, uint32_t outputFreq )
{
    /* ---------------------------------------------------------------------------
      The auto-reload value : TIMx_ARR
      The capture/compare value : TIMx_CCRx
         TIMx_ARR = (TIMx counter clock / TIMx output clock) - 1
         duty cycle = (TIMx_CCRx/ TIMx_ARR)* 100
         TIMx_CCRx = duty cycle * TIMx_ARR / 100
         TIMx_CCRx = duty cycle * (TIMx counter clock / TIMx output clock - 1) / 100

      Note:
       SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
       Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
       function to update SystemCoreClock variable value. Otherwise, any configuration
       based on this variable will be incorrect.
    --------------------------------------------------------------------------- */

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    TIM_TimeBaseStructure = PWM_TimeBaseStructure[PWMtimerId];
    TIM_TimeBaseStructure.TIM_Prescaler = prescalerGet(PWM_TIMER_TBL[PWMtimerId].timerNum, PWM_TIMER_TBL[PWMtimerId].cntClk);
    TIM_TimeBaseStructure.TIM_Period = PWM_TIMER_TBL[PWMtimerId].cntClk / outputFreq - 1;
    TIM_TimeBaseInit(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCInitStructure = PWM_OCInitStructure[PWMtimerId];
    TIM_OCInitStructure.TIM_Pulse = dutyCycle * (PWM_TIMER_TBL[PWMtimerId].cntClk / outputFreq - 1) / 100;

    switch (PWM_TIMER_TBL[PWMtimerId].timerChannel)
    {
        case TIM_CH1:
            TIM_OC1Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        case TIM_CH2:
            TIM_OC2Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        case TIM_CH3:
            TIM_OC3Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        case TIM_CH4:
            TIM_OC4Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        default:
            return;
    }
    TIM_Cmd(PWM_TIMER_TBL[PWMtimerId].timerNum, ENABLE);

    if IS_TIM_LIST4_PERIPH(PWM_TIMER_TBL[PWMtimerId].timerNum)
    {
        /* TIM1 and TIM8 Main Output Enable */
        TIM_CtrlPWMOutputs(PWM_TIMER_TBL[PWMtimerId].timerNum, ENABLE);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// @func    stopPWMTimer
// @brief   stop the PWM output
// @param   PWMtimerId - PWM timer ID
// @return  n/a
//
//////////////////////////////////////////////////////////////////////////////
void stopPWMTimer ( PWMTimerId_t PWMtimerId )
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCInitStructure = PWM_OCInitStructure[PWMtimerId];
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;

    switch (PWM_TIMER_TBL[PWMtimerId].timerChannel)
    {
        case TIM_CH1:
            TIM_OC1Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        case TIM_CH2:
            TIM_OC2Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        case TIM_CH3:
            TIM_OC3Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        case TIM_CH4:
            TIM_OC4Init(PWM_TIMER_TBL[PWMtimerId].timerNum, &TIM_OCInitStructure);
            break;
        default:
            return;
    }
}
