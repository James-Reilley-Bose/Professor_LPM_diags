/*************************************************************************//**
 * @file            nvram_platform_config.h
 * @brief           Abstract NVRAM driver - Platform Configuration
 *
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: nvram_platform_config.h 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      This file contains the platform configuration for the abstract NVRAM
 *      driver.  It contains the public macros and type definitions that
 *      generate the configuration of the NVRAM driver for a specific platform
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/
#ifndef __NVRAM_PLATFORM_CONFIG_H__
#define __NVRAM_PLATFORM_CONFIG_H__

/*** Include Files ***********************************************************/
#include "platform_settings.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/
#define SPI_WAIT_MS     100

#define SPIFLASH_PAGE_SIZE 256
#define SPIFLASH_SECTOR_SIZE 4096
#define SPIFLASH_LARGE_BLOCK_SIZE 0x10000
#define SPIFLASH_SMALL_BLOCK_SIZE 0x8000
#define SPIFLASH_SIZE 0x400000
#define SPIFLASH_ADDR_WIDTH 24

#define LOCK_NVRAM_RETRIES 3

#define NVRAM_ADDR_T    uint32

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

#ifdef  __cplusplus
}
#endif

#endif  /* __NVRAM_PLATFORM_CONFIG_H__ */
