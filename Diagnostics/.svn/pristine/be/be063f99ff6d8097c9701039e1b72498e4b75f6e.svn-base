////////////////////////////////////////////////////////////////////////////////
/// @file          DiagValues.c
/// @brief         Diag group lookup table
////////////////////////////////////////////////////////////////////////////////

#include "DiagData.h"

//DIAG_GROUP_TABLE_ELEMENT{DIAG_GROUP group, DIAG_PRESS_TYPE press_type, const char* name}

const DIAG_GROUP_TABLE_ELEMENT Diag_Info[NUM_DIAG] =
{
    {.group = DIAG_GROUP_DIAG, .press_type = DIAG_SINGLE_PRESS, .name = "zero"},  // 0x00
    {.group = DIAG_GROUP_DIAG, .press_type = DIAG_SINGLE_PRESS, .name = "one"},   // 0x00
    {.group = DIAG_GROUP_DIAG, .press_type = DIAG_SINGLE_PRESS, .name = "two"},   // 0x01
    {.group = DIAG_GROUP_DIAG, .press_type = DIAG_SINGLE_PRESS, .name = "three"}, // 0x02
    {.group = DIAG_GROUP_DIAG, .press_type = DIAG_SINGLE_PRESS, .name = "four"},  // 0x03
    {.group = DIAG_GROUP_DIAG, .press_type = DIAG_SINGLE_PRESS, .name = "five"},  // 0x04
    {.group = DIAG_GROUP_DIAG, .press_type = DIAG_SINGLE_PRESS, .name = ""},      // 0x05
    /* invalid so not in table   {.group = DIAG_GROUP_INVALID, .press_type = DIAG_PRESS_INVALID, .name = ""}, // 0xFF */
};

#define GENERATE_NAME(NAME) #NAME,
const char* DiagGroupNames[] =
{
    FOREACH_DIAG_GROUP(GENERATE_NAME)
};
const char* diagProducerNames[] =
{
    FOREACH_DIAG_PRODUCER(GENERATE_NAME)
};
