//
// DiagTask.h
//

#ifndef DIAG_TASK_H
#define DIAG_TASK_H

#include "DiagData.h"

#define DIAG_CONSOLE_SCAN_RATE_MSEC          10

typedef enum
{
    ADCCNT,
    LEDCNT,
    I2CCNT,
    GPIOCNT,
    SHOWCNT,
    BUTTONCNT,
    PONRESETCNT,
    SNAPOFFCNT,
    SNAPREBOOTCNT,
    LCDBACKLIGHTCNT,
    LCDBACKLIGHTOPTCNT,
    OPTCNT,
    PSOCCMDCNT,
    PSOCINTCNT,
    LEDOPTCNT,
    DIAGTESTCNTLAST
} DIAGTESTCNTS;

extern ManagedTask* DiagTaskHandle;

#if 0
#define DiagTaskPostMsg(_theMsg, _p1, _p2) \
            QueueManagerPostCallbackMsg(&DiagTaskHandle->Queue, \
                                        _theMsg, \
                                        (uint32_t []){_p1, _p2}, \
                                        NOP_CALLBACK, \
                                        NO_BLOCK_TIME);
#endif

#define DiagTaskPostMsg(_theMsg, _p1) \
            QueueManagerPostCallbackMsg(&DiagTaskHandle->Queue, \
                                        _theMsg, \
                                        _p1, \
                                        NOP_CALLBACK, \
                                        NO_BLOCK_TIME);

#define DIAG_TASK_NAME "DiagTask"

void DiagTask_Init(void* p);
void DiagTask(void* pvParameters);
void DiagTask_HandleMsg(GENERIC_MSG_t* msg);
//void Product_HandleDiag(DIAG_DATA_t diag);

#endif // DIAG_TASK_H
