/*
    File    :   DEFS.H
    Title   :   Global Definitions
    Author  :   Ken Lyons
    Created :   11/18/02
    Copyright:  (C) 2002 Bose Corporation, Framingham, MA

===============================================================================
*/
#ifndef HEADER_DEFS
#define HEADER_DEFS

#if defined(USE_USB_OTG_FS) && defined(USE_USB_LOGGER)
#include "usb_defines.h"
#endif

#ifdef BIG_ENDIAN
#define HIGH_BYTE                   0
#define LOW_BYTE                    1
#define HIGH_WORD                   0
#define LOW_WORD                    1
#define LONG_HIGH_BYTE              0
#define LONG_MID_HIGH_BYTE          1
#define LONG_MID_LOW_BYTE           2
#define LONG_LOW_BYTE               3
#else
#define HIGH_BYTE                   1
#define LOW_BYTE                    0
#define HIGH_WORD                   1
#define LOW_WORD                    0
#define LONG_HIGH_BYTE              3
#define LONG_MID_HIGH_BYTE          2
#define LONG_MID_LOW_BYTE           1
#define LONG_LOW_BYTE               0
#endif

#define SYSTEM_TIMER_RATIO_TO_1MS 1

#ifdef NATIVE_32_BIT_PROCESSOR
#define TIMER_Set(V,T)              (T) = (V)
#define TIMER_IsExpired(V,T)        ((T) >= (V))
#define TIMER_ENABLED_BIT           31
#define TIMER_Increment(T)          if (!((T) & (uint32_t)(1<<TIMER_ENABLED_BIT))) ++(T)
#define TIMER_IncrementFull(T)      if ((uint32_t) (T) < 0xFFFFFFFF) ++(T)
#else
#define TIMER_Set(V,T)              (T) = (V)
#define TIMER_IsExpired(V,T)        ((T) >= (V))
#define TIMER_ENABLED_BIT           7
#define TIMER_Increment(T)          if (!((T) & (1<<TIMER_ENABLED_BIT))) ++(T)
#define TIMER_IncrementFull(T)      if ((unsigned char) (T) < 0xFF) ++(T)
#endif

#define GET_HIGH_BYTE(x)            (((x) & 0xFF00) >> 8)
#define GET_LOW_BYTE(x)             ((x) & 0xFF)

#define MAX(a,b)                    ((a) > (b) ? (a) : (b))
#ifndef MIN
#define MIN(a,b)                    ((a) < (b) ? (a) : (b))
#endif

#define ABS(a)              ((a) > 0 ? (a) : (0-(a)))

#pragma pack(1)

#ifdef IAR_COMPILER
// the IAR compiler doesn't pack into bytes if we use unsigned
#define BITFIELD_TYPE unsigned char
#define BITFIELD_SHORT_TYPE unsigned short
#else
#define BITFIELD_TYPE unsigned
#define BITFIELD_SHORT_TYPE unsigned
#endif

typedef struct  /* bit access */
{
    BITFIELD_TYPE b0: 1;
    BITFIELD_TYPE b1: 1;
    BITFIELD_TYPE b2: 1;
    BITFIELD_TYPE b3: 1;
    BITFIELD_TYPE b4: 1;
    BITFIELD_TYPE b5: 1;
    BITFIELD_TYPE b6: 1;
    BITFIELD_TYPE b7: 1;
} ByteBits;


typedef struct  /* bit access */
{
    BITFIELD_TYPE b0: 1;
    BITFIELD_TYPE b1: 1;
    BITFIELD_TYPE b2: 1;
    BITFIELD_TYPE b3: 1;
    BITFIELD_TYPE b4: 1;
    BITFIELD_TYPE b5: 1;
    BITFIELD_TYPE b6: 1;
    BITFIELD_TYPE b7: 1;
    BITFIELD_TYPE b8: 1;
    BITFIELD_TYPE b9: 1;
    BITFIELD_TYPE b10: 1;
    BITFIELD_TYPE b11: 1;
    BITFIELD_TYPE b12: 1;
    BITFIELD_TYPE b13: 1;
    BITFIELD_TYPE b14: 1;
    BITFIELD_TYPE b15: 1;
} WordBits;

typedef struct  /* bit access */
{
    BITFIELD_TYPE b0: 1;
    BITFIELD_TYPE b1: 1;
    BITFIELD_TYPE b2: 1;
    BITFIELD_TYPE b3: 1;
    BITFIELD_TYPE b4: 1;
    BITFIELD_TYPE b5: 1;
    BITFIELD_TYPE b6: 1;
    BITFIELD_TYPE b7: 1;
    BITFIELD_TYPE b8: 1;
    BITFIELD_TYPE b9: 1;
    BITFIELD_TYPE b10: 1;
    BITFIELD_TYPE b11: 1;
    BITFIELD_TYPE b12: 1;
    BITFIELD_TYPE b13: 1;
    BITFIELD_TYPE b14: 1;
    BITFIELD_TYPE b15: 1;
    BITFIELD_TYPE b16: 1;
    BITFIELD_TYPE b17: 1;
    BITFIELD_TYPE b18: 1;
    BITFIELD_TYPE b19: 1;
    BITFIELD_TYPE b20: 1;
    BITFIELD_TYPE b21: 1;
    BITFIELD_TYPE b22: 1;
    BITFIELD_TYPE b23: 1;
    BITFIELD_TYPE b24: 1;
    BITFIELD_TYPE b25: 1;
    BITFIELD_TYPE b26: 1;
    BITFIELD_TYPE b27: 1;
    BITFIELD_TYPE b28: 1;
    BITFIELD_TYPE b29: 1;
    BITFIELD_TYPE b30: 1;
    BITFIELD_TYPE b31: 1;
} LongBits;

// leaving ByteBits in here means the IAR compiler for the 755 will make
//  this struct 32bits
typedef union
{
    signed char     b;
    unsigned char   ub;
    ByteBits        bb;
} BYTE;

typedef union
{
    signed char     b[2];
    unsigned char   ub[2];
    ByteBits         bb[2];
    signed short   w;
    unsigned short  uw;
    WordBits           wb;
} DWORD;

typedef union
{
    signed char     b[4];
    unsigned char   ub[4];
    ByteBits         bb[4];
    signed short    w[2];
    unsigned short  uw[2];
    WordBits         wb[2];
    signed long     l;
    unsigned long   ul;
    LongBits        lb; // ok
} LWORD;

#pragma pack()

#define Bit7        (1<<7)
#define Bit6        (1<<6)
#define Bit5        (1<<5)
#define Bit4        (1<<4)
#define Bit3        (1<<3)
#define Bit2        (1<<2)
#define Bit1        (1<<1)
#define Bit0        (1<<0)

typedef unsigned char BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#endif //HEADER_DEFS
