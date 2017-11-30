////////////////////////////////////////////////////////////////////////////////
/// @file          bitband.h
/// @brief         Macros to deal with the bit-banding feature on the Cortex-M3 core.
///                 This feature maps each bit in an address range to a dword in another range
///                 allowing for single instruction atomic reads and writes.
/// @author        Dylan Reid
/// @date          December 2 2007
/// Copyright      2007-2010 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////
#ifndef _BITBAND_H_
#define _BITBAND_H_

#define SRAM_BITBAND_BASE 0x22000000
#define PERIPH_BITBAND_BASE 0x42000000

// see the memory map section of the reference manual for how this works
#define BITBAND_ADDR(base, addr, bit)                    \
   ( base + (((unsigned)(addr) & 0x0fffffff) * 32) + (bit * 4) )

#define PERIPH_BIT_BAND_ADDR(addr, bit) BITBAND_ADDR(PERIPH_BITBAND_BASE, addr, bit)

#define SRAM_BIT_BAND_ADDR(addr, bit) BITBAND_ADDR(SRAM_BITBAND_BASE, addr, bit)

#define SRAM_BIT_BAND_SET(addr, bit)                \
   *((volatile unsigned *)SRAM_BIT_BAND_ADDR(addr, bit)) = 1
#define SRAM_BIT_BAND_CLEAR(addr, bit)              \
   *((volatile unsigned *)SRAM_BIT_BAND_ADDR(addr, bit)) = 0
#define SRAM_BIT_BAND_TEST(addr, bit)         \
   (*((volatile unsigned *)SRAM_BIT_BAND_ADDR(addr, bit)))

#define PERIPH_BIT_BAND_SET(addr, bit)                \
   *((volatile unsigned *)PERIPH_BIT_BAND_ADDR(addr, bit)) = 1
#define PERIPH_BIT_BAND_CLEAR(addr, bit)              \
   *((volatile unsigned *)PERIPH_BIT_BAND_ADDR(addr, bit)) = 0
#define PERIPH_BIT_BAND_TEST(addr, bit)         \
   (*((volatile unsigned *)PERIPH_BIT_BAND_ADDR(addr, bit)))

#endif //_BITBAND_H_
