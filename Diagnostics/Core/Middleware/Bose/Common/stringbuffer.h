/*
 * stringbuffer.c - some structures/functions to help buffer text/output
 *                  (refactored from Triode's etap.c)
 *
 * Refactorer: Dillon Johnson
 */
#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stdint.h>
#include <stdarg.h>
#include <project.h>

/* Size */
#define SIZE_OF_STRING_BUFFER       128

/* Success/error return values */
#define VSNPRINTF_SUCCESS 0
#define INVALID_BUFFER -1

typedef struct
{
    char line [SIZE_OF_STRING_BUFFER];
    BOOL filled;
} StringBuffer_t;

typedef struct
{
    uint16_t size; // number of lines (aka buffers)
    uint16_t head; // index of newest element
    uint16_t tail; // index of oldest element
    StringBuffer_t* buffers; // pointer to buffer table
} StringBufferTable_t;

int8_t VsnprintfHelper(StringBufferTable_t* table, uint8_t bufNum, uint8_t bufIndex, const char* formatstring, va_list args);
int16_t ReserveNextDebugBufferAtomic(StringBufferTable_t* table);
void UnreserveLastDebugBufferAtomic(StringBufferTable_t* table);
void StringBuffer_IncrementTail(StringBufferTable_t* buffer);

#endif // STRING_BUFFER_H
