#include "project.h"
#include "nv.h"
#include "nv_internal_params.h"
#include "InternalFlashAPI.h"

static uint32_t NV_FindLastValidSlot(nv_internal_params_info* info);
static BOOL NV_IsSlotEmpty(nv_internal_params_info* info, uint32_t slot);

uint32_t NV_FindCurrentParams(nv_internal_params_info* info, void* struct_destination)
{
    uint32_t startLocation = NV_FindLastValidSlot(info);
    if ((startLocation) && (struct_destination))
    {
        memcpy(struct_destination, (void*) startLocation, info->struct_size);
    }
    return startLocation;
}

static uint32_t NV_FindLastValidSlot(nv_internal_params_info* info)
{
    // start at the end of flash and work backwards.  look for a valid block (version not 0xff, and mfgctrlword set)
    uint32_t startLocation = info->end_address;
    do
    {
        startLocation -= info->struct_size;
        uint8_t* version = (uint8_t*) startLocation;
        uint16_t* ctrl_word = (uint16_t*) (startLocation + info->struct_size - sizeof(uint16_t));
        if ((version[0] != 0xFF) && (*ctrl_word == NV_VALID_CTRL_WORD))
        {
            return startLocation;
        }
    }
    while (startLocation > info->start_address);
    return NULL;
}

void NV_CommitParams(nv_internal_params_info* info, void* struct_source)
{
    uint32_t startLocation = NV_FindLastValidSlot(info);
    if (!startLocation)
    {
        InternalFlashAPI_EraseSectorByAddress(info->start_address);
        startLocation = info->start_address;
    }
    // it's the same as previously, we dont need to save it
    if (!memcmp((void*) startLocation, struct_source, info->struct_size))
    {
        return;
    }
    // find the next slot that is all 0xFFs
    for (; startLocation < (uint32_t) info->end_address; startLocation += info->struct_size)
    {
        if (NV_IsSlotEmpty(info, startLocation))
        {
            break;
        }
    }
    // we've used up all our slots, erase and start at the beginning
    if (startLocation >= (uint32_t) info->end_address)
    {
        InternalFlashAPI_EraseSectorByAddress((uint32_t) info->start_address);
        startLocation = (uint32_t) info->start_address;
    }
    // copy to flash
    InternalFlashAPI_Write((uint32_t) startLocation, struct_source, info->struct_size);
}

static BOOL NV_IsSlotEmpty(nv_internal_params_info* info, uint32_t testslot)
{
    uint8_t* slot = (uint8_t*) testslot;
    for (int i = 0; i < info->struct_size; i++)
    {
        if (slot[i] != 0xFF)
        {
            return FALSE;
        }
    }
    return TRUE;
}