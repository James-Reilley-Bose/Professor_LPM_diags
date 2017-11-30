#ifndef ASSERT_H
#define ASSERT_H

#ifdef USE_DEBUG_ASSERT
// See http://docs.klocwork.com/Insight-10.0/Tuning_C/C++_analysis_through_macro_simplification
#ifdef __KLOCWORK__
#define klocwork_assert(expr) do { if (!(expr)) abort(); } while (0)
#define debug_assert(expr) klocwork_assert(expr)
#define configASSERT(expr) klocwork_assert(expr)
#define RTOS_assert(expr, error_str) klocwork_assert(expr)
#else
extern void TAP_AssertPrint(const char* file, const uint32_t line);
#define debug_assert(expr) if(!(expr)) { TAP_AssertPrint(__FILE__,__LINE__); }
#define configASSERT(expr) if(!(expr)) { TAP_AssertPrint(__FILE__,__LINE__); }
#define RTOS_assert(expr, error_str) if(!(expr)) {TAP_AssertPrint(error_str, __LINE__); }
#endif
#else
#define debug_assert(expr)
#define configASSERT(expr)
#define RTOS_assert(expr,_errStr)
#endif

#endif



