/*
    File    :   ETAP.H
    Title   :   Extended TAP (Test Access Port) Interface Header
    Author  :   Ken Lyons
    Created :   3/12/01
    Copyright:  (C) 2001 Bose Corporation, Framingham, MA

    Description:
        Header file for the TAP interface.


    Revisions:

===============================================================================
*/
#ifndef HEADER_ETAP
#define HEADER_ETAP

#include <stddef.h>
#include "project.h"
#include "etapcoms.h"
#include "ascii.h"

#define TAP_NUM_LINES_BUFFERED        2
#define TAP_COMMAND_BUFFER_SIZE     384 /* maximum length of command line */


/* Macros */
#define TAPCommand(x)               void x (CommandLine_t* CommandLine)

/* Command processing */
uint8_t TAP_EnumerateArguments(uint8_t startOfArgs[], char* receivedBuffer);
extern BOOL TAP_ProcessCommand(char* receivedBuffer);
const TAP_Command* TAP_FindTAPCommand(const TAP_Command TAP_Commands[], uint8_t num_commands, char* command);
BOOL TAP_ProcessSubCommand(const TAP_Command commands[], uint8_t numCommands, CommandLine_t* CommandLine);
extern uint8_t TAP_ParseCommand(char* CommandString, CommandLine_t* CommandLine);
extern int32_t TAP_HexArgToInt(CommandLine_t* CommandLine, uint8_t Argnum, BOOL* validHexValue);
extern int32_t TAP_DecimalArgToInt(CommandLine_t* CommandLine, unsigned char Argnum, BOOL* validDecimalValue);

/* Print */
extern void TAP_Printf(const char* formatstring, ...);
extern void TAP_PrintString(const char* str);

void TAP_PrintSubCommands(const TAP_Command commands[], uint8_t numCommands);

BOOL TAP_HexStringToUInt8Array(char hex_string[], uint8_t* destination, uint8_t size);
BOOL TAP_HexCharToHex(const char char_hex, uint8_t* destination);
BOOL TAP_HexCharsToHex(const char char_hex1, const char char_hex2, uint8_t* destination);
char TAP_ToLowerCase(char mightBeUpperCase);

void TapListener_SetForwardToIpc(BOOL forward);

BOOL TapValidRange(int lower_bound, BOOL lower_inclusive, int upper_bound, BOOL upper_inclusive, int test);
void TAP_StringToUpper(char* sPtr);

#endif // HEADER_ETAP
