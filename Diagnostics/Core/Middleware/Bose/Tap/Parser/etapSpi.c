#include "project.h"
#include "etapSpi.h"
#include "SpiBus_API.h"
#include "etap.h"
#include "stdio.h"

SCRIBE_DECL(tap_test_results);

#define SPI_WAIT_TIMEOUT_MS                 100
#define MAX_STM32F2_GPIO_BANKS                9

static GPIO_TypeDef* stm32F2xxGpioBank[MAX_STM32F2_GPIO_BANKS] =
{GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI};

static GPIO_TypeDef* userCsGpioBank;

// For testing callback of Non-blocking transfer
static BOOL nonBlockingDone = FALSE;
static uint32_t nonBlockingLength = 0;
static uint8_t buf[SPI_MAX_BUFFER_LENGTH];

typedef void (*funcptr)(uint16_t, SPI_API_ERROR_CODE);

// ISR context
static void spiCallback(uint16_t numberBytesTransferred, SPI_API_ERROR_CODE errorCode)
{
    if ((errorCode == SPI_API_NO_ERROR) && (nonBlockingLength == numberBytesTransferred))
    {
        nonBlockingDone = TRUE;
    }
}

inline static void user_pull_cs_high(char csPort, uint8_t csPin, uint8_t dividerExponent);

// Returns GPIO bank for given port character
// Note the input csPort has been validated before the call
GPIO_TypeDef* getCsGpioBank(char csPort)
{
    int idx = csPort - 'a';
    return stm32F2xxGpioBank[idx];
}


// Returns TRUE if valid, FALSE otherwise
static BOOL isCsPortPinValid(char csPort, uint8_t csPin)
{
    if (csPort == '0') // User indicated to use default
    {
        return TRUE;
    }

    if ((csPort < 'a') || (csPort > 'i'))
    {
        return FALSE;
    }

    // csPort is between 'a' and 'i'
    if (csPin > GPIO_PinSource15)
    {
        return FALSE;
    }

    return TRUE;
}


TAPCommand(TAP_SPICommand)
{
    char action, csPort;
    uint8_t bus, csPin;
    uint32_t lenData = 0;
    uint16_t actualLoops = 1;
    char errorDescription[32] = "SPI Error";
    int loopCount;
#ifdef SUPPORT_SPI_STAND_BY_MODE
    int on;
    BOOL enter;
#endif
    BOOL valid[3] = {FALSE, FALSE, FALSE};

    if (CommandLine->numArgs < MINIMAL_SPI_NUMBER_ARGUMENTS)
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    // Get arguments
    action = TAP_ToLowerCase(CommandLine->args[0][0]);
    bus    = TAP_DecimalArgToInt(CommandLine, 1, &valid[0]);
    csPort = TAP_ToLowerCase(CommandLine->args[2][0]);
    csPin = TAP_DecimalArgToInt(CommandLine, 3, &valid[1]);
#ifdef SUPPORT_SPI_STAND_BY_MODE
    if (action == 's')
    {
        on = TAP_DecimalArgToInt(CommandLine, 4, &valid[2]);
    }
    else
#endif
        lenData = TAP_DecimalArgToInt(CommandLine, 4, &valid[2]);

    // Validate arguments
    if ( ((action != 'r') && (action != 'w') && (action != 't') && (action != 's')) ||
            ((action == 'r') && (CommandLine->numArgs != MINIMAL_SPI_NUMBER_ARGUMENTS)) ||
            ((action == 'w') && (CommandLine->numArgs <= (MINIMAL_SPI_NUMBER_ARGUMENTS + 1))) ||
            ((action == 't') && (CommandLine->numArgs != (MINIMAL_SPI_NUMBER_ARGUMENTS + 1))) ||
            ((action == 's') && (CommandLine->numArgs != MINIMAL_SPI_NUMBER_ARGUMENTS)) ||
            ((action != 's') && ( lenData == 0)) ||
            !valid[0] || !valid[1] || !valid[2] || (lenData > SPI_MAX_BUFFER_LENGTH) )
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    if ((bus == 0) || (bus > MAX_NUMBER_SPI_BUSES) || (isCsPortPinValid(csPort, csPin) == FALSE) )
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    SPI_Bus_Config_t mySpiConfig;

    memcpy(&mySpiConfig, &spiDefaultConfig[bus - 1], sizeof(mySpiConfig));

    // Change my config only when it differs from the default
    mySpiConfig.baudRateExponent = 2;
    mySpiConfig.disableDMA = 1;

    if (csPort != '0')
    {
        userCsGpioBank = getCsGpioBank(csPort);
        mySpiConfig.chipSelectControl = CHIP_SELECT_CONTROL_API_USERS;
        mySpiConfig.csGpioBank  = NULL;

        // Configure this GPIO pin as output
        ConfigureGpioPin(userCsGpioBank, csPin, GPIO_MODE_OUT_PP, 0);

        // Pull it high
        GPIO_SetBits(userCsGpioBank, (0x0001) << csPin);
    }

    SPI_API_ERROR_CODE spiError = SPI_API_NO_ERROR;

    SPI_BUS_HANDLE_TYPE mySpiHandle = SPIBus_Initialize(&mySpiConfig, &spiError);

    if (mySpiHandle == NULL)
    {
        TAP_PrintString(TAP_OutOfRange);
        return;
    }

    uint16_t idx = 0, ret = 0, timeoutMs;
    uint32_t readTimeMs = 0, writeTimeMs = 0, start;
    int i;

    switch (action)
    {
        case 'w':     // Write cmd
            //Convert to array of bytes
            valid[0] = FALSE;

            timeoutMs = TAP_DecimalArgToInt(CommandLine,
                                            MINIMAL_SPI_NUMBER_ARGUMENTS, &valid[0]);
            if (!valid[0])
            {
                TAP_Printf("Argument %d is invalid.\r\n", MINIMAL_SPI_NUMBER_ARGUMENTS);
                break;
            }

            valid[0] = FALSE;

            for (uint8_t dataIndex = MINIMAL_SPI_NUMBER_ARGUMENTS + 1;
                    dataIndex < CommandLine->numArgs; dataIndex++)
            {
                buf[idx++] = TAP_HexArgToInt(CommandLine, dataIndex, &valid[0]);
                if (!valid[0])
                {
                    TAP_Printf("Argument %d is invalid.\r\n", dataIndex);
                    break;
                }
            }

            if (valid[0] && (idx == lenData))
            {
                funcptr pFunc;

                if (timeoutMs == 0)
                {
                    // Non-blocking
                    nonBlockingLength = lenData;
                    pFunc = &spiCallback;
                }
                else
                {
                    pFunc = NULL;
                }

                if (csPort != '0') // User control CS
                {
                    // Pull it low
                    GPIO_ResetBits(userCsGpioBank, (0x0001) << csPin);
                }

                start = GET_INSTRUMENT_TIME_NOW();

                // write only, since user has input data
                ret = SPIBus_Communicate(mySpiHandle, lenData, buf, NULL,
                                         pFunc, timeoutMs, &spiError );

                LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL,
                    "SPIBus_Communicate, returned with status %d", spiError);

                if (timeoutMs == 0)
                {
                    while (nonBlockingDone == FALSE)
                    {
                        if (MEASURE_TIME_SINCE(start) / 1000 >= 100 * SPI_WAIT_TIMEOUT_MS)
                        {
                            strcpy(errorDescription, "SPI timeout");
                            break;
                        }
                    }

                    if (SPIBus_CommunicateComplete(mySpiHandle, &spiError) == FALSE)
                    {
                        ret = 0;
                    }

                }
                writeTimeMs = MEASURE_TIME_SINCE(start);

                user_pull_cs_high(csPort, csPin, mySpiConfig.baudRateExponent);

                if ((timeoutMs == 0) && (nonBlockingDone == TRUE))
                {
                    TAP_Printf("Got SPI Callback: error = %d\n", spiError);
                    nonBlockingDone = FALSE;
                }
            }
            break;

        case 'r':

            memset(buf, 0, lenData);

            if (csPort != '0') // User control CS
            {
                // Pull it low
                GPIO_ResetBits(userCsGpioBank, (0x0001) << csPin);
            }

            start = GET_INSTRUMENT_TIME_NOW();

            // read only, since user has no input data
            ret = SPIBus_Communicate(mySpiHandle, lenData, NULL, buf,
                                     NULL, SPI_WAIT_TIMEOUT_MS, &spiError );

            readTimeMs = MEASURE_TIME_SINCE(start);

            if (csPort != '0') // User control CS
            {
                // Pull it high
                GPIO_SetBits(userCsGpioBank, (0x0001) << csPin);
            }

            if (ret > 0)
            {
                LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL, "SPI%d Data (%d bytes): ", bus, ret);
                for (i = 0; i < ret; i++)
                {
                    LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL, "%02x ", buf[i]);
                }
            }
            break;

        case 't':
            // The array valid[] should be all TRUE at this point,
            // reuse them for different purposes as below
            valid[0] = FALSE; // Reuse for data validation
            // valid[1] = TRUE means no comparison error

            loopCount = TAP_DecimalArgToInt(CommandLine, MINIMAL_SPI_NUMBER_ARGUMENTS, &valid[0]);
            if (!valid[0] || (loopCount > MAX_ETAP_SPI_LOOP_COUNT) || (loopCount <= 0) )
            {
                TAP_Printf("Argument for testing is invalid.\r\n");
                break;
            }

            for (i = 0; i < loopCount; i++)
            {
                // Use current start time as "random" data pattern
                uint8_t patternStart = GET_INSTRUMENT_TIME_NOW() & 0xFF;
                for (idx = 0; idx < lenData; idx++)
                {
                    buf[idx] = patternStart + idx;
                }

                if (csPort != '0') // User control CS
                {
                    // Pull it low
                    GPIO_ResetBits(userCsGpioBank, (0x0001) << csPin);
                }

                // Start measuring time
                start = GET_INSTRUMENT_TIME_NOW();

                // write
                ret = SPIBus_Communicate(mySpiHandle, lenData, buf, NULL,
                                         NULL, SPI_WAIT_TIMEOUT_MS, &spiError );

                writeTimeMs += MEASURE_TIME_SINCE(start);

                user_pull_cs_high(csPort, csPin, mySpiConfig.baudRateExponent);

                if (ret != lenData) // Write failure
                {
                    strcpy(errorDescription, "SPI write error");
                    break; // i-loop
                }

                memset(buf, 0, lenData);

                if (csPort != '0') // User control CS
                {
                    // Pull it low
                    GPIO_ResetBits(userCsGpioBank, (0x0001) << csPin);
                }

                start = GET_INSTRUMENT_TIME_NOW();

                // read
                ret = SPIBus_Communicate(mySpiHandle, lenData, NULL, buf,
                                         NULL, SPI_WAIT_TIMEOUT_MS, &spiError );

                readTimeMs += MEASURE_TIME_SINCE(start);

                if (csPort != '0') // User control CS
                {
                    // Pull it high
                    GPIO_SetBits(userCsGpioBank, (0x0001) << csPin);
                }

                if (ret != lenData ) // Read failure
                {
                    strcpy(errorDescription, "SPI read error");
                    break; // i-loop
                }

                for (idx = 0; idx < lenData; idx++)
                {
                    if (buf[idx] != ((uint8_t)(patternStart + idx))) // Compare failure
                    {
                        strcpy(errorDescription, "SPI compare error");
                        valid[1] = FALSE;
                        break; // idx-loop
                    }
                }

                if (valid[1] == FALSE)
                {
                    break;    // i-loop
                }

            } // end of i-loop

            actualLoops += i;
            break;

#ifdef SUPPORT_SPI_STAND_BY_MODE
        case 's':

            if (on != 0)
            {
                strcpy(errorDescription, "Enter standby");
                enter = TRUE;
            }
            else
            {
                strcpy(errorDescription, "Exit standby");
                enter = FALSE;
            }

            if (SPIBus_Standby(mySpiHandle, enter, &spiError) == FALSE)
            {
                strcat(errorDescription, " Failed");
            }
            else
            {
                strcat(errorDescription, " OK");
            }

            TAP_PrintString(errorDescription);

            return;
#endif
    }

    readTimeMs /= actualLoops;
    writeTimeMs /= actualLoops;


    if ((ret != lenData) || !valid[1])
    {
        TAP_PrintString("\r\n");
        TAP_PrintString(errorDescription);
        LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL, "%s ", errorDescription );
    }
    else
    {
        sprintf((char*)buf, "ReadTime = %u us\r\n", readTimeMs);
        sprintf((char*)&buf[SPI_MAX_BUFFER_LENGTH >> 1], "WriteTime = %u us\r\n", writeTimeMs);

        if ((action == 'r') || (action == 't'))
        {
            TAP_PrintString((char*)buf);
        }
        if ((action == 'w') || (action == 't'))
        {
            TAP_PrintString((char*)&buf[SPI_MAX_BUFFER_LENGTH >> 1]);
        }

        TAP_Printf("\n\r");
        TAP_PrintString(TAP_OKText);
    }

    TAP_PrintString("\r\n");
}


inline static void user_pull_cs_high(char csPort, uint8_t csPin, uint8_t dividerExponent)
{
    if (csPort != '0') // User control CS
    {
        // Pull it high
        GPIO_SetBits(userCsGpioBank, (0x0001) << csPin);
    }

}
