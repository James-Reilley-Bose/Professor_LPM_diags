#include "project.h"
#include "variant_config.h"
#include "nvram.h"
#include "PowerTask.h"

SCRIBE(timer, ROTTEN_LOGLEVEL_NORMAL);

/*===============================================================================
@func lpmBoardVariantInit
@brief Initialize stuff for LPM board variants
===============================================================================*/
void VariantInit(void)
{
    nvram_init();
    InitializeScribes();
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
