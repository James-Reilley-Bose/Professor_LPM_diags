//
// WirelessAudioMUUtilities.c - general MU utilities
//

#include "WirelessAudioUtilities.h"
#include "WirelessAudioI2C.h"
#include "WirelessAudioISR.h"
#include "WirelessAudioNV.h"
#include "AmpTask.h"
#include "AmpMute.h"

#define DARR_REBOOT_DELAY 250

static const uint8_t defaultMUCoco[WA_COCO_LEN] = {0xFE, 0xFE, 0xFE};
static BOOL skipPair = FALSE;
static BOOL postDarrUpdate = FALSE;

uint32_t WirelessAudioVariantUtils_GetBroadcastAddress(void)
{
    return WA_BROADCAST_TO_CU_ADDR;
}

void WirelessAudioVariantUtils_GetDefaultCoco(uint8_t* coco)
{
    if(coco) memcpy(coco, defaultMUCoco, sizeof(defaultMUCoco));
}

BOOL WirelessAudioVariantUtils_IsDefaultCoco(uint8_t* coco)
{
    if(coco)
    {
        return (memcmp(coco, defaultMUCoco, sizeof(defaultMUCoco)) == 0);
    }
    else
    {
        return FALSE;
    }
}

uint8_t WirelessAudioVariantUtils_GetActiveBandsMask(void)
{
    return WA_BAND_NONE_BIT_MASK;
}

void WirelessAudioVariantUtils_SetActiveBandsMask(uint8_t bandMask)
{
}

void WirelessAudioVariantUtils_Reboot(void)
{
    Amp_DisableStatusMonitor();
    AmpMute_SetMute(AMP_MUTE_DARR, TRUE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(DARR_REBOOT_DELAY));
    Amp_Enable(FALSE);
    system_reboot();
}


void WirelessAudioVariantUtils_SetSkipPair(BOOL state)
{
    skipPair = state;
}

BOOL WirelessAudioVariantUtils_GetSkipPair(void)
{
    return skipPair;
}

void WirelessAudioVariantUtils_SetPostDarrUpdate(BOOL state)
{
    postDarrUpdate = state;
}

BOOL WirelessAudioVariantUtils_GetPostDarrUpdate(void)
{
    return postDarrUpdate;
}

