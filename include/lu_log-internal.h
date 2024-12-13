#ifndef LU_INTERNAL_LOG_H
#define LU_INTERNAL_LOG_H

#include "lu_util.h"
#include "lu_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __GNUC__
// For GCC compiler:
// - LU_EV_CHECK_FMT is a macro that adds the `format(printf, a, b)` attribute to a function,
//   which helps the compiler check the format string and the corresponding arguments at compile time.
// - LU_EV_NORETURN is a macro that adds the `noreturn` attribute, indicating that the function does not return.
// These attributes help with compile-time checks and optimization for the GCC compiler.
#define LU_EV_CHECK_FMT(a,b)    __attribute__((format(printf, a, b)))
#define LU_EV_NORETURN          __attribute__((noreturn))
#else
// For other compilers, these macros are defined as empty and do not add any attributes.
#define LU_EV_CHECK_FMT(a,b)
#define LU_EV_NORETURN
#endif


LU_EVENT_EXPORT_SYMBOL void lu_event_error(int errnum, const char *fmt,...) LU_EV_CHECK_FMT(2,3) LU_EV_NORETURN;



#ifdef __cplusplus
}
#endif



#endif /* LU_INTERNAL_LOG_H */