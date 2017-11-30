//
// Diag_i2c_test.c -
//

#include "project.h"
#include "LoggerTask.h"
#include "Diag_i2c_test.h"
#include "i2cMaster_API.h"

SCRIBE_DECL(diag);

I2C_BUS_HANDLE_TYPE myI2cHandle = NULL;

I2C_Master_Config_t myI2cConfig;

static uint32_t opt_i2c_bus = NFC_I2C_BUS;
static int opt3001_init(void)
{
    uint8_t i2caddr = OPT_I2C3_ADDR;
    uint16_t wrbytecnt = 0;
    uint8_t pRegAddr[] = {0};
    uint8_t a_pData[] = {0, 0};
    OPT3001_CONFIG_REG opt_cfg;

    myI2cConfig.busNumber = opt_i2c_bus;
    myI2cConfig.masterClockFrequency = I2C_MASTER_CLOCK_FAST_SPEED_HZ;
    myI2cConfig.disableDMA = 1;

    I2C_API_ERROR_CODE i2cError = I2C_API_NO_ERROR;
    myI2cHandle = i2cMaster_Init(&myI2cConfig, &i2cError);
    if (i2cError != I2C_API_NO_ERROR)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\ni2cMaster_Init() failed! error code = %x\n\r", i2cError);
        return ERROR;
    }

    if (i2cMaster_Ping(myI2cHandle, i2caddr) == FALSE)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\ndevice %x not found\n\r", i2caddr);
        return ERROR;
    }

    opt_cfg.b.rn  = 0xc; //(0xc) default full scale range
    opt_cfg.b.ct  = 0;   //(1)default conversion time 800ms, (0)100ms
    opt_cfg.b.m   = 0;   //(0)default, shutdown
    opt_cfg.b.ovf = 0;   //overflow
    opt_cfg.b.crf = 0;   //conversion ready
    opt_cfg.b.fh  = 0;   //(0)default off
    opt_cfg.b.fl  = 0;   //(0)default off
    opt_cfg.b.l   = 1;   //latch int
    opt_cfg.b.pol = 0;   //int pol
    opt_cfg.b.me  = 0;   //mask exponent in results
    opt_cfg.b.fc  = 0;   //fualt cnt(default=0, 1 fault causes int



    pRegAddr[0] = OPT_CFG_REGADDR;
    a_pData[0] = opt_cfg.cb[1];   //MSB
    a_pData[1] = opt_cfg.cb[0];

    wrbytecnt = i2cMaster_WriteRegister(myI2cHandle,
                                        i2caddr,          //const uint8_t slaveAddr,
                                        pRegAddr, 1,      //const uint8_t* pRegAddr, const uint8_t reg_len,
                                        a_pData, 2);      //const uint8_t *a_pData, const uint16_t a_len)

    if (wrbytecnt == 0)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\nerror! bytes written = %d\n\r", wrbytecnt);
        return ERROR;
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "\r\nbytes written = %d\n\r", wrbytecnt);
    }
    return SUCCESS;
}

//i2c write register address and 2 bytes of data, stop
//cmd[0] - cmd type
//cmd[1] - regaddr
//cmd[2] - MSB,LSB
static int opt3001_wr(uint32_t* cmd)
{
    const uint8_t i2caddr = OPT_I2C3_ADDR;
    uint16_t wrbytecnt = 0;
    uint8_t pRegAddr[] = {0};
    uint8_t a_pData[] = {0, 0};

    if (myI2cHandle == NULL)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\nmyI2cHandle not valid!\n\r");
        return ERROR;
    }


    if (i2cMaster_Ping(myI2cHandle, i2caddr) == FALSE)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\ndevice %x not found\n\r", i2caddr);
        return ERROR;
    }

    //write to set the register address for the read
    pRegAddr[0] = cmd[1];
    a_pData[0]  = cmd[2] >> 8; //MSB
    a_pData[1]  = cmd[2];    //LSB

    wrbytecnt = i2cMaster_WriteRegister(myI2cHandle,
                                        i2caddr,          //const uint8_t slaveAddr,
                                        pRegAddr, 1,      //const uint8_t* pRegAddr, const uint8_t reg_len,
                                        a_pData, 2);      //const uint8_t *a_pData, const uint16_t a_len)

    if (wrbytecnt == 0)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\nerror! bytes written = %d\n\r", wrbytecnt);
        return ERROR;
    }

    LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "\r\nbytes written = %d\n\r", wrbytecnt);
    return SUCCESS;
}

//i2c write register address, stop
//i2c read 2 bytes, stop
//cmd[0] - cmd type
//cmd[1] - regaddr
//cmd[2] - MSB,LSB
static int opt3001_rd(uint32_t* cmd)
{
    const uint8_t i2caddr = OPT_I2C3_ADDR;
    uint16_t rdbytecnt = 0, wrbytecnt = 0;
    uint8_t pRegAddr[] = {0};
    uint8_t a_pData[] = {1, 1};

    if (myI2cHandle == NULL)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n%s myI2cHandle not valid!\n\r", __FUNCTION__);
        return ERROR;
    }


    if (i2cMaster_Ping(myI2cHandle, i2caddr) == FALSE)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\ndevice %x not found\n\r", i2caddr);
        return ERROR;
    }

    //write to set the register address for the read
    pRegAddr[0] = cmd[1];

    wrbytecnt = i2cMaster_WriteNoRegister( myI2cHandle,
                                           i2caddr,          //const uint8_t slaveAddr,
                                           pRegAddr, 1);      //const uint8_t* pRegAddr, const uint8_t reg_len,

    if (wrbytecnt != 1)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\nerror! bytes written = %d\n\r", wrbytecnt);
        return ERROR;
    }



    rdbytecnt = i2cMaster_ReadNoRegister(myI2cHandle,
                                         i2caddr,          //const uint8_t slaveAddr,
                                         a_pData, 2);      //uint8_t *a_pData, const uint16_t a_len);
    if (rdbytecnt == 2)
    {
        cmd[2] = a_pData[0]; //MSB
        cmd[2] = ((cmd[2] << 8) & 0xff00) | a_pData[1]; //LSB
        return SUCCESS;
    }

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\nerror! bytes read = %d\n\r", rdbytecnt);
    return ERROR;
}

void opt3001_to_iio_ret(uint16_t regval, int* val, int* val2)
{
    uint32_t lux;

    lux = 10 * ((regval & 0xfff) << ((regval >> 12) & 0xf));
    *val = lux / 1000;
    *val2 = (lux - (*val * 1000)) * 1000;
}

//start a single coversion
//adjust th
int opt3001_rd_lux(uint32_t* lux)
{
    int val, val2;
    int status, cc = 0;
    measure_time_t conv_time = OPT3001_RESULT_READY_SHORT;
    uint32_t cmd[] = {0, OPT_CFG_REGADDR, 0};

    if (myI2cHandle == NULL)
    {
        opt3001_init();
    }

    //get current conv setup, and start a convertion
    if (SUCCESS == opt3001_rd(cmd))
    {
        //    conv_time = (((OPT3001_CONFIG_REG*)(&cmd[2]))->b.ct)?OPT3001_RESULT_READY_LONG:OPT3001_RESULT_READY_SHORT;
        conv_time = OPT3001_RESULT_READY_SHORT;
        ((OPT3001_CONFIG_REG*)(&cmd[2]))->b.m = 1; //set single convertion bit
        opt3001_wr(cmd);
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n%s rd OPT_CFG_REGADDR error!\n\r", __FUNCTION__);
        return ERROR;
    }

    do
    {
        if (SUCCESS == (status = opt3001_rd(cmd)))
        {
            if (((OPT3001_CONFIG_REG*)(&cmd[2]))->b.crf) //convertion complete
            {
                cmd[1] = OPT_RES_REGADDR;
                if (SUCCESS == opt3001_rd(cmd))
                {
                    opt3001_to_iio_ret(cmd[2], &val, &val2);
                    lux[0] = val;
                    lux[1] = val2;
                    break;
                }
            }
            Delay_ms(conv_time);

            if (cc++ > 10)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n%s conversion timeout error!\n\r", __FUNCTION__);
                status = ERROR;
            }
        }
    }
    while (status == SUCCESS);
    return status;
}

//cmd[0] = cmd type <init,wr,rd>
//cmd[1] = register
//cmd[2] = data
void opt_cmd(uint32_t* cmd)
{
    uint32_t lux[2];

    switch (cmd[0])
    {
        case OPT3001_INIT:
            opt3001_init();
            break;
        case OPT3001_WR:
            if (myI2cHandle == NULL)
            {
                opt3001_init();
            }
            opt3001_wr(cmd);
            break;
        case OPT3001_RD:
            if (myI2cHandle == NULL)
            {
                opt3001_init();
            }
            if (SUCCESS == opt3001_rd(cmd))
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\nreg[%02x] = [%04x]\n\r", cmd[1], cmd[2]);
            }
            break;
        case OPT3001_LUX_RD:
            if (myI2cHandle == NULL)
            {
                opt3001_init();
            }
            Delay_ms(cmd[1]);
            if (SUCCESS == opt3001_rd_lux(lux))
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "LUX: %6d.%-2d\n\r",
                    lux[0], lux[1] / 10000);
                cmd[2] = lux[0];
            }
            else
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\nrd lux failed!\n\r");
            }
            break;
        case OPT3001_I2C_BUS:
            if ((cmd[1] > 0) && (cmd[1] < 4))
            {
                opt_i2c_bus = cmd[1];
                opt3001_init();
            }
            break;
        default:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "\r\n%s\n\r", OPT_HELP);
            break;
    }
}

