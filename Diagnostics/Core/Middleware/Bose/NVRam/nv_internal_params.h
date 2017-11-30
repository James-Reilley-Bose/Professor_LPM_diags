#ifndef NV_INTERNAL_PARAMS_H
#define NV_INTERNAL_PARAMS_H

typedef struct
{
    uint32_t start_address;
    uint32_t end_address;
    uint32_t struct_size;
} nv_internal_params_info;

uint32_t NV_FindCurrentParams(nv_internal_params_info* info, void* struct_destination);
void NV_CommitParams(nv_internal_params_info* info, void* struct_source);

#endif
