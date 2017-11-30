/*
 * @file    PWMTimer.h
 * @author  sl930193
 * @brief   PWMTimer header
 */

#ifndef __PWMTIMER_h__
#define __PWMTIMER_h__

typedef enum
{
    TIM_CH1,
    TIM_CH2,
    TIM_CH3,
    TIM_CH4
} TimerChannel;

typedef struct
{
    TIM_TypeDef* timerNum;
    TimerChannel timerChannel;
    uint32_t cntClk;
} PWMTimerInfo;

typedef uint32_t PWMTimerId_t;

void initPWMTimer ( PWMTimerId_t PWMtimerId );
void startPWMTimer ( PWMTimerId_t PWMtimerId, uint32_t dutyCycle, uint32_t outputFreq );
void stopPWMTimer ( PWMTimerId_t PWMtimerId );

#endif
