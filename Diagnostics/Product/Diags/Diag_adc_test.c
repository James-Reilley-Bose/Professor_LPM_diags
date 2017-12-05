/**
  ******************************************************************************
  * @file    Diag_adc_test.c
  *           + Diagnostic test for ADC
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f2xx_gpio.h"
#include "Diag_adc_test.h"

ADC_CHANNELS_T adcch[] =
{
    {"SENSE1",  SENSE1_CH},
    {"THERM1",  THERM1_CH},
    {"THERM2",  THERM2_CH},
    {"SENSE2",  SENSE2_CH},
    {"STM16_CH",   STM16_CH}
};

SCRIBE_DECL(diag);

static char fs[64], ss[64];

void ConfigureADC1(void)
{
    ADC_InitTypeDef ADC_InitStruct;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;

    /*Enable ADC1 clock so we can talk to it.*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // Reset to power-on defaults.
    ADC_DeInit();

    // settings from void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
    ADC_InitStruct.ADC_Resolution             = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode           = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode     = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_ExternalTrigConv       = ADC_ExternalTrigConv_T1_CC1; // is this == none?
    ADC_InitStruct.ADC_DataAlign              = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfConversion        = 1;

    ADC_Init(ADC1, &ADC_InitStruct);

    // default values for ADC_CommonStructInit()
    ADC_CommonInitStruct.ADC_Mode             = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_Prescaler        = ADC_Prescaler_Div2;
    ADC_CommonInitStruct.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

    ADC_CommonInit(&ADC_CommonInitStruct);
    ADC_Cmd(ADC1, ENABLE);
}


void config_adc(void)
{
    ConfigureGpioPin(ADC_VSYS_SENSE1_BANK, ADC_VSYS_SENSE1_PIN , GPIO_MODE_IN_ANALOG, NO_ALT_FUNC); //A4 29 ADC12_IN4
    ConfigureGpioPin(ADC_THERM1_BANK, ADC_THERM1_PIN, GPIO_MODE_IN_ANALOG, NO_ALT_FUNC);            //B0 35 ADC12_IN8
    ConfigureGpioPin(ADC_THERM2_BANK, ADC_THERM2_PIN, GPIO_MODE_IN_ANALOG, NO_ALT_FUNC);            //B1 36 ADC12_IN9
#if (PRODUCT_HARDWARE_VARIANT == 0)
    ConfigureGpioPin(ADC_VSYS_SENSE2_BANK,  ADC_VSYS_SENSE2_PIN, GPIO_MODE_IN_ANALOG, NO_ALT_FUNC); //C0 15 ADC
#endif
}


static float ncp21xv103j03ra_temp_converter(uint16_t conv_value, int verb)
{
    float fVoltage, tres, hires_lt, lowres_ht, tempc;
    int i;
    float voltPerDiv = (VOLTAGE_MAX / DIVISIONS_MAX);
    double num, intpart, decpart;

    TCONVERT_T tconvert[] =
    {
        // TVC
        { -40, 328.996 * THERM_NOMIAL_OHMS },  // -40
        { -35, 237.387 * THERM_NOMIAL_OHMS },  // -35
        { -30, 173.185 * THERM_NOMIAL_OHMS },  // -30
        { -25, 127.773 * THERM_NOMIAL_OHMS },  // -25
        { -20, 95.327  * THERM_NOMIAL_OHMS },  // -20
        { -15, 71.746  * THERM_NOMIAL_OHMS },  // -15
        { -10, 54.564  * THERM_NOMIAL_OHMS },  // -10
        { -5 , 41.813  * THERM_NOMIAL_OHMS },  // -5
        {0  , 32.330  * THERM_NOMIAL_OHMS },   //  0
        {5  , 25.194  * THERM_NOMIAL_OHMS },   //  5
        {10 , 19.785  * THERM_NOMIAL_OHMS },   // 10
        {15 , 15.651  * THERM_NOMIAL_OHMS },   // 15
        {20 , 12.468  * THERM_NOMIAL_OHMS },  // 20
        {25 , 10.000  * THERM_NOMIAL_OHMS },  // 25
        {30 , 8.072   * THERM_NOMIAL_OHMS },  // 30
        {35 , 6.556   * THERM_NOMIAL_OHMS },  // 35
        {40 , 5.356   * THERM_NOMIAL_OHMS },  // 40
        {45 , 4.401   * THERM_NOMIAL_OHMS },  // 45
        {50 , 3.635   * THERM_NOMIAL_OHMS },  // 50
        {55 , 3.019   * THERM_NOMIAL_OHMS },  // 55
        {60 , 2.521   * THERM_NOMIAL_OHMS },  // 60
        {65 , 2.115   * THERM_NOMIAL_OHMS },  // 65
        {70 , 1.781   * THERM_NOMIAL_OHMS },  // 70
        {75 , 1.509   * THERM_NOMIAL_OHMS },  // 75
        {80 , 1.284   * THERM_NOMIAL_OHMS }, // 80
        {85 , 1.097   * THERM_NOMIAL_OHMS }, // 85
        {90 , 0.941   * THERM_NOMIAL_OHMS }, // 90
        {95 , 0.810   * THERM_NOMIAL_OHMS }, // 95
        {100, 0.701   * THERM_NOMIAL_OHMS }, // 100
        {105, 0.608   * THERM_NOMIAL_OHMS }, // 105
        {110, 0.530   * THERM_NOMIAL_OHMS }, // 110
        {115, 0.463   * THERM_NOMIAL_OHMS }, // 115
        {120, 0.406   * THERM_NOMIAL_OHMS }, // 120
        {125, 0.358   * THERM_NOMIAL_OHMS }, // 125
        {130, 0 }                            // 130 or more
    };
    // refernce resistor voltage drop
    fVoltage = (voltPerDiv * conv_value);
    // calculate the Thermistor resistance
    tres = fVoltage / ((VOLTAGE_MAX - fVoltage) / THERM_SERIES_OHMS);

    num = fVoltage;
    intpart = (int)num;
    decpart = num - intpart;

    if (verb)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"[%04d] %d.%03dV  ",
                   conv_value,
                   (int)intpart,
                   (int)(decpart * N3_DECIMAL_POINTS_PRECISION));
    }

    num = tres;
    intpart = (int)num;
    decpart = num - intpart;

    for (i = 0; i < (sizeof(tconvert) / sizeof(TCONVERT_T)) - 1; i++)
    {
        hires_lt = tconvert[i].tscale;
        lowres_ht = tconvert[i + 1].tscale;
        if ((tres >= lowres_ht) && (tres <= hires_lt))
        {
            tempc = tconvert[i].cval + (5 * ((hires_lt - tres) / (hires_lt - lowres_ht)));
            return tempc;
        }
    }
    return -1;
}

#if 0
static float ertj1vt302j_temp_converter(uint16_t conv_value, int verb)
{
    float fVoltage, tres, hires_lt, lowres_ht, tempc;
    int i;
    float voltPerDiv = (VOLTAGE_MAX / DIVISIONS_MAX);
    double num, intpart, decpart;

    TCONVERT_T tconvert[] =
    {
        // TVC
        { -40, 63.3    * THERM_NOMIAL_OHMS },  // -40
        { -35, 42.92   * THERM_NOMIAL_OHMS },  // -35
        { -30, 29.5    * THERM_NOMIAL_OHMS },  // -30
        { -25, 20.53   * THERM_NOMIAL_OHMS },  // -25
        { -20, 14.46   * THERM_NOMIAL_OHMS },  // -20
        { -15, 10.30   * THERM_NOMIAL_OHMS },  // -15
        { -10, 7.407   * THERM_NOMIAL_OHMS },  // -10
        { -5 , 5.388   * THERM_NOMIAL_OHMS },  // -5
        {0  , 3.966   * THERM_NOMIAL_OHMS },   //  0
        {5  , 2.953   * THERM_NOMIAL_OHMS },   //  5
        {10 , 2.221   * THERM_NOMIAL_OHMS },   // 10
        {15 , 1.687   * THERM_NOMIAL_OHMS },   // 15
        {20 , 1.293   * THERM_NOMIAL_OHMS },  // 20
        {25 , 1       * THERM_NOMIAL_OHMS },  // 25
        {30 , 0.7799  * THERM_NOMIAL_OHMS },  // 30
        {35 , 0.6131  * THERM_NOMIAL_OHMS },  // 35
        {40 , 0.4856  * THERM_NOMIAL_OHMS },  // 40
        {45 , 0.3874  * THERM_NOMIAL_OHMS },  // 45
        {50 , 0.3111  * THERM_NOMIAL_OHMS },  // 50
        {55 , 0.2513  * THERM_NOMIAL_OHMS },  // 55
        {60 , 0.2042  * THERM_NOMIAL_OHMS },  // 60
        {65 , 0.1670  * THERM_NOMIAL_OHMS },  // 65
        {70 , 0.1377  * THERM_NOMIAL_OHMS },  // 70
        {75 , 0.1144  * THERM_NOMIAL_OHMS },  // 75
        {80 , 0.0956  * THERM_NOMIAL_OHMS }, // 80
        {85 , 0.08033 * THERM_NOMIAL_OHMS }, // 85
        {90 , 0.06782 * THERM_NOMIAL_OHMS }, // 90
        {95 , 0.05753 * THERM_NOMIAL_OHMS }, // 95
        {100, 0.04903 * THERM_NOMIAL_OHMS }, // 100
        {105, 0.04198 * THERM_NOMIAL_OHMS }, // 105
        {110, 0.03609 * THERM_NOMIAL_OHMS }, // 110
        {115, 0.03117 * THERM_NOMIAL_OHMS }, // 115
        {120, 0.02702 * THERM_NOMIAL_OHMS }, // 120
        {125, 0.02351 * THERM_NOMIAL_OHMS }, // 125
        {130, 0 }                            // 130 or more
    };

    // refernce resistor voltage drop
    fVoltage = (voltPerDiv * conv_value);
    // calculate the Thermistor resistance
    tres = fVoltage / ((VOLTAGE_MAX - fVoltage) / THERM_SERIES_OHMS);

    num = fVoltage;
    intpart = (int)num;
    decpart = num - intpart;

    if (verb)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"[%04d] %d.%03dV  ",
                   conv_value,
                   (int)intpart,
                   (int)(decpart * N3_DECIMAL_POINTS_PRECISION));
    }

    num = tres;
    intpart = (int)num;
    decpart = num - intpart;

    for (i = 0; i < (sizeof(tconvert) / sizeof(TCONVERT_T)) - 1; i++)
    {
        hires_lt = tconvert[i].tscale;
        lowres_ht = tconvert[i + 1].tscale;
        if ((tres >= lowres_ht) && (tres <= hires_lt))
        {
            tempc = tconvert[i].cval + (5 * ((hires_lt - tres) / (hires_lt - lowres_ht)));
            return tempc;
        }
    }
    return -1;
}
#endif

uint32_t cmd_adc(int index, int lastindex, uint32_t repeat, int verb, int delay)
{
    //char fs[64], ss[64];
    uint16_t conv_value, cnt;
    int i = 0;
    int ch_strt = 0, ch_end = sizeof(adcch) / sizeof(ADC_CHANNELS_T);
    FlagStatus flgstats;
    uint32_t rptcnt, rtrnstatus = SUCCESS;
    uint8_t ADC_Channel;
    float tempc;
    float fVoltage;
    double num, intpart, decpart;
    float voltPerDiv = (VOLTAGE_MAX / DIVISIONS_MAX);
    char adcstr1[80];

    num = voltPerDiv;
    intpart = (int)num;
    decpart = num - intpart;

    if (verb > 2)
    {
        LOG(diag, ROTTEN_LOGLEVEL_VERBOSE," conversion Mv/per %d.%05d\n",
                   (int)intpart,
                   (int)(decpart * 100000000));
    }
    if (index <= lastindex)
    {
        if ((index < ch_end) && (lastindex < ch_end))
        {
            ch_strt = index;
            ch_end = lastindex;
        }
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"invalid index\n\r");
        return ERROR;
    }

    ADC_Cmd(ADC1, ENABLE);

    Delay_ms(200);

    rptcnt = 0;
    do
    {
        if (rptcnt)
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"");
        }
        for (i = ch_strt; i <= ch_end; i++)
        {
            ADC_Channel = adcch[i].channel;

            ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_56Cycles);

            ADC_SoftwareStartConv(ADC1);

            memset(fs, 0, sizeof(fs));
            
            cnt = 0;
            do
            {
                flgstats =  ADC_GetFlagStatus(ADC1, ADC_FLAG_STRT);
            }
            while ((flgstats == 0) && (cnt++ < 100));

            Delay_ms(200);

            conv_value = ADC_GetConversionValue(ADC1);

            fVoltage = voltPerDiv * conv_value;

            num = fVoltage;
            intpart = (int)num;
            decpart = num - intpart;

            if (verb > 1)
            {
                sprintf(ss,"[%d] %12s ch[%2d] raw[%04d] ",
                           i, adcch[i].adcname, ADC_Channel, conv_value);
                strcat(fs, ss);
                
                sprintf(ss,"%d.%02dV  ", (int)intpart,
                           (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                strcat(fs, ss);
            }
            else
            {
                if (verb > 0)
                {
                    sprintf(ss,"\t%12s ch[%2d] %d.%02dV ",
                               adcch[i].adcname, ADC_Channel,
                               (int)intpart,
                               (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                    strcat(fs, ss);
                }
                else
                {
                    sprintf(ss, "\t%12s ch[%2d] %d.%02dV ",
                            adcch[i].adcname, ADC_Channel,
                            (int)intpart,
                            (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                    strcat(fs, ss);
                 }
            }

            switch (ADC_Channel)
            {
                case SENSE1_CH:
                case SENSE2_CH:
                    num = fVoltage * 10;  // full scale 2.4V * 10.00 = 24.00V
                    intpart = (int)num;
                    decpart = num - intpart;
                    if (verb > 0)
                    {
                        sprintf(ss, " %02d.%02dV",
                                   (int)intpart,
                                   (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                        strcat(fs, ss);
                    }
                    else
                    {
                        sprintf(ss,  "%s %02d.%02dV ", adcstr1,
                            (int)intpart,
                            (int)(decpart * N2_DECIMAL_POINTS_PRECISION));
                        strcat(fs, ss);
                    }

                    break;

                case THERM1_CH:
                case THERM2_CH:
                    tempc = ncp21xv103j03ra_temp_converter(conv_value, 0);
                    num = tempc;
                    intpart = (int)num;
                    decpart = num - intpart;
                    if (verb > 0)
                    {
                        sprintf(ss,  " %d.%02dC",
                                   (int)intpart,
                                   abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                     strcat(fs, ss);
                   }
                    else
                    {
                        sprintf(ss,   "%s %d.%02dC", adcstr1,
                            (int)intpart,
                            abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                     strcat(fs, ss);
                     }
                    break;

                case STM16_CH:
                    tempc = (( fVoltage - V25) / AVG_SLOPE) + TEMP_C_AT_76MV;
                    num = tempc;
                    intpart = (int)num;
                    decpart = num - intpart;
                    if (verb > 0)
                    {
                        sprintf(ss,   " %d.%02dC",
                                   (int)intpart,
                                   abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                      strcat(fs, ss);
                    }
                    else
                    {
                        sprintf(ss,    "%s %d.%02dC", adcstr1,
                            (int)intpart,
                            abs((int)(decpart * N2_DECIMAL_POINTS_PRECISION)));
                      strcat(fs, ss);
                    }

                    break;

                default:
                    break;
            }

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s", fs);

            ADC_ClearFlag(ADC1, ADC_FLAG_STRT);
        }
        if (delay)
        {
            Delay_ms(delay);
        }
        i = 0;
    }
    while (rptcnt++ < repeat);

    return rtrnstatus;
}

void dadc_config(void)
{
    ConfigureADC1();
    ADC->CCR |= ADC_CCR_TSVREFE;
    dacd_init = 0;
    config_adc();

}
