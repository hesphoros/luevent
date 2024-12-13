#ifndef LU_INTERNAL_LOG_H
#define LU_INTERNAL_LOG_H

#include "lu_util.h"
#include "lu_visibility.h"

#include <stdarg.h>

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


#define LU_EVENT_ERROR_ABORT_ ((int)0xdeaddead)


#if !defined(LU_EVENT__DISABLE_DEBUG_MODE) || defined(LU_USE_DEBUG)
#define LU_EVENT_DEBUG_LOGGING_ENABLED
#endif

#ifdef LU_EVENT_DEBUG_LOGGING_ENABLED
LU_EVENT_EXPORT_SYMBOL extern lu_uint32_t   lu_event_debug_logging_mask_;
#define lu_event_debug_get_logging_mask_() (lu_event_debug_logging_mask_)
#else
#define lu_event_debug_get_logging_mask_() (0)
#endif

#ifdef LU_EVENT_DEBUG_LOGGING_ENABLED
#define event_debug(x) do {			\
	if (lu_event_debug_get_logging_mask_()) {	\
		event_debugx_ x;		\
	}					\
	} while (0)
#else
#endif //LU_EVENT_DEBUG_LOGGING_ENABLED

//Log severities defined in lu_utils.h

// #define LU_EVENT_LOG_DEBUG  0
// #define LU_EVENT_LOG_MSG    1
// #define LU_EVENT_LOG_WARN   2
// #define LU_EVENT_LOG_ERROR  3

LU_EVENT_EXPORT_SYMBOL void lu_event_error(int errnum, const char *fmt,...) LU_EV_CHECK_FMT(2,3) LU_EV_NORETURN;
LU_EVENT_EXPORT_SYMBOL void lu_event_warn(const char *fmt,...) LU_EV_CHECK_FMT(1,2);
LU_EVENT_EXPORT_SYMBOL void lu_event_sock_error(int eval,lu_evutil_socket_t sock,const char *fmt,...) LU_EV_CHECK_FMT(3,4) LU_EV_NORETURN;
LU_EVENT_EXPORT_SYMBOL void lu_event_sock_warn(lu_evutil_socket_t sock,const char *fmt,...) LU_EV_CHECK_FMT(2,3);


LU_EVENT_EXPORT_SYMBOL void lu_event_errx(int eval, const char *fmt, ...) LU_EV_CHECK_FMT(2,3) LU_EV_NORETURN;
LU_EVENT_EXPORT_SYMBOL void lu_event_warnx(const char *fmt, ...) LU_EV_CHECK_FMT(1,2);
LU_EVENT_EXPORT_SYMBOL void lu_event_msgx(const char *fmt, ...) LU_EV_CHECK_FMT(1,2);
LU_EVENT_EXPORT_SYMBOL void lu_event_debugx_(const char *fmt, ...) LU_EV_CHECK_FMT(1,2);

LU_EVENT_EXPORT_SYMBOL void lu_event_logv_(int severity, const char *errstr, const char *fmt, va_list ap) LU_EV_CHECK_FMT(3,0);




#ifdef __cplusplus
}
#endif



#endif /* LU_INTERNAL_LOG_H */