//
//--WiredIDUtilities.c: Utility functions used by both console and bassbox 
//
#include "project.h"
#include "WiredIDUtilities.h"
#include "WiredIDVariantUtilities.h"
#include "WiredIDTask.h"
#include "WiredIDInterrupt.h"

static BOOL       s_pulse_enabled = FALSE;
static uint16_t   s_pulse_hi_time_ms = 0;
static uint16_t   s_pulse_low_time_ms = 0;

SCRIBE_DECL(wired_id);

/*
 * @func WiredIDUtils_InitTimer
 * @brief Init HW timer for ID
 */ 
void WiredIDUtils_InitTimer(uint16_t period)
{
    /*
    1. Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
    2. Fill the TIM_TimeBaseInitStruct with the desired parameters.
    3. Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
    with the corresponding configuration
    4. Enable the NVIC if you need to generate the update interrupt.
    5. Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update)
    6. Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
    */
    //1. Done in System_ConfigClock
    //2.
    TIM_TimeBaseInitTypeDef timerInitStruct;
    timerInitStruct.TIM_Prescaler = APB1_TIMER_PERIPHERAL_CLK/1000 - 1; //0~65535
    timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStruct.TIM_Period = period - 1 ;
    timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStruct.TIM_RepetitionCounter = 0;
    //3.
    TIM_TimeBaseInit(ID_TIM, &timerInitStruct);
    //4.
    Interrupt_RegisterISR(ID_TIM_IRQn, WiredID_HW_Timer_IRQHandler);    
    TIM_ClearFlag(ID_TIM, TIM_FLAG_Update);
    //5.
    TIM_ITConfig(ID_TIM, TIM_IT_Update, ENABLE);
}

/* @func WiredIDUtils_StartIDPulse
 * 
 * @brief: Console or Bassbox start sending ID pulse. 
 * @param: high_time - high level duration
          low_time  - low level duration
          level     - starting level, 1 or 0
 */ 
void WiredIDUtils_StartIDPulse(uint16_t high_time, uint16_t low_time, uint8_t level)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "StartIDPulse, high %d, low %d, level %d",\
        high_time, low_time, level);
        
    uint16_t duration;
    
    s_pulse_hi_time_ms = high_time - 1;
    s_pulse_low_time_ms = low_time - 1;
    
    if (level)
    {
        duration = high_time;
    }
    else
    {
        duration = low_time;
    }
    WiredIDVariantUtils_EnablePulseIO();
    WiredIDVariantUtils_SetOutputPulseLevel(level);       
    TIM_SetCounter(ID_TIM, 0);
    TIM_SetAutoreload(ID_TIM, TIMER_MSEC_TO_TICKS(duration));    
    TIM_Cmd(ID_TIM, ENABLE);
    Interrupt_Enable(ID_TIM_IRQn,configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
    s_pulse_enabled = TRUE;
}

/*
 * @func  WiredIDUtils_StopPulse
 * @brief Stop sending ID pulses
 */
 
void WiredIDUtils_StopIDPulse(void)
{
    Interrupt_Disable(ID_TIM_IRQn);
    TIM_Cmd(ID_TIM, DISABLE);
    TIM_SetCounter(ID_TIM, 0);
    WiredIDVariantUtils_DisablePulseIO();    
    s_pulse_enabled = FALSE;
}

/*
 * @func  WiredIDUtils_IsPulseEnabled
 * @brief return whether ID pulse is enabled
 */
BOOL WiredIDUtils_IsPulseEnabled(void)
{
    return s_pulse_enabled;
}

/* @func WiredIDUtils_GetHiTime
 * @brief return return high level duration
 */
uint16_t WiredIDUtils_GetHiTime(void)
{
     return s_pulse_hi_time_ms;
}

/* @func WiredIDUtils_GetLowTime
 * @brief return low-level duration
 */
uint16_t WiredIDUtils_GetLowTime(void)
{
    return s_pulse_low_time_ms;
}

/*
 * @func WiredIDUtils_EnablePulseIO
 * @brief Enable IO for ID, mainly for PBB PWM_TX
 */
void WiredIDUtils_EnablePulseIO(void)
{
    WiredIDVariantUtils_EnablePulseIO();
}

/*
 * @func WiredIDUtils_DisablePulseIO
 * @brief: Disable GPIO for ID, for PBB
 */
void WiredIDUtils_DisablePulseIO(void)
{
    WiredIDVariantUtils_DisablePulseIO();
}

/* 
 * @brief: Functions to change output pulse to high or low
 */
 
void WiredIDUtils_SetOutputPulseLevel(uint8_t val)
{
    WiredIDVariantUtils_SetOutputPulseLevel(val);
}

uint8_t WiredIDUtils_GetOutputPulseLevel(void)
{   
    return WiredIDVariantUtils_GetOutputPulseLevel();
}

uint8_t WiredIDUtils_GetInputPulseLevel(void)
{
    return WiredIDVariantUtils_GetInputPulseLevel();
}



