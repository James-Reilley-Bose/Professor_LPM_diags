/**
 *
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#include <math.h>
#include "project.h"
#include "i2cMaster_API.h"
#include "etap.h"
#include "product_config.h"
#include "TaskDefs.h"
#include "TaskManager.h"
#include "IPCRouterTask.h"
#include "LightSensorTask.h"

// ===========================================================================================================
// Based on James Reilley's code in the LPM diag
// ===========================================================================================================
// See: OPT3001 specs: SB0S682B July 2014
// ===========================================================================================================
// REGISTER        ADDRESS 15  14  13  12  11   10   9   8   7   6   5   4   3   2   1   0
// Result          00h     E3  E2  E1  E0  R11  R10  R9  R8  R7  R6  R5  R4  R3  R2  R1  R0
//                         This register contains the result of the most recent light to digital conversion.
//                         This 16-bit register has two fields: a4-bit exponent and a 12-bit mantissa.
//
// Configuration   01h     RN3 RN2 RN1 RN0 CT   M1   M0  OVF CRF FH  FL  L   POL ME  FC1 FC0   reset = C810h
// Low Limit       02h     LE3 LE2 LE1 LE0 TL11 TL10 TL9 TL8 TL7 TL6 TL5 TL4 TL3 TL2 TL1 TL0   reset = C0000h
// High Limit      03h     HE3 HE2 HE1 HE0 TH11 TH10 TH9 TH8 TH7 TH6 TH5 TH4 TH3 TH2 TH1 TH0   reset = BFFFh
// Manufacturer ID 7Eh     reset = 5449h = "TI"
// Device ID       7Fh     reset = 3001h
// ===========================================================================================================

#define OPT_I2C3_ADDR              0x88
#define OPT3001_RESULT_READY_SHORT 150
#define OPT3001_RESULT_READY_LONG  1000
#define OPT_RES_REGADDR            0
#define OPT_CFG_REGADDR            1
#define OPT_CFG_MODE               0xce10  // continuous conversion mode [10:9]=3
#define OPT_LL_REGADDR             2
#define OPT_HL_REGADDR             3
#define OPT_MFG_ID_REGADDR         0x7e
#define OPT_DEV_ID_REGADDR         0x7f
#define OPT3001_REG_EXPONENT(n)    (((n) >> 12) & 0x000f)
#define OPT3001_REG_MANTISSA(n)    ((n) & 0x0fff)                 

typedef struct{
    uint16_t  fc:2; // Fault count field (read or write)
    uint16_t  me:1; // Mask exponent field (read or write)
    uint16_t pol:1; // Polarity field (read or write)
    uint16_t   l:1; // Latch field (read or write)
    uint16_t  fl:1; // Flag low field (read-only)
    uint16_t  fh:1; // Flag high field (read-only)
    uint16_t crf:1; // Conversion ready field (read-only)
    uint16_t ovf:1; // Overflow flag field (read-only)
    uint16_t   m:2; // Conversion mode (shutdown/00, Single-shot/01, Continuous/11)
    uint16_t  ct:1; // Conversion time field (read or write)
    uint16_t  rn:4; // Range number field (read or write)
}OPT3001_CFG_BITS;

typedef union{
    OPT3001_CFG_BITS b;
    uint8_t          cb[2];
    uint16_t         w;
}OPT3001_CONFIG_REG;

static I2C_BUS_HANDLE_TYPE opt3001_i2c_handle         = NULL;
static uint32_t            opt3001_i2c_bus            = PSOC_I2C_BUS;
static I2C_Master_Config_t opt3001_i2c_config         = {0};
static ManagedTask*        light_sensor_task_handle   = NULL;
static BOOL                conversion_mode_continuous = TRUE;

static ErrorStatus opt3001_init                    (void);
static ErrorStatus opt3001_read                    (uint8_t  reg_addr , uint16_t* reg_value);
static ErrorStatus opt3001_write                   (uint8_t  reg_addr , uint16_t  reg_value);
static ErrorStatus opt3001_result_reg_to_lux       (uint16_t reg_value, uint16_t* lux_decimal, uint16_t* lux_fractional);
static ErrorStatus LightSensorTask_Read_Continuous (uint16_t* lux_decimal, uint16_t* lux_fractional);
static ErrorStatus LightSensorTask_Read_Single_Shot(uint16_t* lux_decimal, uint16_t* lux_fractional);

SCRIBE(lsentask, ROTTEN_LOGLEVEL_NORMAL);

static ErrorStatus opt3001_init(void)
{
    I2C_API_ERROR_CODE i2cError    = I2C_API_NO_ERROR;
    uint8_t            reg_addr    = OPT_CFG_REGADDR;
    uint8_t            reg_data[2] = {0,0};
    OPT3001_CONFIG_REG opt_cfg     = {0};
  
    opt3001_i2c_config.busNumber            = opt3001_i2c_bus;
    opt3001_i2c_config.masterClockFrequency = I2C_MASTER_CLOCK_FAST_SPEED_HZ; 
    opt3001_i2c_config.disableDMA           = 1;
    opt3001_i2c_handle                      = i2cMaster_Init(&opt3001_i2c_config, &i2cError);
  
    if ((opt3001_i2c_handle == NULL) || (i2cError != I2C_API_NO_ERROR)){
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to init I2C master: %d", __FUNCTION__, __LINE__, i2cError);
        return ERROR;
    }
  
    if (i2cMaster_Ping(opt3001_i2c_handle, OPT_I2C3_ADDR) == FALSE){
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to find I2C device: %d", __FUNCTION__, __LINE__, OPT_I2C3_ADDR);
        return ERROR;
    }
  
    opt_cfg.b.rn  = 0xc; // full scale range     , see: "Light Sensor recommandations" document from Fred Hamway
    opt_cfg.b.ct  = 1;   // conversion time 800ms, see: "Light Sensor recommandations" document from Fred Hamway
    opt_cfg.b.m   = 3;   // Continuous conversion, see: "Light Sensor recommandations" document from Fred Hamway
    opt_cfg.b.ovf = 0;   // overflow
    opt_cfg.b.crf = 0;   // conversion ready
    opt_cfg.b.fh  = 0;   // (0)default off
    opt_cfg.b.fl  = 0;   // (0)default off
    opt_cfg.b.l   = 1;   // latch int
    opt_cfg.b.pol = 0;   // int pol
    opt_cfg.b.me  = 0;   // mask exponent in results
    opt_cfg.b.fc  = 0;   // fault cnt(default=0, 1 fault causes int
    
    reg_data[0] = opt_cfg.cb[1];   //MSB
    reg_data[1] = opt_cfg.cb[0];
  
    if (i2cMaster_WriteRegister(opt3001_i2c_handle, OPT_I2C3_ADDR, &reg_addr, 1, reg_data, sizeof(reg_data)) == 0)
    {
        LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to find write register", __FUNCTION__, __LINE__);
        return ERROR;
    }
  
    return SUCCESS;
}// opt3001_init

static ErrorStatus opt3001_read(uint8_t reg_addr, uint16* reg_value)
{
    uint8_t opt3001_reg_value[2] = {0, 0};  
      
    if (opt3001_i2c_handle == NULL){
        LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: invalid I2C handle", __FUNCTION__, __LINE__); 
        return ERROR;
    }
    
    if (i2cMaster_Ping(opt3001_i2c_handle, OPT_I2C3_ADDR) == FALSE){
        LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: device not found at I2C addr: %d", __FUNCTION__, __LINE__, OPT_I2C3_ADDR);
        return ERROR;
    }
       
    if (i2cMaster_WriteNoRegister(opt3001_i2c_handle, OPT_I2C3_ADDR, &reg_addr, sizeof(reg_addr)) != sizeof(reg_addr))
    {
        LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to write register", __FUNCTION__, __LINE__); 
        return ERROR;
    }

    if (i2cMaster_ReadNoRegister(opt3001_i2c_handle, OPT_I2C3_ADDR, opt3001_reg_value, sizeof(opt3001_reg_value)) != sizeof(opt3001_reg_value))
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to read register", __FUNCTION__, __LINE__); 
        return ERROR;
    }
      
    *reg_value = (opt3001_reg_value[0] << 8) | opt3001_reg_value[1];
    
    return SUCCESS;
}// opt3001_read

static ErrorStatus opt3001_write(uint8_t reg_addr, uint16 reg_value)
{ 
    uint8_t opt3001_reg_value[2] = {(reg_value & 0xFF00) >> 8, reg_value & 0x00FF}; 
   
    if (opt3001_i2c_handle == NULL){
        LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: invalid I2C handle", __FUNCTION__, __LINE__); 
        return ERROR;
    }
    
    if (i2cMaster_Ping(opt3001_i2c_handle, OPT_I2C3_ADDR) == FALSE){
        LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: device not found at I2C addr: %d", __FUNCTION__, __LINE__, OPT_I2C3_ADDR);
        return ERROR;
    }

    if (i2cMaster_WriteRegister(opt3001_i2c_handle, OPT_I2C3_ADDR, &reg_addr, 1, opt3001_reg_value, sizeof(opt3001_reg_value)) != sizeof(opt3001_reg_value))
    {
        LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to write to register", __FUNCTION__, __LINE__);
        return ERROR;
    }
  
    return SUCCESS;
}// opt3001_write

static ErrorStatus opt3001_result_reg_to_lux(uint16_t reg_value, uint16_t* lux_decimal, uint16_t* lux_fractional)
{
    // OPT3001 specs: SB0S682B July 2014 p.20, table 8, 9
    static float lsb_size[16] = {0.01, 0.02,  0.04,  0.08,  0.16,  0.32,  0.64,  1.28, 
                                 2.56, 5.12, 10.24, 20.48, 20.48, 20.48, 20.48, 20.48};                     
    uint8_t  exponent         = OPT3001_REG_EXPONENT(reg_value); 
    uint32_t mantissa         = OPT3001_REG_MANTISSA(reg_value);
    float    lux              = lsb_size[exponent] * (float) mantissa;
    float    float_decimal    = 0;
    
    if (exponent > 0xB) 
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: invalid exponent", __FUNCTION__, __LINE__); 
        return ERROR;
    }
     
    *lux_decimal    = (uint16_t) lux;
    float_decimal   = (float) *lux_decimal;
    *lux_fractional = (uint16_t) ((lux - float_decimal) * 1000.0);
             
    return SUCCESS;
}// opt3001_result_reg_to_lux

BOOL LightSensorTask_Read(uint16_t* lux_decimal, uint16_t* lux_fractional)
{
    if (conversion_mode_continuous)
    {
        return LightSensorTask_Read_Continuous(lux_decimal, lux_fractional) == SUCCESS;
    }
  
    return LightSensorTask_Read_Single_Shot(lux_decimal, lux_fractional) == SUCCESS;
}// LightSensorTask_Read

static ErrorStatus LightSensorTask_Read_Continuous(uint16_t* lux_decimal, uint16_t* lux_fractional)
{
    uint16_t reg_res_value = 0;
     
    if ((opt3001_i2c_handle == NULL) && (opt3001_init() != SUCCESS))
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to initialize I2C device", __FUNCTION__, __LINE__); 
        return ERROR;
    }
    
    if (opt3001_read(OPT_RES_REGADDR, &reg_res_value) != SUCCESS)
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to read result register", __FUNCTION__, __LINE__); 
        return ERROR;    
    }
    
    if (opt3001_result_reg_to_lux(reg_res_value, lux_decimal, lux_fractional) != SUCCESS)
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to convert lux value", __FUNCTION__, __LINE__); 
        return ERROR;                 
    }

    return SUCCESS;
}// LightSensorTask_Read_Continuous

static ErrorStatus LightSensorTask_Read_Single_Shot(uint16_t* lux_decimal, uint16_t* lux_fractional)
{
    OPT3001_CONFIG_REG config_reg          = {0};
    measure_time_t     conv_time           = OPT3001_RESULT_READY_SHORT;
    int                max_converstion_try = 10;
    uint16_t           reg_res_value       = 0;
    
    if ((opt3001_i2c_handle == NULL) && (opt3001_init() != SUCCESS))
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to initialize I2C device", __FUNCTION__, __LINE__); 
        return ERROR;
    }
    
    if (opt3001_read(OPT_CFG_REGADDR, (uint16_t*) &config_reg) != SUCCESS)
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to read configuration register", __FUNCTION__, __LINE__); 
        return ERROR;    
    }
    
    config_reg.b.m = 1; //set single conversion bit
    
    if (opt3001_write(OPT_CFG_REGADDR, config_reg.w) != SUCCESS)
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to write configuration register", __FUNCTION__, __LINE__); 
        return ERROR;    
    }
    
    do 
    {
        if (opt3001_read(OPT_CFG_REGADDR, (uint16_t*) &config_reg) != SUCCESS)
        {
            LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to read configuration register", __FUNCTION__, __LINE__); 
            return ERROR;    
        }
        
        if (config_reg.b.crf)
        {
            if (opt3001_read(OPT_RES_REGADDR, &reg_res_value) != SUCCESS)
            {
                LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to read result register", __FUNCTION__, __LINE__); 
                return ERROR;    
            }
            
            if (opt3001_result_reg_to_lux(reg_res_value, lux_decimal, lux_fractional) != SUCCESS)
            {
                LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to convert lux value", __FUNCTION__, __LINE__); 
                return ERROR;                 
            }

            return SUCCESS;
        }// If the conversion is completed
      
        max_converstion_try--;
        Delay_ms(conv_time);
    }
    while (max_converstion_try);
    
    LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to acquire light sensor value", __FUNCTION__, __LINE__);
    return ERROR;
}// LightSensorTask_Read_Single_Shot

/**
 * @brief initialize light sensor task
 * @param p
 */
void LightSensor_TaskInit(void* p)
{
    light_sensor_task_handle = GetManagedTaskPointer(LIGHT_SENSOR_TASK_NAME);
    debug_assert (NULL != light_sensor_task_handle);   
}// LightSensor_TaskInit

/**
 * @func LightSensorTask
 *
 * @brief The task, handles light sensor events
 *
 * @param void *pvParameters
 *
 * @return n/a
 */
void LightSensorTask(void* pvParamaters)
{
    for (;;) TaskManagerPollQueue(light_sensor_task_handle);
}// LightSensorTask

/**
 * @brief Handle queue messages from other tasks
 * @param msg
 */
void LightSensorTask_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case LIGHTSENSOR_MSG_ID_HandleIPC: IPCRouter_ReturnRxBuffer((IpcPacket_t*)msg->params[0]);
                                           break;
        case LIGHTSENSOR_MSG_ID_ReadEvent: break;
        case LIGHTSENSOR_MSG_ID_Get      : LightSensorTask_IpcPacketHandler((IpcPacket_t*) msg->params);
                                           break;
    }// switch on message ID
}// LightSensorTask_HandleMessage

void LightSensorTask_IpcPacketHandler(IpcPacket_t* packet)
{
    switch (packet->s.opcode)
    {
        case IPC_PER_GET_LIGHTSENSOR: 
        {
            IpcLightSensor_t light_sensor = {0, 0};
                                
            if (LightSensorTask_Read (&light_sensor.lux_decimal_value, &light_sensor.lux_fractional_value) == FALSE) 
            {
                LOG(lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d error: failed to read light sensor LUX value", __FUNCTION__, __LINE__);
            }

            IpcSendPacket_t pData =
            {
                .dest         = (Ipc_Device_t) packet->s.sender,
                .op           = IPC_PER_GET_LIGHTSENSOR,
                .params       = NULL,
                .data         = (void*) &light_sensor,
                .length       = sizeof(light_sensor),
                .sequence     = packet->s.sequence,
                .connectionID = packet->s.connectionID,
            };
                           
            IPCRouter_SendResponse(&pData);
            break;
        }// IPC_PER_GET_LIGHTSENSOR
    }
}// LightSensorTask_PacketHandler

BOOL LightSensorTask_PostMsg (MESSAGE_ID_t msg_id, uint32_t* params)
{
    return QueueManagerPostCallbackMsg(&light_sensor_task_handle->Queue, msg_id, params, NOP_CALLBACK, NO_BLOCK_TIME) == 0 ? TRUE : FALSE;
}// LightSensorTask_PostMsg

BOOL LightSensorTask_SetConversionMode(BOOL continuous)
{    
    OPT3001_CONFIG_REG config_reg = {0};
    
    if (conversion_mode_continuous == continuous)
    {
        return TRUE;
    }// If we are already in the right conversion mode
    
    if ((opt3001_i2c_handle == NULL) && (opt3001_init() != SUCCESS))
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to initialize I2C device", __FUNCTION__, __LINE__); 
        return FALSE;
    }
    
    if (opt3001_read(OPT_CFG_REGADDR, (uint16_t*) &config_reg) != SUCCESS)
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to read configuration register", __FUNCTION__, __LINE__); 
        return FALSE;    
    }
    
    config_reg.b.m = continuous ? 3 : 0;
    
    if (opt3001_write(OPT_CFG_REGADDR, config_reg.w) != SUCCESS)
    {
        LOG (lsentask, ROTTEN_LOGLEVEL_NORMAL, "%s:%d, error: failed to write configuration register", __FUNCTION__, __LINE__); 
        return FALSE;    
    }
    
    conversion_mode_continuous = continuous;
  
    return TRUE;
}// LightSensorTask_SetConversionMode