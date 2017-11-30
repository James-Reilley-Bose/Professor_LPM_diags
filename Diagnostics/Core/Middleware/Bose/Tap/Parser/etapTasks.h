////////////////////////////////////////////////////////////////////////////////
/// @file            etapTasks.h
/// @version         $Id: $
/// @brief           Task data via ETAP
/// @author          Stolen from Triode by Dillon Johnson
/// @date            Creation Date: 2015
///
/// Copyright 2015 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_ETAP_TASKS_H
#define INCLUDE_ETAP_TASKS_H

#include "etapcoms.h"

/**
 * @DOCETAP
 * COMMAND:   system
 *
 * ALIAS:     sa
 *
 * DESCRIP:   Prints system profile data
 *
 * PARAM:     n/a
 *
 * REPLY:     empty
 *
 * EXAMPLE:   sa chipcon
 *
 * @DOCETAPEND
 */


/* Values used for Task info dumps to ETAP.  Q items are 16 bytes so it cannot be > 14 */
#define FREE_RTOS_NAME_SIZE     8
#define MAX_SUPPORTED_Q_ITEMS   10

typedef struct
{
    uint32_t    id;
    uint32_t    param1;
    uint32_t    param2;
    uint32_t    param3;
} MSG_STRUCT_t;

#pragma pack(1)

typedef struct
{
    char            name [FREE_RTOS_NAME_SIZE + 1];
    uint8_t         qSize;
    uint8_t         highWaterMark;
    uint16_t        stackSize;
    uint32_t        stackHighWaterMark;
    MSG_STRUCT_t    aMsgs [MAX_SUPPORTED_Q_ITEMS];

} FREE_RTOS_INFO_t;

typedef struct
{
    const char*     name;
    xQueueHandle    Q;
    uint8_t         QSize;
    uint8_t         QHigh;
    uint16_t        stackSize;
    uint16_t        stackHighWater;
    uint8_t         numParams;
    uint8_t         runTimePercentage;
} FREE_RTOS_TASK_DATA_t;

typedef struct
{
    char            name [FREE_RTOS_NAME_SIZE + 1];
    uint8_t         qSize;
    uint8_t         highWaterMark;
    uint16_t        stackSize;
    uint16_t        stackHighWaterMark;
} FREE_RTOS_SUMMARY_DATA_t;

typedef struct
{
    uint32_t        count;
    uint32_t        exceedCount;
    uint32_t        maxTime;
    uint32_t        runTimeUsec;
    char            name [FREE_RTOS_NAME_SIZE + 1];

} FREE_RTOS_INTERRUPT_DATA_t;

#pragma pack()

#define SYSTEM_ANALYSIS_HELP_TEXT "* Print system profile data. Usage: sa <option>\r\n\toption: 0 - clear task\r\n\t\t2 - clear interrupt\r\n\t\ttask_name - print specific task"

void TAP_HandleFREtapMsg (CommandLine_t*);

#endif
