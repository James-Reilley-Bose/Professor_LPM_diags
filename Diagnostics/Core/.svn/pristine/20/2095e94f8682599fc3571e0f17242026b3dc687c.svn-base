/*************************************************************************//**
 * @file            mmsw_byte_ordering.h
 * @brief           Byte ordering macro/function definitions for hardware
 *                  abstraction
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: mmsw_byte_ordering.h 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      Macros and functions for dealing with network & machine byte ordering
 *      dependencies in order to facilitate hardware abstraction.  Users may
 *      override any and all definitions if desired by overriding the
 *      applicable macro(s).
 *
 *      You might do this if you are, for example, on a byte-oriented machine
 *      and therefore have no issues with word alignment and so can use
 *      direct casts and pointer manipulation to extract and store uint16 and
 *      uint32 values in a byte stream.
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/
#ifndef __MMSW_BYTE_ORDERING_H__
#define __MMSW_BYTE_ORDERING_H__

/*** Include Files ***********************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/

#if !defined(BIG_ENDIAN_NETWORK_ORDER) && !defined(LITTLE_ENDIAN_NETWORK_ORDER)
#   error You *MUST* define a byte ordering in your platform definitions file!
#endif

#ifndef EXTRACT_BYTE
/**
 * @brief Extracts the n-th byte from the specified value.  Byte numbers are
 *        zero-based
 *
 * @param n The byte offset
 * @param x The value from which to extract the byte
 * @return n-th byte of x
 */
#   define EXTRACT_BYTE(n,x)        (((x) >> (8*(n))) & 0xFFu)
#endif

#ifndef TO_UINT16
/**
 * @brief Combines two uint8 values into a single uint16
 *
 * @param b1 - the MSB
 * @param b0 - the LSB
 * @return uint16 value with b1 as MSB, b0 as LSB
 */
#   define TO_UINT16(b1,b0)         (((b1) <<  8)|((b0) <<  0))
#endif

#ifndef TO_UINT24
/**
 * @brief Combines three uint8 values into a single uint24
 *
 * @param b2 - the MSB
 * @param b1 - the 2nd-MSB
 * @param b0 - the LSB
 * @return uint24 value with b2 as MSB, b1 as 2nd-MSB, b0 as LSB
 */
#   define TO_UINT24(b2,b1,b0)      (((b2) << 16)|TO_UINT16((b1),(b0)))
#endif

#ifndef TO_UINT32
/**
 * @brief Combines four uint8 values into a single uint32
 *
 * @param b3 - the MSB
 * @param b2 - the 2nd-MSB
 * @param b1 - the 3rd-MSB
 * @param b0 - the LSB
 * @return uint32 value with b3 as MSB, b2 as 2nd-MSB, b1 as 3rd-MSB,
 *         b0 as LSB
 */
#   define TO_UINT32(b3,b2,b1,b0)   (((b3) << 24)|TO_UINT24((b2),(b1),(b0)))
#endif

#ifndef TO_UINT40
/**
 * @brief Combines five uint8 values into a single uint40
 *
 * @param b4 - the MSB
 * @param b3 - the 2nd-MSB
 * @param b2 - the 3rd-MSB
 * @param b1 - the 4th-MSB
 * @param b0 - the LSB
 * @return uint40 value with b4 as MSB, b3 as 2nd-MSB, b2 as 3rd-MSB,
 *         b1 as 4th-MSB, b0 as LSB
 */
#   define TO_UINT40(b4,b3,b2,b1,b0)    (((b4) << 32)|TO_UINT32((b3),(b2),(b1),(b0)))
#endif

#ifndef TO_UINT48
/**
 * @brief Combines six uint8 values into a single uint48
 *
 * @param b5 - the MSB
 * @param b4 - the 2nd-MSB
 * @param b3 - the 3rd-MSB
 * @param b2 - the 4th-MSB
 * @param b1 - the 5th-MSB
 * @param b0 - the LSB
 * @return uint48 value with b5 as MSB, b4 as 2nd-MSB, b3 as 3rd-MSB,
 *         b2 as 4th-MSB, b1 as 5th-MSB, b0 as LSB
 */
#   define TO_UINT48(b5,b4,b3,b2,b1,b0) (((b5) << 40)|TO_UINT40((b4),(b3),(b2),(b1),(b0)))
#endif

#ifndef TO_UINT56
/**
 * @brief Combines six uint8 values into a single uint48
 *
 * @param b6 - the MSB
 * @param b5 - the 2nd-MSB
 * @param b4 - the 3rd-MSB
 * @param b3 - the 4th-MSB
 * @param b2 - the 5th-MSB
 * @param b1 - the 6th-MSB
 * @param b0 - the LSB
 * @return uint56 value with b6 as MSB, b5 as 2nd-MSB, b4 as 3rd-MSB,
 *         b3 as 4th-MSB, b2 as 5th-MSB, b1 as 6th-MSB, b0 as LSB
 */
#   define TO_UINT56(b6,b5,b4,b3,b2,b1,b0) (((b6) << 48)|TO_UINT48((b5),(b4),(b3),(b2),(b1),(b0)))
#endif

#ifndef TO_UINT64
/**
 * @brief Combines six uint8 values into a single uint48
 *
 * @param b7 - the MSB
 * @param b6 - the 2nd-MSB
 * @param b5 - the 3rd-MSB
 * @param b4 - the 4th-MSB
 * @param b3 - the 5th-MSB
 * @param b2 - the 6th-MSB
 * @param b1 - the 7th-MSB
 * @param b0 - the LSB
 * @return uint64 value with b7 as MSB, b6 as 2nd-MSB, b5 as 3rd-MSB,
 *         b4 as 4th-MSB, b3 as 5th-MSB, b2 as 6th-MSB, b1 a 7th-MSB,
 *         b0 as LSB
 */
#   define TO_UINT64(b7,b6,b5,b4,b3,b2,b1,b0) (((b7) << 56)|TO_UINT56((b6),(b5),(b4),(b3),(b2),(b1),(b0)))
#endif

#ifndef EXTRACT_UINT16
/**
 * @brief Extracts a uint16 quantity from a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @return The uint16 quantity
 */
#   define EXTRACT_UINT16(ptr)      TO_UINT16(*((uint8 *)(ptr)+UINT16_B1_OFFSET),   \
                                              *((uint8 *)(ptr)+UINT16_B0_OFFSET))
#endif

#ifndef STORE_UINT16
/**
 * @brief Stores a uint16 quantity into a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @param x The value to be stored
 */
#   define STORE_UINT16(ptr,x)      do { *((uint8 *)(ptr)+UINT16_B1_OFFSET) = EXTRACT_BYTE(1,x); \
                                         *((uint8 *)(ptr)+UINT16_B0_OFFSET) = EXTRACT_BYTE(0,x); } while(0)
#endif

#ifndef EXTRACT_UINT24
/**
 * @brief Extracts a uint24 quantity from a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @return The uint24 quantity
 */
#   define EXTRACT_UINT24(ptr)      TO_UINT24( *((uint8 *)(ptr)+UINT24_B2_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT24_B1_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT24_B0_OFFSET))
#endif

#ifndef STORE_UINT24
/**
 * @brief Stores a uint24 quantity into a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @param x The value to be stored
 */
#   define STORE_UINT24(ptr,x)      do { *((uint8 *)(ptr)+UINT24_B2_OFFSET) = EXTRACT_BYTE(2,x); \
                                         STORE_UINT16(ptr+1,(x)); } while(0)
#endif

#ifndef EXTRACT_UINT32
/**
 * @brief Extracts a uint32 quantity from a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @return The uint32 quantity
 */
#   define EXTRACT_UINT32(ptr)      TO_UINT32( *((uint8 *)(ptr)+UINT32_B3_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT32_B2_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT32_B1_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT32_B0_OFFSET))
#endif

#ifndef STORE_UINT32
/**
 * @brief Stores a uint32 quantity into a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @param x The value to be stored
 */
#   define STORE_UINT32(ptr,x)      do { *((uint8 *)(ptr)+UINT32_B3_OFFSET) = EXTRACT_BYTE(3,x); \
                                         STORE_UINT24(ptr+1,(x)); } while(0)
#endif

#ifndef EXTRACT_UINT40
/**
 * @brief Extracts a uint40 quantity from a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @return The uint40 quantity
 */
#   define EXTRACT_UINT40(ptr)      TO_UINT40( *((uint8 *)(ptr)+UINT40_B4_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT40_B3_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT40_B2_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT40_B1_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT40_B0_OFFSET))
#endif

#ifndef STORE_UINT40
/**
 * @brief Stores a uint40 quantity into a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @param x The value to be stored
 */
#   define STORE_UINT40(ptr,x)      do { *((uint8 *)(ptr)+UINT40_B4_OFFSET) = EXTRACT_BYTE(4,x); \
                                         STORE_UINT32(ptr+1,(x)); } while(0)
#endif

#ifndef EXTRACT_UINT48
/**
 * @brief Extracts a uint48 quantity from a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @return The uint48 quantity
 */
#   define EXTRACT_UINT48(ptr)      TO_UINT48( *((uint8 *)(ptr)+UINT48_B5_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT48_B4_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT48_B3_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT48_B2_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT48_B1_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT48_B0_OFFSET))
#endif

#ifndef STORE_UINT48
/**
 * @brief Stores a uint48 quantity into a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @param x The value to be stored
 */
#   define STORE_UINT48(ptr,x)      do { *((uint8 *)(ptr)+UINT48_B5_OFFSET) = EXTRACT_BYTE(5,x); \
                                         STORE_UINT40(ptr+1,(x)); } while(0)
#endif

#ifndef EXTRACT_UINT56
/**
 * @brief Extracts a uint56 quantity from a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @return The uint56 quantity
 */
#   define EXTRACT_UINT56(ptr)      TO_UINT56( *((uint8 *)(ptr)+UINT56_B6_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT56_B5_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT56_B4_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT56_B3_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT56_B2_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT56_B1_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT56_B0_OFFSET))
#endif

#ifndef STORE_UINT56
/**
 * @brief Stores a uint56 quantity into a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @param x The value to be stored
 */
#   define STORE_UINT56(ptr,x)      do { *((uint8 *)(ptr)+UINT56_B6_OFFSET) = EXTRACT_BYTE(6,x); \
                                         STORE_UINT48(ptr+1,(x)); } while(0)
#endif

#ifndef EXTRACT_UINT64
/**
 * @brief Extracts a uint64 quantity from a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @return The uint64 quantity
 */
#   define EXTRACT_UINT64(ptr)      TO_UINT64( *((uint8 *)(ptr)+UINT64_B7_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT64_B6_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT64_B5_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT64_B4_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT64_B3_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT64_B2_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT64_B1_OFFSET),  \
                                               *((uint8 *)(ptr)+UINT64_B0_OFFSET))
#endif

#ifndef STORE_UINT64
/**
 * @brief Stores a uint64 quantity into a uint8 byte stream
 *
 * @param ptr Pointer into the byte stream
 * @param x The value to be stored
 */
#   define STORE_UINT64(ptr,x)      do { *((uint8 *)(ptr)+UINT64_B7_OFFSET) = EXTRACT_BYTE(7,x); \
                                         STORE_UINT56(ptr+1,(x)); } while(0)
#endif

/*** Module Types ************************************************************/

enum byteOffsets
{
#if defined(BIG_ENDIAN_NETWORK_ORDER)
    UINT64_B7_OFFSET = 0,
    UINT64_B6_OFFSET,
    UINT64_B5_OFFSET,
    UINT64_B4_OFFSET,
    UINT64_B3_OFFSET,
    UINT64_B2_OFFSET,
    UINT64_B1_OFFSET,
    UINT64_B0_OFFSET,
    UINT56_B6_OFFSET = 0,
    UINT56_B5_OFFSET,
    UINT56_B4_OFFSET,
    UINT56_B3_OFFSET,
    UINT56_B2_OFFSET,
    UINT56_B1_OFFSET,
    UINT56_B0_OFFSET,
    UINT48_B5_OFFSET = 0,
    UINT48_B4_OFFSET,
    UINT48_B3_OFFSET,
    UINT48_B2_OFFSET,
    UINT48_B1_OFFSET,
    UINT48_B0_OFFSET,
    UINT40_B4_OFFSET = 0,
    UINT40_B3_OFFSET,
    UINT40_B2_OFFSET,
    UINT40_B1_OFFSET,
    UINT40_B0_OFFSET,
    UINT32_B3_OFFSET = 0,
    UINT32_B2_OFFSET,
    UINT32_B1_OFFSET,
    UINT32_B0_OFFSET,
    UINT24_B2_OFFSET = 0,
    UINT24_B1_OFFSET,
    UINT24_B0_OFFSET,
    UINT16_B1_OFFSET = 0,
    UINT16_B0_OFFSET
#else
    UINT64_B0_OFFSET = 0,
    UINT64_B1_OFFSET,
    UINT64_B2_OFFSET,
    UINT64_B3_OFFSET,
    UINT64_B4_OFFSET,
    UINT64_B5_OFFSET,
    UINT64_B6_OFFSET,
    UINT64_B7_OFFSET,
    UINT56_B0_OFFSET = 0,
    UINT56_B1_OFFSET,
    UINT56_B2_OFFSET,
    UINT56_B3_OFFSET,
    UINT56_B4_OFFSET,
    UINT56_B5_OFFSET,
    UINT56_B6_OFFSET,
    UINT48_B0_OFFSET = 0,
    UINT48_B1_OFFSET,
    UINT48_B2_OFFSET,
    UINT48_B3_OFFSET,
    UINT48_B4_OFFSET,
    UINT48_B5_OFFSET,
    UINT40_B0_OFFSET = 0,
    UINT40_B1_OFFSET,
    UINT40_B2_OFFSET,
    UINT40_B3_OFFSET,
    UINT40_B4_OFFSET,
    UINT32_B0_OFFSET = 0,
    UINT32_B1_OFFSET,
    UINT32_B2_OFFSET,
    UINT32_B3_OFFSET,
    UINT24_B0_OFFSET = 0,
    UINT24_B1_OFFSET,
    UINT24_B2_OFFSET,
    UINT16_B0_OFFSET = 0,
    UINT16_B1_OFFSET
#endif
};

/*** Module Function Prototypes **********************************************/

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

#ifdef  __cplusplus
}
#endif

#endif  /* __MMSW_BYTE_ORDERING_H__ */
