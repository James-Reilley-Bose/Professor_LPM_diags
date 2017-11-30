#include "stm32f2xx_tim.h"
#include "project.h"
#include "EventDefinitions.h"
#include "RivieraLPM_IpcProtocol.h"

#include "WiredIDUtilities.h"
#include "WiredIDTask.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDInterrupt.h"

SCRIBE_DECL(wired_id);

static uint8_t  s_pulse_level = 0;

static uint16_t console_low_time = 0;
static uint16_t console_high_time = 0;

/* 
 * @func    WiredIDVariantUtils_ConfigIO
 * @brief   Config IO during Init
 */
void WiredIDVariantUtils_ConfigIO(void)
{
    ConfigureGpioPin(BASSBOX_DC_RX_GPIO_BANK,
                     BASSBOX_DC_RX_GPIO_PIN,
                     GPIO_MODE_IN_FLOATING,
                     NO_ALT_FUNC);
    
    DEASSERT_GPIO_PIN_FG(CONSOLE_DC_TX_GPIO_BANK, CONSOLE_DC_TX_GPIO_PIN);
    
    ConfigureGpioPin(CONSOLE_DC_TX_GPIO_BANK,
                     CONSOLE_DC_TX_GPIO_PIN,
                     GPIO_MODE_OUT_PP,
                     NO_ALT_FUNC);
#if TEST    
    //Spare PB15 for testing only 
    ConfigureGpioPin(BASSBOX_RX_TEST_BANK,
                     BASSBOX_RX_TEST_PIN,
                     GPIO_MODE_OUT_PP,
                     NO_ALT_FUNC);
#endif
    WiredID_ConfigEdgeDetectEXTI();
}

/* @func WiredIDVariantUtils_GetInputPulseLevel
 * @brief Get Bassbox DC level
 */
uint8_t WiredIDVariantUtils_GetInputPulseLevel(void)
{
    return GPIO_ReadInputDataBit(BASSBOX_DC_RX_GPIO_BANK, BASSBOX_DC_RX_GPIO_PIN_MASK);
}

/* @func  WiredIDVariantUtils_GetOutputPulseLevel
 * @brief Get ConsoleDC_TX level
 */
uint8_t WiredIDVariantUtils_GetOutputPulseLevel(void)
{
    return s_pulse_level;
}

/*
 * @func    WiredIDVariantUtils_SetOutputPulseLevel
 * @brief   Set output pulse level during ID confirmation
 */
void WiredIDVariantUtils_SetOutputPulseLevel(uint8_t val)
{
    s_pulse_level = val;
    GPIO_WriteBit(CONSOLE_DC_TX_GPIO_BANK, CONSOLE_DC_TX_GPIO_PIN_MASK, val? Bit_SET: Bit_RESET);
}

/* 
 * @func   WiredIDConsole_SetOutputPulseTimes
 * @brief  Set output pulse times based on receive pulses from bass
 */
void WiredIDConsole_SetOutputPulseTimes(uint16_t low_time, uint16_t high_time)
{
    console_low_time = low_time;
    console_high_time = high_time;
}

/* 
 * @func    WiredIDConsole_GetOutputPulseTimes
 * @brief   Get the pulse times for confirmation pulses
 */
void WiredIDConsole_GetOutputPulseTimes(uint16_t *low_time, uint16_t *high_time)
{
    *low_time = console_low_time;
    *high_time = console_high_time;
}    

/* 
 * @func   WiredIDConsole_WiredBassConnected
 * @brief  Get bass connection status
 */
BOOL WiredIDConsole_WiredBassConnected(void)
{
    return GPIO_ReadInputDataBit(BASSBOX_DC_RX_GPIO_BANK, BASSBOX_DC_RX_GPIO_PIN_MASK) == Bit_SET;
}
/*
===============================================================================
@func    WiredBass_DC_TX_SetState
@brief   Change DC_TX state.
===============================================================================
*/
void WiredBass_DC_TX_SetState(BOOL enable)
{
    LOG(wired_id, ROTTEN_LOGLEVEL_NORMAL, "Set DC_TX %d", enable);
    if (enable)
    {
        ASSERT_GPIO_PIN_FG(CONSOLE_DC_TX_GPIO_BANK, CONSOLE_DC_TX_GPIO_PIN);
    }
    else
    {
        DEASSERT_GPIO_PIN_FG(CONSOLE_DC_TX_GPIO_BANK, CONSOLE_DC_TX_GPIO_PIN);
    }
}

void WiredIDVariantUtils_EnablePulseIO(void)
{
}

void WiredIDVariantUtils_DisablePulseIO(void)
{
  
}

