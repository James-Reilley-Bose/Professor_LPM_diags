/**
  ******************************************************************************
  * @file    Diag_clock_test.c
  *           + Diagnostic test for clock
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "Diag_clock_test.h"

SCRIBE_DECL(diag);

/* get_set_clk_speed_cmd(mode)
    mode  - result
       0  - show current clk speeds, terse(default)
       1  - show current clk speeds, verbose
       2  - set clk speed 96MHz
       3  - set clk speed 64MHz
       4  - set clk speed 48MHz
       5  - set clk speed 32MHz
       6  - set clk speed 16MHz
       7  - help
*/
uint32_t get_set_clk_speed_cmd(CLKSPD_GET_SET mode)
{
    uint8_t result;
    uint32_t pllcfg, cfgr, rcccr, m, n, p, ahbpre = 1, ahbpre1 = 1, ahbpre2 = 1, sysclk, plli2s;
    uint32_t pllsource = 0, pllm = 8, plln = 192, pllp = 4, pllq = 4;
    uint32_t pre1, pre2, temp, verb = 1;
    uint32_t FLASH_Latency = FLASH_Latency_0;
    //  uint32_t TIM2_Prescaler = 0;
    RCC_ClocksTypeDef RCC_Clocks;

    // get current clock config
    pllcfg = RCC->PLLCFGR;

    RCC->CR &= 0xfffeffff; //turn off HSE enable
    rcccr = RCC->CR;
    cfgr = RCC->CFGR;
    plli2s = RCC->PLLI2SCFGR;

    switch (mode)
    {

        case CLKSPD_HELP:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tdi clk   - show current\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clkv  - show current verbose\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clkh  - show help\n");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk96 - set to 96MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk64 - set to 64MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk48 - set to 48MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk32 - set to 32MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk24 - set to 24MHz\n");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tdi clk16 - set to 16MHz\n");
            return ERROR;
            break;

        case CLKSPD_RD:  // read current
            verb = 0;
        case CLKSPD_RD_V:  // read current
            result = RCC_GetSYSCLKSource();
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tsystem_clk = %s\n",
                       (result == 0) ? "HSI" : \
                       (result == 4) ? "HSE" : \
                       (result == 8) ? "PLL" : "unknown");
            RCC_GetClocksFreq(&RCC_Clocks);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tRCC_Clocks\n");

            m = pllcfg & RCC_PLLCFGR_PLLM;
            n = (pllcfg >> 6) & 0x1ff;
            p = (pllcfg >> 16) & 0x3;
            pllp = (p + 1) * 2;

            switch ((cfgr >> 4) & 0xf)
            {
                case 0x0:
                    ahbpre = 1;
                    break;
                case 0x8:
                    ahbpre = 2;
                    break;
                case 0x9:
                    ahbpre = 4;
                    break;
                case 0xa:
                    ahbpre = 8;
                    break;
                case 0xb:
                    ahbpre = 16;
                    break;
                case 0xc:
                    ahbpre = 64;
                    break;
                case 0xd:
                    ahbpre = 128;
                    break;
                case 0xe:
                    ahbpre = 256;
                    break;
                case 0xf:
                    ahbpre = 512;
                    break;
                default:
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Error invalid ahbpre value:!\n");
                    break;
            }

            temp = (cfgr >> 10) & 0x7;
            switch (temp)
            {
                case 0:
                    ahbpre1 = 1;
                    break;
                case 4:
                    ahbpre1 = 2;
                    break;
                case 5:
                    ahbpre1 = 4;
                    break;
                case 6:
                    ahbpre1 = 8;
                    break;
                case 7:
                    ahbpre1 = 16;
                    break;
                default:
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Error invalid ahbpre1 value:!\n");
                    break;
            }

            temp = (cfgr >> 13);
            switch (temp)
            {
                case 0:
                    ahbpre2 = 1;
                    break;
                case 4:
                    ahbpre2 = 2;
                    break;
                case 5:
                    ahbpre2 = 4;
                    break;
                case 6:
                    ahbpre2 = 8;
                    break;
                case 7:
                    ahbpre2 = 16;
                    break;
                default:
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Error invalid ahbpre2 value:!\n");
                    break;
            }


            if (verb)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%08x\n", "CR", rcccr);
            }
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "PLLI2S",
                       ((rcccr >> 26) & 1) ? "on" : "off",
                       ((rcccr >> 27) & 1) ? "locked" : "unlocked");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "PLLON",
                       ((rcccr >> 24) & 1) ? "on" : "off",
                       ((rcccr >> 25) & 1) ? "locked" : "unlocked");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "HSEON",
                       ((rcccr >> 16) & 1) ? "on" : "off",
                       ((rcccr >> 17) & 1) ? "rdy" : "notrdy");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "HSION",
                       (rcccr & 1) ? "on" : "off",
                       ((rcccr >> 1) & 1) ? "rdy" : "notrdy");

            //    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %-4s  %-s\n", "sysclk",
            //              ((cfgr & 0xc)==0)?"HSI":
            //                (((cfgr & 0xc)==0x4)?"HSE":
            //                  (((cfgr & 0xc)==0x8)?"PLL":"invalid")));

            if (verb)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = 0x%08x\n", "PLLCFGR", pllcfg);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "m", m, m);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "n", n, n);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "p", pllp, pllp);

                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = %08x\n", "CFGR", cfgr);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "ahbpre",  ahbpre, ahbpre);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "ahb1pre", ahbpre1, ahbpre1);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "ahb2pre", ahbpre2, ahbpre2);

                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = 0x%08x\n", "PLLI2SCLK", plli2s);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%x %d\n", "plli2s_n", (plli2s >> 6) & 0x1ff, (plli2s >> 6) & 0x1ff);
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = 0x%02x %d\n", "plli2s_r", (plli2s >> 28) & 0x7, (plli2s >> 28) & 0x7);
            }
            if ((rcccr >> 24) & 1) // PLLON
            {
                sysclk = (HSI_VALUE * n) / (m * pllp);
            }
            else
            {
                sysclk = HSI_VALUE;
            }

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\t%12s = %d\n", "SYSCLK_Freq", sysclk);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "HCLK_Freq", sysclk / ahbpre);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "PCLK1_Freq", sysclk / ahbpre1);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "PCLK2_Freq", sysclk / ahbpre2);
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%12s = %d\n", "I2SCLK_Freq",
                       (HSI_VALUE / m) * (((plli2s >> 6) & 0x1ff) / ((plli2s >> 28) & 0x7))); // (HSI/m)*(n/r)
            return SUCCESS;
            break;

        case CLKSPD_96: // set the new speed 96MHz
            pllsource = 0;
            pllm = 10;
            plln = 240;
            pllp = 4;
            pre1 = RCC_HCLK_Div4;
            pre2 = RCC_HCLK_Div4;
            FLASH_Latency = FLASH_Latency_3;
            //    TIM2_Prescaler = TIM_PRE_48MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(210, 7);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_64: // set the new speed 64MHz
            pllsource = 0;
            pllm = 12;
            plln = 192;
            pllp = 4;
            pre1 = RCC_HCLK_Div4;
            pre2 = RCC_HCLK_Div4;
            RCC_PLLI2SCmd(DISABLE);
            FLASH_Latency = FLASH_Latency_2;
            //    TIM2_Prescaler = TIM_PRE_32MHZ;
            RCC_PLLI2SConfig(216, 6);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_48: // set the new speed 48MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllp = 4;
            pre1 = RCC_HCLK_Div2;
            pre2 = RCC_HCLK_Div2;
            FLASH_Latency = FLASH_Latency_1;
            //    TIM2_Prescaler = TIM_PRE_48MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(216, 6);
            SPI2->I2SPR = 0x6;
            break;
        case CLKSPD_32: // set the new speed 32MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllp = 6;
            pllq = 4;
            pre1 = RCC_HCLK_Div2;
            pre2 = RCC_HCLK_Div2;
            FLASH_Latency = FLASH_Latency_1;
            //    TIM2_Prescaler = TIM_PRE_32MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(192, 4);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_24: // set the new speed 24MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllp = 8;
            pllq = 4;
            pre1 = RCC_HCLK_Div1;
            pre2 = RCC_HCLK_Div1;
            FLASH_Latency = FLASH_Latency_1;
            //    TIM2_Prescaler = TIM_PRE_24MHZ;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(192, 4);
            SPI2->I2SPR = 0x8;
            break;
        case CLKSPD_16: // set the new speed 16MHz
            pllsource = 0;
            pllm = 16;
            plln = 192;
            pllq = 4;
            RCC_PLLI2SCmd(DISABLE);
            RCC_PLLI2SConfig(192, 4);
            RCC_PCLK1Config(RCC_HCLK_Div1);
            RCC_PCLK2Config(RCC_HCLK_Div1);
            FLASH_Latency = FLASH_Latency_0;
            //    TIM2_Prescaler = TIM_PRE_16MHZ;

            //select HSI as sysclk source
            temp = RCC->CFGR & ~RCC_CFGR_SW;
            RCC->CFGR = temp;                   // select HSI as sysclk
            RCC_PLLCmd(DISABLE);
            RCC_PLLConfig(pllsource, pllm, plln, pllp, pllq);
            FLASH_SetLatency(FLASH_Latency);
            if ((rcccr >> 26) & 1) // if it was enabled turn it back on
            {
                RCC_PLLI2SCmd(ENABLE);
            }
            //    TIM2_Prescaler = TIM_PRE_16MHZ;

            //    TIM2_Init(TIM2_Prescaler);
            //    TapUartInit();
            return SUCCESS;
            break;
        default:
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\n\tERROR: invalid Frequency, use 16, 24, 32, 64 or 96\n");
            return ERROR;
            break;
    }


    //select HSI as sysclk source
    temp = RCC->CFGR & ~RCC_CFGR_SW;
    RCC->CFGR = temp;                   // select HSI as sysclk

    RCC_PLLCmd(DISABLE);
    RCC_PCLK1Config(pre1);
    RCC_PCLK2Config(pre2);
    RCC_PLLConfig(pllsource, pllm, plln, pllp, pllq);
    if ((rcccr >> 26) & 1)                  // if it was enabled turn it back on
    {
        RCC_PLLI2SCmd(ENABLE);                // enable PLLI2S clock
        while (!(RCC->CR & RCC_CR_PLLI2SRDY)) {} //wiat for it
    }


    RCC_PLLCmd(ENABLE);                 // enable PLL clocks
    while (!(RCC->CR & RCC_CR_PLLRDY)) {} //wiat for it


    FLASH_SetLatency(FLASH_Latency);
    //select PLL as sysclk source
    temp = RCC->CFGR | RCC_CFGR_SW_1;      // select PLL as sysclk
    RCC->CFGR = temp;
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {} //wiat for it

    //  TIM2_Init(TIM2_Prescaler);
    //  TapUartInit();

    return SUCCESS;
}


