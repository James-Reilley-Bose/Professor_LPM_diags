#include "product_config.h"
#include "stm32f2xx_tim.h"
#include "stm32f2xx_interrupts.h"
#include "EventDefinitions.h"
#include "project.h"
#include "nv_mfg.h"
#include "WiredIDTask.h"
#include "WiredIDInterrupt.h"
#ifdef SKIPPER
#include "WiredIDBassTask.h"
#include "WiredIDBassDisabled.h"
#include "WiredIDBassBase.h"
#endif

static BOOL edge_detect_interrupt_enabled = FALSE;
/* @func WiredID_ConfigEdgeDetectEXTI
 * @brief Configure GPIO pin to detect pulse from other end
 */
void WiredID_ConfigEdgeDetectEXTI(void)
{
    ConfigureGpioPin(WIRED_ID_INTERRUPT_BANK,
                     WIRED_ID_INTERRUPT_PIN,
                     GPIO_MODE_IN_FLOATING,
                     0);
    
    SYSCFG_EXTILineConfig(WIRED_ID_INTERRUPT_EXTI_PORT_SOURCE, WIRED_ID_INTERRUPT_PIN);
    
    WiredID_EnableEdgeDetectEXTI(FALSE);
    EXTI_ClearITPendingBit(WIRED_ID_INTERRUPT_LINE);
    
    Interrupt_RegisterISR(WIRED_ID_INTERRUPT_CHANNEL, WIRED_ID_INTERRUPT_HANDLER);    
}

/* @func WiredID_EnableEdgeDetectEXTI
 * @brief Enable/Disable interrupt for IO edge detection
 */
void WiredID_EnableEdgeDetectEXTI(BOOL enable)
{
    EXTI_InitTypeDef  EXTI_InitStruct;
    
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_LineCmd = enable? ENABLE:DISABLE;
    EXTI_InitStruct.EXTI_Line  = WIRED_ID_INTERRUPT_LINE;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
    
    EXTI_Init(&EXTI_InitStruct);
    if (enable)
    {
        Interrupt_Enable(WIRED_ID_INTERRUPT_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
        edge_detect_interrupt_enabled = TRUE;
    }
    else
    {
        Interrupt_Disable(WIRED_ID_INTERRUPT_CHANNEL);
        edge_detect_interrupt_enabled = FALSE;
    }    
}

BOOL WiredID_EdgeDetectInterruptEnabled(void)
{
    return edge_detect_interrupt_enabled;
}

/* @func WiredID_HW_Timer_IRQHandler
 * @brief ISR for HW timer user to generate ID pulse
 */
void  WiredID_HW_Timer_IRQHandler(void)
{    
    if (TIM_GetITStatus(ID_TIM, TIM_IT_Update) != RESET)
    {
        uint16_t new_level;
        uint16_t new_time;
        
        WiredIDPostMsg(WIREDID_PulseTimerOff, NOP_CALLBACK, NULL);        
        TIM_Cmd(ID_TIM, DISABLE);
        TIM_ClearITPendingBit(ID_TIM, TIM_IT_Update);
     
        if (WiredIDUtils_GetOutputPulseLevel() == 0)
        {
            new_level = 1;
            new_time = WiredIDUtils_GetHiTime();
        }
        else
        {
            new_level = 0;
            new_time = WiredIDUtils_GetLowTime();
        }
        WiredIDUtils_SetOutputPulseLevel(new_level);
        TIM_SetAutoreload(ID_TIM, TIMER_MSEC_TO_TICKS(new_time));
        TIM_Cmd(ID_TIM, ENABLE);
    }
}

        