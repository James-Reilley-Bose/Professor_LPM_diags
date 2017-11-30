//
// WirelessAudioI2C.c
//

#include "project.h"
#include "i2cMaster_API.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioVariant.h"
#include "WirelessAudioData.h"

#define WIRELESS_AUDIO_8B_ADDR 0x80
#define WIRELESS_AUDIO_16B_ADDR 0x82
#define WIRELESS_AUDIO_REG_LEN 2

#define I2C_ACTIVITY_DELAY TIMER_MSEC_TO_TICKS(10)

static BOOL WirelessAudio_I2C_NeedsByteOrderFlipped(WA_Cmd_t command);
static void WirelessAudio_I2C_DelayAfterActivity(void);

static I2C_BUS_HANDLE_TYPE waI2CHandle = NULL;

void WirelessAudio_I2C_Init(void)
{
    I2C_API_ERROR_CODE error = I2C_API_NO_ERROR;
    I2C_Master_Config_t waI2CConfig = {LLR_I2C_BUS,0,I2C_MASTER_CLOCK_STANDARD_SPEED_HZ,1,0};
    waI2CHandle = i2cMaster_Init(&waI2CConfig, &error);
    debug_assert(error == I2C_API_NO_ERROR);
    i2cMaster_DisableBus(LLR_I2C_BUS);
}

void WirelessAudio_I2C_Enable(BOOL enable)
{
    if(enable)
    {
        i2cMaster_EnableBus(LLR_I2C_BUS);
    }
    else
    {
        i2cMaster_DisableBus(LLR_I2C_BUS);
    }
}

void WirelessAudio_I2C_Disable(void)
{
    i2cMaster_DisableBus(LLR_I2C_BUS);
}

BOOL WirelessAudio_I2C_WriteCommand(const void* data, WA_Cmd_t command, uint32_t length)
{
    debug_assert(data && length);
    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {0, command};
    uint16_t bytesWritten = i2cMaster_WriteRegister(waI2CHandle, WIRELESS_AUDIO_8B_ADDR, reg, WIRELESS_AUDIO_REG_LEN, data, length);
    WirelessAudio_I2C_DelayAfterActivity();
    return (length == bytesWritten);
}

BOOL WirelessAudio_I2C_Write8bReg(const void* data, WA_Reg_t regOffset, uint32_t length)
{
    debug_assert(data && length);
    uint16_t regAddr = WA_REGISTER_BASE_ADDR + regOffset;
    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {(regAddr & 0xFF00) >> 8, regAddr & 0xFF};
    uint16_t bytesRead = i2cMaster_WriteRegister(waI2CHandle, WIRELESS_AUDIO_8B_ADDR, reg, WIRELESS_AUDIO_REG_LEN, data, length);
    WirelessAudio_I2C_DelayAfterActivity();
    return (length == bytesRead);
}

BOOL WirelessAudio_I2C_Write16bReg(const void* data, WA_Reg_t regOffset, uint32_t length)
{
    debug_assert(data && length);
    uint16_t regAddr = WA_REGISTER_BASE_ADDR + regOffset;
    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {(regAddr & 0xFF00) >> 8, regAddr & 0xFF};
    uint16_t bytesRead = i2cMaster_WriteRegister(waI2CHandle, WIRELESS_AUDIO_16B_ADDR, reg, WIRELESS_AUDIO_REG_LEN, data, length);
    WirelessAudio_I2C_DelayAfterActivity();
    return (length == bytesRead);
}

BOOL WirelessAudio_I2C_Write16bRegRawAddr(const void* data, uint16_t address, uint32_t length)
{
    debug_assert(data && length);
    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {(address & 0xFF00) >> 8, address & 0xFF};
    uint16_t bytesRead = i2cMaster_WriteRegister(waI2CHandle, WIRELESS_AUDIO_16B_ADDR, reg, WIRELESS_AUDIO_REG_LEN, data, length);
    WirelessAudio_I2C_DelayAfterActivity();
    return (length == bytesRead);
}

BOOL WirelessAudio_I2C_Read(WA_Cmd_t readCommand, void* data, uint32_t length)
{
    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {0, readCommand};
    uint16_t bytesRead = i2cMaster_ReadRegister(waI2CHandle,
                                                WIRELESS_AUDIO_8B_ADDR,
                                                reg,
                                                WIRELESS_AUDIO_REG_LEN,
                                                data,
                                                length);

    // only support for 16-bit for now, don't know if we need 32
    if(WirelessAudio_I2C_NeedsByteOrderFlipped(readCommand))
    {
        uint16_t* p16bit = data;
        *p16bit = (*p16bit >> 8) | (*p16bit << 8);
    }

    WirelessAudio_I2C_DelayAfterActivity();

    return (length == bytesRead);
}

BOOL WirelessAudio_I2C_ReadReg(WA_Reg_t regOffset, void* data, uint32_t length)
{
    uint16_t regAddr = WA_REGISTER_BASE_ADDR + regOffset;
    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {(regAddr & 0xFF00) >> 8, regAddr & 0xFF};
    uint16_t bytesRead = i2cMaster_ReadRegister(waI2CHandle,
                                                WIRELESS_AUDIO_8B_ADDR,
                                                reg,
                                                WIRELESS_AUDIO_REG_LEN,
                                                data,
                                                length);
    WirelessAudio_I2C_DelayAfterActivity();

    return (length == bytesRead);
}

BOOL WirelessAudio_I2C_Read16bReg(WA_Reg_t regOffset, void* data, uint32_t length)
{
    uint16_t regAddr = WA_REGISTER_BASE_ADDR + regOffset;
    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {(regAddr & 0xFF00) >> 8, regAddr & 0xFF};
    uint16_t bytesRead = i2cMaster_ReadRegister(waI2CHandle,
                         WIRELESS_AUDIO_16B_ADDR,
                         reg,
                         WIRELESS_AUDIO_REG_LEN,
                         data,
                         length);
    WirelessAudio_I2C_DelayAfterActivity();

    return (length == bytesRead);
}

BOOL WirelessAudio_I2C_SendDataMessage(WA_BoseCmd_t command, const void* data, uint32_t length)
{
    uint8_t dataBuff[WA_DATA_PKT_LEN] = {0};
    uint32_t address = WirelessAudioUtilities_GetBroadcastAddress();
    dataBuff[0] = (address & 0xFF0000) >> 16;
    dataBuff[1] = (address & 0x00FF00) >> 8;
    dataBuff[2] = (address & 0x0000FF);
    dataBuff[3] = WA_WIRELESS_MSG_ID;
    dataBuff[4] = WA_BOSE_COMMAND_ID;
    dataBuff[5] = command;
    memcpy(&dataBuff[6], data, length);

    uint8_t reg[WIRELESS_AUDIO_REG_LEN] = {0, WA_BOSE_COMMAND};

    uint16_t bytesRead = i2cMaster_WriteRegister(waI2CHandle, WIRELESS_AUDIO_8B_ADDR, reg, WIRELESS_AUDIO_REG_LEN, dataBuff, length + 6);
    WirelessAudio_I2C_DelayAfterActivity();
    
    return (length == bytesRead);
}

BOOL WirelessAudio_I2C_ReadDataMessage(void* message)
{
    return WirelessAudio_I2C_Read(WA_CMD_READ_MESSAGE, message, sizeof(WA_DataMessage_t));
}

static BOOL WirelessAudio_I2C_NeedsByteOrderFlipped(WA_Cmd_t command)
{
    return (command == WA_CMD_READ_CUS_GLOBAL_FLAG);
}


static void WirelessAudio_I2C_DelayAfterActivity(void)
{
    vTaskDelay(I2C_ACTIVITY_DELAY);
}
