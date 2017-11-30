//
// WirelessAudioUpdate.c
//

#include "project.h"
#include "WirelessAudioUpdate.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioISR.h"
#include "darr_prog_new_qual_03.01.25_8437.h"

#include "UITask.h"
#include "UpdateManagerBlob.h"
#include "BufferManager.h"
#include "etap.h"
#include "UpdateVariant.h"

#if defined(MAXWELL) || defined(SKIPPER)
#include "internalFlashAPI.h"
#include "heatshrink_common.h"
#include "heatshrink_config.h"
#include "heatshrink_decoder.h"
#else
#include "nvram.h"
#endif

SCRIBE_DECL(wa_update);

#define WA_MAX_EXPAND_BUFF_SIZE 1024
#define MAX_EXPANSION_FLASH_BUFF 0x8000   
#define BL_IMAGE_HEADER_OFFSET 0
#define MU_IMAGE_HEADER_OFFSET 0x3000
#define CU_IMAGE_HEADER_OFFSET 0x10000
#define WA_MAX_TX_ARRAY 256
#define MAX_TX_SIZE 32 /* Use this to make the traces look like each other */
#define FINALBLOCK_UNCOMPRESSED_DATA_GROUPING_EQUALIZER 40 /* FUDGE */

#if defined(MAXWELL) || defined(SKIPPER)
#define PAGE_SIZE                0x8000
#define BASE_EXPANSION_FLASH     0x08008000
#endif

typedef __packed struct header0
{
    char  marker[4];            // Marker "DARR"
    uint8_t i2cMapAddrSize;     // 0x02 I2C Map Address Size in bytes
    uint8_t i2cMSTR_IMTP_VAL  ; // 0x01 i2c master IMTP register value   |
    uint8_t i2cMSTR_IMSCLP_VAL; // 0x22 i2c master IMSCLP register value | Specify i2c speed
    uint8_t i2cMSTR_IMST;       // 0x22 i2c master IMST register value   |
    uint8_t i2cMSTR_IMDS;       // 0x21 i2c master IMDS register value   |
    uint8_t clkMCUCC1;          // 0x7f MCU clock
    uint8_t clkMCUCC2;          // 0x07 Peripheral Clock
    uint8_t rsvd0;              // 0x00 Currently not in use
    uint8_t numItemsRecTab;     // 0x0a Number of items in record table
    uint8_t rsvd1;              // 0x00 Not in use
    uint16_t checksum;          // 16bit CRC
}waUpdateHeader0_t;

typedef __packed struct header1
{
    uint8_t id_and_record_type; // bit7 in use, bit1 = data record, bit 0 = code record
    uint16_t recordSize;
    uint16_t crc;
    uint16_t eepromAddress;
    uint16_t extDataAddress;
    uint16_t firmwareExecAddress;
    uint8_t majorVersionSource;
    uint8_t minorVersionSource;
    uint8_t buildVersionSource;
    uint16_t checksum; //
} waUpdateHeader1_t;

typedef __packed struct waUpdate
{
    uint32_t muRealLength;  /* The length in the headers is incorrect, pass this to flash function */
    uint32_t cuRealLength;  /* The length in the headers is incorrect, pass this to flash function */
    waUpdateHeader1_t muH1; /* Blob creation Fill (compressed or extenal flash access problematic) */
    waUpdateHeader1_t cuH1; /* Blob creation Fill (compressed or extenal flash access problematic) */
}waUpdateInfo_t;

typedef struct 
{
    uint8_t *txArray;      
    uint8_t *codeData;
    uint8_t *rxBuff;
    uint8_t *codeStart;
    uint8_t *testbuf;
    uint32_t testImageSize;
    uint32_t map_address;
    uint32_t flshOfst;
    uint8_t imageIndex;
    waUpdateHeader0_t *h0;
    waUpdateHeader1_t *h1;
    waUpdateHeader1_t *h1Mu;
    waUpdateHeader1_t *h1Cu;
    uint8_t *extFlashPtr;    /* Real address as reported by blob */
    uint8_t *waOffloadBuffer;/* Address of expanssion buffer    */
    waUpdateInfo_t    *meta; /* Carry metadata here */
    uint16_t numSections; /* Number of sections on this image */
    uint16_t curSection;  /* Section being processed */
    uint16_t carryOver;

    /* 
    ** Following fields deal with the decompressor and are 
    ** meaningful only to maxwell & skipper 
    */
    uint8_t hsw;       /* Filled from manifest entry */
    uint8_t hsl;       /* Filled from manifest entry */
    
    size_t deCompressInLen; 
    size_t deCompressorOutLen;
    void *waUpdtHSd;
    uint8_t *compressedImage;
    size_t compressedImageSize;
    size_t last_sunk;
} waImageStat_t;

static waImageStat_t *waUpgradeSetup(int imageIndex);
static void waUpdatePrepImg(uint8_t* buffer);
static uint8_t waEraseSector( unsigned int  map_addr);
static uint8_t *waUpdateGetBlock( waImageStat_t *wimp, int ofst, uint16_t len);
static uint8_t waUpgradeWrite(unsigned int map_addr, uint8_t *write_buf, unsigned int len);
static void waUpdateCleanup(waImageStat_t * wimp);
static waImageStat_t *waFetchImage(waImageStat_t *wimp);
static uint8_t waPullImageFromBlob(void);

#if defined(MAXWELL) || defined(SKIPPER)
static void waExpandCompress(uint32_t base, size_t len, waImageStat_t *wimp);
#endif

// TODO: delete the old stuff and pull this in
extern const unsigned char codestart[];
extern const unsigned char codedata[];
#define CODE_DATA_SZ 2117

static uint8_t *waImagePtr = NULL;
static uint32_t waImageSize = 0;
static waImageStat_t waImageProfile = {0};
static waUpdateInfo_t waLocalMeta = {0};

#if defined(GINGER) || defined(BARDEEN) || defined(PROFESSOR)
static uint8_t *waOffloadBuffer = NULL;
static waUpdateHeader1_t waLocalHeader1 = {0};
static waUpdateHeader0_t waLocalHeader0 = {0};
#endif

#if defined(MAXWELL) || defined(SKIPPER)
static uint8_t *__imgPtr__ = NULL;
#endif

static BOOL updateInProgress = FALSE;

BOOL WirelessAudioUpdate_IsDarrUpdating(void)
{
    return updateInProgress;
}

void WirelessAudioUpdate_UpgradeImage(void)
{
    updateInProgress = TRUE;
    char *errMsg = NULL;
    uint8_t rxbuf [4] = {0};
    char *str[] = {"Boot Loader:", "\nMU:", "\nCU:"};
    waImageStat_t *wimp = 0;
    uint8_t ret = 0;

    WirelessAudio_EnableEXTI(FALSE);
    WirelessAudioUtilities_AssertReset(TRUE);

#if defined(MAXWELL) || defined(SKIPPER)
    /*
    ** Maxwell and skipper, lacking external flash, hold their images in internal flash, which is 
    ** easier to read, find the blob and point to the compressed image. 
    */
    if(waPullImageFromBlob())
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "Blob problem.");
        updateInProgress = FALSE;
        return;
    }

    /*
    ** Now that we know where the image is, feed it to decompressor and expand 1/3 of it
    ** into the spareblock in internal flash
    */
    waExpandCompress( (uint32_t )BASE_EXPANSION_FLASH, MAX_EXPANSION_FLASH_BUFF, &waImageProfile ); /* Expand 32 K */

    /*
    ** remap image pointer to uncompressed area since we still need to pull in the metadata, etc. Also remember that the image size is the compressed
    ** size, the real size should not be needed since we use internal headers to know when we are done.
    */
    waImagePtr =(uint8_t *) BASE_EXPANSION_FLASH;
    waImageSize = waImageProfile.compressedImageSize;
#endif

    /*
    ** And since we are going to flash boot loader, MU and CU images cycle three times to the 
    ** whole process.
    */
    for(unsigned int imIndex = 0; imIndex < 3; imIndex++)
    {
        TAP_PrintString(str[imIndex]);

        wimp = waUpgradeSetup(imIndex); /* Always start here to force reloading of 8052 ram image */

        if(!wimp || !wimp->testbuf)
        {
            errMsg = "Upgrade Setup returned NULL buffer";
            break;
        }

        uint32_t imgSize = wimp->testImageSize;
        uint32_t map_address = wimp->map_address;
        uint8_t num_recs = imgSize / 256;
        uint8_t remainder = imgSize % 256;
        int delta = 0;

        waUpdatePrepImg(wimp->testbuf);
        /*
        ** This is a blunt instrument, use on the first pass only,
        ** The size should be enough to erase a 1Mbit piece, revisit when
        ** erasing 2 Mbit piece.
        */
        if(imIndex == 0)
        {
            TAP_PrintString("\nClearing flash: ");
            for(unsigned int i = 0; i < 32; i++)
            {
                ret = waEraseSector(i * 0x1000);
                if(ret)
                {
                    /*
                    ** most likely something else will fail, wait for now. 
                    */
                    //errMsg = "Failed to erase sector";
                    //break;  
                }
                TAP_PrintString("#");
            }
            /*
            ** This should be unecessary but it appears in their trace and in order to generate
            ** a trace as close to theirs, we add it here.
            */
            wimp = waUpgradeSetup(imIndex);
            for(unsigned int i = 0; i < 32; i++)
            {
                ret = waEraseSector(i * 0x1000);
                if(ret)
                {
                    /*
                    ** most likely something else will fail, wait for now. 
                    */
                    //errMsg = "Failed to erase sector";
                    //break;  
                }
            }
            /*
            ** And yes... Again!
            */
            wimp = waUpgradeSetup(imIndex);
        }

        /*
        ** Now program Application FW into Flash
        ** map address should be, for BL:0x100, for MU:0x3100, for the CU:0x10100.
        ** trying out this theory on the MU
        */
        delta = 0;
        TAP_PrintString("\nFlashing image: ");

        for(unsigned int i = 0; i < num_recs; i++)
        {
            uint8_t *tmptr = NULL;
            tmptr = waUpdateGetBlock(wimp, delta, WA_MAX_TX_ARRAY);
            ret = waUpgradeWrite(map_address + delta, tmptr, WA_MAX_TX_ARRAY);
            if(ret)
            {
                errMsg = "Failed on upgrade write!";
                break;
            }
            delta += 256;
            TAP_PrintString(".");
        }

        if(remainder)
        {
            uint8_t *tmptr = NULL;
            tmptr = waUpdateGetBlock(wimp, delta, remainder);
            ret = waUpgradeWrite(map_address + delta, tmptr, remainder);
            if(ret)
            {
                errMsg = "Failed on upgrade write!";
                break;
            }
            TAP_PrintString("*");
        }

        if(imIndex == 0)
        {
            /*
            ** Again! undocummented, but they seem to send the headers here
            ** First the header1 then header0
            */
            int ofst = BL_IMAGE_HEADER_OFFSET + sizeof(waUpdateHeader0_t);
            ret = waUpgradeWrite(ofst, (uint8_t*) wimp->h1, sizeof(waUpdateHeader1_t));
            if(ret)
            {
                errMsg = "Failed on upgrade write! h1";
                break;
            }
            ofst = BL_IMAGE_HEADER_OFFSET;

            ret = waUpgradeWrite(ofst, (uint8_t*) wimp->h0, sizeof(waUpdateHeader0_t));
            if(ret)
            {
                errMsg = "Failed on upgrade write! h0";
                break;
            }
        }
        else
        {
            int delta = CU_IMAGE_HEADER_OFFSET; /* Default to CU, add upper page */
            waUpdateHeader1_t *hdr = wimp->h1Cu;

            /*
            ** OK, Pull header depending on the iteration we are in:
            */
            if(imIndex == 1)
            {
                /*
                ** override default initialization
                */
                hdr = wimp->h1Mu;
                delta = MU_IMAGE_HEADER_OFFSET;
            }

            ret = waUpgradeWrite(delta, (uint8_t *) hdr, sizeof(waUpdateHeader0_t));
            if(ret)
            {
                errMsg = "Failed on upgrade write! h1";
                break;
            }
        }

        uint16_t j = 150;
        while(j--)
        {
            if(!WirelessAudio_I2C_Read16bReg(WA_REG_GPCS41, rxbuf, 1))
            {
                errMsg = "Failed to read Byte";
                break;
            }
            if((rxbuf[0] & ~0x80) == 0)
            {
                break;
            }
            vTaskDelay(TIMER_MSEC_TO_TICKS(10));
        }

        
        WirelessAudioUtilities_ResetDarr();
        /*
        ** We should probably re-initialize the DWAM here, for now leave but revisit.
        */
    }

    if(errMsg)
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "%s Failed with message: %s",__func__, errMsg);
    }
    waUpdateCleanup(wimp);
    WirelessAudioUtilities_AssertReset(FALSE);
    updateInProgress = FALSE;
}

static waImageStat_t *waUpgradeSetup(int imageIndex)
{
    waImageStat_t *wimp = NULL;
    char *errMsg = NULL;
    char *unusual = NULL;
    wimp = &waImageProfile;
    waImageProfile.imageIndex = imageIndex;
    unsigned int i = 0;

    /*
    ** This function prepares the DWAM module to start receiving an upgrade 
    ** Initially it does:
    **    a) STOP MCU
    **    b) Initialize DARR With a configuration Data size of 2kB
    **    c) Enable the SPI flash programming mode
    */
    do
    {
        wimp = waFetchImage(&waImageProfile);
        if(NULL == wimp)
        {
            errMsg = "Image to upgrade inaccessible";
            break;
        }
        TAP_PrintString("\nCopying image to 8052 ram: ");

        WirelessAudioUtilities_AssertReset(TRUE);
        vTaskDelay(TIMER_MSEC_TO_TICKS(25));
        WirelessAudioUtilities_AssertReset(FALSE);
        vTaskDelay(TIMER_MSEC_TO_TICKS(250));

        uint8_t data = 0xFE;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_MCUCC2, sizeof(data)))
        {
            errMsg = "Failed to HALT MCU";
            break;
        }

        data = 24;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISCLC, sizeof(data)))
        {
            errMsg = "Failed write to ISCLC";
            break;
        }

        data = 1; // 8051 Stop request
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISDLC, sizeof(data)))
        {
            errMsg = "Failed write to ISDLC";
            break;
        }

        data = 1; // Enable Remap
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_MPRR, sizeof(data)))
        {
            errMsg = "Unable to engage remapping";
            break;
        } 
        vTaskDelay(TIMER_MSEC_TO_TICKS(20)); // Allow any I2C transactions to end

        /*
        ** OK time to start transferring the data to the 8052 MCU
        ** We do so by writing to 0x2000
        */
        int code_cnt = 0;
        int waWriteSize = 0;
        while(code_cnt < CODE_DATA_SZ)
        {
            int delta = CODE_DATA_SZ - code_cnt;
            waWriteSize = (delta >= MAX_TX_SIZE) ? MAX_TX_SIZE : delta;
            uint16_t regAddr = 0x2000 + code_cnt;

            if(!WirelessAudio_I2C_Write16bRegRawAddr((wimp->txArray + code_cnt), regAddr, waWriteSize))
            {
                errMsg = "Error on page write, abandoning update";
                break;
            }
            code_cnt += waWriteSize;
            TAP_PrintString("@");
        }
        if( errMsg )
        {
            /*
            ** An error detected in inner loop
            ** abandon loop
            */
            break;
        }

        data = 0; // Disable Remap
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_MPRR, sizeof(data)))
        {
            errMsg = "Unable to disengage remapping";
            break;
        }

        data = 24;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISCLC, sizeof(data)))
        {
            errMsg = "Failed write to ISCLC";
            break;
        }

        data = 0xFF;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_MCUCC2, sizeof(data)))
        {
            errMsg = "Failed write to write MCUCC2";
            break;
        }

        /*
        ** This chunk disabled on sample source but not on doc. If un-implemented, we time out.
        */
        data = 1;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISDLC, sizeof(data)))
        {
            errMsg = "Failed write to ISDLC";
            break;
        }

        vTaskDelay(TIMER_MSEC_TO_TICKS(10));

        /*
        ** OK Fill in buffer
        */
        if(!WirelessAudio_I2C_Read16bReg(WA_REG_ISDLC, wimp->rxBuff, 1))
        {
            errMsg = "Failed to read byte (a)";
            break;
        }

        /*
        ** We just do not do the read because the time delay needed
        */
        i = 1000;
        while((wimp->rxBuff[0] != 1) && i)
        {
            if(!WirelessAudio_I2C_Read16bReg(WA_REG_ISDLC, wimp->rxBuff, 1))
            {
                errMsg = "Failed to read byte (b)";
                break;
            }
            i--;
        }

        if(errMsg)
        {
            break;
        }
        else if((i == 0) && (wimp->rxBuff[0] != 1))
        {
            /*
            ** Timed out awaiting the required condition
            ** a full second of wait, the sample code does not
            ** handle this condition. We should abort here,
            ** however for now, just continue the sequence. 
            ** errMsg = "Passthrough flash timed out awaiting condition"
            */
            unusual = "Timeout wait cond 1";
        }

        /*
        **  Reset PC to 0x2000
        */
        data = 22;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISCLC, sizeof(data)))
        {
            errMsg = "Failed write to ISCLC";
            break;
        }

        data = wimp->codeStart[1];
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISDLC, sizeof(data)))
        {
            errMsg = "Failed write to ISDLC";
            break;
        }

        data = 23;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISCLC, sizeof(data)))
        {
            errMsg = "Failed write to ISCLC";
            break;
        }

        data = wimp->codeStart[0];
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISDLC, sizeof(data)))
        {
            errMsg = "Failed write to ISDLC";
            break;
        }

        data = 24;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISCLC, sizeof(data)))
        {
            errMsg = "Failed write to ISCLC";
            break;
        }

        data = 3; // Update PC Value
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISDLC, sizeof(data)))
        {
            errMsg = "Failed write to ISDLC";
            break;
        }

        vTaskDelay(TIMER_MSEC_TO_TICKS(10));

        if(!WirelessAudio_I2C_Read16bReg(WA_REG_ISDLC, wimp->rxBuff, 1))
        {
            errMsg = "Failed to read byte (c)";
            break;
        }

        /*
        ** We just do not do the read because the time delay needed
        */
        i = 1000;
        while((wimp->rxBuff[0] != 1) && i)
        {
            if(!WirelessAudio_I2C_Read16bReg(WA_REG_ISDLC, wimp->rxBuff, 1))
            {
                errMsg = "Failed to read byte (d)";
                break;
            }
            i--;
        }

        if(errMsg)
        {
            break;
        }
        else if((i == 0) && (wimp->rxBuff[0] != 1))
        {
            /*
            ** Should handle timeout condition
            ** by aborting here. Revisit
            ** errMsg = "Passthrough flash timed out awaiting condition"
            */
            unusual = "Timeout wait cond 2";
        }

        /*
        ** Run Binary Code if 8051 
        */
        data = 24;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISCLC, sizeof(data)))
        {
            errMsg = "Failed write to ISDLC";
            break;
        }        

        data = 0;
        if(!WirelessAudio_I2C_Write16bReg(&data, WA_REG_ISDLC, sizeof(data)))
        {
            errMsg = "Failed write to ISDLC";
            break;
        }        

        if(!WirelessAudio_I2C_Read16bReg(WA_REG_ISDLC, wimp->rxBuff, 1))
        {
            errMsg = "Failed to read byte (e)";
            break;
        }

        /*
        ** We just do not do the read because the time delay needed
        */
        i = 1000;
        while((wimp->rxBuff[0] != 0x40) && i)
        {
            if(!WirelessAudio_I2C_Read16bReg(WA_REG_ISDLC, wimp->rxBuff, 1))
            {
                errMsg = "Failed to read byte (f)";
                break;
            }
            i--;
        }
        if(errMsg)
        {
            break;
        }
        else if((i == 0) && (wimp->rxBuff[0] != 1))
        {
            /*
            ** Should handle timeout condition
            ** by aborting here. Revisit
            ** errMsg = "Passthrough flash timed out awaiting condition"
            */
            unusual = "Timeout wait cond 3";
        }

        {   /* Start Undocumented chunk */
            /*
            ** It is Mystery time...
            ** By evesdropping on a programming session we can determine
            ** that location fff2 (fc00 + 3f2 (1010d)) is being read that either
            ** Bit 7 or bit 0 satisfies the reading loop.
            ** this register is the GPCS40 (General Purpose Chip Select 4 ?)register
            ** which appears nowhere in the  documentation, no to mention the
            ** function of the bits that are being checked, therefore...
            ** we wing it.
            */

            uint8_t j = 100;
            while(j--)
            {
                if(!WirelessAudio_I2C_Read16bReg(WA_REG_GPCS40, wimp->rxBuff, 1))
                {
                    errMsg = "Failed to read Byte from 0xff f2";
                    break;
                }
                if(wimp->rxBuff[0] & 0x80)
                {
                    break;
                }
            }

            if( errMsg )
            {
                break;
            }
        } /* End of undocumented chunk */

        /*
        ** Just prep to read 
        */
        if(!WirelessAudio_I2C_Read16bReg(WA_REG_GPCS41, wimp->rxBuff, 1))
        {
            errMsg = "Failed to read byte (g)";
            break;
        }

        i = 1000;
        while(!(wimp->rxBuff[0] & 0x80) && i)
        {
            if(!WirelessAudio_I2C_Read16bReg(WA_REG_GPCS41, wimp->rxBuff, 1))
            {
                errMsg = "Failed to read byte (h)";
                break;
            }
            i--;
        }

        if(errMsg)
        {
            break;
        }
        else if((i == 0) && (wimp->rxBuff[0] != 1))
        {
            /*
            ** Timed out awaiting the required condition
            ** a full second of wait, the sample code does not
            ** handle this condition. We should abort here,
            ** however for now, just continue the sequence. 
            ** errMsg = "Passthrough flash timed out awaiting condition"
            */
            unusual = "Timeout wait cond 4";
        }
    }while(0);

    if(errMsg)
    {
        LOG(wa_update,ROTTEN_LOGLEVEL_NORMAL,"%s:%s",__func__,errMsg);
        //waHandleEvent(LLW_EVENT_ERROR);
        wimp = NULL;
    }

    if(unusual)
    {
        unusual = NULL; /* A place to hung a breakpoint. */
    }

    return wimp;
}

static void waUpdatePrepImg(uint8_t* buffer)
{
    /*
    ** This function prepares an image section for programming. 
    ** this is where, depending on if compression is used, 
    ** the section would be obtained and set up for further processing
    */

#if defined(BLOB_IN_NVRAM) && (defined(BARDEEN) || defined(GINGER) || defined(PROFESSOR))
    char *errMsg = NULL;
    /*
    ** Currently not compressing so first allocate a buffer to copy nvram
    */
    do
    {
        int len = WA_MAX_EXPAND_BUFF_SIZE;
        if(waOffloadBuffer == NULL)
        {
            if( NULL == (waOffloadBuffer = BufferManagerGetBuffer( len )))
            {
                errMsg = "Failure to obtain a buffer from Buffer manager\n";
                break;
            }
            /*
            ** This function will do all the mapping under the covers, so just provide length and buffer
            */
            nvram_read((uint32_t) buffer,sizeof(waUpdateInfo_t),(uint8_t *)&waLocalMeta);
            nvram_read((uint32_t)(buffer+sizeof(waUpdateInfo_t)),len,waOffloadBuffer);
            break;
        }
    }while(0);
    if( errMsg )
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "%s:Failed with message: %s", __func__, errMsg);
        
        //TODO - add UI
//        UI_SetErrorState(TRUE, SYSTEM_ERROR_FIRMWARE_CRITICAL);
    }
#else 
    /*
    ** Until we get this working, just use the passed in buffer address and set up a static pointer. 
    */
    
    if( waImageProfile.compressedImageSize == 0 )
    {
        waImageProfile.compressedImage = buffer;
        waImageProfile.compressedImageSize = waImageSize;
    }
    __imgPtr__ = buffer;
#endif      
}

static uint8_t waEraseSector(unsigned int map_addr)
{
    uint8_t ret = 0;
    uint16_t j = 100;
    uint8_t rxbuf [4] = {0};
    uint8_t tmpbuf[3] = {0};
    char *errMsg = NULL;

    do
    {
        while(j--)
        {
            if(!WirelessAudio_I2C_Read16bReg(WA_REG_GPCS41, rxbuf, 1))
            {
                errMsg = "Failed to read Byte";
                break;
            }
            if((rxbuf[0] & ~0x80) == 0)
            {
                break;
            }
            vTaskDelay(TIMER_MSEC_TO_TICKS(10));
        }
        if( errMsg )
        {
            break;
        }

        tmpbuf[0] = (uint8_t)(map_addr);
        tmpbuf[1] = (uint8_t)(map_addr >> 8);
        tmpbuf[2] = (uint8_t)(map_addr >> 16);
        if(!WirelessAudio_I2C_Write16bRegRawAddr(tmpbuf, 0x1C02, 3))
        {
            errMsg = "Unable to write buffer to DWAM";
            break;
        }

        tmpbuf[0] = 0x82;
        if(!WirelessAudio_I2C_Write16bReg(tmpbuf, WA_REG_GPCS41, 1))
        {
            errMsg = "Unable to write byte";
            break;
        }
    }while(0);
    if( errMsg )
    {
        LOG(wa_update,ROTTEN_LOGLEVEL_NORMAL,"%s:%s",__func__,errMsg);
    }

    return ret;
}

static uint8_t *waUpdateGetBlock( waImageStat_t *wimp, int ofst, uint16_t len)
{
    debug_assert(len <= WA_MAX_TX_ARRAY);
    /*
    ** This function returns a chunk of the image, index is an offset into the image
    ** the address of which block will be returned. 
    */
    uint8_t *ret = NULL;

#ifdef BLOB_IN_NVRAM
    debug_assert(wimp);
    uint8_t *temp = wimp->testbuf;
    uint16_t delta = ofst % WA_MAX_EXPAND_BUFF_SIZE;
    uint8_t *ptr = &wimp->waOffloadBuffer[WA_MAX_EXPAND_BUFF_SIZE];
    uint16_t initial = 0;
    
    /*
    ** At the risk of being verbose,
    ** if the image is smack at the beginning of the buffer, things would be easy, however,
    ** the image can be any number of  bytes into the buffer (the beginning can have headers, etc).
    ** This causes the registration between blocks and the buffer to be skewed so to take 
    ** the skew into account, we always add the offset into the calculation and reset the buffer 
    ** to the beginning of the block
    */
    if(wimp->testbuf > wimp->waOffloadBuffer)
    {
        initial = (uint16_t)(wimp->testbuf - wimp->waOffloadBuffer); 
        delta += initial;
        temp = wimp->waOffloadBuffer;
    }

    if( delta && (( temp + delta + len ) <= ptr ))
    {
        /*
        ** Cool no overflow condition,
        ** just make sure to return correct location
        */
        ret = temp + delta;
    }
    else
    {
        nvram_read((uint32_t)wimp->extFlashPtr+ wimp->flshOfst+ ofst + initial,WA_MAX_EXPAND_BUFF_SIZE,wimp->waOffloadBuffer);
        ret = wimp->waOffloadBuffer;
    }
#else
    /*
    ** An ofst = 0 and len = 0 indicates that the static values should be reset
    ** to their original conditions. This should be done when update is done.
    */
    boolean resetStatics = ( ofst == 0 && len == 0 )/*?1:0*/;

    /*
    ** Because we expand in stages, here we must check if we are due for an exapansion
    ** remember that we use offset + imageptr to keep real offset.
    */
    static uint32_t tmp = 0;
    static uint32_t tmp2 = 0;
    static uint8_t overflow[256] = {0};
    static uint32 hdrOfst = 0;
    static uint8_t cuFirstBlock = 1;
    static uint8_t lastBlock = 0;
    
    if (resetStatics)
    {
        /*
        ** On failure, or for future updates, we need to have these in their original state
        ** least we run the risk of old state info seeping into the function
        */
        tmp = tmp2 = hdrOfst = 0;
        lastBlock = 0;
        cuFirstBlock = 1;
    }
    else
    {
        debug_assert(wimp);
        if( (wimp->imageIndex == 2) && (cuFirstBlock == 1) )
        {
            hdrOfst = (uint32_t) -sizeof(waUpdateInfo_t);/* account for  metadata */ 
            cuFirstBlock = 0;
        }

        tmp = tmp2 = (uint32_t )__imgPtr__ + ofst;
        tmp = tmp2 % MAX_EXPANSION_FLASH_BUFF;
        tmp2 += len;
        tmp2 = tmp2 % MAX_EXPANSION_FLASH_BUFF;
        if( tmp2 <= tmp && !lastBlock )
        {
            int fudge = 0; /* A variable since it can have a value other than 0 depending on value of cuFirstBlock */
            LOG(wa_update, ROTTEN_LOGLEVEL_VERBOSE, "Update page wrap around");
            if((cuFirstBlock == 0) && (tmp2 == 0))
            {
                tmp2 = FINALBLOCK_UNCOMPRESSED_DATA_GROUPING_EQUALIZER;
                fudge = FINALBLOCK_UNCOMPRESSED_DATA_GROUPING_EQUALIZER;;
                lastBlock = 1;
            }
            memcpy( overflow, &__imgPtr__[(ofst+fudge) % MAX_EXPANSION_FLASH_BUFF], len - tmp2 );
            waExpandCompress(BASE_EXPANSION_FLASH, MAX_EXPANSION_FLASH_BUFF, wimp); 
            memcpy( overflow+len - tmp2,(uint8_t *)BASE_EXPANSION_FLASH, tmp2 );
            hdrOfst = ofst + len - fudge;
            __imgPtr__ = (uint8_t *)BASE_EXPANSION_FLASH + tmp2;
            if ( lastBlock )
            {
                int num = -0x100;
                /* 
                ** Reasoning:  we are expanding a compressed image into an decompression buffer. 
                ** The decompression buffer is: 0x8000 bytes long. There are three distinct images that 
                ** are embedded in compressed image, and each is governed by a header. 
                ** The first two images are easy, their headers allow us to operate withing the same expansion block,
                ** the last image, not so much. Also because we prepend metadata concerning the image to the compressed 
                ** containg image, the line of demarcation is not as clean.
                ** The hdrOfst variable helps us keep track of where we are in the current (contained) image.
                ** The image helps with registration of the image within the larger containing image. 
                ** When we finish reading the 2 contained image into the expansion buffer we are 9/10s of the way into
                ** the expansion buffer.  instead of pulling in the third image into the the backend of the expansion 
                ** buffer, we chose to eliminate the original metadata offset and pull the third and last image onto the
                ** beginning of the expansion block.
                ** This while good for our expansion, causes strange adjustments to be made onto some of the variables
                ** we have been using to keep track of the image. 
                ** In the case below, this translates to -256 bytes of data 
                ** Because the original code used unsigned integers and this is a fix for an obscure bug to accomodate
                ** a corner case, I did not want to restructure original code to use ints ( I want to lesses the
                ** the risk of this fix), I am forced to play games with the unsigend vaiables.
                ** I assume that -0x100 is more meaningfull than 0xffffff00 so that is what I did. 
                ** the intent is: we will adjust the offest by -255 bytes. 
                */
                
                hdrOfst = (uint32_t )num;
            }
            ret = overflow;
        }
        else
        {
          
          ret =  &__imgPtr__[ (ofst - hdrOfst) % MAX_EXPANSION_FLASH_BUFF ];
        }
    }
#endif

    return ret ;
}

static uint8_t waUpgradeWrite(unsigned int map_addr, uint8_t *write_buf, unsigned int len)
{
    uint8_t ret = 0;
    uint8_t rxbuf [4] = {0};
    uint8_t tmpbuf[5] = {0};
    char *errMsg = NULL;

    do
    {
        uint16_t j = 150;
        while(j--)
        {
            if(!WirelessAudio_I2C_Read16bReg(WA_REG_GPCS41, rxbuf, 1))
            {
                errMsg = "Failed to read Byte";
                break;
            }
            if( (rxbuf[0] & ~0x80) == 0 )
            {
                break;
            }
        }
        if( errMsg )
        {
            break;
        }

        WirelessAudio_I2C_Write16bRegRawAddr(write_buf, 0x0000, len);

        tmpbuf[0] = (uint8_t) map_addr;
        tmpbuf[1] = (uint8_t) (map_addr >> 8);
        tmpbuf[2] = (uint8_t) (map_addr >> 16);
        tmpbuf[3] = (uint8_t) len;
        tmpbuf[4] = (uint8_t) (len >> 8);
        if(!WirelessAudio_I2C_Write16bRegRawAddr(tmpbuf, 0x1C02, 5))
        {
            errMsg = "Unable to write buffer to DWAM";
            break;
        }

        /*
        ** Init Program Operation
        */
        tmpbuf[0] = 0x81;
        if(!WirelessAudio_I2C_Write16bReg(tmpbuf, WA_REG_GPCS41, 1))
        {
            errMsg = "Failed Read Setup";
            break;
        }
    }while(0);

    if(errMsg)
    {
        LOG(wa_update,ROTTEN_LOGLEVEL_NORMAL,"%s:%s",__func__,errMsg);
        ret = 1;
    }

    return ret;
}

static void waUpdateCleanup(waImageStat_t * wimp)
{
    if( wimp )
    {
#if defined(MAXWELL) || defined(SKIPPER) 
        heatshrink_decoder_free(wimp->waUpdtHSd);
        waUpdateGetBlock(NULL,0,0); /* Purge statics */
#endif
        if( wimp->waOffloadBuffer )
        {
            BufferManagerFreeBuffer(wimp->waOffloadBuffer);
        }
#if defined(BARDEEN) || defined(GINGER) || defined(PROFESSOR)
        waOffloadBuffer = NULL;
#endif
        memset(wimp, 0, sizeof(waImageStat_t));
    }
}

static waImageStat_t *waFetchImage(waImageStat_t *wimp)
{
    waImageStat_t *ret = wimp;
    
    char *errMsg = NULL;
    do
    {
        static uint8_t rx_array[10];
        /*
        ** Not well advertized, but... it looks like the codedata block is just code
        ** that is pushed up onto ram to allow the 8052 for the writing of the image,
        ** This is pushed first, then the actual image is filled in (by 256 chunks) to eprom
        */
        wimp->codeData = (uint8_t *)codedata;
        wimp->codeStart = (uint8_t *)codestart;

        wimp->rxBuff = rx_array;
        if( wimp->txArray == NULL )
        {
            wimp->txArray = wimp->codeData;
        }
        
#ifdef BLOB_IN_NVRAM

        /*
        ** First a few notes on setup:   Blob image is compressed so GetImageOffsetByName will Point
        ** to a compressed section. The size is the compressed size
        */
        if((waPullImageFromBlob() || waImagePtr == NULL ) || (waImageSize == NULL))
        {
            errMsg = "Update image region is not yet initialized";
            break;
        }
        /*
        ** OK the image was pulled from the blob, if it existed in external flash, it has already been mapped
        ** so the expand buffer should have the actual pointer to the block we are currently using, 
        ** however we still need the actual address in the external flash so we can pull in further chunks, do
        ** save that address to the wimp, and replace the mapped address.
        */
        wimp->extFlashPtr = waImagePtr + sizeof(waUpdateInfo_t); /* Save real image address, skip metadata*/
        wimp->waOffloadBuffer = waOffloadBuffer;
        waImagePtr = wimp->waOffloadBuffer; /* We have already read and expanded a block point to it.*/
        /*
        ** In the case of a real blob, the waUpdatePrepImg function has already pulled out the metadata structure, 
        ** and adjusted the buffer, just set the wimp so it will not be executed again
        */
        wimp->meta = &waLocalMeta;
        wimp->testbuf = (uint8_t *)waImagePtr;
#else

        /*
        ** Copy the contents of the image pointer into local metadata,
        ** less efficient than just pointing to the location in the block
        ** but more flexible in terms of different update modules (e.g. bogus image, real blob, etc
        */
        if( wimp->meta == NULL )
        {
            waLocalMeta = *((waUpdateInfo_t *)waImagePtr); /* Structure copy */
            wimp->meta = &waLocalMeta;
        }
        wimp->testbuf = (uint8_t *)waImagePtr + sizeof(waUpdateInfo_t);
#endif
        wimp->testImageSize = (uint16_t)waImageSize - sizeof(waUpdateInfo_t);
        /*
        ** OK we have a huge monolitic image, now just check which subimage we need, i.e. what imageIndex we are using
        ** and descend onto the correct location and fill in the rest of the details. 
        */
        if( wimp->imageIndex == 0 )
        {
            /*
            ** Get the boot loader, header 0 and 1 are sequentially from location 0 
            */
#ifdef BLOB_IN_NVRAM
            wimp->h0 = &waLocalHeader0;
            wimp->h1 = &waLocalHeader1; /* This will be overriden but by then we won't need this anymore */
            memcpy(wimp->h0, wimp->testbuf, sizeof(waUpdateHeader0_t));
            memcpy(wimp->h1, wimp->testbuf + sizeof(waUpdateHeader0_t), sizeof(waUpdateHeader1_t));
#else            
            wimp->h0 = (waUpdateHeader0_t*)wimp->testbuf;
            wimp->h1 = (waUpdateHeader1_t*)(wimp->h0+1);
#endif            
            /*
            ** In a linear image, there is no problem with the following assignment
            ** The assumption is that the current location plus the value in the 
            ** header for the eepromaddress will point to a location in the linear 
            ** address space, however this does not necessarily hold true for a system
            ** where there may be an intermediate buffer in place (e.g. reading from blob, or compression)
            ** for the first image (BL) there is almost a 100% likelihood that the address will be within
            ** the width of the retention buffer so let the first one slide, however for h1Mu and h1Cu these 
            ** will have to be comverted upon reading the values from flash as the addresses should be based on
            ** the flash pointer.
            */
            
            wimp->testbuf = (uint8_t *)(wimp->testbuf + wimp->h1->eepromAddress);
            wimp->testImageSize = (uint32_t)wimp->h1->recordSize;
            wimp->map_address = wimp->h1->eepromAddress;
            break;
        }
        if( wimp->imageIndex == 1 || wimp->imageIndex == 2 )
        {

            waUpdateHeader1_t *h1 = NULL;
            
            if( wimp->imageIndex == 1)
            {
#ifdef BLOB_IN_NVRAM
                nvram_read((uint32_t)wimp->extFlashPtr + MU_IMAGE_HEADER_OFFSET,sizeof(struct header1),(uint8_t *)&waLocalHeader1);
                wimp->h1Mu = &waLocalHeader1;
#else
                /*
                ** We now pre-fill metadata so this is not needed anymore
                ** wimp->h1Mu = (struct header1 *)(wimp->testbuf + MU_IMAGE_HEADER_OFFSET);
                */
                wimp->h1Mu = &wimp->meta->muH1;
                
#endif                
                h1 = wimp->h1Mu;
                wimp->testImageSize = wimp->meta->muRealLength;
                
            }
            else
            {

#ifdef BLOB_IN_NVRAM
                nvram_read((uint32_t)wimp->extFlashPtr + CU_IMAGE_HEADER_OFFSET,sizeof(struct header1),(uint8_t *)&waLocalHeader1);
                wimp->h1Cu = &waLocalHeader1;
#else                
                wimp->h1Cu = &wimp->meta->cuH1;
#endif                
                h1 = wimp->h1Cu;
                wimp->testImageSize = wimp->meta->cuRealLength;
            }


            wimp->map_address = h1->eepromAddress;
            if( wimp->imageIndex == 2 )
            {
#if defined(MAXWELL) || defined(SKIPPER)
                waExpandCompress(BASE_EXPANSION_FLASH, MAX_EXPANSION_FLASH_BUFF, wimp);
                wimp->testbuf = BASE_EXPANSION_FLASH + (uint8_t *)h1->eepromAddress;
                wimp->map_address += 0x10000; /* Add upper page */
                break;
#else
                wimp->map_address += 0x10000; /* Add upper page (in this case no need to accound for expansion buffer*/
#endif
            }

#ifdef BLOB_IN_NVRAM
            wimp->testbuf = wimp->waOffloadBuffer; 
            wimp->flshOfst = wimp->map_address;
#else            
            wimp->testbuf += wimp->map_address;
#endif            
            break;
        }
        /*
        ** If we are still here, we have an incorrect index. 
        */
        errMsg = "Incorrect Sub-Image index value.";
        break;   /* I know, Not needed */
    }
    while(0);
    if( errMsg )
    {
        LOG(wa_update,ROTTEN_LOGLEVEL_NORMAL,"%s:%s",__func__,errMsg);
        ret = NULL;
    }
    return ret;
}

static uint8_t waPullImageFromBlob(void)
{
    uint8_t ret = 1;
    ManifestEntry manifest;
    char *errMsg = NULL;

    UpdateBlobHeader blob = {0};
    do
    {
        uint16_t ofset = 0;

        if(!ReadBlobHeaderFromFlash(&blob, BLOB_ADDRESS))
        {
            errMsg = "Unable to obtain blob header from flash";
            break;
        }

        /*
        ** Do we need a to get a manifest???? Maybe we want to at some point sepate
        ** MU, CU, BL sub images, for now do this and if not needed remove 
        */
        if(!GetManifestEntryByName(&blob,BLOB_ADDRESS,&manifest, WA_VARIANT_DARR_FILE_NAME) )
        {
            errMsg = "No entry found for 'wam'";
            break;
        }
        waImageProfile.hsw = manifest.HeatshrinkWindow;
        waImageProfile.hsl = manifest.HeatshrinkLength;
#if defined (MAXWELL) || defined( SKIPPER )
        if( waImageProfile.hsw == 0 && waImageProfile.hsl == 0 )
        {
            errMsg = "Maxwell and Skipper no longer support uncompressed blobs, make sure you are not using wam*.bos";
            break;
        }
#endif

        /*
        ** OK we have a manifest entry, next stop manifest destiny...
        ** Bad puns aside, we now just pull in the image offset and set up in a nice static 
        ** area where our update function con pull it out of with waFetchImag, and we are done 
        */
        if(!GetImageOffsetByName(&blob, BLOB_ADDRESS, WA_VARIANT_DARR_FILE_NAME, (uint32_t *)&waImagePtr, &waImageSize))
        {
            /*
            ** Something went wrong, make sure to reflected on these fields
            */
            waImagePtr = NULL;
            waImageSize = 0;
            
            errMsg = "Unable to find image offset for 'wa'";
            break;
        }
        /*
        ** We have an offset, however depending on the board 
        ** this pointer maybe to an external/internal flash that may or may not be compressed
        ** make sure that when we exit, this section will be clear data
        */

        /*
        ** Calculate offset entry into blob
        */
        ofset = blob.NumberOfImages * sizeof(ManifestEntry) + sizeof(UpdateBlobHeader);
        
        waImagePtr += ofset;
#if defined(MAXWELL) || defined(SKIPPER)        
        waImagePtr +=  BLOB_ADDRESS;
#endif
        waUpdatePrepImg(waImagePtr);
        ret = 0;
    }while(0);
    if( errMsg )
    {
        LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "%s :Failed with message: %s", __FUNCTION__, errMsg);
    }
    return(ret);
}

#if defined(MAXWELL) || defined(SKIPPER)
static void waExpandCompress(uint32_t base, size_t len, waImageStat_t *wimp)
{
    char *errMsg = NULL;
    HSD_sink_res sres = HSDR_SINK_OK;
    HSD_poll_res pres = HSDR_POLL_MORE;
    uint8_t *tmpBuff = NULL;
    uint32_t target = base + len;
    uint32_t delta = 0;
    size_t sunk = wimp->last_sunk;
    size_t polled = 0;
    uint32_t expanded_count = 0;
    uint8_t page_full = 0;
    

    /*
    ** OK we are eating this elephant one byte at a time. 
    ** Since not enough memory to expand the whole thing, we will expand one 1024 byte buffer at a time
    ** then copy it to the internal flash location, lader, rinse, repeat
    */
    
    do
    {
        /*
        ** Pick a reasonable size block and allocate it
        */
        uint16_t block_size = WA_MAX_EXPAND_BUFF_SIZE;
        if( NULL == wimp->waOffloadBuffer )
        {
            if( NULL == (wimp->waOffloadBuffer = BufferManagerGetBuffer( block_size )))
            {
                errMsg = "Failure to obtain a buffer from Buffer manager\n";
                break;
            }
        }
        
        tmpBuff = wimp->waOffloadBuffer;
        
        /*
        ** Get a decoder
        */
        if( wimp->waUpdtHSd == NULL )
        {
            wimp->waUpdtHSd = heatshrink_decoder_alloc(block_size,wimp->hsw,wimp->hsl);
            if( wimp->waUpdtHSd == NULL )
            {
                LOG(wa_update,ROTTEN_LOGLEVEL_NORMAL,"%s:%s",__func__,"Could not allocate Heatshrink decoder state machine, sorry");
                break;
            }
        }
        uint32_t flashAddress = BASE_EXPANSION_FLASH;
        int retries = 5; /* We will do a combined attempt of 5 retries to do two sectors */
        do
        {
            if ( FALSE == InternalFlashAPI_EraseSectorByAddress(flashAddress))
            {
                LOG(wa_update,ROTTEN_LOGLEVEL_NORMAL,"%s:%s %x",__func__,"Unable to erase flash Block", flashAddress);
                vTaskDelay(TIMER_MSEC_TO_TICKS(10));
                continue;
            }
            if( flashAddress == BASE_EXPANSION_FLASH )
            {
                flashAddress += INTERNAL_FLASH_SECTOR_SIZE_16KB;
            }
            else
            {
                break;
            }

            
        }while(retries--);
        
        if( retries <= 0 )
        {
            errMsg = "Exceeded the number of retries attempting to erase flash, abandoning update!";
            break;
        }
        

        /*
        ** Previous pass had spill over onto buffer
        ** write that now to flash
        */
        if( wimp->carryOver )
        {
            delta = (uint32_t)InternalFlashAPI_Write((const uint32_t )base,wimp->waOffloadBuffer,(const uint32_t)wimp->carryOver);
            base += delta;
            expanded_count += delta;
            wimp->deCompressInLen =0;
        }
        
        do
        {
            /*
            ** OK now sync the input buffer to begin expansion
            */
            if( wimp->carryOver == 0 )
            {
                sres = heatshrink_decoder_sink(wimp->waUpdtHSd,&wimp->compressedImage[sunk], wimp->compressedImageSize - sunk , &wimp->deCompressInLen);
                if( sres != HSDR_SINK_OK )
                {
                    errMsg = "Heatshrink sink error";
                    break;
                }
                sunk += wimp->deCompressInLen;
            }
            else
            {
                wimp->carryOver = 0;
                
            }
            
            wimp->deCompressorOutLen = 0;
            polled = 0;
            do
            {
                pres = heatshrink_decoder_poll(wimp->waUpdtHSd,&tmpBuff[polled],block_size - polled,&wimp->deCompressorOutLen );
                polled += wimp->deCompressorOutLen;
                if(polled == block_size)
                {
                    size_t cnt = (expanded_count + polled);
                    if( cnt > PAGE_SIZE )
                    {
                        /*
                        ** OK we overflowed, copy just enough to get up to the page boundary
                        */
                        cnt = PAGE_SIZE - expanded_count;
                        delta = (uint32_t)InternalFlashAPI_Write((const uint32_t )base,tmpBuff,(const uint32_t) cnt );
                        wimp->carryOver = polled - cnt;
                        /*
                        ** write over already fash data from the back of the buffer
                        */
                        memcpy( tmpBuff, tmpBuff+cnt, wimp->carryOver);
                        page_full = 1;
                        polled = 0;
                        expanded_count += delta;
                        base += delta;
                        break;
                    }
                    else
                    {
                        delta = (uint32_t)InternalFlashAPI_Write((const uint32_t )base,tmpBuff,(const uint32_t) polled);
                    }
                    
                    if( delta == 0 )
                    {
                        errMsg = "Failed to write to internal flash";
                        break;
                    }
                    
                    base += delta;
                    expanded_count += delta;
                    polled = 0;
                }
            }while(pres == HSDR_POLL_MORE && errMsg == NULL);
            if( page_full )
            {
                break;
            }
            if( pres != HSDR_POLL_EMPTY)
            {
                errMsg = "Heatshrink poll error ";
                break;
            }
            /*
            ** Above clause dealth with buffer fulls of data, however a partially filled buffer will be
            ** handled in the clause below
            */
            if( polled )
            {
                size_t cnt = ((expanded_count + polled) > PAGE_SIZE )? (expanded_count + polled) - PAGE_SIZE: 0;
                delta = (uint32_t)InternalFlashAPI_Write((const uint32_t )base,tmpBuff,(const uint32_t) polled - cnt);
                if( delta == 0 )
                {
                    errMsg = "Failed to write to internal flash";
                    break;
                }
                if( cnt && cnt < polled )
                {
                    wimp->carryOver = cnt;
                    memcpy( wimp->waOffloadBuffer, tmpBuff+polled-cnt, wimp->carryOver);
                }
                
                base += delta;
                expanded_count += delta;
                polled = 0;
            }

        }while( base < target && (wimp->compressedImageSize - sunk));
        if( errMsg )
        {
            break;
        }
        wimp->last_sunk = sunk;
    }while(0);
    if( tmpBuff)
    {
        /*
        ** Hold this buffer to be returned later, 
        ** it has been copied to wimp->waOffloadbuffer, so use that to return it
        */
        //BufferManagerFreeBuffer(wimp->waOffloadbuffer);
    }

    if( errMsg || (expanded_count != PAGE_SIZE) )
    {
        if( errMsg )
        {
            /*
            ** We may have already erased the image by this point (since this function called multiple times
            ** asserting is meaningless. just issue error condition and may recover on subsequent try.
            */
            LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "%s:Failed with message: %s", __func__,errMsg);
        }
        else if((wimp->compressedImageSize != sunk) &&
                (expanded_count != PAGE_SIZE))
        {
            errMsg = "Expanded count different from page_size";
            LOG(wa_update, ROTTEN_LOGLEVEL_NORMAL, "%s:Failed with message: %s", __func__,errMsg);
        }
    }
}
#endif
