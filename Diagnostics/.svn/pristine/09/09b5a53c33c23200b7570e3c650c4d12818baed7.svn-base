/******************************************************************************
 *
 * 2017 (c) Lattice Semiconductor Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 * 
 * This program is distributed "AS IS" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied, including without limitation the implied warranty
 * of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE or NON-INFRINGEMENT.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * You may contact Lattice Semiconductor Corporation at
 * 111 SW 5th Ave, Suite 700, Portland, OR 97204.
 *
 *****************************************************************************/
/**
 * @file sii9777_user_def.h
 *
 * @brief logging library
 *
 *****************************************************************************/

#ifndef SII9777_USER_DEF_H
#define SII9777_USER_DEF_H

/*****************************************************************************/
/** All definitions below can be modified by user.
******************************************************************************/

/*****************************************************************************/
/**
* If defined with one then host driver relies on the following platform functions to be provided
* user application:
*     SiiPlatformSemaphoreCreate()
*     SiiPlatformSemaphoreDelete()
*     SiiPlatformSemaphoreGive()
*     SiiPlatformSemaphoreTake()
* In Multi-Thread mode each API function is allowed to be called preemptively.
* When defined zero the host driver will not call any of these platform functions.
* In non multi-thread mode user must make sure that non of the host driver API functions
* are called pre-emptively. 
*
******************************************************************************/
#define SII9777_USER_DEF__MULTI_THREAD     1

/*****************************************************************************/
/**
* If defined with zero then host driver writes Flash data provided through Sii9777FlashUpdate()
* directly to flash without cashing the data. The size indicated in Sii9777FlashUpdate() must be
* 256.
* For any value other than zero the host driver will allocate a memory buffer to store data before it is
* programmed in flash. In this mode the user can dynamcally change size of flash packet to any value
* between 0 and SII9777_USER_DEF__ISP_CASH_SIZE.
*
******************************************************************************/
#define SII9777_USER_DEF__ISP_CASH_SIZE    0

#endif /* SII9777_USER_DEF_H */

/***** end of file ***********************************************************/
