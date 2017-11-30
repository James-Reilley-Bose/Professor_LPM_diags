////////////////////////////////////////////////////////////////////////////////
/// @file          DiagData.h
/// @brief         Defines a Diag data structure.
////////////////////////////////////////////////////////////////////////////////
#ifndef _DIAGDATA_H_
#define _DIAGDATA_H_

#include "platform_settings.h"
//#include "DiagRivieraLPM_Values.h"
#if 0

#define GENERATE_ENUM(ENUM) ENUM,

typedef enum
{
    DIAG_STATE_PRESSED,
    DIAG_STATE_RELEASED,
    DIAG_STATE_SINGLE_PRESS,
    DIAG_STATE_SHORT_PRESS_AND_HOLD,
    DIAG_STATE_PRESS_AND_HOLD,
    DIAG_STATE_VENTI_PRESS_AND_HOLD,
    DIAG_STATE_LONG_PRESS_AND_HOLD,
    DIAG_STATE_XL_PRESS_AND_HOLD,
    DIAG_STATE_INVALID,
    NUM_DIAG_STATES = DIAG_STATE_INVALID,
} DIAG_STATE;

/* Diag Key Producers */
#define FOREACH_DIAG_PRODUCER(DIAG_PRODUCER)\
    DIAG_PRODUCER(DIAG_PRODUCER_CONSOLE)\
    DIAG_PRODUCER(DIAG_PRODUCER_RF)\
    DIAG_PRODUCER(DIAG_PRODUCER_IR)\
    DIAG_PRODUCER(DIAG_PRODUCER_TAP)\
    DIAG_PRODUCER(DIAG_PRODUCER_IPC)\
    DIAG_PRODUCER(DIAG_PRODUCER_CEC)\
\
    DIAG_PRODUCER(DIAG_PRODUCER_INVALID)
#define NUM_DIAG_PRODUCERS DIAG_PRODUCER_INVALID

typedef enum
{
    FOREACH_DIAG_PRODUCER (GENERATE_ENUM)
} DIAG_PRODUCER;
extern const char* DiagsProducerNames[];

/* Diag Groups */
#define FOREACH_DIAG_GROUP(DIAG_GROUP)\
    DIAG_GROUP(DIAG_GROUP_SOURCE)\
    DIAG_GROUP(DIAG_GROUP_DIAG)\
    DIAG_GROUP(DIAG_GROUP_TRANSPORT_CONTROL)\
    DIAG_GROUP(DIAG_GROUP_MENU_NAVIGATION)\
    DIAG_GROUP(DIAG_GROUP_SPECIAL)\
    DIAG_GROUP(DIAG_GROUP_NUMERIC)\
        DIAG_GROUP(DIAG_GROUP_COLOR)\
        DIAG_GROUP(DIAG_GROUP_SOFTDIAG)\
\
    DIAG_GROUP(DIAG_GROUP_INVALID)
#define DIAG_GROUP_START DIAG_GROUP_SOURCE  /* First element of above list! */
#define NUM_DIAG_GROUPS DIAG_GROUP_INVALID
typedef enum
{
    FOREACH_DIAG_GROUP (GENERATE_ENUM)
} DIAG_GROUP;
extern const char* DiagGroupNames[];

typedef enum
{
    DIAG_SINGLE_PRESS,
    DIAG_CONTINUOUS_PRESS,
    DIAG_PRESS_AND_HOLD,
    DIAG_PRESS_INVALID,
} DIAG_PRESS_TYPE;

typedef struct
{
    DIAG_GROUP group;
    DIAG_PRESS_TYPE press_type;
    const char* name;
} DIAG_GROUP_TABLE_ELEMENT;
extern const DIAG_GROUP_TABLE_ELEMENT Diag_Info[];

typedef enum
{
    DIAG_VAL_00,
    DIAG_VAL_01,
    DIAG_VAL_02,
    DIAG_VAL_03,
    DIAG_VAL_04,
    DIAG_VAL_05,
    DIAG_VAL_06,
    NUM_DIAG
} DIAG_VALUE;

typedef struct
{
    uint32_t timestamp;
    uint32_t ph_timestamp;
    DIAG_VALUE value;
    DIAG_STATE state;
    DIAG_GROUP group;
    DIAG_PRODUCER producer;
    DIAG_STATE state_before_release;
} DIAG_DATA_t;
#endif

#endif // _DIAGDATA_H_
