/*
 * psocbootloader.c
 *
 *  Created on: May 31, 2017
 *      Author: mike
 */

#include "project.h"
#include <string.h>
#include <cybtldr_parse.h>
#include <cybtldr_command.h>
#include <cybtldr_api.h>
#include "serialPSOCListener.h"
#include "psocbootloader.h"

SCRIBE_DECL(diag);

static CyBtldr_CommunicationsData comms =
{
        .OpenConnection = &psoc_open_connection,
        .CloseConnection = &psoc_close_connection,
        .ReadData = &psoc_read,
        .WriteData = &psoc_write,
        .MaxTransferSize = 64u, // PSoC bootloader can only do 64 bytes at a time DO NOT INCREASE THIS
};

void breakme(void)
{
	volatile int boo = 0;

	boo = 1;
}

int PSoCBootload(const char **imageBuffer, uint8_t numLines)
{
    unsigned char err;
    unsigned char arrayId;
    unsigned short rowNum;
    unsigned short rowSize;
    unsigned char checksum ;
    //unsigned char checksum2;
    unsigned long blVer=0;
    /* rowData buffer size should be equal to the length of data to be send for each flash row
    * Equals 128
    */
    unsigned char rowData[128];
    volatile unsigned int lineLen;
    unsigned long  siliconID;
    unsigned char siliconRev;
    unsigned char packetChkSumType;
    unsigned int lineCntr ;

    /* Initialize line counter */
    lineCntr = 0u;

    /* Get length of the first line in cyacd file*/
    lineLen = strlen(imageBuffer[lineCntr]);

    /* Parse the first line(header) of cyacd file to extract siliconID, siliconRev and packetChkSumType */
    err = CyBtldr_ParseHeader(lineLen ,(unsigned char *)imageBuffer[lineCntr] , &siliconID , &siliconRev ,&packetChkSumType);

    /* Set the packet checksum type for communicating with bootloader. The packet checksum type to be used
    * is determined from the cyacd file header information */
    CyBtldr_SetCheckSumType((CyBtldr_ChecksumType)packetChkSumType);

    if(err == CYRET_SUCCESS)
    {
        /* Start Bootloader operation */
        err = CyBtldr_StartBootloadOperation(&comms ,siliconID, siliconRev ,&blVer, NULL);
        lineCntr++ ;
        while((err == CYRET_SUCCESS)&& ( lineCntr <  numLines ))
        {
            /* Get the string length for the line*/
            lineLen =  strlen(imageBuffer[lineCntr]);
            /*Parse row data*/
            if (lineCntr % 25 == 0)
            {
              LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "bootloaded %d lines so far", lineCntr);
            }
            err = CyBtldr_ParseRowData((unsigned int)lineLen,(unsigned char *)imageBuffer[lineCntr], &arrayId, &rowNum, rowData, &rowSize, &checksum);

            if (CYRET_SUCCESS == err)
            {
                /* Program Row */
                err = CyBtldr_ProgramRow(arrayId, rowNum, rowData, rowSize);
                if (CYRET_SUCCESS == err)
                {
                    /* Verify Row . Check whether the checksum received from bootloader matches
                    * the expected row checksum stored in cyacd file*/
//                    checksum2 = (unsigned char)(checksum + arrayId + rowNum + (rowNum >> 8) + rowSize + (rowSize >> 8));
//                    err = CyBtldr_VerifyRow(arrayId, rowNum, checksum2);
//                    if (err != CYRET_SUCCESS)
//                    {
//                    	breakme();
//                    }
                }
                else
                {
                	breakme();
                }
            }
            else
            {
            	breakme();
            }
            /* Increment the linCntr */
            lineCntr ++;
        }
        // Verify the application
        err = CyBtldr_VerifyApplication();
        /* End Bootloader Operation */
        CyBtldr_EndBootloadOperation();
    }
    return(err);
}
