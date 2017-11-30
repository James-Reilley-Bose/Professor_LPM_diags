#include "project.h"
#include "etapI2C.h"
#include "etap.h"
#include "stdio.h"

SCRIBE_DECL(tap_test_results);

#define ERROR_DESCRIPTION_STRING_LENGTH 18


TAPCommand(TAP_IICCommand)
{
    char action;
    uint8_t bus, addr, reg[I2C_MAX_REGISTER_LENGTH], lenReg;
    uint16_t lenData, actualLoops = 1;
    uint32_t readTimeMs = 0, writeTimeMs = 0, start, regInput;
    char errorDescription[ERROR_DESCRIPTION_STRING_LENGTH] = "I2C Error";
    BOOL valid[MINIMAL_I2C_NUMBER_ARGUMENTS] = {FALSE, FALSE, FALSE, FALSE};

    lenReg = regInput = 0;

    if (CommandLine->numArgs < MINIMAL_I2C_PING_NUMBER_ARGUMENTS)
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    // Get arguments
    action = TAP_ToLowerCase(CommandLine->args[0][0]);
    bus    = TAP_DecimalArgToInt(CommandLine, 1, &valid[0]);
    addr   = TAP_HexArgToInt(CommandLine, 2, &valid[1]);

    if (action != 'p')
    {
        if (CommandLine->numArgs < MINIMAL_I2C_PING_NUMBER_ARGUMENTS)
        {
            TAP_PrintString(TAP_InvalidArg);
            return;
        }
        regInput     = TAP_HexArgToInt(CommandLine, 3, &valid[2]);
        lenReg = TAP_DecimalArgToInt(CommandLine, 4, &valid[3]);
        lenData = TAP_DecimalArgToInt(CommandLine, 5, &valid[4]);
    }

    // Validate arguments
    if ( ((action != 'p') && (action != 'r') && (action != 'w') && (action != 't')) ||
            ((action == 'p') && (CommandLine->numArgs != MINIMAL_I2C_PING_NUMBER_ARGUMENTS)) ||
            ((action == 'r') && (CommandLine->numArgs != MINIMAL_I2C_NUMBER_ARGUMENTS)) ||
            ((action == 'w') && (CommandLine->numArgs == MINIMAL_I2C_NUMBER_ARGUMENTS)) ||
            ((action == 't') && (CommandLine->numArgs != (MINIMAL_I2C_NUMBER_ARGUMENTS + 1))) ||
            !valid[0] || !valid[1] ||
            ((action != 'p') && (!lenData || (lenData > I2C_MAX_DATA_LENGTH)
                                 || !valid[2] || !valid[3] || !valid[4]) ))
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    if (bus == 0)
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    for (uint8_t i = 0; i < lenReg; i++)
    {
        reg[i] = ( (0xFF) & (regInput >> ((lenReg - 1 - i) * 8)));
    }

    I2C_Master_Config_t myI2cConfig;
    myI2cConfig.busNumber = bus;
    myI2cConfig.masterClockFrequency = 400000;
    myI2cConfig.disableDMA = 1;

    I2C_API_ERROR_CODE i2cError = I2C_API_NO_ERROR;

    I2C_BUS_HANDLE_TYPE myI2cHandle = i2cMaster_Init(&myI2cConfig, &i2cError);

    if (myI2cHandle == NULL)
    {
        TAP_PrintString(TAP_OutOfRange);
        return;
    }

    uint8_t buf[I2C_MAX_DATA_LENGTH];
    uint16_t bufIndex = 0, ret = 0;
    int i;


    switch (action)
    {
        case 'p':
            if (i2cMaster_Ping(myI2cHandle, addr) == FALSE)
            {
                TAP_PrintString("Ping Failed");
            }
            else
            {
                TAP_PrintString("Ping OK");
            }
            return;

        case 'w':     // Write cmd
            //Convert to array of bytes
            valid[0] = FALSE;
            for (uint8_t dataIndex = MINIMAL_I2C_NUMBER_ARGUMENTS;
                    dataIndex < CommandLine->numArgs; dataIndex++)
            {
                buf[bufIndex++] = TAP_HexArgToInt(CommandLine, dataIndex, &valid[0]);
                if (!valid[0])
                {
                    TAP_Printf("Argument %d is invalid.\r\n", dataIndex);
                    break;
                }
            }

            if (valid[0])
            {
                addr &= (~I2C_Direction_Receiver);

                start = GET_INSTRUMENT_TIME_NOW();
                if (regInput == I2C_SLAVE_NO_REGISTER)
                {
                    ret = i2cMaster_WriteNoRegister(myI2cHandle, addr, (uint8_t const*)buf, lenData);
                }
                else
                {
                    ret = i2cMaster_WriteRegister(myI2cHandle, addr, reg, lenReg, (uint8_t const*)buf, lenData);
                }

                writeTimeMs = MEASURE_TIME_SINCE(start);

            }
            break;

        case 'r':
            addr = addr | I2C_Direction_Receiver;

            start = GET_INSTRUMENT_TIME_NOW();
            if (regInput == I2C_SLAVE_NO_REGISTER)
            {
                ret = i2cMaster_ReadNoRegister(myI2cHandle, addr, buf, lenData);
            }
            else
            {
                ret = i2cMaster_ReadRegister(myI2cHandle, addr, reg, lenReg, buf, lenData);
            }

            readTimeMs = MEASURE_TIME_SINCE(start);

            if (ret > 0)
            {
                LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL, "I2C%d Data (%d bytes): ", bus, ret);
                for (bufIndex = 0; bufIndex < ret; bufIndex++)
                {
                    TAP_Printf("%02x\n", buf[bufIndex]);
                }
            }
            break;

        case 't':
            // The array valid[] should be all TRUE at this point,
            // reuse them for different purposes as below
            valid[0] = FALSE; // Reuse for data validation
            // valid[1] = TRUE means no comparison error

            int loopCount = TAP_DecimalArgToInt(CommandLine, MINIMAL_I2C_NUMBER_ARGUMENTS, &valid[0]);
            if (!valid[0] || (loopCount > MAX_ETAP_I2C_LOOP_COUNT) || (loopCount <= 0) )
            {
                TAP_Printf("Argument for testing is invalid.\r\n");
                break;
            }

            for (i = 0; i < loopCount; i++)
            {
                // Use current start time as "random" data pattern
                uint8_t patternStart = GET_INSTRUMENT_TIME_NOW() & 0xFF;
                for (bufIndex = 0; bufIndex < lenData; bufIndex++)
                {
                    buf[bufIndex] = patternStart + bufIndex;
                }

                // Start measuring time
                start = GET_INSTRUMENT_TIME_NOW();

                if (regInput == I2C_SLAVE_NO_REGISTER)
                {
                    ret = i2cMaster_WriteNoRegister(myI2cHandle, addr, (uint8_t const*)buf, lenData);
                }
                else
                {
                    ret = i2cMaster_WriteRegister(myI2cHandle, addr, reg, lenReg, (uint8_t const*)buf, lenData);
                }

                writeTimeMs += MEASURE_TIME_SINCE(start);

                if (ret != lenData) // Write failure
                {
                    strcpy(errorDescription, "I2C write error");
                    break; // i-loop
                }

                memset(buf, 0, lenData);

                // Dealy a bit to allow the slave to absorb data
                vTaskDelay(TIMER_MSEC_TO_TICKS(8));

                start = GET_INSTRUMENT_TIME_NOW();

                if (regInput == I2C_SLAVE_NO_REGISTER)
                {
                    ret = i2cMaster_ReadNoRegister(myI2cHandle, addr, buf, lenData);
                }
                else
                {
                    ret = i2cMaster_ReadRegister(myI2cHandle, addr, reg, lenReg, buf, lenData);
                }

                readTimeMs += MEASURE_TIME_SINCE(start);

                if (ret != lenData ) // Read failure
                {
                    strcpy(errorDescription, "I2C read error");
                    break; // i-loop
                }

                for (bufIndex = 0; bufIndex < lenData; bufIndex++)
                {
                    if (buf[bufIndex] != ((uint8_t)(patternStart + bufIndex))) // Compare failure
                    {
                        strcpy(errorDescription, "I2C compare error");
                        valid[1] = FALSE;
                        break; // bufIndex-loop
                    }
                }

                if (valid[1] == FALSE)
                {
                    break;    // i-loop
                }

            } // end of i-loop

            actualLoops += i;
            break;
    }

    readTimeMs /= actualLoops;
    writeTimeMs /= actualLoops;

    if ((ret != lenData) || !valid[1])
    {
        TAP_PrintString("\r\n");
        TAP_PrintString(errorDescription);
        LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL, "%s ", errorDescription);
    }
    else
    {
        sprintf((char*)buf, "ReadTime = %u us\r\n", readTimeMs);
        sprintf((char*)&buf[I2C_MAX_DATA_LENGTH >> 1], "WriteTime = %u us\r\n", writeTimeMs);

        if ((action == 'r') || (action == 't'))
        {
            TAP_PrintString((char*)buf);
        }
        if ((action == 'w') || (action == 't'))
        {
            TAP_PrintString((char*)&buf[I2C_MAX_DATA_LENGTH >> 1]);
        }

        TAP_Printf(TAP_NEW_LINE);
        TAP_PrintString(TAP_OKText);
    }

    TAP_PrintString("\r\n");
}
