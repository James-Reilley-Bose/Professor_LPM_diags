//
// PowerRiviera.c - Interface to PMIC
//

#include "project.h"
#include "PowerRiviera.h"

SCRIBE_DECL(power);

#define RIVIERA_PON_RESET_BOOTUP_TIMEOUT_MS 100
#define RIVIERA_PON_RESET_SHUTDOWN_TIMEOUT_MS 5000
#define RIVIERA_PS_HOLD_TIMEOUT_MS 240

static BOOL PowerRiviera_TurnOn(void);
static void PowerRiviera_TurnOff(void);
static void PowerRiviera_EnableTranslator(BOOL enable);
static BOOL PowerRiviera_WaitForPinTransition(GPIO_TypeDef* bank, uint16_t pin, BitAction state, unsigned int timeout);

#ifndef INVERT_RIVIERA_SIGNALS
#error "Please define INVERT_RIVIERA_SIGNALS <0|1> in the product configuration."
#endif

// Not all signals require inversion
#if INVERT_RIVIERA_SIGNALS
#define KYPD_POWER_INIT_STATE GPIO_LOW
#define PMIC_RESIN_INIT_STATE GPIO_LOW
#define CBL_PWR_INIT_STATE GPIO_LOW
#define TRANS_EN_INIT_STATE GPIO_HIGH
#else
#define KYPD_POWER_INIT_STATE GPIO_HIGH
#define PMIC_RESIN_INIT_STATE GPIO_HIGH
#define CBL_PWR_INIT_STATE GPIO_HIGH
#define TRANS_EN_INIT_STATE GPIO_LOW
#endif

static const GPIO_INIT_t RivieraPowerPins[] =
{
    // outputs
    {RIVIERA_TRANSLATOR_EN_BANK, RIVIERA_TRANSLATOR_EN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, TRANS_EN_INIT_STATE},
    {RIVIERA_CBL_POWER_BANK, RIVIERA_CBL_POWER_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, CBL_PWR_INIT_STATE},
    {RIVIERA_KYPD_POWER_BANK, RIVIERA_KYPD_POWER_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, KYPD_POWER_INIT_STATE},
    {RIVIERA_PMIC_RESIN_BANK, RIVIERA_PMIC_RESIN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, PMIC_RESIN_INIT_STATE},
    {RIVIERA_PON_1_BANK, RIVIERA_PON_1_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {RIVIERA_FORCE_USB_BOOT_BANK, RIVIERA_FORCE_USB_BOOT_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},

    //inputs
    {RIVIERA_MSM_RESOUT_BANK, RIVIERA_MSM_RESOUT_PIN, GPIO_MODE_IN_PULLDN, NO_ALT_FUNC, GPIO_NONE},
    {RIVIERA_RESET_REQ_BANK, RIVIERA_RESET_REQ_PIN, GPIO_MODE_IN_FLOATING, NO_ALT_FUNC, GPIO_NONE},
    {RIVIERA_PS_HOLD_BANK, RIVIERA_PS_HOLD_PIN, GPIO_MODE_IN_FLOATING, NO_ALT_FUNC, GPIO_NONE},
    {RIVIERA_PON_RESET_BANK, RIVIERA_PON_RESET_PIN, GPIO_MODE_IN_FLOATING, NO_ALT_FUNC, GPIO_NONE},
};

static const GPIO_INIT_t RivieraSpiPins[] =
{
    {RIVIERA_SPI_CLK_BANK, RIVIERA_SPI_CLK_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {RIVIERA_SPI_MOSI_BANK, RIVIERA_SPI_MOSI_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {RIVIERA_SPI_MISO_BANK, RIVIERA_SPI_MISO_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {RIVIERA_SPI_CS_BANK, RIVIERA_SPI_CS_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {RIVIERA_RTS_GPIO_BANK, RIVIERA_RTS_GPIO_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {RIVIERA_CTS_GPIO_BANK, RIVIERA_CTS_GPIO_PIN, GPIO_MODE_IN_PULLDN, NO_ALT_FUNC, GPIO_LOW},
};

void PowerRiviera_Init(void)
{
    ConfigureGpioPins(RivieraPowerPins, sizeof(RivieraPowerPins) / sizeof(GPIO_INIT_t));
    ConfigureGpioPins(RivieraSpiPins, sizeof(RivieraSpiPins) / sizeof(GPIO_INIT_t));
    PowerRiviera_EnableTranslator(TRUE);
}

void PowerRiviera_Enable(BOOL enable)
{
    if (enable)
    {
        PowerRiviera_TurnOn();
    }
    else
    {
        PowerRiviera_TurnOff();
    }
}

void PowerRiviera_Reset(void)
{
    PowerRiviera_TurnOff();
    vTaskDelay(TIMER_SEC_TO_TICKS(1)); // just a guess
    PowerRiviera_TurnOn();
}

static BOOL PowerRiviera_TurnOn(void)
{
    BOOL success = TRUE;
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "Starting 8017 power up sequence.");

    PowerRiviera_EnableTranslator(TRUE);
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1 << RIVIERA_PMIC_RESIN_PIN), INVERT_RIVIERA_SIGNALS ? Bit_RESET : Bit_SET);
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1 << RIVIERA_KYPD_POWER_PIN), INVERT_RIVIERA_SIGNALS ? Bit_SET : Bit_RESET);
    vTaskDelay(TIMER_MSEC_TO_TICKS(65));
    success = PowerRiviera_WaitForPinTransition(RIVIERA_PON_RESET_BANK,
              RIVIERA_PON_RESET_PIN,
              INVERT_RIVIERA_SIGNALS ? Bit_RESET : Bit_SET,
              RIVIERA_PON_RESET_BOOTUP_TIMEOUT_MS);
    if (success)
    {
        success = PowerRiviera_WaitForPinTransition(RIVIERA_PS_HOLD_BANK,
                  RIVIERA_PS_HOLD_PIN,
                  INVERT_RIVIERA_SIGNALS ? Bit_RESET : Bit_SET,
                  RIVIERA_PS_HOLD_TIMEOUT_MS);
    }
    vTaskDelay(TIMER_MSEC_TO_TICKS(10)); //wait 10MS before pulling KYPD high
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1 << RIVIERA_KYPD_POWER_PIN), INVERT_RIVIERA_SIGNALS ? Bit_RESET : Bit_SET);
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "8017 power up %s.", success ? "successful" : "unsuccessful");
    return success;
}

static void PowerRiviera_TurnOff(void)
{
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1 << RIVIERA_PMIC_RESIN_PIN), INVERT_RIVIERA_SIGNALS ? Bit_SET : Bit_RESET);
    vTaskDelay(TIMER_MSEC_TO_TICKS(10));
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1 << RIVIERA_KYPD_POWER_PIN), INVERT_RIVIERA_SIGNALS ? Bit_SET : Bit_RESET);
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));
    GPIO_WriteBit(RIVIERA_PMIC_RESIN_BANK, (1 << RIVIERA_PMIC_RESIN_PIN), INVERT_RIVIERA_SIGNALS ? Bit_RESET : Bit_SET);
    vTaskDelay(TIMER_SEC_TO_TICKS(8));
    GPIO_WriteBit(RIVIERA_KYPD_POWER_BANK, (1 << RIVIERA_KYPD_POWER_PIN), INVERT_RIVIERA_SIGNALS ? Bit_RESET : Bit_SET);
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "Waiting for PON_RESET");
    PowerRiviera_WaitForPinTransition(RIVIERA_PON_RESET_BANK,
                                      RIVIERA_PON_RESET_PIN,
                                      INVERT_RIVIERA_SIGNALS ? Bit_SET : Bit_RESET,
                                      RIVIERA_PON_RESET_SHUTDOWN_TIMEOUT_MS);
    PowerRiviera_EnableTranslator(FALSE);
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "PON_RESET wait complete");
}

static void PowerRiviera_EnableTranslator(BOOL enable)
{
    if (INVERT_RIVIERA_SIGNALS)
    {
        enable = !enable;
    }

    GPIO_WriteBit(RIVIERA_TRANSLATOR_EN_BANK,
                  (1 << RIVIERA_TRANSLATOR_EN_PIN),
                  (enable ? Bit_SET : Bit_RESET));
}

static BOOL PowerRiviera_WaitForPinTransition(GPIO_TypeDef* bank, uint16_t pin, BitAction state, unsigned int timeout)
{
    BOOL success = TRUE;
    unsigned int startTime = GET_SYSTEM_UPTIME_MS();
    while (GPIO_ReadInputDataBit(bank, (1 << pin)) != state)
    {
        vTaskDelay(TIMER_MSEC_TO_TICKS(1));
        if (GET_MILLI_SINCE(startTime) > timeout)
        {
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "Wait for pin %d failed.", pin);
            success = FALSE;
            break;
        }
    }
    return success;
}
