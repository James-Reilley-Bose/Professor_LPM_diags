#include "project.h"
#include "TIMx.h"
#include "ir.h"
#include "KeyData.h"
#include "SystemAnalysis.h"
#include "KeyHandlerTask.h"
#include "KeyHandler.h"
#include "KeyTimings.h"
#include <stdio.h>
#include "RivieraLPM_CompressedKeys.h"
#include "etap.h"

/* function prototypes */

/**** defines ****/
#define IR_EDGE_BUFFER_SIZE 8
#define TIMING_TOLERANCE 0.45f                   // 45% timing error is allowed
// times in uSecs
#define LEADER_POSITIVE_PULSE_WIDTH              9000
#define LEADER_POSITIVE_PULSE_WIDTH_MIN          7000
#define LEADER_POSITIVE_PULSE_WIDTH_MAX         11000
#define LEADER_NEGATIVE_PULSE_WIDTH              3000
#define LEADER_NEGATIVE_PULSE_WIDTH_MIN          1500
#define LEADER_NEGATIVE_PULSE_WIDTH_MAX          6000
#define BIT_PULSE_WIDTH                          560
#define BIT_PULSE_WIDTH_MIN                      400
#define BIT_PULSE_WIDTH_MAX                      720
#define ZERO_BIT_NEGATIVE_PULSE_WIDTH            565
#define ZERO_BIT_NEGATIVE_PULSE_WIDTH_MIN        400
#define ZERO_BIT_NEGATIVE_PULSE_WIDTH_MAX        725
#define ONE_BIT_NEGATIVE_PULSE_WIDTH           ( 2250 - BIT_PULSE_WIDTH )
#define ONE_BIT_NEGATIVE_PULSE_WIDTH_MIN        (unsigned)( ONE_BIT_NEGATIVE_PULSE_WIDTH * (1-TIMING_TOLERANCE) )
#define ONE_BIT_NEGATIVE_PULSE_WIDTH_MAX        (unsigned)( ONE_BIT_NEGATIVE_PULSE_WIDTH * (1+TIMING_TOLERANCE) )
#define ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH ( 2250 - BIT_PULSE_WIDTH )
#define ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH_MIN (unsigned)(ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH * (1-TIMING_TOLERANCE) )
#define ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH_MAX (unsigned)(ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH * (1*TIMING_TOLERANCE) )
#define POSITIVE_EDGE 0
#define NEGATIVE_EDGE 1
// max number of pulses we can learn
#define IR_LEARN_MAX_PULSES 512 // *2 Kb of buffer used based on this
#define IR_SEND_LEARNED_PULSES_BUF_LEN  4
// time in ms. The timeout for the shortest IR signal that 7200 expects is 200ms
// so excluding the duration of the signal, we should not be exceeding 100ms,
// otherwise 7200 will timeout and we don't need to block the task any longer.
#define IR_SEND_LEARNED_PULSES_MSG_TIMEOUT      100


// Wave times to have the compressed controller be compatable with the professor
// times are in us
#define IR_DECODER_COMPRESSED 1
#define IR_DECODER_COMPRESSED_LEADER_POSITIVE_PULSE_WIDTH          1000
#define IR_DECODER_COMPRESSED_LEADER_POSITIVE_PULSE_WIDTH_MIN      778
#define IR_DECODER_COMPRESSED_LEADER_POSITIVE_PULSE_WIDTH_MAX      1222
#define IR_DECODER_COMPRESSED_LEADER_NEGATIVE_PULSE_WIDTH          1500
#define IR_DECODER_COMPRESSED_LEADER_NEGATIVE_PULSE_WIDTH_MIN      750
#define IR_DECODER_COMPRESSED_LEADER_NEGATIVE_PULSE_WIDTH_MAX      3000
#define IR_DECODER_COMPRESSED_BIT_PULSE_WIDTH                      500
#define IR_DECODER_COMPRESSED_BIT_PULSE_WIDTH_MIN                  357
#define IR_DECODER_COMPRESSED_BIT_PULSE_WIDTH_MAX                  643
#define IR_DECODER_COMPRESSED_ZERO_BIT_NEGATIVE_PULSE_WIDTH        505
#define IR_DECODER_COMPRESSED_ZERO_BIT_NEGATIVE_PULSE_WIDTH_MIN    357
#define IR_DECODER_COMPRESSED_ZERO_BIT_NEGATIVE_PULSE_WIDTH_MAX    647

#define IR_DECODER_COMPRESSED_ONE_BIT_NEGATIVE_PULSE_WIDTH           ( 2250 - IR_DECODER_COMPRESSED_BIT_PULSE_WIDTH )
#define IR_DECODER_COMPRESSED_ONE_BIT_NEGATIVE_PULSE_WIDTH_MIN        (unsigned)( IR_DECODER_COMPRESSED_ONE_BIT_NEGATIVE_PULSE_WIDTH * (1-TIMING_TOLERANCE) )
#define IR_DECODER_COMPRESSED_ONE_BIT_NEGATIVE_PULSE_WIDTH_MAX        (unsigned)( IR_DECODER_COMPRESSED_ONE_BIT_NEGATIVE_PULSE_WIDTH * (1+TIMING_TOLERANCE) )
#define IR_DECODER_COMPRESSED_ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH ( 2250 - IR_DECODER_COMPRESSED_BIT_PULSE_WIDTH )
#define IR_DECODER_COMPRESSED_ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH_MIN (unsigned)(IR_DECODER_COMPRESSED_ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH * (1-TIMING_TOLERANCE) )
#define IR_DECODER_COMPRESSED_ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH_MAX (unsigned)(IR_DECODER_COMPRESSED_ZERO_BIT_TO_CLOCK_NEGATIVE_PULSE_WIDTH * (1*TIMING_TOLERANCE) )

// macros for enabling & disabling both types of waves, later of the two is compressed
#define SUPPORT_IR_NEC_DECODE 1
#define SUPPORT_IR_BOSE_COMP_DECODE 1


typedef union
{
    uint32_t dword;
    IRKeyData keyData;
} IRLastKeyUnion;

static IRLastKeyUnion OutputValue;

static int BitCounter;

// This data type/variables are used for the Bose Compressed Wave

static uint16_t OutputValueCompressed;
static uint8_t KeyValueCompressed;
static unsigned int BitCounterComp;

// needed so we don't lose data in the wrong state machine
static BOOL IsCompressedWave = TRUE;

/**** macros ****/
#define PulseCorrect( Ticks, Polarity, MinimumTime, MaximumTime, DesiredPolarity ) \
   ((Polarity == DesiredPolarity) && (Ticks >= MinimumTime) && \
    (Ticks <= MaximumTime))


/* prototypes */
static void IR_RX_IRQHandler(void);

/* data */
uint32_t g_NumIrEdges = 0;
uint32_t g_LastIrKey = 0;
BOOL     g_EnableIrTaskLoopTimerPending = FALSE;
IR_RECEIVER_STATE IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
static ICEdgeType IrCurrentPulse;

uint16_t lastEdgeTime = 0;
uint16_t edgeTime = 0;

static BOOL handleIR = TRUE;


//STM32 Initialization structure
static const TIM_TimeBaseInitTypeDef gTimInit =
{
    .TIM_Prescaler = IR_RX_PRESCALER_ON - 1, // 1 us per tick
    .TIM_CounterMode = TIM_CounterMode_Up,
    .TIM_Period = 0xffff, // update event means we wrapped
    .TIM_ClockDivision = 0x0
};

CREATE_KEY_PRODUCER(kpIR, KEY_PRODUCER_IR, IR_HOLD_TIME, IR_RELEASE_TIME, IR_REPEAT_TIME, TIMER_ID_IRRelease, TIMER_ID_IRRepeat);

BOOL IR_GetAllowIr()
{
    return handleIR;
}

void IR_SetAllowIr(BOOL enabled)
{
    handleIR = enabled;
}

// check if Key matches KeyInv for Compressed Wave IR
BOOL IsCorrectValue( uint16_t hexcode)
{
    KeyValueCompressed = (hexcode & 0xff);
    uint8_t InverseByte = (hexcode & 0xff00) >> 8;

    if (KeyValueCompressed == (InverseByte ^ 0xff))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/**************************************************************************************
***
**************************************************************************************/
void
IR_Init(void)
{
    kpIR_init();
    ConfigureGpioPin(IR_RX_DATA_GPIO_BANK, IR_RX_DATA_GPIO_PIN, GPIO_MODE_AF_IN_FLOATING, IR_RX_DATA_GPIO_AF);

    TIM_DeInit(IR_RX_TIMER);
    TIM_TimeBaseInit(IR_RX_TIMER, (TIM_TimeBaseInitTypeDef*)&gTimInit);

    GPIO_PinAFConfig(IR_RX_DATA_GPIO_BANK, IR_RX_DATA_GPIO_PIN, IR_RX_DATA_GPIO_AF);

    TIM_GEN_UPDATE_UNDER_OVER_ONLY(IR_RX_TIMER); // we want to use the update int to detect overflow
    IR_EnableRx();

    Interrupt_RegisterISR(IR_RX_TIMER_INTERRUPT_CHANNEL, IR_RX_IRQHandler);
    Interrupt_Enable(IR_RX_TIMER_INTERRUPT_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);

    Interrupt_RegisterISR(IR_RX_OVERFLOW_INTERRUPT_CHANNEL, IR_RX_IRQHandler);
    Interrupt_Enable(IR_RX_OVERFLOW_INTERRUPT_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

/**************************************************************************************
***
**************************************************************************************/
void
IR_EnableRx(void)
{
    IR_RX_DISABLE_TIMER();
    IR_RX_INPUT_MODE(IR_RX_INPUT_SELECT);
    IR_RX_SELECT_FALLING_EDGE();
    IR_CLEAR_RX_INTERRUPTS();
    IR_CLEAR_UPDATE_INT();
    IR_ENABLE_RX_INTERRUPTS();
    TIM_SET_SLAVE_MODE(IR_RX_TIMER, TIM_Slave_ResetMode);
    //TODO: The trigger should be defined in Configure system.
    TIM_SET_TRIGGER_SELECTION(IR_RX_TIMER, IR_RX_TRIGGER_SOURCE);

    IR_RX_ENABLE_UPDATE_FLAG();
    IR_RX_ENABLE_TIMER();

    //Enable Timer
    TIM_ENABLE(IR_RX_TIMER);

    //STM32 force update for prescalar to take effect
    BASIC_TIM_FORCE_UPDATE_EVENT(IR_RX_TIMER);
}

/**************************************************************************************
***
**************************************************************************************/
void
IR_ProcessPulse(ICEdgeType* pulse)
{
    /***************begin  Caesar Ir Parse code *****************/
    /*
     * Run this overly complicated state machine to recognize the header, extract bits, etc.
     *
     * See the enumerated type at the beginning of the file for an enumeration of the states
     * in this state machine.
     */

    switch ( IR_ReceiverState )
    {
        /*
         * Waiting for the positive edge of the leader.  If we get it we move to the next state.
         * If we don't get a positive edge, we stay in this state until we do.
         */
        default: // JKDEBUG
            IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
        /*
         * Waiting for the end of the leader.  We know if this is really a leader
         * if the time between edges is 9 ms +/- 5%
         */
        case IR_WAITING_FOR_END_OF_LEADER:


            if ( PulseCorrect( pulse->PulseWidth,
                               pulse->PulsePolarity,
                               LEADER_POSITIVE_PULSE_WIDTH_MIN,
                               LEADER_POSITIVE_PULSE_WIDTH_MAX,
                               NEGATIVE_EDGE
                             )
               )
            {
                IR_ReceiverState = IR_WAITING_FOR_START_OF_FIRST_CLOCK_BIT;
            }
            else
            {
                IsCompressedWave = TRUE;
            }
            break;
        /*
         * Waiting for the rest of the leader to elapse.  If we get the correct pulse width,
         * move on to extract bits, else go back to look for a leader.
         */
        case IR_WAITING_FOR_START_OF_FIRST_CLOCK_BIT:

            if ( PulseCorrect( pulse->PulseWidth,
                               pulse->PulsePolarity,
                               LEADER_NEGATIVE_PULSE_WIDTH_MIN,
                               LEADER_NEGATIVE_PULSE_WIDTH_MAX,
                               POSITIVE_EDGE
                             )
               )
            {
                OutputValue.dword      = 0;
                BitCounter       = 32;
                IR_ReceiverState = IR_WAITING_FOR_END_OF_CLOCK_BIT;
            }
            else // This is NOT a leader ... keep looking!
            {
                IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
            }
            break;

        /*
         *  We have 2 cases to consider here: either we are going to get a 0 (which is
         *  indicated by a pulse between clock pulses) or we are going to get the start
         *  of another clock (which means we got a 1).
         *
         *  Determine which case we have and act accordingly ...
         */

        case IR_WAITING_FOR_START_OF_CLOCK_BIT:
            if ( PulseCorrect( pulse->PulseWidth,
                               pulse->PulsePolarity,
                               ZERO_BIT_NEGATIVE_PULSE_WIDTH_MIN,
                               ZERO_BIT_NEGATIVE_PULSE_WIDTH_MAX,
                               POSITIVE_EDGE
                             )
               )
            {
                /*
                 * Got a zero!
                 *
                 * Shift the zero into the output variable and decrement the bit count.  Next state
                 * will be the detection of the end of this bit.
                 */
                OutputValue.dword      <<= 1; // Shift in a zero
                BitCounter        -= 1;
                IR_ReceiverState = IR_WAITING_FOR_END_OF_CLOCK_BIT;
            }
            else if ( PulseCorrect( pulse->PulseWidth,
                                    pulse->PulsePolarity,
                                    ONE_BIT_NEGATIVE_PULSE_WIDTH_MIN,
                                    ONE_BIT_NEGATIVE_PULSE_WIDTH_MAX,
                                    POSITIVE_EDGE
                                  )
                    )
            {
                /*
                 * Got a 1!
                 *
                 * Shift the 1 into the output variable and decrement the bit count.  Next state
                 * will be the detection of the end of this bit.
                 */

                OutputValue.dword        = ( OutputValue.dword << 1 ) + 0x00000001; // Shift in a 1
                BitCounter        -= 1;
                IR_ReceiverState = IR_WAITING_FOR_END_OF_CLOCK_BIT;
            }
            else // Not a 0 or a 1 ... reset the whole state machine
            {
                IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
            }
            break;

        /*
         * Waiting for negative edge of the clock bit.
         *
         * This can mean 3 things:
         *
         * 1) The end of the first clock after leader.  Action: Wait for the next clock.
         * 2) The end of a 1 bit.  We know this if the bit counter != 0.  Action: just
         *      move on to the looking for the start of the next clock bit.
         * 3) The end of the stop bit clock.  We know this if the bit counter = 0.  Action:
         *      return the word to the caller and reset the state machine to look for the
         *      leader
         */

        case IR_WAITING_FOR_END_OF_CLOCK_BIT:
            if ( PulseCorrect( pulse->PulseWidth,
                               pulse->PulsePolarity,
                               BIT_PULSE_WIDTH_MIN,
                               BIT_PULSE_WIDTH_MAX,
                               NEGATIVE_EDGE
                             )
               )
            {
                if ( BitCounter)
                {
                    IR_ReceiverState = IR_WAITING_FOR_START_OF_CLOCK_BIT;
                }
                else
                {

                    //JTC This is just for testing out ABL

                    //RCSendSourceSelection(0xb, 0, 1, 1,0);

                    IR_HandleCompleteKey(&OutputValue.keyData);
                    IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
                    BitCounter = 0;
                }
            }
            else
            {
                // Unexpected edge ... start over!
                IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
                BitCounter = 0;
            }
            break;
        case IR_WAITING_TO_PROCESS_KEY:
            debug_assert(0);
            break;
    } // end of switch ( IR_ReceiverState )
}

// used to process a compressed pulse IR remote
void
IR_ProcessPulseCompressed(ICEdgeType* pulse)
{

    switch ( IR_ReceiverState )
    {
        /*
         * Waiting for the positive edge of the leader.  If we get it we move to the next state.
         * If we don't get a positive edge, we stay in this state until we do.
         */
        default: // JKDEBUG
            IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
        /*
         * Waiting for the end of the leader.  We know if this is really a leader
         * if the time between edges is 1 ms +/- 5%
         */
        case IR_WAITING_FOR_END_OF_LEADER:


            if ( PulseCorrect( pulse->PulseWidth,
                               pulse->PulsePolarity,
                               IR_DECODER_COMPRESSED_LEADER_POSITIVE_PULSE_WIDTH_MIN,
                               IR_DECODER_COMPRESSED_LEADER_POSITIVE_PULSE_WIDTH_MAX,
                               NEGATIVE_EDGE )
               )
            {
                IR_ReceiverState = IR_WAITING_FOR_START_OF_FIRST_CLOCK_BIT;
            }
            else
            {
                IsCompressedWave = FALSE;
            }
            break;
        /*
         * Waiting for the rest of the leader to elapse.  If we get the correct pulse width,
         * move on to extract bits, else go back to look for a leader.
         */
        case IR_WAITING_FOR_START_OF_FIRST_CLOCK_BIT:

            if (    PulseCorrect( pulse->PulseWidth,
                                  pulse->PulsePolarity,
                                  IR_DECODER_COMPRESSED_LEADER_NEGATIVE_PULSE_WIDTH_MIN,
                                  IR_DECODER_COMPRESSED_LEADER_NEGATIVE_PULSE_WIDTH_MAX,
                                  POSITIVE_EDGE
                                )
               )
            {
                OutputValueCompressed = 0;
                BitCounterComp = 16;
                IR_ReceiverState = IR_WAITING_FOR_END_OF_CLOCK_BIT;
            }
            else // This is NOT a leader ... keep looking!
            {
                IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
            }
            break;

        /*
         *  We have 2 cases to consider here: either we are going to get a 0 (which is
         *  indicated by a pulse between clock pulses) or we are going to get the start
         *  of another clock (which means we got a 1).
         *
         *  Determine which case we have and act accordingly ...
         */

        case IR_WAITING_FOR_START_OF_CLOCK_BIT:
            if ( PulseCorrect( pulse->PulseWidth,
                               pulse->PulsePolarity,
                               IR_DECODER_COMPRESSED_ZERO_BIT_NEGATIVE_PULSE_WIDTH_MIN,
                               IR_DECODER_COMPRESSED_ZERO_BIT_NEGATIVE_PULSE_WIDTH_MAX,
                               POSITIVE_EDGE
                             )
               )
            {
                /*
                 * Got a zero!
                 *
                 * Shift the zero into the output variable and decrement the bit count.  Next state
                 * will be the detection of the end of this bit.
                 */                
                OutputValueCompressed <<= 1;
                BitCounterComp -= 1;
                IR_ReceiverState = IR_WAITING_FOR_END_OF_CLOCK_BIT;
            }
            else if ( PulseCorrect( pulse->PulseWidth,
                                    pulse->PulsePolarity,
                                    IR_DECODER_COMPRESSED_ONE_BIT_NEGATIVE_PULSE_WIDTH_MIN,
                                    IR_DECODER_COMPRESSED_ONE_BIT_NEGATIVE_PULSE_WIDTH_MAX,
                                    POSITIVE_EDGE
                                  )
                    )
            {
                
                /*
                 * Got a 1!
                 *
                 * Shift the 1 into the output variable and decrement the bit count.  Next state
                 * will be the detection of the end of this bit.
                 */

                OutputValueCompressed = (OutputValueCompressed << 1 ) + 0x0001;
                          
                BitCounterComp -= 1;
                IR_ReceiverState = IR_WAITING_FOR_END_OF_CLOCK_BIT;
            }
            else // Not a 0 or a 1 ... reset the whole state machine
            {
                IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
            }
            break;

        /*
         * Waiting for negative edge of the clock bit.
         *
         * This can mean 3 things:
         *
         * 1) The end of the first clock after leader.  Action: Wait for the next clock.
         * 2) The end of a 1 bit.  We know this if the bit counter != 0.  Action: just
         *      move on to the looking for the start of the next clock bit.
         * 3) The end of the stop bit clock.  We know this if the bit counter = 0.  Action:
         *      return the word to the caller and reset the state machine to look for the
         *      leader
         */

        case IR_WAITING_FOR_END_OF_CLOCK_BIT:
            if ( PulseCorrect( pulse->PulseWidth,
                               pulse->PulsePolarity,
                               IR_DECODER_COMPRESSED_BIT_PULSE_WIDTH_MIN,
                               IR_DECODER_COMPRESSED_BIT_PULSE_WIDTH_MAX,
                               NEGATIVE_EDGE
                             )
               )
            {
                if ( BitCounterComp)
                {
                    IR_ReceiverState = IR_WAITING_FOR_START_OF_CLOCK_BIT;
                }
                else
                {

                    if ( IsCorrectValue(OutputValueCompressed))
                    {
                        HandleKey(IR_ConvertCompressedKey(KeyValueCompressed), &kpIR);
                    }
                    IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
                    BitCounterComp = 0;
                }
            }
            else
            {
                // Unexpected edge ... start over!
                IR_ReceiverState = IR_WAITING_FOR_END_OF_LEADER;
                BitCounterComp = 0;
            }
            break;
        case IR_WAITING_TO_PROCESS_KEY:
            debug_assert(0);
            break;
    } // end of switch ( IR_ReceiverState )
}
/*************************************************************************/

void
IR_HandleCompleteKey(const IRKeyData* a_keyValue)
{
    // check that this key is valid
    // and for us
      if ( (a_keyValue->KeyCode == (a_keyValue->KeyInverse ^ 0xff)) &&
            (a_keyValue->ProductCode == PROGRAM_PRODUCT_CODE) &&
            (a_keyValue->CustomerCode == BOSE_MANUFACTURER_CODE) &&
            handleIR)
    {
        // TODO cc1014948: verify this behaviour when key handler is completed
        // kpIR.HeldKey.state = LPM_KEY_STATE_PRESSED;
        HandleKey((KEY_VALUE)a_keyValue->KeyCode, &kpIR);
    }
}


/*
* Function: IR_RX_IRQHandler
*
* This function will parse the raw input edges.
* This queues up edges to be parsed in the background.
*
*
* Returns: nothing
*/

static void IR_RX_IRQHandler(void)
{
    INTERRUPT_RUN_LATENCY_BEGIN(IR_TIMER_ISR);

    // handle an update event which signals that the timer has overflown
    if (TIM_CHECK_UPDATE_INT_ENABLED(IR_RX_TIMER) &&
            TIM_CHECK_UPDATE_INT(IR_RX_TIMER))
    {
        TIM_CLEAR_UPDATE_INT(IR_RX_TIMER);
        IrCurrentPulse.PulseWidth += (0xFFFF - lastEdgeTime);
        lastEdgeTime = 0;
    }

    // now check if we caught an edge
    if (TIM_CHECK_CC_INT_ENABLED(IR_RX_TIMER, IR_RX_CC_CHANNEL) &&
            TIM_CHECK_COMPARE_INT(IR_RX_TIMER, IR_RX_CC_CHANNEL))
    {
        TIM_CLEAR_COMPARE_INT(IR_RX_TIMER, IR_RX_CC_CHANNEL);
        IR_RX_SELECT_DATA_OPP_EDGE(); // look for other edge type
        // found an edge
        g_NumIrEdges++;

        // read in the current time and subtract the last time from it
        edgeTime = TIM_GET_CAPTURE_REG(IR_RX_TIMER, IR_RX_CC_CHANNEL_REG);
        IrCurrentPulse.PulseWidth += (edgeTime - lastEdgeTime);

        //mark as a positive or negative edge
        if (IR_RX_GET_EDGE_POLARITY())
        {
            IrCurrentPulse.PulsePolarity = 1;
        }
        else
        {
            IrCurrentPulse.PulsePolarity = 0;
        }

        /* determines which state machine to use. The else statements are a
         * fail safe so IsCompressedWave doesn't get stuck at one value when
         * only one wave type is permitted
         */
        if (SUPPORT_IR_BOSE_COMP_DECODE && IsCompressedWave)
        {
            IR_ProcessPulseCompressed(&IrCurrentPulse);
        }
        else
        {
            IsCompressedWave = FALSE;
        }

        if (SUPPORT_IR_NEC_DECODE && !IsCompressedWave)
        {
            IR_ProcessPulse(&IrCurrentPulse);
        }
        else
        {
            IsCompressedWave = TRUE;
        }

        // reset pulse count
        IrCurrentPulse.PulseWidth = 0;
        lastEdgeTime = edgeTime;
        portEND_SWITCHING_ISR ( TRUE );
    }

    INTERRUPT_RUN_LATENCY_END(IR_TIMER_ISR, 0);
}
