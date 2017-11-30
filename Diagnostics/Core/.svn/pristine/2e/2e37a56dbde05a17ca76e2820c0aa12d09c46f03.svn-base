/*
File    :   ETAP.C
Title   :   Extended TAP (Test Access Port) Interface Byte Parser
Author  :   Dillon Johnson
Created :   11/12/2014
Copyright:  (C) 2014 Bose Corporation, Framingham, MA
*/

#define BODY_ETAP

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "etap.h"
#include "TapParserAPI.h"
#include "SystemAnalysis.h"
#include "TapListenerAPI.h"
#include "serialTapListener.h"
#include "stringbuffer.h"
#include "IpcPassThrough.h"
#include "PowerTask.h"
#include "versionlib.h"
#include "nv_system.h"

SCRIBE_DECL(tap_events);
SCRIBE_DECL(system);
SCRIBE_DECL(annotate);

/* Buffer space for printf's in non-TAP context*/
#define TAP_NUM_STRING_BUFFERS 64
static StringBuffer_t sbuffer[TAP_NUM_STRING_BUFFERS];
static StringBufferTable_t TAPBufferTable = {.size = TAP_NUM_STRING_BUFFERS, .head = 0, .tail = 0, .buffers = sbuffer};

static char PrintfBuffer[SIZE_OF_STRING_BUFFER]; // buffer printf uses in TAP context

static BOOL forwardToIPC = FALSE;

/* local functions */
char TAP_ToLowerCase(char upperCase);
BOOL TAP_ExecuteCommand (const TAP_Command* commandToExecute, CommandLine_t* CommandLine);
uint8_t TAP_EnumerateArguments(uint8_t startOfArgs[], char* receivedBuffer);
inline BOOL TAP_IsAlphaNumeric(char character);
inline BOOL TAP_IsHex(char character);
inline BOOL TAP_IsDecimal(char character);

/*
 * @func TAP_EnumerateArguments
 *
 * @brief Indexes all the arguments, converts to lowercase.
 *
 * @param uint8_t startOfArgs[] - array of argument indexes.
 * @param char* receivedBuffer - pointer to the command string.
 *
 * @return - n/a
 */
uint8_t TAP_EnumerateArguments(uint8_t startOfArgs[], char* receivedBuffer)
{
    if (receivedBuffer == '\0')
    {
        return 0;
    }
    memset(startOfArgs, 0, NUM_ARGS_SUPPORTED);
    uint8_t numberOfArgsFound = 0;
    uint8_t currentArgIndex = 0;
    uint16_t startArgs = 0;

    while ((receivedBuffer[startArgs] != ASCII_SPACE) && (receivedBuffer[startArgs] != '\0'))
    {
        startArgs++;
    }

    /* find first non-space after command or comma*/
    for (uint16_t l = startArgs; l < strlen(receivedBuffer); l++)
    {
        if (receivedBuffer[l] == ASCII_SPACE)
        {
            continue;
        }
        else
        {
            currentArgIndex = l;
            for (; l < strlen(receivedBuffer); l++)
            {
                if (receivedBuffer[l] == ASCII_COMMA)
                {
                    startOfArgs[numberOfArgsFound++] = currentArgIndex;
                    break;
                }
                //dr1005920 - simple parser change for pass through tap
                //only handles quoted string at end of tap command
                //is it a string in quotes?
                else if (receivedBuffer[l] == '\"')
                {
                    //exit loop
                    l = strlen(receivedBuffer);
                }
                else
                {
                    receivedBuffer[l] = TAP_ToLowerCase(receivedBuffer[l]);
                }
            }
        }
    }
    if (currentArgIndex)
    {
        startOfArgs[numberOfArgsFound++] = currentArgIndex;
    }
    return numberOfArgsFound;
}


/*
 * @func TAP_ProcessCommand
 *
 * @brief Figure out if the incoming string is a valid command. If so, execute it.
 *
 * @param char* receivedBuffer - pointer to incoming command.
 *
 * @return - n/a
 */
BOOL TAP_ProcessCommand(char* receivedBuffer)
{
    CommandLine_t CommandLine;

    // Break the command and args into substrings
    TAP_ParseCommand(receivedBuffer, &CommandLine);

    // Search list of TAP commands for a match.
    const TAP_Command* foundCommand = TAP_FindTAPCommand(TAP_Commands, TAPNumCommands, CommandLine.command);

    // Execute this TAP command/
    BOOL executed = TAP_ExecuteCommand (foundCommand, &CommandLine);

    if (executed) // did the command run?
    {
        LOG(tap_events, ROTTEN_LOGLEVEL_VERBOSE, "Executed %s TAP command.", CommandLine.command);
    }
    else
    {
        TAP_PrintString((const char*)TAP_UnknownCommand);
        LOG(tap_events, ROTTEN_LOGLEVEL_VERBOSE, "Unknown TAP command (%s)", CommandLine.command);
    }

    return executed;      /* command not found */
}

BOOL TAP_ProcessSubCommand(const TAP_Command commands[], uint8_t numCommands, CommandLine_t* CommandLine)
{
    if (CommandLine->numArgs == 0)
    {
        return FALSE;
    }
    // Create newCommandLine
    CommandLine_t newCommandLine;
    newCommandLine.command = CommandLine->args[0];
    newCommandLine.numArgs = MIN(NUM_ARGS_SUPPORTED - 1, CommandLine->numArgs - 1);
    for (uint8_t i = 0; i < NUM_ARGS_SUPPORTED; i++)
    {
        if (i < newCommandLine.numArgs)
        {
            newCommandLine.args[i] = CommandLine->args[i + 1];
        }
        else
        {
            newCommandLine.args[i] = "";
        }
    }
    const TAP_Command* foundCommand = TAP_FindTAPCommand(commands, numCommands, newCommandLine.command);
    // Execute this TAP command/
    BOOL executed = TAP_ExecuteCommand(foundCommand, &newCommandLine);

    if (executed) // did the command run?
    {
        LOG(tap_events, ROTTEN_LOGLEVEL_VERBOSE, "Executed %s SUB TAP command.", newCommandLine.command);
    }
    else
    {
        TAP_PrintString("Unknown Subcommand!");
        LOG(tap_events, ROTTEN_LOGLEVEL_VERBOSE, "Unknown SUB TAP command (%s)", newCommandLine.command);
    }
    return executed;      /* command not found */
}

void TAP_PrintSubCommands(const TAP_Command commands[], uint8_t numCommands)
{
    TAP_PrintString("\n\r\n\rAvailable subcommands:\n\r");
    TAP_PrintString("--------------\n\r");
    for (int i = 0; i < (numCommands); i++)
    {
        for (int j = 0; j < (TAP_NUMBER_OF_ALIASES); j++)
        {
            TAP_PrintString(commands[i].command[j]);
            TAP_PrintString("\n\r");
        }
        TAP_PrintString(commands[i].helpString);
        TAP_PrintString("\n\r--------------\n\r");
    }
}

/*
 * @func TAP_ToLowerCase
 *
 * @brief Convert upper-case characters to lower case.
 *
 * @param char mightBeUpperCase - character to convert.
 *
 * @return - the lower-case character.
 */
char TAP_ToLowerCase(char mightBeUpperCase)
{
    if (mightBeUpperCase >= 'A' && mightBeUpperCase <= 'Z')
    {
        mightBeUpperCase |= ASCII_LOWER_CASE_MASK;
    }

    return mightBeUpperCase;
}


/*
 * @func TAP_FindTAPCommand
 *
 * @brief Checks if the incomming command string is a valid command.
 *
 * @param char* command - the string with the command.
 *
 * @return - If a matching command found, the structure with the command.
 *           If no match, NULL.
 */
const TAP_Command* TAP_FindTAPCommand(const TAP_Command TAP_Commands[], uint8_t num_commands, char* command)
{
    uint8_t len = strlen(command);
    for (int l = 0 ; l < num_commands; l++) //Loop through commands
    {
        for (int j = 0; j < (TAP_NUMBER_OF_ALIASES); j++) //Loop through aliases
        {
            if ((strncmp(TAP_Commands[l].command[j], command, len) == 0 ) && (strlen(TAP_Commands[l].command[j]) == len))
            {
                return &TAP_Commands[l];
            }
        }
    }

    return NULL;
}

/*******************************************************************************
Function:   TAP_ExecuteCommand()
Returns:   N/A
Description:    Execute a TAP command if valid
Globals:
Locals:
Calls:
Called by:   TAP_ProcessCommand
********************************************************************************/
/*
 * @func TAP_ExecuteCommand
 *
 * @brief Calls a TAP command function and passes the CommandLine struct.
 *
 * @param const TAP_Command* commandToExecute - the command.
 * @param CommandLine_t* - struct with command/arg strings.
 *
 * @return - True if executed, False if not.
 */
BOOL TAP_ExecuteCommand (const TAP_Command* commandToExecute, CommandLine_t* CommandLine)
{
    if (commandToExecute != NULL)
    {
        commandToExecute->TAP_Function(CommandLine);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

char assert_str[128];
/*
 * @func TAP_AssertPrint
 *
 * @brief Sends an asyncronous assert message.  Turns off etap interrupts, dumps the
 *        TX buffer and prints out an assert message.
 *
 *        Note: Called in response to the debug_assert or assert() macros.
 *
 * @param const char *file - a buffer holding the file name.
 * @param const uint32_t line - the line of the assert.
 *
 * @return - n/a
 */
void TAP_AssertPrint(const char* file, const uint32_t line)
{
    static int in_progress = FALSE; //Flag used to keep from getting into an assert loop

    // There is an issue when logging an assert in an interrupt context. It can
    // cause an assert loop, a flag has been added to break the loop. Also the LOG command
    // will fail to log the assert.
    if (in_progress)
    {
        return;
    }
    in_progress = TRUE;

    if (!InInterrupt())
    {
        taskENTER_CRITICAL();
    }

    Listener_PutString("\n\n\n");

    snprintf(assert_str, sizeof(assert_str), "Assert: %s:%d", file, line);
    Listener_PutString(assert_str);
    Listener_PutString("\n");

    snprintf(assert_str, sizeof(assert_str), "LPM Version: %s", VersionGetVersionLong());
    Listener_PutString(assert_str);
    Listener_PutString("\n");

    if (InInterrupt())
    {
        uint32_t reg_val = (REG_NVIC_ICSR & MSK_NVIC_ICSR_VECT_ACTIVE);
        IRQn_Type src_nbr = (IRQn_Type)(reg_val - INT_EXT0);
        snprintf(assert_str, sizeof(assert_str), "ISR: %d (See stm32f2xx.h:145)", src_nbr);
        Listener_PutString(assert_str);
        Listener_PutString("\n");
    }
    else
    {
        snprintf(assert_str, sizeof(assert_str), "Task: %s", xTaskGetSchedulerState() == taskSCHEDULER_RUNNING ? pcTaskGetTaskName(NULL) : "PreScheduler");
        Listener_PutString(assert_str);
        Listener_PutString("\n");
    }

#ifdef NOT_YET // TODO: dj1005472 - should we keep this here?
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        snprintf(assert_str, sizeof(assert_str), "UI Source: %s", UITask_GetUnifySrcName(SourceChangeAPI_GetCurrentSource()));
        Listener_PutString(assert_str);
        Listener_PutString("\n");
    }

    snprintf(assert_str, sizeof(assert_str), "Power State: %s", PowerStateStrings[PowerAPI_GetCurrentPowerState()]);
    Listener_PutString(assert_str);
    Listener_PutString("\n");

#if defined(BARDEEN) || defined(GINGER) || defined(PROFESSOR)
    snprintf(assert_str, sizeof(assert_str), "Update State: %s", GetUpdateManagerStateString(UpdateManagerTask_GetState()));
    Listener_PutString(assert_str);
    Listener_PutString("\n");
#endif
#endif // NOT_YET

    //dump ring buffer to nv
    Rotten_Logger* l = rotten();
    if (l != NULL)
    {
        // Todo: figure out if we came from a flash assert
        //l->store_dump(l, "rb", "nv");
    }

    //let the logs flush out
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        vTaskDelay(TIMER_MSEC_TO_TICKS(1000));
    }
    else
    {
        Delay_ms(1000);
    }


    //suspend all tasks, the ships going down
    vTaskSuspendAll();
#ifdef HAS_SYSPARAMS_NVRAM
    if ((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (NV_GetRebootOnAssert()))
    {
        system_core_reboot();
    }
    else
    {
        asm("bkpt #0");
        while (1);
    }
#else
    system_core_reboot();
#endif
}

/*
 * @func TAP_PrintfHelper
 *
 * @brief Reserves and fills a buffer with a formatted string.
 *
 * @param const char *formatstring - null-terminated character string.
 * @param ... - printf arguments.
 *
 * @return - the index of the reserved buffer
 */
int16_t TAP_PrintfHelper(const char* formatstring, va_list args)
{
    int16_t bufNum = ReserveNextDebugBufferAtomic(&TAPBufferTable);

    if (bufNum >= 0)
    {
        if (VsnprintfHelper (&TAPBufferTable, bufNum, 0, formatstring, args ) < 0)
        {
            UnreserveLastDebugBufferAtomic(&TAPBufferTable);
            bufNum = -1;
        }
    }

    return bufNum;
}

/*
 * @func TAP_Printf
 *
 * @brief Create the string and print (blocking) if in the TAP context.
 *        Otherwise, buffer the string and post to the TAP queue so we
 *        don't block.
 *
 * @param const char *formatstring - null-terminated character string.
 * @param ... - printf arguments.
 *
 * @return - n/a
 */
uint32_t droppedCount = 0;
void TAP_Printf(const char* formatstring, ...)
{
    va_list args;
    va_start(args, formatstring);

    if (ManagedTapParserTask->TaskHandle == xTaskGetCurrentTaskHandle())
    {
        vsnprintf(PrintfBuffer, sizeof(PrintfBuffer), formatstring, args);
        if (forwardToIPC)
        {
            IpcPassThrough_SendResponse(PrintfBuffer);
        }
        else
        {
            Listener_PrintString(PrintfBuffer);
        }
    }
    else
    {
        int16_t bufNum = TAP_PrintfHelper(formatstring, args);

        if (bufNum >= 0)
        {
            TapListener_PrintBuffer(&TAPBufferTable);
        }
        else
        {
            //todo: should we block until one is free?
            // we're presumably already in a situation where buffers are arriving
            // way too fast for us to handle, so print in-place here rather than
            // calling TAP_PrintString, which will end up trying to queue the message,
            // eventually overflowing the tap queue and causing an assertion
            Listener_PrintString("Out of TAP string buffers - dropped a message\n\r");
        }
    }

    va_end(args);
}

/*
 * @func TAP_Printf
 *
 * @brief Print (blocking) if in the TAP context.
 *        Otherwise, buffer the string and post to the TAP queue so we
 *        don't block.
 *
 * @param const char *str - the string to print
 *
 * @return - n/a
 */
void TAP_PrintString(const char* str)
{
    debug_assert(str);

    if (str)
    {
        if (ManagedTapParserTask->TaskHandle == xTaskGetCurrentTaskHandle())
        {
            if (forwardToIPC)
            {
                IpcPassThrough_SendResponse(str);
            }
            else
            {
                Listener_PrintString(str);
            }
        }
        else
        {
            TapListener_PrintString(str); // queue it up
        }
    }
    else
    {
        TapListener_PrintString("\nTap_PrintString: y u give me null pointer...\n\n");
    }
}

/*
 * @func TAP_ParseCommand
 *
 * @brief Parses a command line into null-terminated command and args.
 *
 * @param char *CommandString - the full command line.
 * @param CommandLine_t* CommandLine - the struct to be filled with command/args.
 *
 * @return - n/a
 */
uint8_t TAP_ParseCommand(char* CommandString, CommandLine_t* CommandLine)
{
    uint8_t startofEachArg[NUM_ARGS_SUPPORTED] = {0};
    CommandLine->numArgs = TAP_EnumerateArguments(startofEachArg, CommandString);

    /* fill in the struct */
    CommandLine->command = CommandString;
    for (uint8_t i = 0; i < NUM_ARGS_SUPPORTED; i++)
    {
        if (i < CommandLine->numArgs) // if there's an arg, point to it
        {
            CommandLine->args[i] = &CommandString[startofEachArg[i]];
        }
        else // if there's no arg, make it empty string
        {
            CommandLine->args[i] = "";
        }
    }

    /* Place NULLs at the end of each argument. */
    uint8_t l = 0;
    while (TAP_IsAlphaNumeric(CommandLine->command[l])) //always a command
    {
        l++;
    }
    CommandLine->command[l] = '\0';
    if (CommandLine->numArgs > 1) // if only one arg, already done :)
    {
        for (int8_t i = 1; i < CommandLine->numArgs; i++)
        {
            int8_t j = -1;
            while (!TAP_IsAlphaNumeric(CommandString[startofEachArg[i] + j]))
            {
                j--;
            }
            CommandString[startofEachArg[i] + j + 1] = '\0';
        }
    }

    return CommandLine->numArgs;
}


/*
 * @func TAP_HexArgToInt
 *
 * @brief Converts a null-terminated arg string containing a hex value into an int.
 *
 * @param CommandLine_t* CommandLine - the struct with args.
 * @param uint8_t Argnum - argument index.
 * @param BOOL* validHexValue - pointer to store if function is returning valid int.
 *
 * @return - an integer of the same value as the string.
 */
int32_t TAP_HexArgToInt (CommandLine_t* CommandLine, uint8_t Argnum, BOOL* validHexValue)
{
    int32_t accumulator = 0;
    int8_t sign = 1;
    const char* subStr = CommandLine->args[Argnum];

    /* Make sure there's really an argument */
    if (subStr == NULL) // if not valid arg
    {
        *validHexValue = FALSE;
        return -1;
    }
    else // if valid arg
    {
        subStr = CommandLine->args[Argnum];
        *validHexValue = TRUE; // assume for now
    }

    /* Check if negative */
    if (*subStr == '-')
    {
        subStr++;
        sign = -1;
    }

    /* If prepended with '0x', throw it away */
    if ((subStr[0] == '0') && (subStr[1] == 'x'))
    {
        subStr += 2;
    }

    /* Convert to int, character by character */
    while (*subStr != '\0')
    {
        if (TAP_IsHex(*subStr))
        {
            accumulator <<= 4;
            accumulator += subStr[0] - ((subStr[0] < 'a') ? '0' : ('a' - 10));
        }
        else
        {
            *validHexValue = FALSE;
            break;
        }

        subStr++;
    }

    return (accumulator * sign);
}


/*
 * @func TAP_DecimalArgToInt
 *
 * @brief Converts a null-terminated arg string containing a decimal value into an int.
 *
 * @param CommandLine_t* CommandLine - the struct with args.
 * @param uint8_t Argnum - argument index.
 * @param BOOL* validDecimalValue - pointer to store if function is returning valid int.
 *
 * @return - an integer of the same value as the string.
 */
int32_t TAP_DecimalArgToInt (CommandLine_t* CommandLine, unsigned char Argnum, BOOL* validDecimalValue)
{
    int32_t accumulator = 0;
    int8_t sign = 1;
    const char* subStr = CommandLine->args[Argnum];

    /* Make sure there's really an argument */
    if (subStr == NULL) // if not valid arg
    {
        *validDecimalValue = FALSE;
        return -1;
    }
    else // if valid arg
    {
        *validDecimalValue = TRUE; // assume for now
    }

    /* Check if negative */
    if (*subStr == '-')
    {
        subStr++;
        sign = -1;
    }

    /* Convert to int, character by character */
    while (*subStr != '\0')
    {
        if (TAP_IsDecimal(*subStr))
        {
            accumulator *= 10;
            accumulator += *subStr - '0';
        }
        else
        {
            *validDecimalValue = FALSE;
            break;
        }

        subStr++;
    }

    return (accumulator * sign);
}


/*
 * @func TAP_IsAlphaNumeric
 *
 * @brief Checks if a character is a letter or number.
 *
 * @param char character - the character to check.
 *
 * @return - True if letter or number, false otherwise.
 */
inline BOOL TAP_IsAlphaNumeric(char character)
{
    return ((character >= 'a') && (character <= 'z') ||
            (character >= 'A') && (character <= 'Z') ||
            (character >= '0') && (character <= '9')) ||
           (character == '?');
}


/*
 * @func TAP_IsHex
 *
 * @brief Checks if a character is a hex value.
 *
 * @param char character - the character to check.
 *
 * @return - True if 0-f, false otherwise.
 */
inline BOOL TAP_IsHex(char character)
{
    return ((character >= 'a') && (character <= 'f') ||
            (character >= 'A') && (character <= 'F') ||
            (character >= '0') && (character <= '9'));
}


/*
 * @func TAP_IsDecimal
 *
 * @brief Checks if a character is a decimal value.
 *
 * @param char character - the character to check.
 *
 * @return - True if 0-9, false otherwise.
 */
inline BOOL TAP_IsDecimal(char character)
{
    return ((character >= '0') && (character <= '9'));
}

BOOL TAP_HexStringToUInt8Array(char hex_string[], uint8_t* destination, uint8_t size)
{
    uint8_t data[255] = { 0 };
    uint8_t data_count = 0;
    for (uint8_t i = 0; hex_string[i] != '\0'; i += 2)
    {
        BOOL valid = TAP_HexCharsToHex(hex_string[i], hex_string[i + 1], &(data[data_count]));
        if (valid == FALSE)
        {
            //invalid string
            return FALSE;
        }

        //stop if we're at the size of the destination buffer
        if (++data_count == size)
        {
            break;
        }
    }
    //pad the rest of data with 0x00
    if (data_count < size)
    {
        memset(&data[data_count], 0x00, size - data_count);
    }
    memcpy(destination, data, size);
    return TRUE;
}

BOOL TAP_HexCharsToHex(const char char_hex1, const char char_hex2, uint8_t* destination)
{
    uint8_t hex1, hex2;
    BOOL valid1 = TAP_HexCharToHex(char_hex1, &hex1);
    BOOL valid2 = TAP_HexCharToHex(char_hex2, &hex2);
    if ((valid1 == FALSE) || (valid2 == FALSE))
    {
        return FALSE;
    }
    *destination = (hex1 << 4) | hex2;
    return TRUE;
}

BOOL TAP_HexCharToHex(const char char_hex, uint8_t* destination)
{
    switch (char_hex)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            *destination = char_hex - '0';
            return TRUE;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            *destination = char_hex - 'A' + 0xA;
            return TRUE;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            *destination = char_hex - 'a' + 0xA;
            return TRUE;
        default:
            return FALSE;
    }
}

void TapListener_SetForwardToIpc(BOOL forward)
{
    forwardToIPC = forward;
}

BOOL TapValidRange(int lower_bound, BOOL lower_inclusive, int upper_bound, BOOL upper_inclusive, int test)
{
    BOOL lower = FALSE;
    if (lower_inclusive)
    {
        lower = lower_bound <= test ? TRUE : FALSE;
    }
    else
    {
        lower = lower_bound < test ? TRUE : FALSE;
    }
    BOOL upper = FALSE;
    if (upper_inclusive)
    {
        upper = upper_bound >= test ? TRUE : FALSE;
    }
    else
    {
        upper = upper_bound > test ? TRUE : FALSE;
    }
    return (lower && upper) ? TRUE : FALSE;
}

int toupper(int);
void TAP_StringToUpper(char* sPtr)
{
    while ( *sPtr != ASCII_NULL )
    {
        *sPtr = toupper((uint8_t) * sPtr);
        ++sPtr;
    }
}

/* ======== END OF FILE ======== */
