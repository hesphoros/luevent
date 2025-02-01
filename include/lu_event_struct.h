#ifndef LU_EVENT_STRUCT_H
#define LU_EVENT_STRUCT_H


#include <sys/queue.h>
#include <sys/time.h>
#include <time.h>

#include "lu_util.h"
#include "lu_evmap.h"



#ifdef __cplusplus
extern "C" {
#endif



/* Fix so that people don't have to run with <sys/queue.h> */
#ifndef TAILQ_ENTRY
#define EVENT_DEFINED_TQENTRY_
#define TAILQ_ENTRY(type)						\
struct {								\
	struct type *tqe_next;	/* next element */			\
	struct type **tqe_prev;	/* address of previous next element */	\
}
#endif /* !TAILQ_ENTRY */

#ifndef TAILQ_HEAD
#define EVENT_DEFINED_TQHEAD_
#define TAILQ_HEAD(name, type)			\
struct name {					\
	struct type *tqh_first;			\
	struct type **tqh_last;			\
}
#endif

/* Fix so that people don't have to run with <sys/queue.h> */
#ifndef LIST_ENTRY
#define EVENT_DEFINED_LISTENTRY_
#define LIST_ENTRY(type)						\
struct {								\
	struct type *le_next;	/* next element */			\
	struct type **le_prev;	/* address of previous next element */	\
}
#endif /* !LIST_ENTRY */

#ifndef LIST_HEAD
#define EVENT_DEFINED_LISTHEAD_
#define LIST_HEAD(name, type)						\
struct name {								\
	struct type *lh_first;  /* first element */			\
	}
#endif /* !LIST_HEAD */

typedef struct lu_event_base_s  lu_event_base_t;
typedef struct lu_event_s lu_event_t;
// typedef struct lu_event_callback_s lu_event_callback_t;



typedef struct lu_event_callback_s{


    TAILQ_ENTRY(lu_event_callback_s) evcb_active_next;
    short evcb_flags;

    //Smaller numbers are higher priority.
    lu_uint8_t evcb_pri;//优先级
    lu_uint8_t evcb_closure;//闭包

    /**Allows us to adopt for different types of events*/
    union
    {
        //used for io events
        void (*evcb_callback)(lu_evutil_socket_t,short,void*);//回调函数
        void (*evcb_selfcb)(struct lu_event_callback_s*,void*);//自身回调函数
        void (*evcb_evfinalize)(struct lu_event_s*,void*);//事件最终化回调函数
        void (*evcb_cbfinalize)(struct lu_event_callback_s*,void*);//回调最终化回调函数
    }evcb_cb_union;
    void *evcb_arg;//回调函数的参数

}lu_event_callback_t;



typedef struct lu_event_s{
    lu_event_callback_t ev_evcallback;
    //表示事件在不同类型的超时列表中的位置。根据事件类型，这个成员的不同部分会被使用。
    union
    {
        TAILQ_ENTRY(lu_event_s) ev_next_with_common_timeout;
        lu_size_t min_heap_idx;//该事件在最小堆（min heap）中的索引，用于快速查找最早的超时事件。
    }ev_timeout_pos;

    lu_evutil_socket_t ev_fd;
    short ev_events;
    short ev_res;//result passed to event callback

    lu_event_base_t* ev_base;

    union {
        //used for io events
        struct{
            LIST_ENTRY( lu_event_s) ev_io_next;
            struct timeval ev_timeout;
        }ev_io;
        //used for signal events
        struct{
            LIST_ENTRY(lu_event_s) ev_signal_next;
            short ev_ncalls;
            //Allow deletes in signal callback
            short* ev_pncalls;
        }ev_signal;
    }ev_;

    struct timeval ev_timeout;


}lu_event_t;



#define LU_EVLIST_TIMEOUT	    0x01
#define LU_EVLIST_INSERTED	    0x02
#define LU_EVLIST_SIGNAL	    0x04
#define LU_EVLIST_ACTIVE	    0x08
#define LU_EVLIST_INTERNAL	    0x10
#define LU_EVLIST_ACTIVE_LATER  0x20
#define LU_EVLIST_FINALIZING    0x40
#define LU_EVLIST_INIT	        0x80

#define LU_EVLIST_ALL           0xff


#ifdef __cplusplus
}
#endif

#endif // LU_EVENT_STRUCT_H