#include "project.h"
#include "product_config.h"
#include "nvram.h"
#include "PowerTask.h"
#include "nv_mfg.h"
#include "DiagsDispatcher.h"
#include "SpiIpcTask.h"

SCRIBE(demo, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(diag, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(keys, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(wa_task, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(wa_update, ROTTEN_LOGLEVEL_NORMAL);

/*===============================================================================
@func lpmBoardVariantInit
@brief Initialize stuff for LPM board variants
===============================================================================*/
void VariantInit(void)
{
    NV_InitMfgParams();
    nvram_init();
    InitializeScribes();

    //new 4_10_17
    amp_init();
    darr_init();
    nfc_init();
    ledbar_init();

}

/*===============================================================================
@func InitBoardVariantScribes
@brief Initialize board specific scribes
@return Number of scribes initialized in this function
===============================================================================*/
uint8_t InitVariantScribes(void)
{
    uint8_t numberOfScribes = 0;

    INIT_LOG_SCRIBE(demo, ROTTEN_LOGLEVEL_INSANE, numberOfScribes);
    INIT_LOG_SCRIBE(diag, ROTTEN_LOGLEVEL_INSANE, numberOfScribes);
    INIT_LOG_SCRIBE(keys, ROTTEN_LOGLEVEL_INSANE, numberOfScribes);
    INIT_LOG_SCRIBE(wa_task, ROTTEN_LOGLEVEL_INSANE, numberOfScribes);
    INIT_LOG_SCRIBE(wa_update, ROTTEN_LOGLEVEL_INSANE, numberOfScribes);

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

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(RIVIERA_CTS_IRQ_LINE) != RESET)
    {
        EXTI_ClearITPendingBit(RIVIERA_CTS_IRQ_LINE);
        BaseType_t taskUnblocked = pdFALSE;
        xSemaphoreGiveFromISR(SpiIpcTask_GetCtsSemHandle(), &taskUnblocked);
        portYIELD_FROM_ISR(taskUnblocked);
    }
}
