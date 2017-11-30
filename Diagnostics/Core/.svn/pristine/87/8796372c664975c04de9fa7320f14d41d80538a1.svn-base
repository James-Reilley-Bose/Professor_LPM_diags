#include "project.h"
#include "TIMx.h"
#include "watchdog.h"

// init data

static TIM_TimeBaseInitTypeDef gInstTimOnInit =
{
    .TIM_Period = 0xFFFFFFFF,
    // From the timer section of the processor chip documentation :
    // The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).
    // Which means that the hardware adds one to what you put in it so
    // you need to subtract one to compensate
    .TIM_Prescaler = INSTR_PRESCALER_ON - 1,  // for 1 tick per uS;
    .TIM_ClockDivision = 0x0,
    .TIM_CounterMode = TIM_CounterMode_Up
};

void Instrument_Init(void)
{
    TIM_DeInit(TIME_MEASURE_TIMER);

    TIM_TimeBaseInit(TIME_MEASURE_TIMER, &gInstTimOnInit);

    TIM_Cmd(TIME_MEASURE_TIMER, ENABLE);

    //STM32 force update for prescalar to take effect
    BASIC_TIM_FORCE_UPDATE_EVENT(TIME_MEASURE_TIMER);

}

void Instrument_waitForMS(uint32_t mS)
{
    uint32_t count = 0;
    uint32_t then = GET_MEASURE_TIME_NOW();

    // wait for some time between mS and (mS + 1)
    mS = mS + 1;

    while (count < mS)
    {
        while (MEASURE_TIME_SINCE(then) < 1000);
        PAT_WATCHDOG();
        then = GET_MEASURE_TIME_NOW();
        count++;
    }

}


void Delay_us( measure_time_t TimeToDelay )
{
    measure_time_t start = GET_MEASURE_TIME_NOW();
    while ( MEASURE_TIME_SINCE(start) < TimeToDelay )
    {
        // spin
    }
}
//----------------------------------------------------------------------
// Function:  Delay_ms()
//
// Description: Uses TIM2 as a delay counter. The counter value
//              can runs up to 0xFFFFFFFF and then auto reloads.
//              Each count increment == 1uS.
//
// Parameters:
//              measure_time_t mS - delay amount in milliseconds
//
//----------------------------------------------------------------------
void Delay_ms(measure_time_t mS)
{
    Delay_us(mS * 1000);
}


