#include "project.h"
#include "etap.h"
#include "nv_mfg.h"
#include "nv_system.h"
#include "etapNV.h"
#include "DeviceModelTask.h"
#include "buffermanager.h"
#include "IpcDsp.h"
#ifdef LPM_HAS_USER_INTERFACE
#include "UITask.h"
#endif
//#include "fdSourceInterface.h"
#include "ProductSystemParams.h"
#include <string.h>

#ifdef LPM_HAS_GLOBAL_PARAMETERS
#include "globalParameterList.h"
#endif

#ifdef HAS_SYSPARAMS_NVRAM

/**
* @DOCETAP
* COMMAND:   commitnv
*
* ALIAS:     cnv
*
* DESCRIP:   Store current sys parameters to NV
*
* PARAM:     None
*
* REPLY:     System Params force save
*
* EXAMPLE:   cnv -- Store NV
*            
* @DOCETAPEND
*
*/

TAPCommand(TAP_CommitSysNV)
{
   NV_CommitSystemParams();

   TAP_PrintString("\n\rSystem Params force save");

}
#endif

TAPCommand(TAP_SysVariant)
{
    if (CommandLine->numArgs == 0)
    {
        REGION_VARIANT regionVariant  = NV_GetRegionVariant();        
        PRODUCT_VARIANT productVariant = NV_GetProductVariant();
        COUNTRY_VARIANT countryVariant = NV_GetCountryVariant();
        LANGUAGE_SELECTION defaultLanguage = NV_GetDefaultLanguage();
#if 0 //TODO - add NV_GetSystemColor
        SYSTEM_COLOR colorVariant = NV_GetSystemColor();
#endif
        SYSTEM_COLOR colorVariant = (SYSTEM_COLOR)0;
        
        TAP_Printf("Product: %d\r\n",(uint8_t)productVariant);
        TAP_Printf("Country: %d\r\n",(uint8_t)countryVariant);
        TAP_Printf("Region: %d\r\n",(uint8_t)regionVariant);
        TAP_Printf("Language: %d\r\n",(uint8_t)defaultLanguage);
        TAP_Printf("Color: %d\r\n",(uint8_t)colorVariant);
    }
    else if (CommandLine->numArgs == 6)
    {
        BOOL valid[] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, };

        PRODUCT_VARIANT productVariant = (PRODUCT_VARIANT) TAP_DecimalArgToInt(CommandLine, 0, &valid[0]);
        if (!TapValidRange(PRODUCT_UNDEFINED, FALSE, PRODUCT_NUM, FALSE, productVariant))
        {
            TAP_Printf("Invalid Product %d\n\r", productVariant);
            valid[0] = FALSE;
        }

        COUNTRY_VARIANT countryVariant = (COUNTRY_VARIANT) TAP_DecimalArgToInt(CommandLine, 2, &valid[2]);
        if (!TapValidRange(COUNTRY_UNDEFINED, FALSE, COUNTRY_NUM, FALSE, countryVariant))
        {
            TAP_Printf("Invalid Country: %d\n\r", countryVariant);
            valid[2] = FALSE;
        }

        REGION_VARIANT regionVariant = (REGION_VARIANT) TAP_DecimalArgToInt(CommandLine, 3, &valid[3]);
        if (!TapValidRange(REGION_UNDEFINED, FALSE, REGION_NUM, FALSE, regionVariant))
        {
            TAP_Printf("Invalid Region: %d\n\r", regionVariant);
            valid[3] = FALSE;
        }

        LANGUAGE_SELECTION defaultLanguage = (LANGUAGE_SELECTION) TAP_DecimalArgToInt(CommandLine, 4, &valid[4]);
        if (!TapValidRange(LANGUAGE_ENGLISH, TRUE, LANGUAGE_NUM, FALSE, defaultLanguage))
        {
            TAP_Printf("Invalid Default Language: %d\n\r", defaultLanguage);
            valid[4] = FALSE;
        }

        SYSTEM_COLOR colorVariant = (SYSTEM_COLOR) TAP_DecimalArgToInt(CommandLine, 5, &valid[5]);
        if ((colorVariant != SYSTEM_COLOR_WHITE) && (colorVariant != SYSTEM_COLOR_BLACK))
        {
            TAP_Printf("Invalid Color: %d\n\r", colorVariant);
            valid[5] = FALSE;
        }
        for (int i = 0; i < sizeof(valid); i++) {
            if (!valid[i])
            {
                TAP_PrintString(SYS_VARIANT_HELP_TEXT);
                return;
            }
        }

        // save values
        NV_SetProductVariant(productVariant);

        NV_SetCountryVariant(countryVariant);
        NV_SetRegionVariant(regionVariant);
        NV_SetDefaultLanguage(defaultLanguage);
#if 0 //TODO - add NV_SetSystemColor
        NV_SetSystemColor(colorVariant);
#endif
#ifdef LPM_HAS_GLOBAL_PARAMETERS
        GP_initializeGlobalParameters(productVariant, regionVariant);
#endif

        // commit mfg
        NV_CommitMfgParams();
   }
   else
   {
        TAP_PrintString(SYS_VARIANT_HELP_TEXT);
   }
}

#if 0
TAPCommand(TAP_FactoryDefault)
{

   if (CommandLine->numArgs == 0)
   {
#if 0 //TODO - add fdSourceInterface and UITasks
     FdSource_SetRebootState(TRUE);
     UI_SetUnifySource(UI_GetUnifySourceIndex(SOURCE_FACTORY_DEFAULT));
#endif
   }
   else if (CommandLine->numArgs == 1 && !strncmp(CommandLine->args[0], "lp", 2))
   {
#if 0 //TODO - add fdSourceInterface and UITasks
     FdSource_SetRebootState(FALSE);
     UI_SetUnifySource(UI_GetUnifySourceIndex(SOURCE_FACTORY_DEFAULT));
#endif
   }
   else
   {
     TAP_PrintString(SYS_FACTORY_DEFAULT_HELP_TEXT);
   }

}


const struct TAP_Command NV_Commands[] =
{
    {
        {"oob", "outofbox"},
        TAP_NV_OOB,
        NV_OOB_HELP_TEXT,
    },
#if defined(GINGER) //TODO - If autowake is supported || defined(PROFESSOR)
    {
        {"aw", "autowake"},
        TAP_NV_Autowake,
        NV_AW_HELP_TEXT,
    },
#endif
    {
        {"ssu", "skipspeakerupdate"},
        TAP_NV_SSU,
        NV_SSU_HELP_TEXT,
    },
#ifdef SUPPORT_DSPWATCHDOG
    {
        {"dwd", "dspwatchdog"},
        TAP_NV_DspWD,
        NV_DWD_HELP_TEXT,
    },
#endif
    {
        {"stg", "settings"},
        TAP_NV_SETTINGS,
        NV_STG_HELP_TEXT,
    },
};

const int Num_NV_Commands = sizeof(NV_Commands) / sizeof(NV_Commands[0]);

TAPCommand(TAP_SysNV)
{
    if(TAP_ProcessSubCommand(NV_Commands, Num_NV_Commands, CommandLine) == FALSE)
    {
        TAP_PrintString(NV_HELP_TEXT);
        TAP_PrintSubCommands(NV_Commands, Num_NV_Commands);
    }
}

TAPCommand(TAP_NV_OOB)
{
    if(CommandLine->numArgs > 1)
    {
        TAP_PrintString(NV_OOB_HELP_TEXT);
    }
    else if(CommandLine->numArgs == 0)
    {
        TAP_Printf("Out of Box: %d", NV_GetFirstTimeInit());
    }
    else
    {
        BOOL valid;
        uint8_t val = TAP_DecimalArgToInt(CommandLine, 0, &valid);
        if(valid)
        {
            NV_SetFirstTimeInit(val == 0 ? 0 : 1);
        }
        else
        {
            TAP_PrintString(NV_OOB_HELP_TEXT);
        }
    }
}

#if defined(GINGER) || defined(PROFESSOR)
TAPCommand(TAP_NV_Autowake)
{
    if(CommandLine->numArgs > 1)
    {
        TAP_PrintString(NV_AW_HELP_TEXT);
    }
    else if(CommandLine->numArgs == 0)
    {
        TAP_Printf("AutoWake Enable: %d", NV_GetAutoWakEnabled());
    }
    else
    {
        BOOL valid;
        uint8_t val = TAP_DecimalArgToInt(CommandLine, 0, &valid);
        if(valid)
        {
            NV_SetAutoWakeEnabled(val == 0 ? 0 : 1);
            NV_CommitSystemParams();
        }
        else
        {
            TAP_PrintString(NV_AW_HELP_TEXT);
        }
    }
}
#endif

#ifdef SUPPORT_ACCESSORY_UPDATE
TAPCommand(TAP_NV_SSU)
{
    if(CommandLine->numArgs == 0)
    {
        if(NV_GetSkipSpeakerUpdate())
        {
            TAP_PrintString("Speaker update will be skipped.");
        }
        else
        {
            TAP_PrintString("Speaker update will not be skipped.");
        }
    }
    else if(CommandLine->numArgs == 1)
    {
        NV_SetSkipSpeakerUpdate(CommandLine->args[0][0] == '1');
    }
    else
    {
        TAP_PrintString(NV_SSU_HELP_TEXT);
    }
}
#endif

#ifdef SUPPORT_DSPWATCHDOG
TAPCommand(TAP_NV_DspWD)
{
    if(CommandLine->numArgs > 1)
    {
        TAP_PrintString(NV_DWD_HELP_TEXT);
    }
    else if(CommandLine->numArgs == 0)
    {
        TAP_Printf("DspWD Enable: %d", NV_GetDspWDEnabled());
    }
    else
    {
        BOOL valid;
        uint8_t val = TAP_DecimalArgToInt(CommandLine, 0, &valid);
        if(valid)
        {
            NV_SetDspWDEnabled(val == 0 ? 0 : 1);
            NV_CommitSystemParams();
        }
        else
        {
            TAP_PrintString(NV_DWD_HELP_TEXT);
        }
    }
}
#endif

TAPCommand(TAP_NV_SETTINGS)
{
    if (CommandLine->numArgs > 0)
    {
        TAP_PrintString(NV_STG_HELP_TEXT);
    }
    else
    {
        TAP_Printf("%-36s 0x%x\n", "NV system struct version:", NV_GetSystemStructVer());
    }
}
#endif

/**
* @DOCETAP
* COMMAND:   manuf 
*
* ALIAS:     mfg
*
* DESCRIP:   Get the stored manufacturing data or set it.  Auto-commits.
*
* PARAM:     param:  string - the parameter to read/set, or "commit"
*            value:  depends on the parameter, empty to read
*
* REPLY:     The parameter value (if reading)
*
* EXAMPLE:   mfg mask24 -- Retrieve the 2.4GHz Radio Mask
*
*            mfg csum, 0 -- Set the RFID Checksum to 0
*
* @DOCETAPEND
*/

TAPCommand(TAP_ManufacturingData)
{
    if (CommandLine->numArgs < 1)
    {
        TAP_PrintString(MFG_DATA_HELP_TEXT);
        NV_DumpMfgFieldNames();
        return;
    }

    // Commit
    if (!strncmp(CommandLine->args[0], "com", 3))
    {
        NV_CommitMfgParams();
        TAP_PrintString("Committed\r\n");
        return;
    }

    uint8_t* value = BufferManagerGetBuffer(NV_MfgStructSize());
    debug_assert(value);

    // Read
    if (CommandLine->numArgs < 2)
    {
        if (!strcmp(CommandLine->args[0], "all"))
        {
            NV_DumpMfgFields();
            BufferManagerFreeBuffer(value);
            return;
        }

        uint8_t size;
        if (!NV_GetMfgField(CommandLine->args[0], value, &size))
        {
            TAP_PrintString(MFG_DATA_HELP_TEXT);
            NV_DumpMfgFieldNames();
            BufferManagerFreeBuffer(value);
            return;
        }

        /* Special case: display ss, ss2, cs as strings. */
        if(!strcmp(CommandLine->args[0], "ss") || !strcmp(CommandLine->args[0], "ss2") || !strcmp(CommandLine->args[0], "cs"))
        {
            TAP_Printf("%04s (%02d) = %s\r\n", CommandLine->args[0], size, value);
        }
        // Everything else gets displayed as hex
        else
        {
            for (uint8_t i = 0; i < size; i++ )
            {
                TAP_Printf("%02x ", value[i]);
            }
            TAP_PrintString("\r\n");
        }
        BufferManagerFreeBuffer(value);
        return;
    }

    // Write
    // Special case for "ss", "ss2", "cs". These are ascii and not hex.
    if(!strcmp(CommandLine->args[0], "ss") || !strcmp(CommandLine->args[0], "ss2") || !strcmp(CommandLine->args[0], "cs"))
    {
        TAP_StringToUpper((char *)CommandLine->args[1]);
        NV_SetMfgField(CommandLine->args[0], (const uint8_t *)CommandLine->args[1]);
        return;
    }

    uint8_t size = NV_GetMfgFieldSize(CommandLine->args[0]);
    if (size == 0)
    {
        TAP_PrintString(MFG_DATA_HELP_TEXT);
        NV_DumpMfgFieldNames();
        BufferManagerFreeBuffer(value);
        return;
    }

    if (!TAP_HexStringToUInt8Array(CommandLine->args[1], value, size))
    {
        TAP_PrintString(MFG_DATA_HELP_TEXT);
        NV_DumpMfgFieldNames();
        BufferManagerFreeBuffer(value);
        return;
    }

    if (!NV_SetMfgField(CommandLine->args[0], value))
    {
        TAP_PrintString(MFG_DATA_HELP_TEXT);
        NV_DumpMfgFieldNames();
        BufferManagerFreeBuffer(value);
        return;
    }

    BufferManagerFreeBuffer(value);
    TAP_PrintString("OK\r\n");
}
