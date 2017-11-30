/*
 * stringbuffer.c - some structures/functions to help buffer text/output
 *                  (refactored from Triode's etap.c)
 *
 * Refactorer: Dillon Johnson
 */

#include <stdio.h>
#include "stringbuffer.h"
#include "project.h"
#include "TBLSerial.h"

void StringBuffer_IncrementTail(StringBufferTable_t* buffer)
{
    buffer->buffers[buffer->tail].filled = FALSE;
    buffer->tail = (buffer->tail + 1) % buffer->size;
}

/*
 * @func VsnprintfHelper
 *
 * @brief Manages buffer space before handing off string and args to vsnprintf.
 *
 * @param StringBufferTable_t *table - a specific module's buffer table.
 * @param uint8_t bufNum - index into the buffer table.
 * @param uint8_t bufIndex - index of where the first character goes.
 * @param const char *formatstring - pointer to null-terminated character string.
 * @param va_list args - printf arguments.
 *
 * @return - 0 if a buffer was successfully filled, -1 otherwise
 */
int8_t VsnprintfHelper
(
    StringBufferTable_t* table,
    uint8_t bufNum,
    uint8_t bufIndex,
    const char* formatstring,
    va_list args
)
{
    int8_t ret; // return success/failure

    if (bufNum < table->size) // If a valid buffer, insert the formatted string
    {
        uint8_t remaining = (SIZE_OF_STRING_BUFFER - bufIndex) - 1;
        debug_assert (bufIndex < SIZE_OF_STRING_BUFFER - 1);
        bufIndex += vsnprintf(table->buffers[bufNum].line,
                              remaining, formatstring, args) + 1;

        if (bufIndex >= SIZE_OF_STRING_BUFFER)
        {
            bufIndex = SIZE_OF_STRING_BUFFER - 1;
        }
        table->buffers[bufNum].line[bufIndex] = '\0';

        ret = VSNPRINTF_SUCCESS;
    }
    else
    {
        ret = INVALID_BUFFER;
    }

    return ret;
}

/*
 * @func ReserveNextDebugBufferAtomic
 *
 * @brief Locates a free buffer from a module's buffer table and claims it.
 *
 * @param StringBufferTable_t *table - a specific module's buffer table.
 *
 * @return - the index of the chosen buffer on success,
 *           -1 on fail
 */
int16_t ReserveNextDebugBufferAtomic(StringBufferTable_t* table)
{
    int16_t ret = INVALID_BUFFER;

    portENTER_CRITICAL();
    /* If the next buffer is free, take it */
    if ((table->head + 1) % table->size != table->tail)
    {
        ret = table->head;
        table->head = (table->head + 1) % table->size;
    }
    portEXIT_CRITICAL();

    return ret;
}

/*
 * @func UnreserveLastDebugBufferAtomic
 *
 * @brief Removes the last thing added to the buffer table.
 *
 * @param StringBufferTable_t *table - a specific module's buffer table.
 *
 * @return - n/a
 */
void UnreserveLastDebugBufferAtomic(StringBufferTable_t* table)
{
    portENTER_CRITICAL();
    if (table->head != 0)
    {
        table->head--;
    }
    else
    {
        table->head = table->size - 1;
    }
    portEXIT_CRITICAL();
}
