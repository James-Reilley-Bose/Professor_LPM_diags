/*
    File    :   Watchdog.h
    Title   :
    Author  :   Dylan Reid
    Created :  9/18/6
    Language:   C
    Copyright:  (C) 2006 Bose Corporation, Framingham, MA

    Description:   manage the system watchdog

===============================================================================
*/
#ifndef WATCHDOG_H
#define WATCHDOG_H

#define USE_WDG_INT // if you change this also chagne 75x_flash.c

#define PAT_WATCHDOG()  IWDG->KR = 0xAAAA

void WD_Init(void);
void WD_Enable(void);

#endif
