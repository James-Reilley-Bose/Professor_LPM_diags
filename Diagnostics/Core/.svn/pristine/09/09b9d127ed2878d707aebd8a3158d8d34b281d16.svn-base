/*************************************************************************//**
 * @file            nvram_device.h
 * @brief           Generic NVRAM device driver - Interface
 *
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: nvram_device.h 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      This file is the interface for a generic NVRAM device driver.  It
 *      contains the public macros and function prototypes that can be used
 *      to communicate directly with a single generic NVRAM device.
 *
 *      This file should NOT be modified for platform-specific purposes.  All
 *      configuration should be done in nvram_platform_config.[ch]
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/
#ifndef __NVRAM_DEVICE_H__
#define __NVRAM_DEVICE_H__

/*** Include Files ***********************************************************/
#include "nvram_platform.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/
void nvram_device_read(  uint8 dev, uint32 addr, uint32 num_bytes, uint8* const pDest );
void nvram_device_write( uint8 dev, uint32 addr, uint32 num_bytes, const uint8* pSrc );
void nvram_device_erase( uint8 dev, uint32 startAddr, uint32 endAddr );

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

#ifdef  __cplusplus
}
#endif

#endif  /* __NVRAM_DEVICE_H__ */
