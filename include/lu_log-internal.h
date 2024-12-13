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




/**
  A callback function used to intercept lu_event's log messages.

  @see lu_event_set_log_callback
 */
typedef void (*lu_event_log_cb)(int severity, const char *msg) ;


/**
 * A callback function to be called if luevent encounters a fatal error.
 */
typedef void (*lu_event_fatal_cb)(int error_code);

static lu_event_log_cb 		lu_event_log_global_cb = NULL;
static lu_event_fatal_cb 	lu_event_fatal_global_cb = NULL;

/**
 * Redirect luevent's log messages to a custom callback function.
 * @param log_cb The callback function to use. If NULL, the default log handler is used.
   two arguments: severity (between LU_EVENT_LOG_DEBUG to LU_EVENT_LOG_ERROR) and message (a string)
 */
LU_EVENT_EXPORT_SYMBOL void lu_event_set_log_callback(lu_event_log_cb log_cb);

/**
 * Turn on debuging logs and have them output to the default log handeler.
 * This is a global setting and affects all logs.
 * @param which_mask Controls which debug messages are enabled. 
 * This function set lu_event_debug_logging_mask_ to the new mask(which_mask).
   you must pass the constant "LU_EVENT_DBG_NONE" to turn off all debug messages
   you can pass the constant "LU_EVENT_DBG_ALL" to turn on all debug messages   
*/
LU_EVENT_EXPORT_SYMBOL void lu_event_enable_debug_logging(lu_uint32_t which_mask);


/**
 * Set a callback function to be called if luevent encounters a fatal error.
 * @param fatal_cb The callback function to use. If NULL, the default fatal error handler is used.
   one argument: error_code (an integer)
 * By default, luevent will call exit(1) if a fatal error occurs.Note that if the function is ever invoked,
   it means something is wrong with your program, or with luevent: any subsequent calls
 * This function will set lu_event_fatal_global_cb(init is NULL) to the new callback function.
 */
LU_EVENT_EXPORT_SYMBOL void lu_event_set_fatal_callback(lu_event_fatal_cb fatal_cb);

#ifdef __cplusplus
}
#endif



#endif /* LU_INTERNAL_LOG_H */