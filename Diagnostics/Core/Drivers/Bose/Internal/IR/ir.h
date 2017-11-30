#ifndef IR_H
#define IR_H

#include "etapcoms.h"

/* defines and enums */
/* Value extracted from PT639414_04.doc*/
/* Manufacturer code */
#define BOSE_MANUFACTURER_CODE   0xBA

/* Product code */
#if defined (GINGER) || defined(EXAMPLE_VARIANT)
#define PROGRAM_PRODUCT_CODE    0xA0    /* Adobo / Pike / Ginger */
#else
#define PROGRAM_PRODUCT_CODE    0x4B    /* Triode / Bardeen */
#endif

typedef enum
{
    IR_WAITING_FOR_END_OF_LEADER,
    IR_WAITING_FOR_START_OF_FIRST_CLOCK_BIT,
    IR_WAITING_FOR_START_OF_CLOCK_BIT,
    IR_WAITING_FOR_START_OF_ZERO_CLOCK_BIT,
    IR_WAITING_FOR_END_OF_ZERO_BIT,
    IR_WAITING_FOR_END_OF_CLOCK_BIT,
    IR_WAITING_TO_PROCESS_KEY
} IR_RECEIVER_STATE;

/* structures */
#pragma pack(1)
typedef struct
{
    unsigned PulseWidth: 31;
    unsigned PulsePolarity: 1;
} ICEdgeType;

typedef struct
{
    unsigned CustomerCode : 8;
    unsigned ProductCode  : 8;
    unsigned KeyCode     : 8;
    unsigned KeyInverse   : 8;
}   IRKeyData;
#pragma pack()

/* functions */
BOOL IR_GetAllowIr();
void IR_SetAllowIr(BOOL enabled);
void IR_Init(void);
void IR_EnableRx(void);
void IR_HandleCompleteKey(const IRKeyData* a_keyValue);

/* macros */
#define IR_RX_INPUT_SELECT       TIM_STANDARD_INPUT_PAD
#define IR_RX_INPUT_MODE(input)  TIM_CC_INPUT_MODE(IR_RX_TIMER, IR_RX_CC_CHANNEL, input)
#define IR_RX_DISABLE_TIMER()       TIM_DISABLE_CAPTURE_COMPARE(IR_RX_TIMER,IR_RX_CC_CHANNEL)
#define IR_RX_ENABLE_TIMER()        TIM_ENABLE_CAPTURE_COMPARE(IR_RX_TIMER,IR_RX_CC_CHANNEL)
#define IR_ENABLE_RX_INTERRUPTS()   TIM_ENABLE_CAPTURE_COMPARE_INTERUPT(IR_RX_TIMER,IR_RX_CC_CHANNEL)
#define IR_CLEAR_RX_INTERRUPTS()    TIM_CLEAR_COMPARE_INT(IR_RX_TIMER,IR_RX_CC_CHANNEL)
#define IR_CLEAR_UPDATE_INT()       TIM_CLEAR_UPDATE_INT(IR_RX_TIMER)

//Input Capture Edge configuration
#define IR_RX_SELECT_DATA_OPP_EDGE()             (IR_RX_TIMER->CCER ^= IR_RX_DATA_EDGE_SELECT)
#define IR_RX_SELECT_FALLING_EDGE()              (IR_RX_TIMER->CCER |= IR_RX_DATA_EDGE_SELECT)
#define IR_RX_GET_EDGE_POLARITY()                (IR_RX_TIMER->CCER & IR_RX_DATA_EDGE_SELECT)

// disable simple turns off both of the interrupts for the receiver
#define IR_RX_DISABLE()             TIM_DISABLE_CAPTURE_COMPARE_INTERUPT(IR_RX_TIMER,IR_RX_CC_CHANNEL)
// enable will clear the interrupt mask, then re-enabel the interrutps
//  and the timer
#define IR_RX_ENABLE()              IR_EnableRx()
#define IR_RX_ENABLE_UPDATE_FLAG()  TIM_ENABLE_UPDATE_INTERUPT(IR_RX_TIMER)

#define TAP_IR_HELP_TEXT "IR Toggle : Enter ir to see current value\n Enter ir <0,1> to enable or disable\n"

#endif //IR_H
