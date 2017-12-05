
/**
  ******************************************************************************
  * @file    Diag_lcd_test.h
  *           + Diagnostic test for LCD
 @verbatim
 ===============================================================================
 **/
#ifndef DIAG_LCD_TEST_H
#define DIAG_LCD_TEST_H

#define CLKPCS 24000000
#define LCD_BCKLIGHT_PSC 120 //pre-scaler for max freq = 400KHz

void lcd_backlight(uint16_t *);
void set_led4_af(void);
void strtoflt(char *inptstr, uint32_t  *arg);
void set_pwm_percent(char *inptstr);

extern float pwm_percent;

#endif

/* ======== END OF FILE ======== */
