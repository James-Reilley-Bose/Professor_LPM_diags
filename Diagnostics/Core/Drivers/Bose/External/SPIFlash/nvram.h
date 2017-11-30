/*************************************************************************//**
 * @file            nvram.h
 * @brief           Abstract NVRAM driver - Interface
 *
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: nvram.h 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      This file is the master interface for clients that wish to use the
 *      abstract NVRAM driver.  It contains the public macros and function
 *      prototypes that can be used.
 *
 *      This file should NOT be modified for platform-specific purposes.  All
 *      configuration should be done in nvram_platform_config.[ch]
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/
#ifndef __NVRAM_H__
#define __NVRAM_H__

/*** Include Files ***********************************************************/
#include "nvram_platform_config.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/
#define nvram_read(a,n,p)   nvram_op( a, n, NULL, p )
#define nvram_write(a,n,p)  nvram_op( a, n, p, NULL )

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/
void nvram_init( void );
void nvram_op( NVRAM_ADDR_T addr, uint32 num_bytes, const uint8* pSrc, uint8* pDest );
void nvram_erase( NVRAM_ADDR_T startAddr, NVRAM_ADDR_T endAddr );

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

#ifdef  __cplusplus
}
#endif

#endif  /* __NVRAM_H__ */
