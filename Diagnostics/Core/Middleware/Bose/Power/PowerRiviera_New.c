//
// PowerRiviera.c - Interface to PMIC
//

#include "project.h"
#include "PowerRiviera.h"

SCRIBE_DECL(power);

#define RIVIERA_PON_RESET_BOOTUP_TIMEOUT_MS 100
#define RIVIERA_PON_RESET_SHUTDOWN_TIMEOUT_MS 5000
#define RIVIERA_PS_HOLD_TIMEOUT_MS 240
#define RIVIERA_S3_RESET_DELAY_MS 3000
#define RIVIERA_PON_RESET_S3_TIMEOUT_MS 1650

static BOOL PowerRiviera_TurnOn(void);
static void PowerRiviera_TurnOff(void);
static void PowerRiviera_EnableTranslator(BOOL enable);
static BOOL PowerRiviera_WaitForPinTransition(GPIO_TypeDef* bank, uint16_t pin, BitAction state, unsigned int timeout);

#if (RIVIERA_KYPD_POWER_ACTIVE_STATE == GPIO_LOW)
#define KYPD_POWER_INIT_STATE GPIO_HIGH
#else
#define KYPD_POWER_INIT_STATE GPIO_LOW
#endif

#if (RIVIERA_PMIC_RESIN_ACTIVE_STATE == GPIO_LOW)
#define PMIC_RESIN_INIT_STATE GPIO_HIGH
#else
#define PMIC_RESIN_INIT_STATE GPIO_LOW
#endif

#if (RIVIERA_TRANSLATOR_ACTIVE_STATE == GPIO_HIGH)
#define TRANS_EN_INIT_STATE GPIO_LOW
#else
#define TRANS_EN_INIT_STATE GPIO_HIGH
#endif

#if (RIVIERA_FORCE_USB_BOOT_ACTIVE_STATE == GPIO_HIGH)
#define FORCE_USB_BOOT_INIT_STATE GPIO_LOW
#else
#define FORCE_USB_BOOT_INIT_STATE GPIO_HIGH
#endif

static const GPIO_INIT_t RivieraPowerPins[] =
{
    // outputs
#ifdef RIVIERA_TRANSLATOR_EN_BANK // not all products have this
    {RIVIERA_TRANSLATOR_EN_BANK, RIVIERA_TRANSLATOR_EN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, TRANS_EN_INIT_STATE},
#endif
    {RIVIERA_KYPD_POWER_BANK, RIVIERA_KYPD_POWER_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, KYPD_POWER_INIT_STATE},
    {RIVIERA_PMIC_RESIN_BANK, RIVIERA_PMIC_RESIN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, PMIC_RESIN_INIT_STATE},
    {RIVIERA_FORCE_USB_BOOT_BANK, RIVIERA_FORCE_USB_BOOT_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, FORCE_USB_BOOT_INIT_STATE},

    //inputs
#ifdef RIVIERA_RESET_REQ_BANK // not all products have this
    {RIVIERA_RESET_REQ_BANK, RIVIERA_RESET_REQ_PIN, GPIO_MODE_IN_FLOATING, NO_ALT_FUNC, GPIO_NONE},
#endif
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
    WriteGPIO(RIVIERA_PMIC_RESIN_BANK, RIVIERA_PMIC_RESIN_PIN, RIVIERA_PMIC_RESIN_ACTIVE_STATE, TRUE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(RIVIERA_S3_RESET_DELAY_MS));
    BOOL success = TRUE;
#if PON_RESET_DISCONNECTED
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "Skipping PON_RESET check - disconnected.");
#else
    success = PowerRiviera_WaitForPinTransition(RIVIERA_PON_RESET_BANK,
              RIVIERA_PON_RESET_PIN,
              (RIVIERA_PON_RESET_ACTIVE_STATE == GPIO_HIGH) ? Bit_SET : Bit_RESET,
              RIVIERA_PON_RESET_S3_TIMEOUT_MS);
#endif
    WriteGPIO(RIVIERA_PMIC_RESIN_BANK, RIVIERA_PMIC_RESIN_PIN, RIVIERA_PMIC_RESIN_ACTIVE_STATE, FALSE);
    if(!success)
    {
        LOG(power, ROTTEN_LOGLEVEL_NORMAL, "S3 reset timed out.");
    }
}

static BOOL PowerRiviera_TurnOn(void)
{
    BOOL success = TRUE;
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "Starting 8017 power up sequence.");

    PowerRiviera_EnableTranslator(TRUE);
    WriteGPIO(RIVIERA_PMIC_RESIN_BANK, RIVIERA_PMIC_RESIN_PIN, RIVIERA_PMIC_RESIN_ACTIVE_STATE, FALSE);
    WriteGPIO(RIVIERA_KYPD_POWER_BANK, RIVIERA_KYPD_POWER_PIN, RIVIERA_KYPD_POWER_ACTIVE_STATE, TRUE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(65));
#if PON_RESET_DISCONNECTED
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "Skipping PON_RESET check - disconnected.");
#else
    success = PowerRiviera_WaitForPinTransition(RIVIERA_PON_RESET_BANK,
              RIVIERA_PON_RESET_PIN,
              (RIVIERA_PON_RESET_ACTIVE_STATE == GPIO_HIGH) ? Bit_SET : Bit_RESET,
              RIVIERA_PON_RESET_BOOTUP_TIMEOUT_MS);
#endif
    if (success)
    {
        success = PowerRiviera_WaitForPinTransition(RIVIERA_PS_HOLD_BANK,
                  RIVIERA_PS_HOLD_PIN,
                  (RIVIERA_PS_HOLD_ACTIVE_STATE == GPIO_HIGH) ? Bit_SET : Bit_RESET,
                  RIVIERA_PS_HOLD_TIMEOUT_MS);
    }
    vTaskDelay(TIMER_MSEC_TO_TICKS(10));
    WriteGPIO(RIVIERA_KYPD_POWER_BANK, RIVIERA_KYPD_POWER_PIN, RIVIERA_KYPD_POWER_ACTIVE_STATE, FALSE);
    LOG(power, ROTTEN_LOGLEVEL_NORMAL, "8017 power up %s.", success ? "successful" : "unsuccessful");
    return success;
}

static void PowerRiviera_TurnOff(void)
{
    PowerRiviera_Reset();
}

static void PowerRiviera_EnableTranslator(BOOL enable)
{
#ifdef RIVIERA_TRANSLATOR_EN_BANK // not all products have this
    WriteGPIO(RIVIERA_TRANSLATOR_EN_BANK, RIVIERA_TRANSLATOR_EN_PIN, RIVIERA_TRANSLATOR_ACTIVE_STATE, enable);
#endif
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
