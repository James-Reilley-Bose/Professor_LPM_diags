/*
  File   :  watchdog.c
  Title  :
  Author :  Dylan Reid
  Created   :  9/18/6
  Language: C
  Copyright:   (C) 2006, 2009 Bose Corporation, Framingham, MA

  Description:   manage the system watchdog

  ===============================================================================
*/

#include "project.h"
#include "watchdog.h"

void
WD_Init(void)
{
    // Enable Write access
    IWDG->KR = IWDG_WriteAccess_Enable;

    /* Set prescalar. This is based on a 32kHz clock
    *
    *  IWDG_Prescaler_X  timeout in seconds
    *        4               0.512
    *        8               1.024
    *        16              2.048
    *        32              4.096
    *        64              8.192
    *        128             16.384
    *        256             32.768
    */
    IWDG->PR = IWDG_Prescaler_128;

    IWDG->KR = IWDG_WriteAccess_Disable;
}

void
WD_Enable(void)
{
    IWDG->KR = 0xCCCC;
}
