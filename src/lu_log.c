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


#define LU_LOG_USE_COLOR
#define MAX_CALLBACKS 32

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



typedef struct lu_log_callback_s {
	lu_log_handler_t handler;
	void* data;
	lu_log_level_t level;
}lu_log_callback_t;

static  struct {
	void* data;
	lu_log_lock_fn lock;
	lu_log_level_t level;
	int quiet;
	lu_log_callback_t callbacks[MAX_CALLBACKS];
}lu_log_config_t;

static const char* lu_log_level_strings[] = {
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL",
};

#ifdef LU_LOG_USE_COLOR
static const char* lu_level_colors[] = {
	"\x1b[94m",  // TRACE
	"\x1b[36m",  // DEBUG
	"\x1b[32m",  // INFO
	"\x1b[33m",  // WARN
	"\x1b[31m",  // ERROR
	"\x1b[35m",  // FATAL
};
#endif

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

static void lu_stdout_handler(lu_log_event_t* log_event) {
	char buf[16];
	buf[strftime(buf, sizeof(buf), "%H:%M:%S", log_event->time_info)] = '\0';
#ifdef LU_LOG_USE_COLOR
	if (isatty(fileno(log_event->data))) {
		fprintf(
			log_event->data, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
			buf, lu_level_colors[log_event->level], lu_log_level_strings[log_event->level],
			log_event->file, log_event->line);
	}
	else {
		fprintf(
			log_event->data, "%s %-5s %s:%d: ",
			buf, lu_log_level_strings[log_event->level], log_event->file, log_event->line);
	}
#else
	fprintf(
		log_event->data, "%s %-5s %s:%d: ",
		buf, lu_log_level_strings[log_event->level], log_event->file, log_event->line);
#endif
	vfprintf(log_event->data, log_event->fmt, log_event->ap);
	fprintf(log_event->data, "\n");
	fflush(log_event->data);
}


static void lu_file_handler(lu_log_event_t* log_event) {
	char buf[64];
	buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", log_event->time_info)] = '\0';
	fprintf(
		log_event->data, "%s %-5s %s:%d: ",
		buf, lu_log_level_strings[log_event->level], log_event->file, log_event->line);
	vfprintf(log_event->data, log_event->fmt, log_event->ap);
	fprintf(log_event->data, "\n");
	fflush(log_event->data);
}


static void lu_lock(void) {
	if (lu_log_config_t.lock) { lu_log_config_t.lock(1, lu_log_config_t.data); }
}

static void lu_unlock(void) {
	if (lu_log_config_t.lock) { lu_log_config_t.lock(0, lu_log_config_t.data); }
}

const char* lu_log_level_to_string(lu_log_level_t level)
{
	return lu_log_level_strings[level];
}

void lu_log_set_lock(lu_log_lock_fn lock, void* data)
{
	lu_log_config_t.lock = lock;
	lu_log_config_t.data = data;
}

void lu_log_set_level(lu_log_level_t level)
{
	lu_log_config_t.level = level;
}


void lu_log_set_quiet(int enable)
{
	lu_log_config_t.quiet = enable;
}

int lu_log_add_handler(lu_log_handler_t handler, void* data, lu_log_level_t level)
{
	for (size_t i = 0; i < MAX_CALLBACKS; i++)
	{
		if (!lu_log_config_t.callbacks[i].handler) {
			lu_log_config_t.callbacks[i] = (lu_log_callback_t){ handler,data,level };
			return 0;
		}
	}
	return -1;
}

int lu_log_add_fp(FILE* fp, lu_log_level_t level)
{
	return lu_log_add_handler(lu_file_handler, fp, level);
}

static void lu_init_event(lu_log_event_t* log_event, void* data) {
	if (!log_event->time_info) {
		time_t t = time(NULL);
		log_event->time_info = localtime(&t);
	}
	log_event->data = data;
}


void lu_log_log(lu_log_level_t level, const char* file, int line, const char* fmt, ...)
{
	//
	lu_log_event_t log_event = {
		.fmt = fmt,
		.file = file,
		.line = line,
		.level = level,
	};

	lu_lock();
	if (!lu_log_config_t.quiet && level >= lu_log_config_t.level) {
		lu_init_event(&log_event, stderr);
		va_start(log_event.ap, fmt);
		lu_stdout_handler(&log_event);
		va_end(log_event.ap);
	}

	for (size_t i = 0; i <= MAX_CALLBACKS && lu_log_config_t.callbacks[i].handler; i++) {
		lu_log_callback_t* cb = &lu_log_config_t.callbacks[i];
		if (level >= cb->level) {
			lu_init_event(&log_event, cb->data);
			va_start(log_event.ap, fmt);
			cb->handler(&log_event);
			va_end(log_event.ap);
		}
	}
	lu_unlock();
}

