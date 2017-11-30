/*************************************************************************//**
 * @file            nvram_device.c
 * @brief           Generic NVRAM device driver - Implementation
 *
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: nvram_device.c 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      This file contains the implementation of a generic NVRAM device driver.
 *      It allows platform-independent access to a single NVRAM deviced, based
 *      on device number.  This driver can also be used stand-alone without
 *      the Abstract NVRAM driver above it for systems that have only one
 *      NVRAM device
 *
 *      This file should NOT be modified for platform-specific purposes.  All
 *      configuration should be done in nvram_platform_config.[ch]
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/

/*** Include Files ***********************************************************/
#include "nvram_platform.h"
#include "mmsw_byte_ordering.h"

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/
extern const struct nvramDescriptor nvramDeviceMap[];

/*** Module Macros ***********************************************************/
#define CMD_READ                        0x03u
#define CMD_FAST_READ                   0x0Bu
#define CMD_READ_ID                     0x9Fu
#define CMD_READ_MFG_AND_DEVID          0x90u
#define CMD_WRITE_ENABLE                0x06u
#define CMD_WRITE_DISABLE               0x04u
#define CMD_BLOCK_ERASE                 0xD8u
#define CMD_SECTOR_ERASE                0x20u
#define CMD_SMALL_BLOCK_ERASE           0x52u
#define CMD_LARGE_BLOCK_ERASE           0xD8u
#define CMD_BULK_ERASE                  0xC7u
#define CMD_PAGE_PROGRAM                0x02u
#define CMD_READ_STATUS_REG             0x05u
#define CMD_WRITE_STATUS_REG            0x01u
#define CMD_DEEP_POWER_DOWN             0xABu
#define CMD_RELEASE_DEEP_POWER_DOWN     0xABu

/**
  * Status register values common to all vendors
  */
#define STATUS_WIP                      BIT(0)
#define STATUS_WEL                      BIT(1)
#define STATUS_SR_WRITE_DISABLE         BIT(7)

#if !defined(NVRAM_SIZE)
#    define  NVRAM_SIZE(dev)            nvramDeviceMap[dev].size
#endif

#if !defined(NVRAM_ADDR_WIDTH)
#    define  NVRAM_ADDR_WIDTH(dev)      nvramDeviceMap[dev].addrWidth
#endif

#if !defined(NVRAM_PAGE_SIZE)
#    define  NVRAM_PAGE_SIZE(dev)       nvramDeviceMap[dev].pageSize
#endif

#if !defined(NVRAM_SECTOR_SIZE)
#    define  NVRAM_SECTOR_SIZE(dev)     nvramDeviceMap[dev].sectorSize
#endif

#if !defined(NVRAM_SMALL_BLOCK_SIZE)
#    define  NVRAM_SMALL_BLOCK_SIZE(dev)     nvramDeviceMap[dev].smallBlockSize
#endif

#if !defined(NVRAM_LARGE_BLOCK_SIZE)
#    define  NVRAM_LARGE_BLOCK_SIZE(dev)     nvramDeviceMap[dev].largeBlockSize
#endif

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/
static uint8 readStatusRegister( uint8 dev );
static void waitOperationComplete( uint8 dev );
static void writeEnable( uint8 dev );

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

/**
 * Reads num_bytes of data from the NVRAM to the buffer
 * pointed to by pDest, starting at address addr
 *
 * @author rc10851
 *
 * @param dev The number of the NVRAM device upon which to operate
 * @param addr The address in NVRAM from which to start reading
 * @param num_bytes The number of bytes to read from NVRAM
 * @param pDest Pointer to the location at which the read data
 *              should be stored
 */
void nvram_device_read( uint8 dev, uint32 addr, uint32 num_bytes, uint8* const pDest )
{
    uint8 cmd[1 + sizeof(addr)];

    if ( (addr + num_bytes - 1) < NVRAM_SIZE(dev) )
    {
        cmd[0] = CMD_READ;

        if ( NVRAM_ADDR_WIDTH(dev) > 24 )
        {
            STORE_UINT32( &cmd[1], addr );
        }
        else if ( NVRAM_ADDR_WIDTH(dev) > 16 )
        {
            STORE_UINT24( &cmd[1], addr );
        }
        else if ( NVRAM_ADDR_WIDTH(dev) > 8 )
        {
            STORE_UINT16( &cmd[1], addr );
        }
        else
        {
            cmd[1] = EXTRACT_BYTE( 0, addr );
        }

        if (platform_nvram_lock())
        {
            platform_nvram_open( dev );
            platform_nvram_write( dev, cmd, 1 + (NVRAM_ADDR_WIDTH(dev) / 8) );
            platform_nvram_read( dev, pDest, num_bytes );
            platform_nvram_close( dev );
            platform_nvram_unlock();
        }
    }
}

/**
 * Writes num_bytes of data to the NVRAM from the buffer
 * pointed to by pSrc, starting at address addr
 *
 * @author rc10851
 *
 * @param dev The number of the NVRAM device upon which to operate
 * @param addr The address in NVRAM at which to start writing
 * @param num_bytes The number of bytes to write to NVRAM
 * @param pDest Pointer to the location at which the data to be
 *              written is stored
 */
void nvram_device_write( uint8 dev, uint32 addr, uint32 num_bytes, const uint8* pSrc )
{
    uint8 cmd[1 + sizeof(addr)];
    uint32 xferAmount;
    uint8_t retries = 0;

    if ( (addr + num_bytes) <= NVRAM_SIZE(dev) )
    {
        while ( num_bytes && (retries < LOCK_NVRAM_RETRIES))
        {
            /* Do not exceed the page size or cross page boundaries when writing */
            xferAmount = MIN( num_bytes,
                              NVRAM_PAGE_SIZE(dev) - (addr % NVRAM_PAGE_SIZE(dev)) );

            if (!platform_nvram_lock())
            {
                retries++;
                continue;
            }
            retries = 0;

            /* Write-enable the device, then write the page */
            writeEnable( dev );

            cmd[0] = CMD_PAGE_PROGRAM;

            if ( NVRAM_ADDR_WIDTH(dev) > 24 )
            {
                STORE_UINT32( &cmd[1], addr );
            }
            else if ( NVRAM_ADDR_WIDTH(dev) > 16 )
            {
                STORE_UINT24( &cmd[1], addr );
            }
            else if ( NVRAM_ADDR_WIDTH(dev) > 8 )
            {
                STORE_UINT16( &cmd[1], addr );
            }
            else
            {
                cmd[1] = EXTRACT_BYTE( 0, addr );
            }


            platform_nvram_open( dev );
            platform_nvram_write( dev, cmd, 1 + (NVRAM_ADDR_WIDTH(dev) / 8) );
            platform_nvram_write( dev, pSrc, xferAmount );
            platform_nvram_close( dev );

            /* Wait for the write to complete */
            waitOperationComplete( dev );
            platform_nvram_unlock();

            num_bytes -= xferAmount;
            addr += xferAmount;
            pSrc += xferAmount;
        }
    }
}

/**
 * Erases the NVRAM segment(s) associated with the specified address range.
 * If the range given matches the address space of the device, then a
 * bulk erase is performed
 *
 * @author rc10851
 *
 * @param dev The number of the NVRAM device upon which to operate
 * @param startAddr The address in NVRAM at which the erase should begin
 * @param endAddr The address in NVRAM at which the erase should end
 */
void nvram_device_erase( uint8 dev, uint32 startAddr, uint32 endAddr )
{
    debug_assert(startAddr % NVRAM_SECTOR_SIZE(dev) == 0);
    debug_assert(startAddr <= endAddr);
    boolean isBulkErase = (endAddr - startAddr + 1) == NVRAM_SIZE(dev);
    uint8 cmd[1 + sizeof(startAddr)];
    uint32_t bytesToErase = 0;
    uint8_t retries = 0;

    if ( (startAddr < NVRAM_SIZE(dev)) && (endAddr < NVRAM_SIZE(dev)) )
    {
        if (isBulkErase)
        {
            cmd[0] = CMD_BULK_ERASE;
        }

        do
        {
            if ( !isBulkErase )
            {
                if ( NVRAM_ADDR_WIDTH(dev) > 24 )
                {
                    STORE_UINT32( &cmd[1], startAddr );
                }
                else if ( NVRAM_ADDR_WIDTH(dev) > 16 )
                {
                    STORE_UINT24( &cmd[1], startAddr );
                }
                else if ( NVRAM_ADDR_WIDTH(dev) > 8 )
                {
                    STORE_UINT16( &cmd[1], startAddr );
                }
                else
                {
                    cmd[1] = EXTRACT_BYTE( 0, startAddr );
                }

                if (NVRAM_LARGE_BLOCK_SIZE(dev) &&
                        ((endAddr - startAddr) >= NVRAM_LARGE_BLOCK_SIZE(dev)) &&
                        ((startAddr % NVRAM_LARGE_BLOCK_SIZE(dev)) == 0))
                {
                    cmd[0] = CMD_LARGE_BLOCK_ERASE;
                    bytesToErase = NVRAM_LARGE_BLOCK_SIZE(dev);
                }
                else if (NVRAM_SMALL_BLOCK_SIZE(dev) &&
                         ((endAddr - startAddr) >= NVRAM_SMALL_BLOCK_SIZE(dev)) &&
                         ((startAddr % NVRAM_SMALL_BLOCK_SIZE(dev)) == 0))
                {
                    cmd[0] = CMD_SMALL_BLOCK_ERASE;
                    bytesToErase = NVRAM_SMALL_BLOCK_SIZE(dev);
                }
                else
                {
                    cmd[0] = CMD_SECTOR_ERASE;
                    bytesToErase = NVRAM_SECTOR_SIZE(dev);
                }
            }

            if (!platform_nvram_lock())
            {
                retries++;
                continue;
            }
            retries = 0;

            /* Write-enable the device, then erase the page */
            writeEnable( dev );

            platform_nvram_open( dev );
            platform_nvram_write( dev, cmd, 1 + (isBulkErase ? 0 : (NVRAM_ADDR_WIDTH(dev) / 8)) );
            platform_nvram_close( dev );

            startAddr += bytesToErase;

            /* Wait for the erase to complete */
            waitOperationComplete( dev );
            platform_nvram_unlock();
        }
        while ( !isBulkErase && (startAddr < endAddr) && (retries < LOCK_NVRAM_RETRIES) );
    }
}

/**
 * Reads the status register on the NVRAM
 *
 * @author rc10851
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
static uint8 readStatusRegister( uint8 dev )
{
    uint8 cmd[] = { CMD_READ_STATUS_REG };
    uint8 status = 0xFF;

    platform_nvram_open( dev );
    platform_nvram_write( dev, cmd, sizeof(cmd) );
    platform_nvram_read( dev, &status, sizeof(status) );
    platform_nvram_close( dev );

    return status;
}

/**
 * Waits for a write or erase operation to complete
 *
 * @author rc10851
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
static void waitOperationComplete( uint8 dev )
{
    /* Check the status register until the write is done */
    while ( readStatusRegister( dev ) & STATUS_WIP )
    {
        platform_nvram_idle_callback( dev );
    }
}

/**
 * Enables writes on the NVRAM
 *
 * @author rc10851
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
static void writeEnable( uint8 dev )
{
    uint8 cmd[] = { CMD_WRITE_ENABLE };

    platform_nvram_open( dev );
    platform_nvram_write( dev, cmd, sizeof(cmd) );
    platform_nvram_close( dev );
}
