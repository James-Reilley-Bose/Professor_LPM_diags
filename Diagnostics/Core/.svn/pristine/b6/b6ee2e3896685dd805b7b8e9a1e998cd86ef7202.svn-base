/*
    File    :   adc.h
    Author  :   Frank Mazzarella
    Created :   06/19/15
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:  Support for the ADC on the STM32F205


===============================================================================
*/

#ifndef __ADC_H__
#define __ADC_H__

#define VOLTAGE_MAX                (3300)   // highest possible voltage.
#define DIVISIONS_MAX              (0xFFF + 1) // 12-bits == 4096 divisions

void ConfigureADC(ADC_TypeDef* ADCx, FunctionalState ADC_ScanConvMode, FunctionalState ADC_ContinuousConvMode);
void ADC_Watchdog_init( ADC_TypeDef* ADCx, uint8_t ADC_Channel, IRQn_Type  id, INTERRUPT_FNCT_PTR isr, int16_t HighThreshold, uint16_t LowThreshold );
void ADC_Enable( ADC_TypeDef* ADCx );
void ADC_Disable( ADC_TypeDef* ADCx );
uint16_t ReadOffsetChannelValue(ADC_TypeDef* ADCx, uint8_t channelId);
uint16_t ConvertDigitalToTemp(uint16_t adcSteps);
float ConvertDigitalToVoltage(uint16_t digital);
uint16_t ReadADCTemp(ADC_TypeDef* ADCx, uint8_t channelId);
uint32_t ConvertDigitalToMillivolt( uint16_t digital );
BOOL ADC_isEnabled( ADC_TypeDef* ADCx );
#endif /* __ADC_H__ */


