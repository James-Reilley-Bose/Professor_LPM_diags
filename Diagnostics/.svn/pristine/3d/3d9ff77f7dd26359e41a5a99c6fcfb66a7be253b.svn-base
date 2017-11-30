/****************************************************************************
 * Copyright:  (C) 2017 Bose Corporation, Framingham, MA                    *
 ****************************************************************************/

/**
 * @file Sii9437Driver.c
 * @author lb1032816
 * @date 5/26/2017
 * @brief Wrapper for the driver provided by Lattice of the Sii9437Task.
 */

#include "project.h"
#include "Sii9437Driver.h"
#include "BufferManager.h"

SCRIBE_DECL(sii9437);

static restart_callback restart_callback_func = NULL;

#define SII_FREQ_44_1_KHZ       0x00        /* "44.1Khz" */
#define SII_FREQ_48_KHZ         0x02        /* "48Khz" */
#define SII_FREQ_32_KHZ         0x03        /* "32Khz" */
#define SII_FREQ_88_2_KHZ       0x08        /* "88.2Khz" */
#define SII_FREQ_96_KHZ         0x0A        /* "96Khz" */
#define SII_FREQ_176_4_KHZ      0x0C        /* "176.4Khz" */
#define SII_FREQ_192_KHZ        0x0E        /* "192Khz" */
#define SII_FREQ_4x192_KHZ      0x09        /* "4x192Khz" */

//Macro that handles boilerplate logic of handling return values for sii9437 function invokations.
#define handle_sii_return(ret, log_str) \
{\
    switch(ret)\
    {\
    case SII_RETURN_VALUE__SUCCESS:\
        return FALSE;\
    case SII_RETURN_VALUE__DEV_ID_ERR:\
        /*This should not happen since there is only one instance*/\
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Failure due to invalid device ID");\
        break;\
    case SII_RETURN_VALUE__INVALID_ARG_ERR:\
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Failure due to invalid input");\
        break;\
    case SII_RETURN_VALUE_ASSERTION_ERR:\
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Failure due to assertion error");\
        Sii9437Driver_AssertionQuery();\
        break;\
    default:\
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Failure due to unknown reasons..");\
    }\
    LOG(sii9437, ROTTEN_LOGLEVEL_VERBOSE, log_str);\
    return TRUE;\
}

void Sii9437Driver_CreateInstance(restart_callback reset_cb)
{
    debug_assert(reset_cb != NULL);
    restart_callback_func = reset_cb;
    sii9437_create(&Sii9437Config, &Sii9437Instance);
}

BOOL Sii9437Driver_SetPowerState(BOOL power_state)
{
    uint32_t sii_ret = sii9437_standby_set(Sii9437Instance, &power_state);
    handle_sii_return(sii_ret, "Could not set power state");
}

BOOL Sii9437Driver_GetPowerState(BOOL* power_state)
{
    uint32 sii_ret = sii9437_standby_get(Sii9437Instance, power_state);
    handle_sii_return(sii_ret, "Could not get eARC power state");
}

BOOL Sii9437Driver_GetHotPlugDetected(BOOL* hpd_state)
{
    uint32 sii_ret = sii9437_standby_get(Sii9437Instance, hpd_state);
    handle_sii_return(sii_ret, "Could not query HPD state");
}

BOOL Sii9437Driver_SetHotPlugDetected(BOOL hpd_state)
{
    uint32 sii_ret = sii9437_earc_hpd_set(Sii9437Instance, &hpd_state);
    handle_sii_return(sii_ret, "Could not set HPD state");
}

BOOL Sii9437Driver_SetArcMode(Sii9437_ArcMode_t arc_mode)
{
    uint32 sii_ret = sii9437_arc_mode_set(Sii9437Instance, &arc_mode);
    handle_sii_return(sii_ret, "Could not set arc mode");
}

BOOL Sii9437Driver_GetArcMode(Sii9437_ArcMode_t* arc_mode)
{
    uint32 sii_ret = sii9437_arc_mode_get(Sii9437Instance, arc_mode);
    handle_sii_return(sii_ret, "Could not query arc mode");
}

BOOL Sii9437Driver_SetPreferedExtractionMode(Sii9437_PreferedExtractMode_t extration_mode)
{
    uint32 sii_ret = sii9437_preferred_extraction_mode_set(Sii9437Instance, &extration_mode);
    handle_sii_return(sii_ret, "Could not set prefered extraction mode");
}

BOOL Sii9437Driver_SetEDID(uint8_t audio_edid[], uint16_t length)
{
    uint32_t sii_ret = sii9437_earc_caps_ds_set(Sii9437Instance, 0, audio_edid, (length < 128) ? length : 128);
    handle_sii_return(sii_ret, "Could not reset audio edid");
}

BOOL Sii9437Driver_UpdateAudioChannel(void)
{
    struct sii_channel_status channel_status;
    uint32_t sii_ret = sii9437_channel_status_query(Sii9437Instance, &channel_status);
    if (sii_ret)
    {
        handle_sii_return(sii_ret, "Could not query channel status");
    }

    //This was lifted from the Sii9437 example program for the eval-kit
    uint8_t samp_freq  = channel_status.data[3] & 0x0F;
    enum sii9437_mclk_mode mclk_mode =  SII9437_MCLK_MODE__FS128;

    switch (samp_freq)
    {
        case SII_FREQ_4x192_KHZ:
        case SII_FREQ_192_KHZ:
        case SII_FREQ_176_4_KHZ:
            mclk_mode = SII9437_MCLK_MODE__FS128;
            break;
        case SII_FREQ_96_KHZ:
        case SII_FREQ_88_2_KHZ:
            mclk_mode = SII9437_MCLK_MODE__FS256;
            break;
        case SII_FREQ_32_KHZ:
        case SII_FREQ_48_KHZ:
        case SII_FREQ_44_1_KHZ:
            mclk_mode = SII9437_MCLK_MODE__FS512;
            break;
        default:
            break;
    }

    LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "samp freq: 0x%x mclk mode set: 0x%x\n",
        samp_freq, mclk_mode);

    handle_sii_return(sii9437_i2s_mclk_mode_set(Sii9437Instance, &mclk_mode), "Could not set mclk mode");
}
        
BOOL Sii9437Driver_GetExtractionMode(Sii9437_ExtractMode_t *extration_mode)
{
    handle_sii_return(sii9437_extraction_mode_query(Sii9437Instance, extration_mode), "Could not query extraction mode");
}
            
void Sii9437Driver_AssertionQuery(void)
{
    //TODO: Aditional processing should be done on the assertion error
    uint32_t assert_val = 0;
    sii9437_assert_query(Sii9437Instance, &assert_val);
    LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Assertion failure: %d\n", assert_val);
    restart_callback_func();
}


void Sii9437Driver_EarcLinkStatus(void)
{
    BOOL earc_status;
    sii9437_earc_link_query(Sii9437Instance, &earc_status);
    LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "eARC Link status changed to: %s\n",
        earc_status ? "Connected" : "Disconnected");
}