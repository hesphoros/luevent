#include "lu_log-internal.h"
#include "lu_mm-internal.h"
#include "lu_util.h"
#include "lu_erron.h"

/**
 * @brief Log an event to the event log.
 * @param severity The severity of the event.
 * @param msg The message to log.
*/
static void lu_event_log_(int severity, const char *msg);

static void lu_event_log_(int severity, const char *msg) { 
        
    if(lu_event_log_global_cb)
        lu_event_log_global_cb(severity, msg);
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