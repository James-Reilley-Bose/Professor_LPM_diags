//
// BMutex.h - wrapper around FreeRTOS mutex.
//

#ifndef B_MUTEX_B
#define B_MUTEX_B

#include "project.h"

BaseType_t BMutex_Take(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
BaseType_t BMutex_TakeFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken);
BaseType_t BMutex_Give(SemaphoreHandle_t xSemaphore);
BaseType_t BMutex_GiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken);

#endif // B_MUTEX_B
