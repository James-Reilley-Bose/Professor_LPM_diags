/*************************************************************************//**
 * @file            mmsw_util.h
 * @brief           Utility macro/function definitions
 * @date            $Date: 2011-12-20 16:49:52 -0500 (Tue, 20 Dec 2011) $
 * @author          $Author: cv13058 $
 * @version         $Id: mmsw_util.h 885 2011-12-20 21:49:52Z cv13058 $
 *
 * Description ***************************************************************
 *      Utility/general-purpose macros and functions used by the MMSW group.
 *      Users may override any and all definitions if desired by overriding
 *      the applicable macro(s).
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/
#ifndef __MMSW_UTIL_H__
#define __MMSW_UTIL_H__

/*** Include Files ***********************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/

#ifndef ARRAY_SIZE
/**
 * @brief Reports the number of elements in an array
 *
 * @param x The array
 * @return The number of elements in the array
 */
#   define ARRAY_SIZE(x)            (sizeof(x)/sizeof(x[0]))
#endif

#ifndef BIT
/**
 * @brief Evaluates to the value of the bit number specified
 *
 * e.g., BIT(4) evaluates to 0x10 == 16
 *
 * @param x The bit number
 * @return The value obtained by setting the specified bit
 */
#   define BIT(x)                   ((unsigned int)1 << (x))
#endif

#ifndef MAX
/**
 * @brief Computes the maximum of two values
 *
 * @param a The first value
 * @param b The second value
 * @return The maximum of a and b
 */
#   define MAX(a,b)                 ((a)>(b)?(a):(b))
#endif

#ifndef MIN
/**
 * @brief Computes the minimum of two values
 *
 * @param a The first value
 * @param b The second value
 * @return The minimum of a and b
 */
#   define MIN(a,b)                 ((a)<(b)?(a):(b))
#endif

#ifndef COMPILEASSERT
/**
 * @brief Causes a compilation error if the assertion fails
 *
 * @param x Boolean expression to evaluate for the assertion
 */
#ifdef  __cplusplus
#   define COMPILEASSERT(x) extern "C" char CompileAssert_##__FILE__##__LINE__[(x) ? 1 : -1]
#else
#   define COMPILEASSERT(x) extern     char CompileAssert_##__FILE__##__LINE__[(x) ? 1 : -1]
#endif
#endif

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

#ifdef  __cplusplus
}
#endif

#endif  /* __MMSW_UTIL_H__ */
