/**
  ******************************************************************************
  * @file    Diag_optbyter_test.c
  *           + Diagnostic test for optbyter
 @verbatim
 ===============================================================================
 **/

#include "etap.h"
#include "stdlib.h"
#include "Diag_optbyter_test.h"

#define DEVICE_ID1 0x1fff7a10
#define DEVICE_ID2 0x1fff7a14
#define DEVICE_ID3 0x1fff7a18
#define FLASH_SIZE 0x1fff7a22

SCRIBE_DECL(diag);

void cmd_optbyter(int argc, char *argv[])
{
    //uint32_t optbyter(int argc, char *argv[]){
    uint32_t ob_bor = OB_BOR_OFF, seclock = 0, slb;
    uint8_t get_bor = 0;
    uint16_t sectormask = 0;
    FLASH_Status status;
    FunctionalState state = ENABLE;

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tUnique DEVICE_ID1,2,3 = %08x ", *(int*)DEVICE_ID1);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%08x", *(int*)DEVICE_ID2);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%08x", *(int*)DEVICE_ID3);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_SIZE = %04x ", (*(int*)FLASH_SIZE) & 0xffff);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t= %d Bytes", ((*(int*)FLASH_SIZE) & 0xffff) * 0x400);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tCPUID = %04x", SCB->CPUID);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t Implementer =  %02x", ((SCB->CPUID) >> 24) & 0xff);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t Variant     =   %01x", ((SCB->CPUID) >> 20) & 0xf);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t PartNo      = %03x", ((SCB->CPUID) >> 4) & 0xfff);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t Revision    =   %01x", (SCB->CPUID) & 0xf);


    status = FLASH_GetStatus();
    if (FLASH_COMPLETE == status)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH ready!");
    }
    else
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH status FAIL! %x", status);
    }

    // current sector lock and BOR
    seclock = FLASH_OB_GetWRP();
    get_bor = FLASH_OB_GetBOR();
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tcurrent BOR = %x seclock = %03x", get_bor, seclock);

    if (argc > 0)
    {
        if (0 == strncmp(argv[0], "help", 2))
        {
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt <arg1> <arg2>");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt def  - set factory defaults, unlock all, BOR off");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt bor off");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t        l1");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t        l2");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t        l3");

            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt secl  <0 to 11>  - lock a flach sector");
            LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\topt secu  <0 to 11>  - unlock a flach sector");

            return;
        }

        if (0 == strncmp(argv[0], "def", 2))
        {
            FLASH_OB_Unlock();
            FLASH_OB_BORConfig(OB_BOR_OFF);
            FLASH_OB_WRPConfig(0xfff, DISABLE); //unprotect all sectors
            status = FLASH_OB_Launch();
            if (FLASH_COMPLETE == status)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR Success!");
            }
            else
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR FAIL! %x", status);
            }
            FLASH_OB_Lock();
        }

        if (0 == strncmp(argv[0], "bor", 2))
        {
            if (argc > 2)
            {
                if (0 == strncmp(argv[1], "l1", 2))
                {
                    ob_bor = OB_BOR_LEVEL1;
                }
                else
                {
                    if (0 == strncmp(argv[1], "l2", 2))
                    {
                        ob_bor = OB_BOR_LEVEL2;
                    }
                    else
                    {
                        if (0 == strncmp(argv[1], "l3", 2))
                        {
                            ob_bor = OB_BOR_LEVEL3;
                        }
                    }
                }
            }

            FLASH_OB_Unlock();
            FLASH_OB_BORConfig(ob_bor);
            // keep current sector locks
            FLASH_OB_WRPConfig(seclock, DISABLE);

            status = FLASH_OB_Launch();
            if (FLASH_COMPLETE != status)
            {
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR FAILED! %x", status);
            }

            FLASH_OB_Lock();
        }



        if ((0 == strncmp(argv[0], "secl", 4)) || (0 == strncmp(argv[0], "secu", 4)))
        {
            if (0 == strncmp(argv[0], "secu", 4))
            {
                state = DISABLE;
            }
            if (argc > 2)
            {
                slb = atoi(argv[1]);

                if (state == ENABLE)
                {
                    sectormask = ~seclock | (0x1 << slb);
                }
                else
                {
                    sectormask = (0x1 << slb);
                }

                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tsector %slock %x mask %03x", (state == ENABLE) ? "" : "un", seclock, sectormask);

                FLASH_OB_Unlock();

                FLASH_OB_BORConfig(get_bor);
                FLASH_OB_WRPConfig(sectormask, state);

                status = FLASH_OB_Launch();
                LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tFLASH_OB WR ");
                if (FLASH_COMPLETE == status)
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"Success!");
                }
                else
                {
                    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"FAILED! %x", status);
                }
                FLASH_OB_Lock();
            }
        }
        get_bor = FLASH_OB_GetBOR();
        seclock = FLASH_OB_GetWRP();
    }

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%s = (0x%02x) Level_", "BOR", get_bor);
    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"%s",
               (get_bor == OB_BOR_LEVEL1) ? "1 (2.19-2.29V)" :
               (get_bor == OB_BOR_LEVEL2) ? "2 (2.50-2.59V)" :
               (get_bor == OB_BOR_LEVEL3) ? "3 (2.83-2.92V)" :
               (get_bor == OB_BOR_OFF) ? "OFF  (1.88-1.72V)" : "invalid");

    LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\t%10s = [%03x]", "sector wr prot", seclock);
    for (int i = 0; i < 12; i++)
    {
        LOG(diag, ROTTEN_LOGLEVEL_NORMAL,"\tsector %2d %slocked", i, ((seclock >> i) & 0x1) ? "un" : "");
    }
    return;
}

