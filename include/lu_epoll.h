#ifndef _LU_EVENT_EPOLL_INCLUDE_H_
#define _LU_EVENT_EPOLL_INCLUDE_H_

#include  <lu_event-internal.h>
#include <sys/epoll.h>
#include "lu_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef int lu_epoll_handle_t;
#define INVALID_EPOLL_HANDLE -1
static void close_epoll_handle(int h) { close(h); }

#define INITIAL_NEVENT 32
#define MAX_NEVENT 4096

//DELTETME:
#define USING_TIMERFD

void* lu_epoll_init(lu_event_base_t* );

int lu_epoll_nochangelist_add(lu_event_base_t* , lu_evutil_socket_t , short , short , void* );

int lu_epollnochangelist_del(lu_event_base_t *, lu_evutil_socket_t ,short , short , void *);

int lu_epoll_dispatch(lu_event_base_t*, struct timeval *);

void lu_epoll_dealloc(lu_event_base_t *);


int lu_event_changelist_add_(lu_event_base_t *base, lu_evutil_socket_t fd, short old, short events,void *fdinfo);

int lu_event_changelist_del_(lu_event_base_t *base, lu_evutil_socket_t fd, short old, short events, void *fdinfo);


const  lu_event_op_t epool_ops = {
    "epoll",
    lu_epoll_init,
    lu_epoll_nochangelist_add,
    lu_epollnochangelist_del,
    lu_epoll_dispatch,
    lu_epoll_dealloc,
    1, /* need_reinit */
    LU_EVENT_FEATURE_ET | LU_EVENT_FEATURE_O1 | LU_EVENT_FEATURE_EARLY_CLOSE,
    0,
};


struct epool_op{
    struct epoll_event* events;
    int nevents;
    lu_epoll_handle_t epoll_fd;
#ifdef USING_TIMERFD
    int timerfd;
#endif

};

#define LU_EVENT_CHANGELIST_FDINFO_SIZE sizeof(int)

static const lu_event_op_t epollops_changelist = {
    "epoll (with changelist)",
    lu_epoll_init,
    lu_event_changelist_add_,
    lu_event_changelist_del_,
    lu_epoll_dispatch,
    lu_epoll_dealloc,
    1, /* need reinit */
	LU_EVENT_FEATURE_ET|LU_EVENT_FEATURE_O1| LU_EVENT_FEATURE_EARLY_CLOSE,
	LU_EVENT_CHANGELIST_FDINFO_SIZE,
};

#ifdef __cplusplus
}
#endif

#endif // _LU_EVENT_EPOLL_INCLUDE_H_