
/**
  ******************************************************************************
  * @file    Diag_adc_test.h
  *           + Diagnostic test for ADC
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_ADC_TEST_H
#define DIAG_ADC_TEST_H

#define SENSE1_CH 4   //GPIO A4
#define THERM1_CH 8   //GPIO B0
#define THERM2_CH 9   //GPIO B1
#define SENSE2_CH 10  //GPIO C0

#define VOLTAGE_MAX (3.3)
#define DIVISIONS_MAX (0xfff +1) // 12bit
#define N2_DECIMAL_POINTS_PRECISION (100)
#define N3_DECIMAL_POINTS_PRECISION (1000)
#define STM16_CH 16
#define STM17_CH 17

#ifdef PROFESSOR
#define THERM_SERIES_OHMS          (2740)      // OHMs
#define THERM_NOMIAL_OHMS          (1000)
#define V25                        (.76)
#define AVG_SLOPE                  (.0025)       //volts/C
#else

#define THERM_SERIES_OHMS          (1020)      // OHMs
#define THERM_NOMIAL_OHMS          (3000)
#define ff             (25)
#define V25                        (.76)
#define AVG_SLOPE                  (.025)      //volts/C
#endif

#define TEMP_C_AT_76MV             (25)

typedef struct {
  int16_t cval;
  float tscale;
}TCONVERT_T;

typedef struct {
  char* adcname;
  uint8_t channel;
}ADC_CHANNELS_T;

uint32_t cmd_adc(int index, int lastindex, uint32_t repeat, int verb, int delay);
void dadc_config(void);

extern int dacd_init;

#endif

/* ======== END OF FILE ======== */
