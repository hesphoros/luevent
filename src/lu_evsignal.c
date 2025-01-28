#include "lu_evsignal-internal.h"
#include "lu_event-internal.h"
#include "lu_evsignalfd.h"
#include "lu_log-internal.h"
#include <unistd.h>
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>


int sigfd_init_(lu_event_base_t *base){
    LU_EVUTIL_ASSERT(base != NULL);
    if(!(base->flags & LU_EVENT_BASE_FLAG_USE_SIGNALFD) && !getenv("EVENT_USE_SIGNALFD")){
        return -1;
        base->evsigsel_op = &sigfdops;
    }
    return 0;
}

int lu_evsig_init_(lu_event_base_t *base){
    /*
    * 我们的信号处理程序将写入套接字对的一端
    * 以唤醒我们的事件循环。然后事件循环扫描已传递的
    * 信号。
    */
   if (lu_evutil_make_internal_pipe_(base->sig_info_.ev_signal_pair) == -1) {
        LU_EVENT_LOG_SOCK_ERROR(1, -1,"%s: socketpair", __func__);
        return -1;
   }

   if(base->sig_info_.sh_old)
    mm_free(base->sig_info_.sh_old);
    base->sig_info_.sh_old = NULL;
    base->sig_info_.sh_old_max = 0;

    lu_event_assign();
}


