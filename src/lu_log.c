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
#include "lu_util.h"
#include "lu_erron.h"

#include <pthread.h>
#include <errno.h>
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
	int severity;
}lu_log_callback_t;

static  struct {
	void* data;
		
	int severity;
	int quiet;
	lu_log_callback_t callbacks[MAX_CALLBACKS];
}lu_log_config_t;

//添加lu_log_config_t 的锁的逻辑 使用pthread_mutex_t
static pthread_mutex_t lu_log_config_lock = PTHREAD_MUTEX_INITIALIZER;



// static const char* lu_log_level_strings[] = {
// 	"TRACE",
// 	"DEBUG",
// 	"INFO",
// 	"WARN",
// 	"ERROR",
// 	"FATAL",
// };

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
                severity_str = "DEBUG";
                break;
            case LU_EVENT_LOG_MSG:
                severity_str = "MSG";
                break;
            case LU_EVENT_LOG_WARN:
                severity_str = "WARN";
                break;
            case LU_EVENT_LOG_ERROR:
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

//#define lu_log_trace(...) lu_log_log(LU_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)

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

// const char* lu_log_level_to_string(lu_log_level_t level)
// {
// 	return lu_log_level_strings[level];
// }



// void lu_log_set_level(lu_log_level_t level)
// {
// 	lu_log_config_t.level = level;
// }


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
	
	lu_log_event_t log_event = {
		.fmt = fmt,
		.file = file,
		.line = line,
		.severity = level,
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



void lu_event_log_logv_(int severity, const char* errstr, const char *file, int line, const char* fmt, ...) {
	
	va_list ap;	
	char buff[1024];
	size_t len;

	//// 如果是调试级别并且日志调试掩码为 0，跳过日志记录
	if(severity == LU_EVENT_LOG_DEBUG && !(lu_event_debug_get_logging_mask_()))
		return;

	
    // 使用 va_start 来处理变长参数
    va_start(ap, fmt);

	 // 使用 vsnprintf 格式化日志消息
	if(NULL != fmt)
		len = lu_evutil_vsnprintf(buff, sizeof(buff), fmt, ap); 
	else
		buff[0] = '\0';

	
   

	  // 如果有 errstr，附加到日志消息
    if (errstr) {
        size_t errstr_len = strlen(errstr);
        if (len + errstr_len + 2 < sizeof(buff)) {  // +2 for ": " separator
            lu_evutil_snprintf(buff + len, sizeof(buff) - len, ": %s", errstr);
            len += errstr_len + 2;  // 更新 len 以包含 errstr 长度
        }
    }

	lu_log_event_t log_event = {
		.fmt = buff,
		.file = file,
		.line = line,
		.severity = severity,
		};
	lu_lock();
   // 只输出符合日志级别的日志
    if (!lu_log_config_t.quiet && severity >= lu_log_config_t.severity) {
        // 初始化事件并输出到标准输出
        lu_init_event(&log_event, stderr);
        lu_stdout_handler(&log_event);
    }

    // 调用各个回调处理日志
    for (size_t i = 0; i < MAX_CALLBACKS && lu_log_config_t.callbacks[i].handler; i++) {
        lu_log_callback_t* cb = &lu_log_config_t.callbacks[i];
        if (severity >= cb->severity) {
            lu_init_event(&log_event, cb->data);
            cb->handler(&log_event);  // 调用回调函数处理日志
        }
    }

    // 解锁日志操作
    lu_unlock();

    // 结束变长参数处理
    va_end(ap);
	

}
