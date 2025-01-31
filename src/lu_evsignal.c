#define _GNU_SOURCE     /* See feature_test_macros(7) */
#include "lu_evsignal-internal.h"
#include "lu_event-internal.h"
#include "lu_evsignalfd.h"
#include "lu_log-internal.h"
#include <unistd.h>
           /* See feature_test_macros(7) */
#include <fcntl.h>
#include "lu_util.h"
#include <signal.h>
#include "lu_event.h"
#include "lu_evthread-internal.h"

static void lu_evsig_cb(lu_evutil_socket_t fd, short what, void *arg);

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

    lu_event_assign_(&base->sig_info_.ev_signal, base, base->sig_info_.ev_signal_pair[0],
		LU_EV_READ | LU_EV_PERSIST, lu_evsig_cb, base);
	base->sig_info_.ev_signal.ev_flags |= LU_EVLIST_INTERNAL;
	lu_event_priority_set(&base->sig_info_.ev_signal, 0);
	
	base->evsigsel_op = &evsigops;

	return 0;
}



#ifndef NSIG
#define NSIG 32
#endif


/*
该函数lu_evsig_cb是一个静态函数，用于处理文件描述符fd上的信号事件。当有信号到达时，该函数会被调用，读取并处理这些信号。*/
/* Callback for when the signal handler write a byte to our signaling socket */
static void
lu_evsig_cb(lu_evutil_socket_t fd, short what, void *arg)
{
	//TODO: Finish this function
	static char signals[1024];
	lu_ssize_t n;
	int i;
	int ncaught[NSIG];
	lu_event_base_t *base;

	base = arg;

	memset(&ncaught, 0, sizeof(ncaught));

	while (1) {

		n = read(fd, signals, sizeof(signals));

		if (n == -1) {
			int err = lu_evutil_socket_geterror(fd);
			if (! LU_EVUTIL_ERR_RW_RETRIABLE(err))
				event_sock_err(1, fd, "%s: recv", __func__);
			break;
		} else if (n == 0) {
			/* XXX warn? */
			break;
		}
		for (i = 0; i < n; ++i) {
			lu_uint8_t sig = signals[i];
			if (sig < NSIG)
				ncaught[sig]++;
		}
	}

	LU_EVBASE_ACQUIRE_LOCK(base, th_base_lock);
	for (i = 0; i < NSIG; ++i) {
		if (ncaught[i])
			evmap_signal_active_(base, i, ncaught[i]);
	}
	LU_EVBASE_RELEASE_LOCK(base, th_base_lock);
}
