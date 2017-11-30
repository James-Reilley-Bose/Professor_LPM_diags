//
// BMutex.c - wrapper around FreeRTOS mutex.
//

#include "BMutex.h"

BaseType_t BMutex_Take(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    {
        return pdTRUE;
    }
    else
    {
        return xSemaphoreTake(xSemaphore, xTicksToWait);
    }
}

BaseType_t BMutex_TakeFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken)
{
    return xSemaphoreTakeFromISR(xSemaphore, pxHigherPriorityTaskWoken);
}

BaseType_t BMutex_Give(SemaphoreHandle_t xSemaphore)
{
    return xSemaphoreGive(xSemaphore);
}

BaseType_t BMutex_GiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken)
{
    return xSemaphoreGiveFromISR(xSemaphore, pxHigherPriorityTaskWoken);
}
