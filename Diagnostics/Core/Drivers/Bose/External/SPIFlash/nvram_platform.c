/*************************************************************************//**
 * @file            nvram_platform.c
 * @brief           Platform-specific routines for Abstract NVRAM driver
 *                  and generic NVRAM device driver
 *
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: nvram_platform.c 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      This file contains the platform definitions for the abstract NVRAM
 *      driver and generic NVRAM device driver.  It includes the public macros,
 *      type definitions and functions that must be supplied by the platform
 *      in order for the drivers to function.
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/

/*** Include Files ***********************************************************/
#include "nvram_platform.h"

/* Include other files from your platform here */
#include "project.h"
#include "SpiBus_API.h"

SCRIBE_DECL(system)

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/
SCRIBE_DECL(flash);

/*** Module Macros ***********************************************************/
#define NVRAM_SEMAPHORE_TIMEOUT TIMER_MSEC_TO_TICKS(5000)

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/

/*** Module Variables ********************************************************/
static SPI_BUS_HANDLE_TYPE SpiHandle = NULL;
static xSemaphoreHandle NVRAMMutex;

/*** Module Functions ********************************************************/

/**
 * Platform-specific initialization
 *
 * Performs any required platform-specific initialization for
 * using the NVRAM.  This should include hardware/GPIO
 * configuration, SPI peripheral configuration if necessary,
 * etc.
 *
 * @author dj1005472
 */
void platform_nvram_init( void )
{
    SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;

    SPI_Bus_Config_t SpiConfig = {0};

    // Beginning of SPI Bus driver info
    memcpy( &SpiConfig,
            &spiDefaultConfig[SPI_FLASH_BUS_NUM - 1],
            sizeof(SpiConfig) );

    // use default SPI bus clocking.
    SpiConfig.baudRateExponent = 0;

    // gpio bank must be NULL when the Chip Select pin is under user control.
    // This tells the SPI bus driver that we will be driving chip select pin.
    SpiConfig.csGpioBank        = NULL;
    SpiConfig.chipSelectControl = CHIP_SELECT_CONTROL_API_USERS;

    // For user control, explicitly configure the csPin as an output
    ConfigureGpioPin(SPI_FLASH_CS_BANK, SPI_FLASH_CS_PIN, SPI_FLASH_CS_MODE, SPI_FLASH_CS_AF);
    ASSERT_GPIO_PIN_INT(SPI_FLASH_CS_BANK, SPI_FLASH_CS_PIN);

    SpiHandle = SPIBus_Initialize(&SpiConfig, &SpiError);

    NVRAMMutex = xSemaphoreCreateMutex();

    debug_assert(SpiHandle);
}

/**
 * Platform-specific idle routine
 *
 * Performs any platform-specific method of busy-waiting; this is
 * primarily used when checking the status register for completion
 * of a write or erase operation.  This provides a mechanism for
 * the platform to "nice" this process so that MIPS are not eaten
 * checking the status register at an insane speed
 *
 * @author dj1005472
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
void platform_nvram_idle_callback( uint8 dev )
{

}

/**
 * Platform-specific routine for opening the communications
 * channel to the NVRAM device
 *
 * Performs the necessary steps to open the communications
 * channel to the NVRAM device.  For example, chip-selecting a
 * device that uses SPI
 *
 * @author dj1005472
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
void platform_nvram_open( uint8 dev )
{
    DEASSERT_GPIO_PIN_INT(SPI_FLASH_CS_BANK, SPI_FLASH_CS_PIN);
}

/**
 * Platform-specific routine for closing the communications
 * channel to the NVRAM device
 *
 * Performs the necessary steps to close the communications
 * channel to the NVRAM device.  For example, de-chip-selecting
 * a device that uses SPI
 *
 * @author dj1005472
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
void platform_nvram_close( uint8 dev )
{
    ASSERT_GPIO_PIN_INT(SPI_FLASH_CS_BANK, SPI_FLASH_CS_PIN);
}

/**
 * Platform-specific routine for reading data from the NVRAM
 * device via the comms channel
 *
 * Performs the necessary steps to read data from the NVRAM
 * device depending on the physical communications medium
 *
 * @author dj1005472
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
void platform_nvram_read( uint8 dev, uint8* const dest, uint32 count )
{
    SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;

    uint16_t spi_bytes_ret = SPIBus_Communicate( SpiHandle,
                             count,      /* read len  */
                             NULL,       /* no write */
                             dest,       /* read */
                             NULL,       /* blocking */
                             SPI_WAIT_MS,/* blocking timeout*/
                             &SpiError );

    if (SpiError)
    {
        LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "Spi error: %d (%s)", SpiError, __func__);
        debug_assert(0);
    }
}

/**
 * Platform-specific routine for writing data to the NVRAM
 * device via the comms channel
 *
 * Performs the necessary steps to write data to the NVRAM
 * device depending on the physical communications medium
 *
 * @author dj1005472
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
void platform_nvram_write( uint8 dev, const uint8* src, uint32 count )
{
    SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;

    uint16_t spi_bytes_ret = SPIBus_Communicate( SpiHandle,
                             count,          /* page write len */
                             (uint8_t*) src, /* write buffer */
                             NULL,           /* no read */
                             NULL,           /* blocking */
                             SPI_WAIT_MS,    /* blocking timeout*/
                             &SpiError );

    if (SpiError)
    {
        LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "Spi error: %d (%s)", SpiError, __func__);
        debug_assert(0);
    }
}

/**
 * Platform-specific routine for locking access to nvram.
 *
 * @author dj1005472
 *
 * @param dev The number of the NVRAM device upon which to operate
 */
BOOL platform_nvram_lock(void)
{
    BOOL locked = TRUE;
    if (xSemaphoreTake(NVRAMMutex, NVRAM_SEMAPHORE_TIMEOUT) == pdFALSE)
    {
        LOG(system, ROTTEN_LOGLEVEL_NORMAL, "%s timed out!", __func__);
        locked = FALSE;
    }
    return locked;
}

void platform_nvram_unlock(void)
{
    xSemaphoreGive(NVRAMMutex);
}
