#include "project.h"
#include "product_config.h"
#include "nvram.h"
#include "PowerTask.h"
#include "nv_mfg.h"
#include "nv_system.h"
#include "IpcI2CRxTxTask.h"
#include "SpiIpcTask.h"
#include "Sii9437Task.h"
#include "edidapi.h"
#include "A4VEDID.h"
#include "SystemAnalysis.h"
#include "WirelessAudioTask.h"
#include "LightBarCapSenseTask.h"
#include "EventDefinitions.h"
#include "WiredIDTask.h"
#include "i2cMaster_API.h"
#include "RemoteTask.h"

// How did this ever build before?  This was checked in previously with these uncommented
// However, now if you uncomment them you get link errors
//SCRIBE(annotate, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(demo, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(device_model_dsp, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(diag, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(flash, ROTTEN_LOGLEVEL_DISABLED);
//SCRIBE(ipc, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(ipc_bulk, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(ipc_pt, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(ipc_raw, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(ipc_router, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(ipc_rx, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(ipc_tx, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(ipc_update, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(i2c, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(keys, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(power, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(sourcechange, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(system, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(sys_events, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(tap_events, ROTTEN_LOGLEVEL_DISABLED);
//SCRIBE(tap_test_results, ROTTEN_LOGLEVEL_NORMAL);
//SCRIBE(timer, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(wa_task, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(wa_update, ROTTEN_LOGLEVEL_NORMAL);

/*
===============================================================================
@func EXTI1_IRQHandler
@brief External interrupt handler for EXTI_Line1
===============================================================================
*/
int diags_cts_int_cnt=0;
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(RIVIERA_CTS_IRQ_LINE) != RESET)
    {
        diags_cts_int_cnt++;
        EXTI_ClearITPendingBit(RIVIERA_CTS_IRQ_LINE);
        BaseType_t taskUnblocked = pdFALSE;
        xSemaphoreGiveFromISR(SpiIpcTask_GetCtsSemHandle(), &taskUnblocked);
        portYIELD_FROM_ISR(taskUnblocked);
    }
}
/*
===============================================================================
@func EXTI2_IRQHandler
@brief External interrupt handler for EXTI_Line2
===============================================================================
*/
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EARC_IRQ_LINE) != RESET)
    {
        EXTI_ClearITPendingBit(EARC_IRQ_LINE);
        Sii9437PostMsg(SII9437_MESSAGE_ID_ServiceInterrupt, NOP_CALLBACK, 0);
    }
    if (EXTI_GetITStatus(WIRED_ID_INTERRUPT_LINE) != RESET)
    {
      EXTI_ClearITPendingBit(WIRED_ID_INTERRUPT_LINE);
      WiredIDPostMsg(WIREDID_InputPulse, NOP_CALLBACK, NULL);
    }
}

/*===============================================================================
@func lpmBoardVariantInit
@brief Initialize stuff for LPM board variants
===============================================================================*/
void VariantInit(void)
{  
    NV_InitMfgParams();
    NV_CreateSystemParameterBlock();
    NV_SetBootState(BL_STATE_BOOT_DIAG);
    nvram_init();
    InitializeScribes();
    
    // TODO: (lb1032816) Temporary solution, find a more appropraite place for this.
    EDID_SetSpkrAllocation((uint8_t*)A4V_SPEAKER_ALLOCATION);
    EDID_AddAudioCodec(AUDIO_ID_LPCM,  CHANNELS_8, RATE_ALL_RATES, BITS_ALL_BIT_DEPTH);
    EDID_AddAudioCodec(AUDIO_ID_AC3,   CHANNELS_6, RATE_32_KHZ | RATE_44_1_KHZ | RATE_48_KHZ, RATE_KHZ_TO_AUDIO_DESCRIPTOR(640 /*kHz*/));
    EDID_AddAudioCodec(AUDIO_ID_DTS,   CHANNELS_6, RATE_32_KHZ | RATE_44_1_KHZ | RATE_48_KHZ, RATE_KHZ_TO_AUDIO_DESCRIPTOR(1536 /*kHz*/));
    EDID_PrepRefEDID();
}

/*===============================================================================
@func InitBoardVariantScribes
@brief Initialize board specific scribes
@return Number of scribes initialized in this function
===============================================================================*/
uint8_t InitVariantScribes(void)
{
    uint8_t numberOfScribes = 0;

    INIT_LOG_SCRIBE(timer, ROTTEN_LOGLEVEL_INSANE, numberOfScribes);

    return numberOfScribes;
}

// This overrides the function in SystemCoreReboot.c
void system_reboot(void)
{
    PowerAPI_SetPowerState(POWER_STATE_LOW_POWER);

    /* shut down the UART so we don't see
       junk on the TAP output */
    USART_DeInit(TAP_UART);

    // not really necessary, but doesn't hurt
    __disable_irq();

    // resets EXTI (which I think we need here)
    // and the other alternate functions
    SYSCFG_DeInit();

    /* Now reboot */
    NVIC_SystemReset();
}

/* IpcI2CTask Configurations */
void IpcI2CTask_ConfigureEXTI()
{
    /*
    *   In stm32f2xx_exti.c, there is :
    *          In order to use an I/O pin as an external interrupt source, follow
    *          steps below:
    *            1- Configure the I/O in input mode using GPIO_Init()
    *            2- Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig()
    *            3- Select the mode(interrupt, event) and configure the trigger
    *               selection (Rising, falling or both) using EXTI_Init()
    *            4- Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init()
    */
    EXTI_InitTypeDef EXTI_InitStructure;

    // step 1,   ConfigureGpioPin() calls GPIO_Init()
    ConfigureGpioPin(F0_I2C_INTERRUPT_GPIO_BANK, F0_I2C_INTERRUPT_GPIO_PIN, GPIO_MODE_IN_FLOATING, 0);

    //step 2
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, F0_I2C_INTERRUPT_GPIO_PIN);

    // step 3.
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Line    = F0_I2C_INTERRUPT_LINE;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_ClearITPendingBit(F0_I2C_INTERRUPT_LINE);

    // step 4
    // Register the ISR
    Interrupt_RegisterISR(F0_I2C_INTERRUPT_CHANNEL, F0_I2C_INTERRUPT_HANDLER);
    // Enable it
    Interrupt_Enable(F0_I2C_INTERRUPT_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(F0_I2C_INTERRUPT_LINE) != RESET)
    {
        EXTI_ClearITPendingBit(F0_I2C_INTERRUPT_LINE);
        IpcI2CRxTxPostMsg(IPC_I2C_MESSAGE_ID_Receive, IPC_DEVICE_F0, NOP_CALLBACK);
    }

    if (EXTI_GetITStatus(DWAM_INTERRUPT_LINE) != RESET)
    {
        INTERRUPT_RUN_LATENCY_BEGIN(DWAM_E);
        EXTI_ClearITPendingBit(DWAM_INTERRUPT_LINE);
        WirelessAudioPostMsg(WA_MSG_ID_HandleEXTI,NOP_CALLBACK,0);
        INTERRUPT_RUN_LATENCY_END(DWAM_E,0);
    }

    if(EXTI_GetITStatus(REMOTE_INTERRUPT_LINE) != RESET)
    {
        EXTI_ClearITPendingBit(REMOTE_INTERRUPT_LINE);
        RemoteTaskPostMsg(REMOTE_MESSAGE_IRQ_CHANGED, 0);
        portYIELD_FROM_ISR(pdTRUE);
    }
}

void Sii9437_EnableEXTI()
{
    /*
    *   In stm32f2xx_exti.c, there is :
    *          In order to use an I/O pin as an external interrupt source, follow
    *          steps below:
    *            1- Configure the I/O in input mode using GPIO_Init()
    *            2- Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig()
    *            3- Select the mode(interrupt, event) and configure the trigger
    *               selection (Rising, falling or both) using EXTI_Init()
    *            4- Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init()
    */
    EXTI_InitTypeDef EXTI_InitStructure;

    // step 1,   ConfigureGpioPin() calls GPIO_Init()
    ConfigureGpioPin(EARC_IRQ_GPIO_BANK, EARC_IRQ_GPIO_PIN, GPIO_MODE_IN_PULLUP, 0);

    //step 2
    SYSCFG_EXTILineConfig(EARC_IRQ_EXTI_PORT_SOURCE, EARC_IRQ_GPIO_PIN);

    // step 3.
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Line    = EARC_IRQ_LINE;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_ClearITPendingBit(EARC_IRQ_LINE);

    // step 4
    // Register the ISR
    Interrupt_RegisterISR(EARC_IRQ_CHANNEL, EARC_IRQ_HANDLER);
    // Enable it
    Interrupt_Enable(EARC_IRQ_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

I2C_BUS_HANDLE_TYPE PSoCTask_ConfigureI2C(void)
{
    I2C_Master_Config_t config =
    {
        .busNumber = PSOC_I2C_BUS,
        .masterClockFrequency = PSOC_I2C_SPEED,
        .disableDMA = 1,
    };
    I2C_API_ERROR_CODE error;
    I2C_BUS_HANDLE_TYPE handle = i2cMaster_Init(&config, &error);
    debug_assert(I2C_API_NO_ERROR == error);
    debug_assert(NULL != handle);

    return handle;
}

void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(PSOC_READ_IRQ_LINE) != RESET)
    {
        EXTI_ClearITPendingBit(PSOC_READ_IRQ_LINE);
        QueueManagerPostCallbackMsg(&lbcsTaskHandle->Queue, LBCS_MSG_ID_ReadEvent, (uint32_t []){NULL},NOP_CALLBACK, NO_BLOCK_TIME);
       portYIELD_FROM_ISR(pdTRUE);
    }
}


extern ManagedTask* DiagTaskHandle;

int gbl_psoc_int_det = 0;
void Psoc_Int_Handler(void)
{
#if 1
   if (EXTI_GetITStatus(PSOC_READ_IRQ_LINE) != RESET)
    {
      gbl_psoc_int_det += 1;
//      EXTI->IMR &= ~PSOC_READ_IRQ_LINE;  // mask it
      EXTI_ClearITPendingBit(PSOC_READ_IRQ_LINE);
      QueueManagerPostCallbackMsg(&DiagTaskHandle->Queue, DIAG_MESSAGE_ID_PSOC_READ_CMD, (uint32_t []){NULL},NOP_CALLBACK, NO_BLOCK_TIME);
//      EXTI_ClearITPendingBit(PSOC_READ_IRQ_LINE);
//      EXTI->IMR |= PSOC_READ_IRQ_LINE;  // enable it
     portYIELD_FROM_ISR(pdTRUE);
    }
    
#else
    if (EXTI_GetITStatus(PSOC_READ_IRQ_LINE) != RESET)
    {
      gbl_psoc_int_det += 1;
      EXTI_ClearITPendingBit(PSOC_READ_IRQ_LINE);
      QueueManagerPostCallbackMsg(&lbcsTaskHandle->Queue, LBCS_MSG_ID_ReadEvent, (uint32_t []){NULL},NOP_CALLBACK, NO_BLOCK_TIME);
      portYIELD_FROM_ISR(pdTRUE);
    }
#endif
}

void PSoCTask_ConfigureEXTI(void)
{
    /*
    *   In stm32f2xx_exti.c, there is :
    *          In order to use an I/O pin as an external interrupt source, follow
    *          steps below:
    *            1- Configure the I/O in input mode using GPIO_Init()
    *            2- Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig()
    *            3- Select the mode(interrupt, event) and configure the trigger
    *               selection (Rising, falling or both) using EXTI_Init()
    *            4- Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init()
    */
    EXTI_InitTypeDef EXTI_InitStructure;

    // step 1,   ConfigureGpioPin() calls GPIO_Init()
    ConfigureGpioPin(PSOC_READ_INT_GPIO_BANK, PSOC_READ_INT_GPIO_PIN, GPIO_MODE_IN_PULLDN, 0);

    //step 2
    SYSCFG_EXTILineConfig(PSOC_READ_IRQ_PORT_SOURCE, PSOC_READ_INT_GPIO_PIN);

    // step 3.
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Line    = PSOC_READ_IRQ_LINE;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_ClearITPendingBit(PSOC_READ_IRQ_LINE);

    // step 4
    // Register the ISR
    Interrupt_RegisterISR(PSOC_READ_IRQ_CHANNEL, PSOC_READ_IRQ_HANDLER);
    // Enable it
    Interrupt_Enable(PSOC_READ_IRQ_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

void PSoCInt_en_dis(int en)
{
  if(en){
    Interrupt_Enable(PSOC_READ_IRQ_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
  }else{
    Interrupt_Disable(EXTI3_IRQn);
  }
}