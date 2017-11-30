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

#include "project.h"
#include "product_config.h"
#include "BackLightTask.h"
#include "TaskDefs.h"
#include "TaskManager.h"

#include "IPCRouterTask.h"
   
#define CLKPCS           24000000
#define LCD_BCKLIGHT_PSC 120 // pre-scaler for max freq = 400KHz
#define VERBOSE          0

#if VERBOSE
    SCRIBE(bsentask, ROTTEN_LOGLEVEL_VERBOSE);
#else
    SCRIBE(bsentask, ROTTEN_LOGLEVEL_NORMAL);
#endif // VERBOSE

ManagedTask* backLightTaskHandle = NULL;

static const GPIO_INIT_t lcd_back_light_gpios = {LCD_BACKLIGHT_BANK, LCD_BACKLIGHT_PIN, GPIO_MODE_AF_OUT_PP_50MHZ, GPIO_AF_TIM1, GPIO_NONE};
static uint8_t           intensity_percent    = 0;

static void BackLight_Init(void)
{    
    ConfigureGpioPins(&lcd_back_light_gpios, sizeof(lcd_back_light_gpios) / sizeof(GPIO_INIT_t));
}// BackLight_Init

// ==================================================
// TODO: FIX ME: can we read the value from the chip?
// ==================================================
ipc_uint8_t BackLightTask_Get() 
{ 
#if VERBOSE
    LOG(bsentask, ROTTEN_LOGLEVEL_VERBOSE, "%s:%d, setting back light intensity to: %u", __FUNCTION__, __LINE__, intensity_percent);
#endif // VERBOSE
    
    return intensity_percent; 
}// BackLightTask_Get

// =================================================
// TODO: FIX ME: Mike M. has a better way to do this
// =================================================
void BackLightTask_Set (ipc_uint8_t intensity)
{
    float    clkpsc = (CLKPCS / LCD_BCKLIGHT_PSC) * 2;
    float    arrflt = (uint16_t)(clkpsc / 100); //Freq 100  
    uint16_t ccr3   = (uint16_t)(arrflt * ((float) intensity / 100.0));
    
#if VERBOSE
    LOG(bsentask, ROTTEN_LOGLEVEL_VERBOSE, "%s:%d, setting back light intensity to: %u%", __FUNCTION__, __LINE__, intensity);
#endif // VERBOSE    

    TIM1->CCMR1       = 0x0068;   //OC1M=6, OC1E
    TIM1->CCMR2       = 0x0068;   //OC3M=6, OC3E
    TIM1->CCER        = 0x0101;   //CC3E  enable channel 3
    TIM1->PSC         = (uint16_t)LCD_BCKLIGHT_PSC - 1;    
    TIM1->ARR         = (uint16_t)arrflt;      //Freq
    TIM1->CCR1        = ccr3;     //PWM
    TIM1->CCR3        = ccr3;     //PWM
    TIM1->CR1         = 0x81;     //auto preload, en
    TIM1->BDTR        = 0x8000;   //MOE
    intensity_percent = intensity;
}// BackLightTask_Set

/**
 * @brief initialize back light task
 * @param p
 */
void BackLight_TaskInit(void* p)
{
    BackLight_Init();
    backLightTaskHandle = GetManagedTaskPointer(BACK_LIGHT_TASK_NAME);
    debug_assert (NULL != backLightTaskHandle);
}

/**
 * @func BackLightTask
 *
 * @brief The task, handles back light events
 *
 * @param void *pvParameters
 *
 * @return n/a
 */
void BackLightTask(void* pvParamaters)
{
    for (;;)
    {
        TaskManagerPollQueue(backLightTaskHandle);
    }
}// BackLightTask

/**
 * @brief Handle queue messages from other tasks
 * @param msg
 */
void BackLightTask_HandleMessage(GENERIC_MSG_t* msg)
{
    if ((msg->msgID > BACKLIGHT_MSG_ID_Start) && (msg->msgID < BACKLIGHT_MSG_ID_End))
    {
        switch (msg->msgID)
        {
            case BACKLIGHT_MSG_ID_HandleIPC:
#if VERBOSE              
                LOG(bsentask, ROTTEN_LOGLEVEL_VERBOSE, "%s:%d handle IPC", __FUNCTION__, __LINE__);
#endif // VERBOSE                
              
                IPCRouter_ReturnRxBuffer((IpcPacket_t*)msg->params[0]);
                break;

            case BACKLIGHT_MSG_ID_ReadEvent:
#if VERBOSE              
                LOG(bsentask, ROTTEN_LOGLEVEL_VERBOSE, "%s:%d read event", __FUNCTION__, __LINE__);
#endif // VERBOSE                
                break;
        }
    }
}// BackLightTask_HandleMessage

void BackLightTask_IpcPacketHandler(IpcPacket_t* packet)
{
    switch (packet->s.opcode)
    {
        case IPC_PER_GET_BACKLIGHT: 
        {
            IpcBackLight_t back_light = {BackLightTask_Get()};
          
            IpcSendPacket_t pData =
            {
                .dest         = (Ipc_Device_t) packet->s.sender,
                .op           = IPC_PER_GET_BACKLIGHT,
                .params       = NULL,
                .data         = (void*) &back_light,
                .length       = sizeof(back_light),
                .sequence     = packet->s.sequence,
                .connectionID = packet->s.connectionID,
            };
                           
            IPCRouter_SendResponse(&pData);
            break;
        }// IPC_PER_GET_BACKLIGHT
          
        case IPC_PER_SET_BACKLIGHT:
        {
            IpcBackLight_t* back_light = (IpcBackLight_t*) packet->s.data.b;
          
            BackLightTask_Set(back_light->value);
            break;
        }// IPC_PER_SET_BACKLIGHT
    }// switch packet->s.opcode
  
}// BackLightTask_PacketHandler
