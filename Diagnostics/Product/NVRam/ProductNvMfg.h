//
// ProductNvMfg.h
//

#ifndef PRODUCT_NV_MFG_H
#define PRODUCT_NV_MFG_H

#define NV_MFG_PRODUCT_STRUCT_VERSION 2

typedef struct
{
    uint8_t radioMask24GHz;
    uint8_t radioMask52GHz;
    uint8_t radioMask58GHz;
    
} ProductParams_t;

#endif
