/**
 * @file lu_log.c
 * @author <hesphoros hesphoros@gmail.com>
 * @date 2025-12-13
 * @brief Log an event to the event log.
*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "lu_log-internal.h"
#include <stdlib.h>
#include "lu_util.h"
#include "lu_erron.h"
//#include "lu_mm-internal.h"

/**
 * @brief Log an event to the event log.
 * @param severity The severity of the event.
 * @param msg The message to log.
*/
static void lu_event_log_(int severity, const char *msg);



#ifdef LU_EVENT_DEBUG_LOGGING_ENABLED
#ifdef LU_USE_DEBUG
#define DEFAULT_MASK LU_EVENT_DBG_ALL
#else
#define DEFAULT_MASK 0
#endif
LU_EVENT_EXPORT_SYMBOL lu_uint32_t lu_event_debug_logging_mask_ = DEFAULT_MASK;
#endif /* LU_EVENT_DEBUG_LOGGING_ENABLED */


static void lu_event_log_(int severity, const char *msg) { 
        
    if(lu_event_log_global_fn_)
        lu_event_log_global_fn_(severity, msg);
    else{
        const char * severity_str;
        switch(severity){
            case LU_EVENT_LOG_DEBUG:
                severity_str = "debug";
                break;
            case LU_EVENT_LOG_MSG:
                severity_str = "msg";
                break;
            case LU_EVENT_LOG_WARN:
                severity_str = "warn";
                break;
            case LU_EVENT_LOG_ERROR:
                severity_str = "error";
                break;
            default:
                severity_str = "unknown_severity";
                break;
        }
        //void to avoid unused variable warning
        (void)fprintf(stderr, "[%s] %s\n", severity_str, msg);
    }

}

void lu_event_set_log_callback(lu_event_log_cb log_cb){
    lu_event_log_global_fn_ = log_cb;    
}

void lu_event_enable_debug_logging(lu_uint32_t which_mask)
{
    #ifdef LU_EVENT_DEBUG_LOGGING_ENABLED
    lu_event_debug_logging_mask_ = which_mask;
    #endif /* LU_EVENT_DEBUG_LOGGING_ENABLED */
}

void lu_event_set_fatal_callback(lu_event_fatal_cb fatal_cb){
    lu_event_fatal_global_fn_ = fatal_cb;
}

void lu_event_logv_(int severity, const char *errstr, const char *fmt, va_list ap){
    char buff[1024];
    size_t len;

    // if severity is LU_EVENT_LOG_DEBUG and lu_event_debug_get_logging_mask_ is 0(LU_EVENT_DBG_NONE),return
    if(LU_EVENT_LOG_DEBUG == severity && !(lu_event_debug_get_logging_mask_()))
        return;

    if(NULL != fmt)
        len = lu_evutil_vsnprintf(buff, sizeof(buff), fmt, ap); 
    else
        buff[0] = '\0';
    
    if(errstr){
        len = strlen(buff);
        if(len < sizeof(buff)-1){
            lu_evutil_snprintf(buff+len, sizeof(buff)-len, ": %s", errstr);
        }        
    }
    lu_event_log_(severity, buff);
}

void lu_event_error(int errnum, const char *fmt,...) {
    va_list ap;
    va_start(ap, fmt);
    lu_event_logv_(LU_EVENT_LOG_ERROR, strerror(errnum), fmt, ap);
    va_end(ap);
}

void lu_event_warn(const char *fmt,...){
    va_list ap;
    va_start(ap, fmt);
    lu_event_logv_(LU_EVENT_LOG_WARN, NULL, fmt, ap);
    va_end(ap);
}


static void lu_event_exit(int errcode)
{
	if (lu_event_fatal_global_fn_) {
		lu_event_fatal_global_fn_(errcode);
		exit(errcode); /* should never be reached */
	} else if (errcode == LU_EVENT_ERROR_ABORT_)
		abort();
	else
		exit(errcode);
}


void lu_event_sock_error(int eval,lu_evutil_socket_t sock,const char *fmt,...){
    va_list ap;
	int err = lu_evutil_socket_geterror(sock);

	va_start(ap, fmt);
	lu_event_logv_(LU_EVENT_LOG_ERROR, lu_evutil_socket_error_to_string(err), fmt, ap);
	va_end(ap);
	lu_event_exit(eval);
}

void lu_event_sock_warn(lu_evutil_socket_t sock,const char *fmt,...){
    va_list ap;
	int err = lu_evutil_socket_geterror(sock);

	va_start(ap, fmt);
	lu_event_logv_(LU_EVENT_LOG_ERROR, lu_evutil_socket_error_to_string(err), fmt, ap);
	va_end(ap);
}


/**
 * 
 * 
LU_EVENT_EXPORT_SYMBOL LU_EVENT_EXPORT_SYMBOL 

LU_EVENT_EXPORT_SYMBOL void lu_event_msgx(const char *fmt, ...) LU_EV_CHECK_FMT(1,2);
LU_EVENT_EXPORT_SYMBOL void lu_event_debugx_(const char *fmt, ...) LU_EV_CHECK_FMT(1,2);
LU_EVENT_EXPORT_SYMBOL void lu_event_logv_(int severity, const char *errstr, const char *fmt, va_list ap) LU_EV_CHECK_FMT(3,0);



 */

void lu_event_errorx(int eval, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    lu_event_logv_(LU_EVENT_LOG_ERROR, strerror(eval), fmt, ap);
    va_end(ap);
    lu_event_exit(eval);    
}


void lu_event_warnx(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    lu_event_logv_(LU_EVENT_LOG_WARN, NULL, fmt, ap);
    va_end(ap);
}

void lu_event_msgx(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    lu_event_logv_(LU_EVENT_LOG_MSG, NULL, fmt, ap);
    va_end(ap);
}

void lu_event_debugx_(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    lu_event_logv_(LU_EVENT_LOG_DEBUG, NULL, fmt, ap);
    va_end(ap);
}

