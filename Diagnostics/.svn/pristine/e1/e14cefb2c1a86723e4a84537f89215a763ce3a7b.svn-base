#include "sourceInterface.h"
#include "PowerTask.h"

//This array is unique for each product variant
extern const SourceInterface sourceArray[];

// Compile time error check to make sure the constant definitions stay in sync.  That is,
// make sure SOURCE_ID and A4V_IPC_SOURCE_ID are same size.
uint8_t srcInterfaceErrorCheck[(NUM_SOURCES == (SOURCE_ID)LPM_IPC_NUM_SOURCES) ? 1 : 0];


BOOL IsValidForVariant(SOURCE_ID id)
{
    //By convention, valid source must have an activation function.
    //If this function is null then the source is not valid
    debug_assert(id < NUM_SOURCES);
    return (GetSourceInterface(id)->activate != NULL);
}

const SourceInterface*  GetSourceInterface(SOURCE_ID id)
{
    debug_assert(id < NUM_SOURCES);
    return &sourceArray[id];
}


// Generic activate function that can be used by any source that only needs
// to go to FULL power when activated
void SourceInterface_ActivateGoToFullPowerOnly()
{
    PowerAPI_SetPowerState(POWER_STATE_FULL_POWER);
}

BOOL SourceInterface_IsTVSource(SOURCE_ID sourceID)
{
    return (SOURCE_TV == sourceID);
}

/*
============================================================================================
@func UI_IsStandbySource
@brief Check for standby source.
============================================================================================
*/
BOOL SourceInterface_IsStandbySource(SOURCE_ID sourceID)
{
    return (SOURCE_STANDBY == sourceID);
}

BOOL SourceInterface_IsPTSSource(SOURCE_ID sourceID)
{
    return (SOURCE_PTS == sourceID);
}

/*
============================================================================================
@func UI_IsColdbootSource
@brief Check for standby source.
============================================================================================
*/
BOOL SourceInterface_IsColdbootSource(SOURCE_ID sourceID)
{
    return (SOURCE_COLDBOOT == sourceID);
}

/*
============================================================================================
@func UI_IsUpdateSource
@brief Check for update source.
============================================================================================
*/
BOOL SourceInterface_IsUpdateSource(SOURCE_ID sourceID)
{
    return (SOURCE_UPDATE == sourceID);
}

/*
============================================================================================
@func UI_IsUnifySource
@brief Check for update source.
============================================================================================
*/
BOOL SourceInterface_IsUnifySource(SOURCE_ID sourceID)
{
    return (SOURCE_UNIFY == sourceID);
}

/*
============================================================================================
@func UI_IsFactoryDefaultSource
@brief Check for factory default source.
============================================================================================
*/
BOOL SourceInterface_IsFactoryDefaultSource(SOURCE_ID sourceID)
{
    return (SOURCE_FACTORY_DEFAULT == sourceID);
}

/*
============================================================================================
@func UI_IsAdaptIQSource
@brief Check for AdaptIQ source.
============================================================================================
*/
BOOL SourceInterface_IsAdaptIQSource(SOURCE_ID sourceID)
{
    return (SOURCE_ADAPTIQ == sourceID);
}

/*
============================================================================================
@func UI_IsASOCSource
@brief Check if source needs sm2 to be booted
============================================================================================
*/
BOOL SourceInterface_IsSM2Source(SOURCE_ID sourceID)
{
    switch (sourceID)
    {
        case SOURCE_SHELBY:
        case SOURCE_BLUETOOTH:
            return TRUE;
        default:
            return FALSE;
    }
}

/*
============================================================================================
@func UI_IsASOCSource
@brief Check if source needs asoc ui
============================================================================================
*/
BOOL SourceInterface_IsASOCSource(SOURCE_ID sourceID)
{
    switch (sourceID)
    {
        case SOURCE_ADAPTIQ:
        case SOURCE_UNIFY:
        case SOURCE_ASOC_INTERNAL:
        case SOURCE_PTS:
            return TRUE;
        default:
            return FALSE;
    }
}

/*
============================================================================================
@func UI_IsInternalSource
@brief Check for internal source.
============================================================================================
*/
BOOL SourceInterface_IsInternalSource(SOURCE_ID sourceID)
{
    switch (sourceID)
    {
        case SOURCE_SIDE_AUX:
        case SOURCE_SHELBY:
        case SOURCE_BLUETOOTH:
        case SOURCE_ADAPTIQ:
        case SOURCE_ANALOG_FRONT:
        case SOURCE_ANALOG1:
        case SOURCE_ANALOG2:
        case SOURCE_COAX1:
        case SOURCE_COAX2:
        case SOURCE_OPTICAL1:
        case SOURCE_OPTICAL2:
        case SOURCE_UNIFY:
        case SOURCE_ASOC_INTERNAL:
        case SOURCE_PTS:
            return TRUE;
        default:
            return FALSE;
    }
}

/*
============================================================================================
@func UI_IsValidAlternateAudioSource
@brief Can we use this for alternate audio
============================================================================================
*/
BOOL SourceInterface_IsAudioSource(SOURCE_ID sourceID)
{
    switch (sourceID)
    {
        case SOURCE_SIDE_AUX:
        case SOURCE_ANALOG_FRONT:
        case SOURCE_SHELBY:
        case SOURCE_BLUETOOTH:
        case SOURCE_ANALOG1:
        case SOURCE_ANALOG2:
        case SOURCE_COAX1:
        case SOURCE_COAX2:
        case SOURCE_OPTICAL1:
        case SOURCE_OPTICAL2:
            return TRUE;
        default:
            return FALSE;
    }
}

/*
===============================================================================
@func UI_IsHDMISource
@brief Is this an HDMI rx input source or not?
===============================================================================
*/
BOOL SourceInterface_IsHDMISource(SOURCE_ID sourceID)
{
    switch (sourceID)
    {
        case SOURCE_HDMI_1:
        case SOURCE_HDMI_2:
        case SOURCE_HDMI_3:
        case SOURCE_HDMI_4:
        case SOURCE_HDMI_5:
        case SOURCE_HDMI_6:
            return TRUE;
        default:
            return FALSE;
    }
}

/*
============================================================================================
@func UI_GetHDMIPortFromSourceID
@brief Using Source ID, get the HDMI port.
============================================================================================
*/
hdmi_port SourceInterface_GetHDMIPortFromSourceID(SOURCE_ID sourceID)
{
    return GetSourceInterface(sourceID)->hdmiInput;
}
