/*
    File    :   adc.c
    Author  :   Frank Mazzarella
    Created :   06/04/15
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Support for the ADC on the STM32F205


===============================================================================
*/

#include "project.h"
#include "stm32f2xx_gpio.h"
#include "adc.h"

SCRIBE_DECL(power);

#define ADC_TIMEOUT TIMER_MSEC_TO_TICKS(100)

//-------------------------------------------------------------------
// Function:  void ConfigureADC(ADC_TypeDef* ADCx, FunctionalState ADC_ScanConvMode, FunctionalState ADC_ContinuousConvMode)
//
// Description:

//  We are using ADC1 for this implementation.  Since we are only reading
//  the offsets via a TAP command and not monitoring the signals continuously,
//  we set the config for noncontinuous mode, i.e., we only perform a conversion
//  prior to doing a read.
//
//  The software application will probably want to set the config to continuous
//  and also enable an interrupt to fire if the offset voltages should go
//  above or below predetermined values.
//
//  It is not yet clear if an interrupt can
//  be fired if the difference between the L+R offset should exceed a predefined
//  value.
//
//--------------------------------------------------------------------
void ConfigureADC(ADC_TypeDef* ADCx, FunctionalState ADC_ScanConvMode, FunctionalState ADC_ContinuousConvMode)
{
    ADC_InitTypeDef ADC_InitStruct;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;

    /*Enable ADC clock so we can talk to it.*/
    if (ADCx == ADC1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    }
    else if (ADCx == ADC2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    }
    else if (ADCx == ADC3)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    }

    // settings from void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
    ADC_InitStruct.ADC_Resolution             = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode           = ADC_ScanConvMode;
    ADC_InitStruct.ADC_ContinuousConvMode     = ADC_ContinuousConvMode;
    ADC_InitStruct.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_ExternalTrigConv       = ADC_ExternalTrigConv_T1_CC1; // is this == none?
    ADC_InitStruct.ADC_DataAlign              = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfConversion        = 1;

    ADC_Init(ADCx, &ADC_InitStruct);

    // default values for ADC_CommonStructInit()
    ADC_CommonInitStruct.ADC_Mode             = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_Prescaler        = ADC_Prescaler_Div2;
    ADC_CommonInitStruct.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

    ADC_CommonInit(&ADC_CommonInitStruct);
}

//----------------------------------------------------------------------
// Function:  void ADC_Watchdog_init( ADC_TypeDef* ADCx, uint8_t ADC_Channel, IRQn_Type  id, INTERRUPT_FNCT_PTR isr, int16_t HighThreshold, uint16_t LowThreshold )
//
// Description: Enable ADC watchdog
//
// Parameters:
//
// Returns:
//
//----------------------------------------------------------------------
void ADC_Watchdog_init( ADC_TypeDef* ADCx, uint8_t ADC_Channel, IRQn_Type  id, INTERRUPT_FNCT_PTR isr, int16_t HighThreshold, uint16_t LowThreshold )
{
    // Analog watchdog
    ADC_AnalogWatchdogSingleChannelConfig(ADCx, ADC_Channel);
    ADC_AnalogWatchdogThresholdsConfig(ADCx, HighThreshold, LowThreshold);
    ADC_AnalogWatchdogCmd(ADCx, ADC_AnalogWatchdog_SingleRegEnable);

    // Register the ISR
    Interrupt_RegisterISR(id, isr);
    // Enable it
    Interrupt_Enable(id, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);

    // Enable analog watchdog interrupt
    ADC_ITConfig(ADCx, ADC_IT_AWD, ENABLE);
}

//----------------------------------------------------------------------
// Function:  void ADC_Enable( ADC_TypeDef* ADCx )
//
// Description: Enable ADC
//
// Parameters:
//
// Returns:
//
//----------------------------------------------------------------------
void ADC_Enable( ADC_TypeDef* ADCx )
{
    ADC_Cmd(ADCx, ENABLE);
}

//----------------------------------------------------------------------
// Function:  BOOL ADC_isEnabled( ADC_TypeDef* ADCx )
//
// Description: Check whether ADC is enabled.
//
// Parameters:
//
// Returns: TRUE-Enabled; FALSE-not enabled.
//
//----------------------------------------------------------------------
BOOL ADC_isEnabled( ADC_TypeDef* ADCx )
{
    return (ADCx->CR2 & ADC_CR2_ADON);
}
//----------------------------------------------------------------------
// Function:  void ADC_Disable( ADC_TypeDef* ADCx )
//
// Description: Disable ADC conversion and go to power down mode
//
// Parameters:
//
// Returns:
//
//----------------------------------------------------------------------
void ADC_Disable( ADC_TypeDef* ADCx )
{
    ADC_Cmd(ADCx, DISABLE);
}
//-------------------------------------------------------------------
// Function:  ReadOffsetChannelValue()
//
// Description:
//
//  Reads the ADC value from the GPIO pin.
//  This routine will set up the sample time, then tell the ADC
//  to perform a conversion on the GPIO pin (sw conversion).
//  Once the conversion is completed, then the read of the digital
//  data is performed.
//
// Parameters:
//              ADC_TypeDef *ADCx - Pointer to ADC register set.
//              uint8_t channelID - GPIO pin ADC channel ID (0-18)
//
//--------------------------------------------------------------------
uint16_t ReadOffsetChannelValue( ADC_TypeDef* ADCx, uint8_t channelId )
{
    /* Check the parameters */
    if ( !IS_ADC_ALL_PERIPH(ADCx) || !IS_ADC_CHANNEL(channelId) || !ADC_isEnabled(ADCx) )
    {
        return 0;
    }

    ADC_RegularChannelConfig( ADCx, channelId, 1, ADC_SampleTime_3Cycles );

    ADC_SoftwareStartConv( ADCx );

    uint32_t startTime = GET_SYSTEM_UPTIME_MS();
    while (ADC_GetFlagStatus( ADCx, ADC_FLAG_EOC) == RESET )
    {
        if (GET_MILLI_SINCE(startTime) >= ADC_TIMEOUT)
        {
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "ADC timed out reading on channel %d", channelId);
            return 0;
        }
    }

    return ADC_GetConversionValue( ADCx );
}
/// THIS HAS NOT BEEN TESTED FOR Bardeen (or Ginger). MAY NEED FINE TUNING!!- FAM
/// @fn ConvertVoltageToTemp
///
/// @brief Convert ADC voltage to corresponding temperature.
/// These numbers come from an anaylsis done on the datasheet for
/// the thermistor. There are two lines. One interpolated from 10-80*C and the other
/// extrapolated from the points at 80-120*C (to make this critical range more accurate).
///
/// The numbers used are (10*C, 2.87V), (80*C, 1.19V), (120*C, 0.54V)
///
uint16_t ConvertDigitalToTemp(uint16_t adcSteps)
{
    float stepSizeinMV = .805; //Ideal step is VDDa/ADCnum_steps = 3.3/4096
    float mv = adcSteps * stepSizeinMV;
    uint16_t degC = 0;
    if (mv >= 1192.0)
    {
        degC = (uint16_t)(129.0 - ((35.0 * mv) / 841.0));
    }
    else
    {
        degC = (uint16_t)(153.0 - ((40.0 * mv) / 649.0));
    }
    return degC;
}
//----------------------------------------------------------------------
// Function:  float ConvertDigitalToVoltage(uint16_t digital)
//
// Description: Converts the 12-bit digital value returned from the ADC into
//              a floating value voltage.
//
// Parameters:
//              uint16_t digital - 12-bit ADC value representing voltage.
//
// Returns: voltage represented as a float.
//
//----------------------------------------------------------------------
float ConvertDigitalToVoltage(uint16_t digital)
{
    float voltPerDiv = (VOLTAGE_MAX / DIVISIONS_MAX);

    return ( voltPerDiv * digital );
}

uint16_t ReadADCTemp(ADC_TypeDef* ADCx, uint8_t channelId)
{
    // read twice due to chip errata
    ReadOffsetChannelValue(ADCx, channelId);
    return (ConvertDigitalToTemp(ReadOffsetChannelValue(ADCx, channelId)) );
}

//----------------------------------------------------------------------
// Function:  float ConvertDigitalToMillivolt(uint16_t digital)
//
// Description: Converts the 12-bit digital value returned from the ADC into
//              a  integer value of  millivolt.
//
// Parameters:
//              uint16_t digital - 12-bit ADC value representing voltage.
//
// Returns: voltage represented as a  integer value of  millivolt.
//
//----------------------------------------------------------------------
uint32_t ConvertDigitalToMillivolt( uint16_t digital )
{
    float voltPerDiv = ( ( float )VOLTAGE_MAX / DIVISIONS_MAX );
    return ( (uint32_t)(voltPerDiv * digital) );
}
