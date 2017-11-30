#ifndef INSTRUMENT_H
#define INSTRUMENT_H

/* defines */
#define MEASURE_TIME_TICKS_PER_MICROSECOND 1

#define INSTR_PRESCALER_ON      (APB1_TIMER_PERIPHERAL_CLK/1000000) /* 1us resolution */

#if MEASURE_TIME_TICKS_PER_MICROSECOND != 1
#error "Make MEASURE_TIME_TICKS_PER_MICROSECOND 1 or fix below macros"
#endif

typedef uint32_t measure_time_t;

/* macros */
#define GET_MEASURE_TIME_NOW() GET_INSTRUMENT_TIME_NOW()
#define GET_INSTRUMENT_TIME_NOW() TIME_MEASURE_TIMER->CNT

#define DELTA_TIME( Start,Finish )                                      \
   ( (measure_time_t)((measure_time_t)Finish - (measure_time_t)Start) )
#define MEASURE_TIME_SINCE(LastTime) DELTA_TIME(LastTime, GET_MEASURE_TIME_NOW())

#define INSTRUMENT_DISABLE_TIMER() TIME_MEASURE_TIMER->CR1 &= TIM_COUNTER_Stop
#define INSTRUMENT_ENABLE_TIMER() TIME_MEASURE_TIMER->CR1 |= TIM_COUNTER_Start

/* function prototypes */
void Instrument_Init(void);
void Instrument_waitForMS(uint32_t mS);
void Delay_us( measure_time_t TimeToDelay );
void Delay_ms(measure_time_t mS);

#endif //INSTRAMENT_H
