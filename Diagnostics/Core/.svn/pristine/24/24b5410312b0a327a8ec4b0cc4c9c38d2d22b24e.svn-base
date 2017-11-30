/*************************************************************************//**
 * @file            nvram.c
 * @brief           Abstract NVRAM driver - Implementation
 *
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: nvram.c 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      This file contains the implementation of the abstract NVRAM driver.
 *      The this driver provides a uniform interface to an NVRAM 'subsystem'
 *      which allows clients to remain blissfully unaware of how many or what
 *      type of NVRAM devices make up their NVRAM storage space.
 *
 *      A contiguous virtual address space is provided that is mapped into
 *      available devices based on the platform configuration.
 *
 *      This file should NOT be modified for platform-specific purposes.  All
 *      configuration should be done in nvram_platform_config.[ch]
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/

/*** Include Files ***********************************************************/
#include "nvram.h"
#include "nvram_device.h"

/* #included to maintain lexical scope of variables declared within */
//#include "nvram_platform_config.c"

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/
#define NUM_DEVICES ARRAY_SIZE(nvramDeviceMap)

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/
static boolean decodeVirtAddr( NVRAM_ADDR_T vAddr, uint8* const pDev,
                               NVRAM_ADDR_T* const pAddr );

/*** Module Variables ********************************************************/
const struct nvramDescriptor nvramDeviceMap[] =
{
    /*      Size          Address Width        Page Size          SectorSize             SmallBlockSize             LargeBlockSize  */
    { SPIFLASH_SIZE,  SPIFLASH_ADDR_WIDTH, SPIFLASH_PAGE_SIZE, SPIFLASH_SECTOR_SIZE, SPIFLASH_SMALL_BLOCK_SIZE, SPIFLASH_LARGE_BLOCK_SIZE },
};
COMPILEASSERT( ARRAY_SIZE(nvramDeviceMap) >= 1 );

/*** Module Functions ********************************************************/

/**
 * Initializes the NVRAM driver
 *
 * @author rc10851
 */
void nvram_init( void )
{
    /* Perform any platform-specific initialization */
    platform_nvram_init();
}

/**
 * If pSrc is specified, writes num_bytes of data to the NVRAM from the buffer
 * pointed to by pSrc, starting at address addr.  If pDest is specified,
 * reads num_bytes of data from the NVRAM to the buffer pointed to by pDest,
 * starting at address addr
 *
 * @author rc10851
 *
 * @param addr The address in NVRAM from which to start reading
 * @param num_bytes The number of bytes to read from NVRAM
 * @param pSrc Pointer to the location at which the data to be written is stored
 * @param pDest Pointer to the location at which the read data should be stored
 */
void nvram_op( NVRAM_ADDR_T addr, uint32 num_bytes, const uint8* pSrc, uint8* pDest )
{
    uint8 dev;
    uint32 pAddr;
    uint32 count;

    while ( (pSrc || pDest) && num_bytes && decodeVirtAddr( addr, &dev, &pAddr ) )
    {
        /* Limit transfers to one device at a time */
        count = MIN( num_bytes, nvramDeviceMap[dev].size - pAddr );

        /* Perform read or write based on supplied pointer */
        if ( pSrc )
        {
            nvram_device_write( dev, pAddr, count, pSrc );
            pSrc += count;
        }
        else
        {
            nvram_device_read( dev, pAddr, count, pDest );
            pDest += count;
        }

        /* Update status of overall transfer */
        addr += count;
        num_bytes -= count;
    }
}

/**
 * Erases the NVRAM segment associated with the specified
 * address.  If NVRAM_SIZE is specified, bulk
 * erases the device.
 *
 * @author rc10851
 *
 * @param startAddr The address in NVRAM at which the erase should begin
 * @param endAddr The address in NVRAM at which the erase should end
 */
void nvram_erase( NVRAM_ADDR_T startAddr, NVRAM_ADDR_T endAddr )
{
    uint8 devStart;
    uint8 devEnd;
    uint32 pAddrStart;
    uint32 pAddrEnd;

    while ( (startAddr < endAddr) &&
            decodeVirtAddr( startAddr, &devStart, &pAddrStart ) &&
            decodeVirtAddr( endAddr, &devEnd, &pAddrEnd ) )
    {
        if ( devStart == devEnd )
        {
            /* Only erasing within one device, so do it in one shot */
            nvram_device_erase( devStart, pAddrStart, pAddrEnd );
            startAddr = endAddr;
        }
        else
        {
            /* Erase crosses devices; operate on only one device at a time */
            nvram_device_erase( devStart, pAddrStart, nvramDeviceMap[devStart].size - 1 );
            startAddr += nvramDeviceMap[devStart].size - pAddrStart;
        }
    }
}

/**
 * Decodes a virtual address into a (device,physical address) pair
 *
 * @author rc10851
 *
 * @param vAddr the virtual address to decode
 * @param dev Pointer to location to store device number
 * @param pAddr Pointer to location to store physical address
 *
 * @return TRUE if the address was decoded successfully, FALSE otherwise
 */
static boolean decodeVirtAddr( NVRAM_ADDR_T vAddr, uint8* const pDev,
                               NVRAM_ADDR_T* const pAddr )
{
    boolean decodeOK = FALSE;
    uint8 i = 0;

    while ( !decodeOK && (i < NUM_DEVICES) )
    {
        if ( vAddr >= nvramDeviceMap[i].size )
        {
            /*
             * The virtual address does not map anywhere into the current device,
             * so adjust it and let the loop continue to the next device
             */
            vAddr -= nvramDeviceMap[i].size;
        }
        else
        {
            /*
             * The virtual address mapped into this device.  Because the address
             * space is contiguous, the virtual address is now the same as the
             * physical address, so set things accordingly
             */
            *pDev = i;
            *pAddr = vAddr;
            decodeOK = TRUE;
            break;
        }

        ++i;
    }

    return decodeOK;
}
