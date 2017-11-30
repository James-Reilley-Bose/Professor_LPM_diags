/*
    File    :   RFTimer.c
    Title   :
    Author  :   jc45540
    Created :   02/16/2016
    Language:   C
    Copyright:  (C) 2016 Bose Corporation, Framingham, MA

    Description:   Bose RF Timer Implementation for Bardeen

===============================================================================
*/
// System
#include "project.h"
#include "SystemAnalysis.h"

// Application
#include "RFTask.h"
#include "RFTimer.h"

// Function prototypes
static void RFTimer_IRQHandler(void);

/*
 * @func RFProtocolStateMachine_ConfigureTimer, called once
 *
 * @brief Configure a timer for RF Protocol State Machine.
 *
 * @return n/a
 */
void RFTimer_Configure( void )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

    TIM_TimeBaseStructure.TIM_Prescaler = RFTIMER_PRESCALER - 1;
    TIM_TimeBaseStructure.TIM_Period = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;         // this is a zero
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit( CHIPCON_TASK_TIMER, &TIM_TimeBaseStructure );

    TIM_ITConfig( CHIPCON_TASK_TIMER, TIM_IT_Update, DISABLE );
    TIM_Cmd( CHIPCON_TASK_TIMER, DISABLE );

    /* Configure the Nested Interrupt Vector Controller */
    Interrupt_RegisterISR( CHIPCON_TASK_TIMERIRQ, &RFTimer_IRQHandler );
    Interrupt_Enable( CHIPCON_TASK_TIMERIRQ, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0 );
}


/*
 * @func RFProtocolStateMachine_StartTimer, called often
 *  For maximum speed, write to timer registers directly, not through the lib.
 *
 * @brief Start the timer for RF task.
 *
 * @return n/a
 */
void RFTimer_StartMS( uint32_t TimeInMilliseconds )
{
    RFTimer_StartUS( TimeInMilliseconds * 1000 );
}

/*
 * @func RFTask_StartTimer, called often
 *  For maximum speed, write to timer registers directly, not through the lib.
 *
 * @brief Start the timer for RF task.
 *
 * @return n/a
 */
void RFTimer_StartUS( uint32_t TimeInMicroseconds )
{
    /* Disable */
    CHIPCON_TASK_TIMER->DIER &= ( uint16_t )~TIM_FLAG_Update;
    CHIPCON_TASK_TIMER->CR1 &= ( uint16_t )~TIM_CR1_CEN;

    // Make sure the timer is off, no interupts generated until we are ready.
    // Put in the user time value and fire the puppy up, generating a
    // interrupt when the interval is completed.  The ISR will also stop
    // the timer.  This makes this timer a one shot, one interval delay.
    debug_assert ( RFTIMER_TO_ACTION_DELAY_OVERHEAD_IN_USEC < TimeInMicroseconds );
    TimeInMicroseconds -= RFTIMER_TO_ACTION_DELAY_OVERHEAD_IN_USEC;

    /* count up uses ARRL register. Set the count to 0 (setting it to the desired time will
    *  cause an immediate interrupt).
    */
    CHIPCON_TASK_TIMER->CNT = 0;
    CHIPCON_TASK_TIMER->ARR = TimeInMicroseconds;

    /* Clear Update Flag - it may have been set during the time it takes to restart the timer. If
    *  so, an immediate interrupt will happen when it is enabled.
    */
    CHIPCON_TASK_TIMER->SR = ( uint16_t )~TIM_FLAG_Update;

    /* Finally, enable interrupts and enable the timer */
    CHIPCON_TASK_TIMER->DIER |= TIM_FLAG_Update;
    CHIPCON_TASK_TIMER->CR1 |= TIM_CR1_CEN;
}

/*******************************************************************************
* Function Name  : RFTimer_IRQHandler
* Description    : This function handles RF Timer's interrupt request.
*
*******************************************************************************/
static void RFTimer_IRQHandler(void)
{
    INTERRUPT_RUN_LATENCY_BEGIN( CC_T );

    /* Clear interrupt and clear flag (same register and register bits as it turns out) */
    CHIPCON_TASK_TIMER->SR = ( uint16_t )~TIM_IT_Update;
    /* Disable */
    CHIPCON_TASK_TIMER->DIER &= ( uint16_t )~TIM_FLAG_Update;
    CHIPCON_TASK_TIMER->CR1 &= ( uint16_t )~TIM_CR1_CEN;

    RFPostMsg( RFPROTOCOLSTATEMACHINE_MESSAGE_ID_RFTimer, 0, 0 );

    INTERRUPT_RUN_LATENCY_END( CC_T, 0 );

    /* force an immediate RF Protocol State Machine task execution rather than waiting for the next
    *  RTOS tick.
    */
    portEND_SWITCHING_ISR( TRUE );
}


