#include "lu_evsignal-internal.h"
#include "lu_event-internal.h"
#include "lu_evsignalfd.h"
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

int lu_evsig_init(lu_event_base_t *base){
    /*
    * 我们的信号处理程序将写入套接字对的一端
    * 以唤醒我们的事件循环。然后事件循环扫描已传递的
    * 信号。
    */

}


