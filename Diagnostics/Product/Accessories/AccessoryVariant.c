/*
  File  : AccessortVariant.c
  Title :
  Author  : D. W. Richardson
  Created : 9/19/2017
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Product implementation for handling wireless / wired accessorts

===============================================================================
*/

#include "RivieraLPM_IpcProtocol.h"
#include "AccessoryManager.h"
#include "AccessoryVariant.h"
#include "WiredIDConsoleTask.h"

AccessoryConnectionStatus_t AccessoryVariant_GetSpeakerDefault(AccessoryType_t type)
{
    // left param just so we can maybe have soundbar with bass box always etc
    if(type == ACCESSORY_SKIPPER)
    {
        return ( IsWiredBassConnected() ? ACCESSORY_CONNECTION_WIRED : ACCESSORY_CONNECTION_EXPECTED );
    }
    else
    {
        return ACCESSORY_CONNECTION_EXPECTED;
    }
}

AccessoryConnectionStatus_t AccessoryVariant_GetSpeakerNeverConfig(void)
{
    return ACCESSORY_CONNECTION_NONE;
}

BOOL AccessoryVariant_OverrideWiredAccessory(void)
{
    return TRUE;
}

BOOL AccessoryVariant_SupportedAccessory(uint8_t channel)
{
    BOOL supported = FALSE;
    
    switch (channel)
    {
        case WA_CHANNEL_BASS_SKIPPER:
        case WA_CHANNEL_SURROUND_LEFT:
        case WA_CHANNEL_SURROUND_RIGHT:
            supported = TRUE;
            break;
        case WA_CHANNEL_BASS_PBB:
        default:
            break;
    }
    
    return supported;
}

BOOL AccessoryVariant_SaveSpeakerPackageOnShutdown(void)
{
    return TRUE;
}

void AccessoryVariant_HandleLlwPairingClosed(void)
{
    // TODO: Send full acc list to APQ 
}

IpcSpeakerPackage_t AccessoryVariant_CheckSpeakerPackageActive(IpcSpeakerPackage_t speakerPackage)
{
    IpcSpeakerPackage_t retVal = IPC_SPEAKER_PACKAGE_INVALID;
    
    if(NV_GetProductVariant() == PRODUCT_GINGER)
    {
        retVal = IPC_SPEAKER_PACKAGE_GINGER;
    }
    else if(NV_GetProductVariant() == PRODUCT_PROFESSOR)
    {
        retVal = IPC_SPEAKER_PACKAGE_PROFESSOR;
    }
    
    debug_assert(retVal != IPC_SPEAKER_PACKAGE_INVALID);

    uint32_t accessorts = (speakerPackage - retVal);

    switch(accessorts)
    {
        case HAS_SKIPPER:
            {
                if(AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].active != ACCESSORY_DEACTIVATED)
                {
                    return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_SKIPPER(retVal);
                }
                else
                {
                    return retVal;
                }
            }
            break;

        case HAS_MAXWELL:
            {
                if((AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_1].active != ACCESSORY_DEACTIVATED) &&
                     (AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_2].active != ACCESSORY_DEACTIVATED))
                {
                    return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_MAXWELL(retVal);
                }
                else
                {
                    return retVal;
                }
            }
            break;

        case HAS_SKIPPER_MAXWELL:
            {
                if((AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_1].active != ACCESSORY_DEACTIVATED) &&
                     (AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_2].active != ACCESSORY_DEACTIVATED))
                {
                    if(AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].active != ACCESSORY_DEACTIVATED)
                    {
                        return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_SKIPPERMAXWELL(retVal);
                    }
                    else
                    {
                        return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_MAXWELL(retVal);
                    }
                }
                else
                {
                    if(AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].active != ACCESSORY_DEACTIVATED)
                    {
                        return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_SKIPPER(retVal);
                    }
                    else
                    {
                        return retVal;
                    }
                }
            }
            break;
        case HAS_LOVEY:
            {
                if(AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].active != ACCESSORY_DEACTIVATED)
                {
                    return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_LOVEY(retVal);
                }
                else
                {
                    return retVal;
                }
            }
            break;
        case HAS_LOVEY_MAXWELL:
            {
                if((AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_1].active != ACCESSORY_DEACTIVATED) &&
                     (AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_2].active != ACCESSORY_DEACTIVATED))
                {
                    if(AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].active != ACCESSORY_DEACTIVATED)
                    {
                        return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_LOVEYMAXWELL(retVal);
                    }
                    else
                    {
                        return retVal;
                    }
                }
                else
                {
                    if(AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].active != ACCESSORY_DEACTIVATED)
                    {
                        return (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_LOVEY(retVal);
                    }
                    else
                    {
                        return retVal;
                    }
                }
            }
            break;

        default: // No accessorts so stay the same
            return retVal;
    }
}

IpcSpeakerPackage_t AccessoryVariant_GetSpeakerVariant(void)
{

    IpcSpeakerPackage_t retVal = IPC_SPEAKER_PACKAGE_INVALID;
    
    if(NV_GetProductVariant() == PRODUCT_GINGER)
    {
        retVal = IPC_SPEAKER_PACKAGE_GINGER;
    }
    else if(NV_GetProductVariant() == PRODUCT_PROFESSOR)
    {
        retVal = IPC_SPEAKER_PACKAGE_PROFESSOR;
    }
    
    debug_assert(retVal != IPC_SPEAKER_PACKAGE_INVALID);


    if(IS_ACCESSORY_CONNECTED(AccessoryManager_GetAccList()->accessory[ACC_POS_SUB]))
    {
        if ((IS_ACCESSORY_CONNECTED(AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_1]) &&
            IS_ACCESSORY_CONNECTED(AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_2])) &&
            !IS_ACCESSORY_MISCONFIG(AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_1], AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_2]))
        {
            if (AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].type == ACCESSORY_SKIPPER &&
                AccessoryManager_GetAccList()->accessory[ACC_POS_SUB_2].type != ACCESSORY_LOVEY)
            {
                retVal = (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_SKIPPERMAXWELL(retVal);
            }
            else if (AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].type == ACCESSORY_LOVEY &&
                     AccessoryManager_GetAccList()->accessory[ACC_POS_SUB_2].type != ACCESSORY_SKIPPER)
            {
                retVal = (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_LOVEYMAXWELL(retVal);
            }
            else
            {
                //unknown bass or bass off or misconfiged bass?
                retVal = (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_MAXWELL(retVal);
            }
        }
        else
        {
            if (AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].type == ACCESSORY_SKIPPER &&
                AccessoryManager_GetAccList()->accessory[ACC_POS_SUB_2].type != ACCESSORY_LOVEY)
            {
                retVal = (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_SKIPPER(retVal);
            }
            else if (AccessoryManager_GetAccList()->accessory[ACC_POS_SUB].type == ACCESSORY_LOVEY &&
                     AccessoryManager_GetAccList()->accessory[ACC_POS_SUB_2].type != ACCESSORY_SKIPPER)
            {
                retVal = (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_LOVEY(retVal);
            }
        }
    }
    else if((IS_ACCESSORY_CONNECTED(AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_1]) &&
            IS_ACCESSORY_CONNECTED(AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_2])) &&
            !IS_ACCESSORY_MISCONFIG(AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_1], AccessoryManager_GetAccList()->accessory[ACC_POS_REAR_2]))
    {
         retVal = (IpcSpeakerPackage_t)SPEAKER_PACKAGE_HAS_MAXWELL(retVal);
    }

    return retVal;
}

AccessoryDescriptionShort_t AccessoryVariant_GetDescShort(AccessoryDescription_t* acc, BOOL* valid)
{
    AccessoryDescriptionShort_t shortie;

    if(acc->status <= ACCESSORY_CONNECTION_EXPECTED && acc->status > ACCESSORY_CONNECTION_NONE)
    {
        shortie.type = (AccessoryType_t)acc->type;
        shortie.position = (AccessoryPosition_t)acc->position;
        shortie.active = (AccessoryActiveState_t)acc->active;

        switch( acc->status)
        {
            case ACCESSORY_CONNECTION_WIRED:
            case ACCESSORY_CONNECTION_BOTH:
                shortie.status = ACCESSORY_CONNECTION_WIRED;
                break;
            case ACCESSORY_CONNECTION_WIRELESS:
            case ACCESSORY_CONNECTION_EXPECTED:
                shortie.status = ACCESSORY_CONNECTION_WIRELESS;
                break;
            default:
                shortie.status = ACCESSORY_CONNECTION_NONE;
        }
        *valid = TRUE;
    }
    else
    {
        *valid = FALSE;
    }
    return shortie;
}

static const SpeakerPackageVariant_t gingerSpkrVar = 
{
    .numOfAccessories = 1,
    .accessory = {
      {
        .type = ACCESSORY_GINGER_BAR,
        .position = ACCESSORY_POSITION_CENTER_SOUNDBAR,
        .status = ACCESSORY_CONNECTION_WIRED,
        .active = ACCESSORY_ACTIVATED,
      },
    }
};

static const SpeakerPackageVariant_t professorSpkrVar = 
{
    .numOfAccessories = 1,
    .accessory = {
      {
        .type = ACCESSORY_PROFESSOR_BAR,
        .position = ACCESSORY_POSITION_CENTER_SOUNDBAR,
        .status = ACCESSORY_CONNECTION_WIRED,
        .active = ACCESSORY_ACTIVATED,
      }
    }
};

SpeakerPackageVariant_t const* AccessoryVariant_GetSpeakerPackageVaraint(void)
{
    switch(NV_GetProductVariant())
    {
      case PRODUCT_PROFESSOR:
        return &gingerSpkrVar;
      case PRODUCT_GINGER:
        return &professorSpkrVar;
      default:
        return NULL;
    }
}