/**
  ******************************************************************************
  * @file    Diag_i2c_test.c
  *           + Diagnostic test for I2C
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stdlib.h"
#include "i2cMaster_API.h"
#include "Diag_i2c_test.h"

SCRIBE_DECL(diag);

#define I2CPINGDELAY 64  //Ms
 
char dev_list[256]; 
uint32_t cmd_i2cscan(int argc, char* argv[])
{
    uint32_t bus = 0;
    uint8_t addr;
    int log = 0;
    // I2C_InitTypeDef I2C_InitStruct;

    if (argc > 0)
    {
        bus = atoi(argv[0]);
        if (argc > 1)
        {
            log = 1;
        }
    }

    if ((bus > 0) && (bus < 4))
    {
        if (!log)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Scan i2c bus %d\r\n", bus);
        }
        else
        {
            //      LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Scan i2c bus %d\r\n", bus);
        }

#if 1
        I2C_Master_Config_t myI2cConfig;
        myI2cConfig.busNumber = bus;
        myI2cConfig.masterClockFrequency = 400000;
        myI2cConfig.disableDMA = 1;

        I2C_API_ERROR_CODE i2cError = I2C_API_NO_ERROR;
        I2C_BUS_HANDLE_TYPE myI2cHandle = i2cMaster_Init(&myI2cConfig, &i2cError);
#endif



        //    I2C_InitStruct.I2C_Ack = 1;
        //    I2C_InitStruct.I2C_ClockSpeed = 400000;
        //    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
        //    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;

        switch (bus)
        {
            case 1:
                //      I2C_InitStruct.I2C_AcknowledgedAddress = 0x20;
                //      I2C_InitStruct.I2C_OwnAddress1 = 0x20;
                //      I2C_Init(I2C1, &I2C_InitStruct);
                I2C1->OAR1 = 0x4020;
                I2C1->OAR2 = 0x21;
                break;
            case 2:
                //      I2C_InitStruct.I2C_AcknowledgedAddress = 0x40;
                //      I2C_InitStruct.I2C_OwnAddress1 = 0x40;
                //      I2C_Init(I2C2, &I2C_InitStruct);
                I2C2->OAR1 = 0x4040;
                I2C2->OAR2 = 0x41;
                break;
            case 3:
                //      I2C_InitStruct.I2C_AcknowledgedAddress = 0x60;
                //      I2C_InitStruct.I2C_OwnAddress1 = 0x60;
                //      I2C_Init(I2C3, &I2C_InitStruct);
                I2C3->OAR1 = 0x4060;
                I2C3->OAR2 = 0x61;
                break;
        }



#if 1
        char buffer[40];
        int devfound = 0;
        sprintf(buffer, "bus %d: ", bus);
        strcpy(dev_list, buffer);
        for (addr = 0; addr < 254; addr += 2)
        {
            if (i2cMaster_Ping(myI2cHandle, addr) != FALSE)
            {
                sprintf(buffer, "%02x ", addr);
                strcat(dev_list, buffer);
                devfound++;
            }
            //without a delay subsequent pings may fail to discover a device
            //      Delay_ms(I2CPINGDELAY);
        }

        sprintf(buffer, " - %d devices found\n\r", devfound);
        strcat(dev_list, buffer);

        if (!log)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\r\n%s\n\r", dev_list);
        }
        else
        {
            sprintf(buffer, "\r\n");
            strcat(dev_list, buffer);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n%s\n\r", dev_list);
        }


#else

        for (addr = 0; addr < 254; addr += 2)
        {
            if (((addr % 32) == 0) || (addr == 0))
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\r\n%02x:", addr);
            }
            if (i2cMaster_Ping(myI2cHandle, addr) == FALSE)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, " .");
            }
            else
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%02x", addr);
            }
            //without a delay subsequent pings may fail to discover a device
            Delay_ms(I2CPINGDELAY);
        }
#endif


    }
    else
    {
        if (!log)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Invalid arg, %s", I2CSCAN_HELP);
        }
        else
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "Invalid arg, %s", I2CSCAN_HELP);
        }
        return ERROR;
    }
    return SUCCESS;
}
