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
#include <unistd.h>
//#include "lu_mm-internal.h"
#include "lu_util.h"
#include "lu_erron.h"

#include <pthread.h>
#include <errno.h>
#include "lu_memory_manager.h"


#define LU_LOG_USE_COLOR
#define MAX_CALLBACKS 32

/**
 * @brief Log an event to the event log.
 * @param severity The severity of the event.
 * @param msg The message to log.
*/
static void lu_event_log_(int severity, const char *msg);

//TODO: DELELTE ME
#define LU_EVENT_DEBUG_LOGGING_ENABLED
#define LU_USE_DEBUG

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
	int severity;
}lu_log_callback_t;

static  struct {
	void* data;
		
	int severity;
	int quiet;
	lu_log_callback_t callbacks;
}lu_log_config_t;

//添加lu_log_config_t 的锁的逻辑 使用pthread_mutex_t
static pthread_mutex_t lu_log_config_lock = PTHREAD_MUTEX_INITIALIZER;



static const char* lu_log_level_strings[] = {
	"DEBUG",
	"MSG",
	"WARN",
	"ERROR",
	"FATAL",
};

#ifdef LU_LOG_USE_COLOR
static const char* lu_level_colors[] = {
	
	"\x1b[36m",  // DEBUG
	"\x1b[32m",  // MSG
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
            case LU_EVENT_LOG_LEVEL_DEBUG:
                severity_str = "DEBUG";
                break;
            case LU_EVENT_LOG_LEVEL_MSG:
                severity_str = "MSG";
                break;
            case LU_EVENT_LOG_LEVEL_WARN:
                severity_str = "WARN";
                break;
            case LU_EVENT_LOG_LEVEL_ERROR:
                severity_str = "ERROR";
                break;
            default:
                severity_str = "UNKNOWN_LEVEL";
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


void lu_log_set_level(int level)
{
	lu_log_config_t.severity = level;
}


void lu_event_logv_(int severity, const char *errstr, const char *fmt, va_list ap){
    char buff[1024];
    size_t len;

    // if severity is LU_EVENT_LOG_LEVEL_DEBUG and lu_event_debug_get_logging_mask_ is 0(LU_EVENT_DBG_NONE),return
    if(LU_EVENT_LOG_LEVEL_DEBUG == severity && !(lu_event_debug_get_logging_mask_()))
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

//void lu_event_error(int errnum, const char *fmt,...)
void lu_event_error(int errnum, const char* file, int line, const char *fmt,...) {
    LU_UNUSED(file);
    LU_UNUSED(line);
    va_list ap;
    va_start(ap, fmt);
    lu_event_logv_(LU_EVENT_LOG_LEVEL_ERROR, strerror(errnum), fmt, ap);
    //lu_event_log_logv_(LU_EVENT_LOG_LEVEL_ERROR, strerror(errnum), file, line, fmt, ap);
    va_end(ap);
}

void lu_event_fatal(const char* file, int line, const char* fmt, ...){
   
    va_list ap;
    va_start(ap, fmt);
    //lu_event_logv_(LU_EVENT_LOG_LEVEL_FATAL, NULL, fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_FATAL, NULL, file, line, fmt, ap);
    va_end(ap);

}


void lu_event_errorv(const char* file, int line, const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    //lu_event_logv_(LU_EVENT_LOG_LEVEL_ERROR, strerror(errno), fmt, ap);

    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_ERROR, NULL, file, line, fmt, ap);
    va_end(ap);
}

//void lu_event_warn(const char *fmt,...) 
void lu_event_warn(const char *file, int line, const char *fmt,...){
    va_list ap;
    va_start(ap, fmt);
    //lu_event_logv_(LU_EVENT_LOG_LEVEL_WARN, NULL, fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_WARN, NULL, file, line, fmt, ap);
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

//void lu_event_sock_error(int eval,lu_evutil_socket_t sock,const char *fmt,...)
void lu_event_sock_error(int eval,lu_evutil_socket_t sock,const char* file,int line,const char *fmt,...){
    va_list ap;
	int err = lu_evutil_socket_geterror(sock);

	va_start(ap, fmt);
	//lu_event_logv_(LU_EVENT_LOG_LEVEL_ERROR, lu_evutil_socket_error_to_string(err), fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_ERROR, lu_evutil_socket_error_to_string(err), file, line, fmt, ap);
	va_end(ap);
	lu_event_exit(eval);
}

//void lu_event_sock_warn(lu_evutil_socket_t sock,const char *fmt,...)
void lu_event_sock_warn(lu_evutil_socket_t sock,const char* file,int line,const char *fmt,...){
    va_list ap;
	int err = lu_evutil_socket_geterror(sock);

	va_start(ap, fmt);
	//lu_event_logv_(LU_EVENT_LOG_LEVEL_WARN, lu_evutil_socket_error_to_string(err), fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_WARN, lu_evutil_socket_error_to_string(err), file, line, fmt, ap);
	va_end(ap);
}


//void lu_event_errorx(int eval, const char *fmt, ...)
void lu_event_errorx(int eval, const char* file, int line,const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    //lu_event_logv_(LU_EVENT_LOG_LEVEL_ERROR, strerror(eval), fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_ERROR,  strerror(eval), file ,line, fmt, ap);
    va_end(ap);
    lu_event_exit(eval);    
}

//void lu_event_warnx(const char *fmt, ...) 
void lu_event_warnx(const char *file, int line,const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    //lu_event_logv_(LU_EVENT_LOG_LEVEL_WARN, NULL, fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_WARN, NULL, file, line, fmt, ap);
    va_end(ap);
}

//void lu_event_msgx(const char *fmt, ...)
void lu_event_msgx(const char *file, int line,const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    //lu_event_logv_(LU_EVENT_LOG_LEVEL_MSG, NULL, fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_MSG,NULL, file, line, fmt, ap);
    va_end(ap);
}


//void lu_event_debugx_(const char *fmt, ...)
void lu_event_debugx_(const char *file, int line,const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    //lu_event_logv_(LU_EVENT_LOG_LEVEL_DEBUG, NULL, fmt, ap);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_DEBUG, NULL, file, line, fmt, ap);
    va_end(ap);
}


void lu_event_errorv_(const char* file, int line, const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    lu_event_log_logv_(LU_EVENT_LOG_LEVEL_ERROR, NULL, file, line, fmt, ap);
    va_end(ap);
}

static void lu_stdout_handler(lu_log_event_t* log_event) {
	char buf[32];
	buf[strftime(buf, sizeof(buf), "%H:%M:%S", log_event->time_info)] = '\0';
#ifdef LU_LOG_USE_COLOR
	if (isatty(fileno(log_event->data))) {
		fprintf(
			log_event->data, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
			buf, lu_level_colors[log_event->severity], lu_log_level_strings[log_event->severity],
			log_event->file, log_event->line);
	}
	else {
		fprintf(
			log_event->data, "%s %-5s %s:%d: ",
			buf, lu_log_level_strings[log_event->severity], log_event->file, log_event->line);
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
		buf, lu_log_level_strings[log_event->severity], log_event->file, log_event->line);
	vfprintf(log_event->data, log_event->fmt, log_event->ap);
	fprintf(log_event->data, "\n");
	fflush(log_event->data);
}


static void lu_lock(void) {
	pthread_mutex_lock(&lu_log_config_lock);
}

static void lu_unlock(void) {
		pthread_mutex_unlock(&lu_log_config_lock);
}






void lu_log_set_quiet(int enable)
{
	lu_log_config_t.quiet = enable;
}

int lu_log_add_handler(lu_log_handler_t handler, void* data, int level)
{
	
    lu_log_config_t.callbacks = (lu_log_callback_t){ handler,data,level };

	return 0;
}

int lu_log_add_fp(FILE* fp, int level)
{
	return lu_log_add_handler(lu_file_handler, fp, level);
}

static void lu_init_event(lu_log_event_t* log_event, void* data) {
    if (!log_event) return;

    // 在栈上分配 time_info
    static struct tm time_info;  // 不再使用 malloc，而是在栈上分配

    time_t t = time(NULL);
    if (localtime_r(&t, &time_info) == NULL) {
        fprintf(stderr, "Error: localtime_r failed, check TZ environment variable.\n");
        return;
    }

    // 将栈上的 time_info 地址赋给 log_event->time_info
    log_event->time_info = &time_info;  // 这里的指针指向栈上的结构体

    if (!data) {
        fprintf(stderr, "Warning: data pointer is NULL.\n");
    }

    log_event->data = data;

    // 此时不再需要手动释放 memory，避免内存泄漏
}


 


//int severity, const char *errstr, const char *fmt, va_list ap
void lu_event_log_logv_(int severity, const char* errstr, const char *file, int line, const char* fmt, va_list ap) {
    // Declare the buffer for formatted log message
    char buff[1024];
    size_t len = 0; // Initialize len to zero

    // If the severity is DEBUG and the debug logging mask is not set, skip logging
    if (severity == LU_EVENT_LOG_LEVEL_DEBUG && !(lu_event_debug_get_logging_mask_())) {
        printf("DEBUG logging is disabled is %d\n", lu_event_debug_get_logging_mask_());
        return;
    }

    // If fmt is not NULL, format the log message using vsnprintf
    if (fmt != NULL) {
        len = lu_evutil_vsnprintf(buff, sizeof(buff), fmt, ap);
    } else {
        buff[0] = '\0'; // If fmt is NULL, set buff to empty string
    }

    // If there is an error string, append it to the log message
    if (errstr) {
        size_t errstr_len = strlen(errstr);
        // Ensure there is enough space for errstr and ": " separator
        if (len + errstr_len + 2 < sizeof(buff)) {  // +2 for ": " separator
            lu_evutil_snprintf(buff + len, sizeof(buff) - len, ": %s", errstr);
            len += errstr_len + 2;  // Update len to include errstr length
        } else {
            // If not enough space, append only part of the errstr or skip appending
            buff[sizeof(buff) - 1] = '\0'; // Ensure buffer ends with null terminator
        }
    }

    // Initialize the log event structure
    lu_log_event_t log_event = {
        .fmt = buff,
        .file = file,
        .line = line,
        .severity = severity,
    };

    // Lock the logging mechanism for thread safety
    lu_lock();

    // Only output logs that match the current severity level and are not in quiet mode
    if (!lu_log_config_t.quiet && severity >= lu_log_config_t.severity) {
        // Initialize event and output to standard error
        
        lu_init_event(&log_event, stderr);
        lu_stdout_handler(&log_event);
    }
    lu_log_callback_t * cb = &lu_log_config_t.callbacks;
    if(cb->handler && severity >= cb->severity) {
        // Initialize event and output to registered callback
        lu_init_event(&log_event, cb->data);
        cb->handler(&log_event);  // Call the callback function to handle the log
    }




    // Unlock the logging mechanism
    lu_unlock();
}
