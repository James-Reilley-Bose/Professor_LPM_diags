/*************************************************************************//**
 * @file            nvram_platform.h
 * @brief           Abstract NVRAM driver - Platform Definitions
 *
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: nvram_platform.h 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      This file contains the platform definitions for the abstract NVRAM
 *      driver.  It outlines the public macros, type definitions and function
 *      prototypes that must be supplied by the platform in order for the
 *      driver to function.
 *
 *      This file should NOT be modified for platform-specific purposes.  All
 *      configuration should be done in nvram_platform_config.[ch]
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/
#ifndef __NVRAM_PLATFORM_H__
#define __NVRAM_PLATFORM_H__

/*** Include Files ***********************************************************/
#include "nvram_platform_config.h"

/**
 * All parts use big-endian byte ordering for things like address
 */
#define BIG_ENDIAN_NETWORK_ORDER
#include "mmsw_util.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/

#if !defined(NVRAM_ADDR_T)
#   error You must define NVRAM_ADDR_T
#endif

/*** Module Types ************************************************************/

struct nvramDescriptor
{
    NVRAM_ADDR_T    size;
    uint8           addrWidth;
    uint16          pageSize;
    uint32          sectorSize;
    uint32_t        smallBlockSize;
    uint32_t        largeBlockSize;
};

/*** Module Function Prototypes **********************************************/

/* The platform must declare functions or macros for all prototypes here */
void platform_nvram_init( void );
void platform_nvram_idle_callback( uint8 dev );

void platform_nvram_open( uint8 dev );
void platform_nvram_close( uint8 dev );
void platform_nvram_read( uint8 dev, uint8* const dest, uint32 count );
void platform_nvram_write( uint8 dev, const uint8* src, uint32 count );
BOOL platform_nvram_lock(void);
void platform_nvram_unlock(void);

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

#ifdef  __cplusplus
}
#endif

#endif  /* __NVRAM_PLATFORM_H__ */
