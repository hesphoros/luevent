#include "lu_epoll.h"
#include "lu_util.h"
#include <errno.h>
#include "lu_log-internal.h"
#include "lu_mm-internal.h"
#include <time.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/types.h>

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

//初始化epoll句柄
static void* lu_epoll_init(lu_event_base_t* base){

    lu_epoll_handle_t epoll_fd = INVALID_EPOLL_HANDLE;
    struct epool_op* epoll_op;
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if(epoll_fd == INVALID_EPOLL_HANDLE)
    {
        if ((epoll_fd = epoll_create(32000)) == INVALID_EPOLL_HANDLE)
        {
                if (errno != ENOSYS)
                    LU_EVENT_LOG_WARN("epoll_create");
                return (NULL);
        }
    }

    lu_evutil_make_socket_closeonexec(epoll_fd);
    if (!(epoll_op = mm_calloc(1, sizeof(struct epool_op))))
    {
        close_epoll_handle(epoll_fd);
        LU_EVENT_LOG_ERRORV("epoll_init: mm_calloc failed");
        return (NULL);
    }
    epoll_op->epoll_fd = epoll_fd;

	/* Initialize fields */
    epoll_op->events = mm_calloc(INITIAL_NEVENT, sizeof(struct epoll_event));
    if(epoll_op->events == NULL){
        mm_free(epoll_op);
        close_epoll_handle(epoll_fd);
        LU_EVENT_LOG_WARN("epoll_init: mm_calloc failed");
        return (NULL);
    }
    epoll_op->nevents = INITIAL_NEVENT;


	if ((base->flags & LU_EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST) != 0 ||
	    ((base->flags & LU_EVENT_BASE_FLAG_IGNORE_ENV) == 0 &&
		lu_evutil_getenv_("EVENT_EPOLL_USE_CHANGELIST") != NULL)) {

		base->evsel_op = &epollops_changelist;
	}


#ifdef USING_TIMERFD
	/*
	  The epoll interface ordinarily gives us one-millisecond precision,
	  so on Linux it makes perfect sense to use the CLOCK_MONOTONIC_COARSE
	  timer.  But when the user has set the new PRECISE_TIMER flag for an
	  event_base, we can try to use timerfd to give them finer granularity.
	*/
	if ((base->flags & LU_EVENT_BASE_FLAG_PRECISE_TIMER) &&
	    !(base->flags & LU_EVENT_BASE_FLAG_EPOLL_DISALLOW_TIMERFD) &&
	    base->monotonic_timer.monotonic_clock == CLOCK_MONOTONIC) {
		int fd;
		fd = epoll_op->timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
		if (epoll_op->timerfd >= 0) {
			struct epoll_event epev;
			memset(&epev, 0, sizeof(epev));
			epev.data.fd = epoll_op->timerfd;
			epev.events = EPOLLIN;
			if (epoll_ctl(epoll_op->epoll_fd, EPOLL_CTL_ADD, fd, &epev) < 0) {
				LU_EVENT_LOG_WARN("epoll_ctl(timerfd)");
				close(fd);
				epoll_op->timerfd = -1;
			}
		} else {
			if (errno != EINVAL && errno != ENOSYS) {
				/* These errors probably mean that we were
				 * compiled with timerfd/TFD_* support, but
				 * we're running on a kernel that lacks those.
				 */
				LU_EVENT_LOG_WARN("timerfd_create");
			}
			epoll_op->timerfd = -1;
		}
	} else {
		epoll_op->timerfd = -1;
	}
#endif

	if (sigfd_init_(base) < 0)
		lu_evsig_init_(base);

	return (epoll_op);

}



static int lu_epoll_nochangelist_add(lu_event_base_t* base, lu_evutil_socket_t fd, short old, short events, void* fdinfo){
    LU_UNUSED(base);
    LU_UNUSED(fd);
    LU_UNUSED(old);
    LU_UNUSED(events);
    LU_UNUSED(fdinfo);
    return 0;
}




static int lu_epollnochangelist_del(lu_event_base_t *base, lu_evutil_socket_t fd,short old, short events, void *fdinfo)
{
    LU_UNUSED(base);
    LU_UNUSED(fd);
    LU_UNUSED(old);
    LU_UNUSED(events);
    LU_UNUSED(fdinfo);
    return 0;
}


static int lu_epoll_dispatch(lu_event_base_t*base, struct timeval *tv){
    LU_UNUSED(base);
    LU_UNUSED(tv);
    return 0;
}

static void lu_epoll_dealloc(lu_event_base_t *base){
    LU_UNUSED(base);

}