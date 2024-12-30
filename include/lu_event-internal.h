#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#include "lu_changelist-internal.h"
#include <bits/types/struct_timeval.h>
#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <sys/queue.h>
#include "lu_util.h"
#include <sys/time.h>
#include "lu_mm-internal.h"
#include "lu_changelist-internal.h"

TAILQ_HEAD(lu_evcallback_list, lu_event_callback_t);


typedef struct lu_event_base_s lu_event_base_t;
typedef struct lu_event_op_s lu_event_op_t;

typedef struct lu_event_callback_s lu_event_callback_t;
typedef struct lu_event_s lu_event_t;

typedef enum lu_event_base_config_flag_u {

    /**
     * 不要为event_base分配锁。
     * 设置这个选项可以为event_base节省一点用于锁定和解锁的时间，
     * 但是让在多个线程中访event_base成为不安全的。
     */
    LU_EVENT_BASE_FLAG_NOLOCK = 0x01,
    /**
     * Ignore environment variables when configuring the event base.
     */
    LU_EVENT_BASE_FLAG_IGNORE_ENV = 0x02,

    /** 在事件循环准备运行超时回调时，检查当前时间。  
    设置此标志后，检查时间将发生在每个超时回调之后，而不是每次准备运行超时回调时。
    */
    LU_EVENT_BASE_FLAG_NO_CACHE_TIME = 0x08,
    

    /** 如果使用 epoll 后端，该标志表示可以安全地使用 Libevent 的内部变更列表代码来批量添加和删除事件，
        从而尽量减少系统调用的次数。设置此标志可以提高代码运行速度，但如果有任何文件描述符被 `dup()` 或其变体克隆，
        可能会触发 Linux bug，导致难以诊断的问题。

        如果最终使用的是其他后端，该标志将不起作用。
    */
    LU_EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST = 0x10,
    /**
     * 但如果设置此标志，
     * 我们将使用一个效率较低但更精确的定时器（假设系统有此定时器）。
     */
    LU_EVENT_BASE_FLAG_PRECISE_TIMER = 0x20,
    /** 如果启用了 `LU_EVENT_BASE_FLAG_PRECISE_TIMER`，则 epoll 后端将使用 `timerfd` 来获得更精确的定时器。
        此标志允许禁用此功能。

        这意味着该设置在没有启用 `EVENT_BASE_FLAG_PRECISE_TIMER` 的情况下类似于缺少精确定时器（CLOCK_MONOTONIC_COARSE），
        启用时则使用 `CLOCK_MONOTONIC` + `timerfd` 来实现更精确的定时。
        如果使用的是非 epoll 后端或者没有启用 `LU_EVENT_BASE_FLAG_EPOLL_DISALLOW_TIMERFD`，则此标志无效。
    */
    LU_EVENT_BASE_FLAG_EPOLL_DISALLOW_TIMERFD = 0x40,

    /** 使用 `signalfd(2)` 来处理信号，而不是使用 `sigaction` 或 `signal`。
    需要注意的是，在某些极端情况下，`signalfd()` 的工作方式可能与传统的信号处理机制有所不同。
    */
    LU_EVENT_BASE_FLAG_USE_SIGNALFD = 0x80,

}lu_event_base_config_flag_t;

//TODO:


typedef struct lu_event_changelist_s{
//TODO:
    lu_event_change_t *changes;
    int n_changes;
	int changes_size;
}lu_event_changelist_t;


typedef struct lu_evsig_info_s{
    //TODO:
    int summyl;
}lu_evsig_info_t;


typedef struct lu_common_timeout_list_s {
	//TODO:
    int paser;
}lu_common_timeout_list_t;



typedef struct lu_event_io_map_s {
    //TODO:
    int summyl;
}lu_event_io_map_t;

typedef struct lu_event_signal_map_s{
    //TODO:
    int summyl;
}lu_event_signal_map_t;

typedef struct min_heap {
    //TODO:
    int paser;
}min_heap_t;

typedef struct evutil_weakrand_state_s{
    //TODO:
    int summy;
} evutil_weakrand_state_t;

typedef struct evwatch_list_s{
    //TODO:
    int summy;
}evwatch_list_t;


 


typedef struct lu_event_callback_s{
  
    TAILQ_ENTRY(lu_event_callback_s) evcb_active_next;
    short evcb_events;
    
    //Smaller numbers are higher priority.
    lu_uint8_t evcb_pri;//优先级
    lu_uint8_t evcb_closure;//闭包

    /**Allows us to adopt for different types of events*/
    union 
    {
        //used for io events
        void (*evcb_callback)(lu_evutil_socket_t,short,void*);//回调函数
        void (*evcb_selfcb)(struct lu_event_callback_s*,void*);//自身回调函数
        void (*evcb_evfinialize)(struct lu_event_s*,void*);//事件最终化回调函数
        void (*evcb_cbfinalize)(struct lu_event_callback_s*,void*);//回调最终化回调函数
    }evcb_cb_union;
    void *evcb_arg;//回调函数的参数

}lu_event_callback_t;



typedef struct lu_event_s{
    lu_event_callback_t ev_callback_;
    union 
    {
        TAILQ_ENTRY(lu_event_s) ev_next_with_common_timeout;
        lu_size_t min_heap_idx;
    }ev_timeout_pos;
    short ev_events;
    short ev_res;//result passed to event callback

    lu_event_base_t* ev_base;

    union {
        //used for io events
        struct{
            LIST_ENTRY(lu_event_s) ev_io_next;
            struct timeval ev_timeout;
        }ev_io;
        //used for signal events
        struct{
            LIST_ENTRY(lu_event_s) ev_signal_next;
            short ev_signum;
            //Allow deletes in signal callback
            short* ev_pncalls; 
        }ev_signal;
    }ev_;

    struct timeval ev_timeout;
    

}lu_event_t;


#define EVWATCH_MAX     2
typedef struct lu_event_base_s {
    
    /** Function pointers and other data to describe this event_base's
	 * backend. */
    const struct lu_event_op_s* evsel_op;  
    void* evbase;

    //List of changes to tell backend about next dispatch.Only used bt O(1) backends.
    lu_event_changelist_t changelist;
    /** Function pointers used to describe the backend that this event_base
	 * uses for signals */
    const struct lu_event_op_s* evsigsel_op;

    /**Data to implement the common signal handler code */
    lu_evsig_info_t evsig;

    int virtual_event_count; //Number of virtual events in this event_base.
    int virtual_event_count_max; // Maximum number of virtual events active
    int event_count;//Number of total events added to this event_base.
    int event_count_max;//Maximum number of events added to this event_base.
    int event_count_active;//Number of active events in this event_base.
    int event_count_active_max;//Maximum number of active events in this event_base.

    /**Set if we should terminate the loop once we're done processing events.*/
    int event_gotterm;//Set if we should terminate the loop once we're done processing events.

    /**Set if we should terminate the loop immediately. */
    int event_break ;
    /**Set if we should start a new instance of the loop immediately. */
    int event_continue;

    /**The currently running priority of events. */
    int event_running_priority;

    /** Set if we're running the event_base_loop function, to prevent
	 * reentrant invocation. */    
	int running_loop;

    /** Set to the number of deferred_cbs we've made 'active' in the
	 * loop.  This is a hack to prevent starvation; it would be smarter
	 * to just use event_config_set_max_dispatch_interval's max_callbacks
	 * feature */
    //用于标记已deferred_cbs的数量
    int n_deferred_queued;

    struct lu_evcallback_list* active_queues;
    /**Common timeout logic */
    struct common_timeout_list** common_timeout_queues;
    /** The number of entries used in common_timeout_queues */
	int n_common_timeouts;
   /** The total size of common_timeout_queues. */
	int n_common_timeouts_allocated;

    /**Mapping from file descriptor to enabled(added)   events */
    lu_event_io_map_t io;
    /**Mapping from signal number to enabled(added) events */
    lu_event_signal_map_t signal;


    /** Priority queue of events with timeouts. */
	struct min_heap timeheap;
    /** Stored timeval: used to avoid calling gettimeofday/clock_gettime
	 * too often. */
	struct timeval tv_cache;

    lu_evutil_monotonic_timer_t monotonic_timer;


    /** Difference between internal time (maybe from clock_gettime) and
	 * gettimeofday. */
	struct timeval tv_clock_diff;
	/** Second in which we last updated tv_clock_diff, in monotonic time. */
	time_t last_updated_clock_diff;

	


    lu_event_callback_t *current_event;
    
    /** Flags that this base was configured with */
	lu_event_base_config_flag_t flags;

    struct timeval max_dispatch_time;
    int max_dispatch_callbacks;
    int limit_callbacks_after_priority;
    /* Notify main thread to wake up break, etc. */
	/** True if the base already has a pending notify, and we don't need
	 * to add any more. */
	int is_notify_pending;
	/** A socketpair used by some th_notify functions to wake up the main
	 * thread. */
    lu_evutil_socket_t th_notify_fd[2];
   
    lu_event_t th_notify;

    //----
    /** A function used to wake up the main thread from another thread. */
	int (*th_notify_fn)(struct lu_event_base_s *base);

	/** Saved seed for weak random number generator. Some backends use
	 * this to produce fairness among sockets. Protected by th_base_lock. */
	struct evutil_weakrand_state_s weakrand_seed;

	/** List of event_onces that have not yet fired. */
	LIST_HEAD(once_event_list, event_once) once_events;

	/** "Prepare" and "check" watchers. */
	struct evwatch_list_s watchers[EVWATCH_MAX];

} lu_event_base_t;

/**
   A flag used to describe which features an event_base (must) provide.

   Because of OS limitations, not every luevent backend supports every
   possible feature.  You can use this type with
   event_config_require_features() to tell luevent to only proceed if your
   event_base implements a given feature, and you can receive this type from
   event_base_get_features() to see which features are available.
*/
typedef enum lu_event_method_feature_u {
   
    LU_EVENT_FEATURE_ET = 0x01,
    LU_EVENT_FEATURE_O1 = 0x02,
    LU_EVENT_FEATURE_FDS = 0x04,
    LU_EVENT_FEATURE_EARLY_CLOSE = 0x08,
}lu_event_method_feature_t;

typedef struct lu_event_op_s {
    const char* name;
    /**
     *  Function to set up an lu_event_base_t to use this backend.It should 
     *  create a new lu_event_base_t and return it.On failture,this function should return NULL
     */
    void* (*init)(lu_event_base_t*);  
    int* (*add)(lu_event_base_t*, lu_evutil_socket_t fd,short old,short events, void* fdinfo);
     /** 类似于'add'函数，但'events'参数表示我们要禁用的事件类型。 */
    int (*del)( lu_event_base_t *, lu_evutil_socket_t fd, short old, short events, void *fdinfo);

    /** 实现事件循环的核心功能。它需要检查哪些已添加的事件已准备就绪，并为每个活动事件
     * 调用event_active函数（通常通过event_io_active等方式）。该函数应在成功时返回0，
     * 在错误时返回-1。 */
    int (*dispatch)( lu_event_base_t *, struct timeval *);

    /** 用于清理和释放事件基础结构中的数据的函数。 */
    void (*dealloc)( lu_event_base_t *);

    /** 标志：如果我们在fork之后需要重新初始化事件基础结构，则设置此标志。 */
    int need_reinit;
    /** 支持的事件方法特性的位数组。 */
    lu_event_method_feature_t features;
    /** 每个具有一个或多个活动事件的文件描述符应记录的额外信息的长度。
     * 此信息作为每个文件描述符的evmap条目的一部分记录，并作为参数传递给上述的
     * 'add'和'del'函数。 */
    size_t fdinfo_len;
} lu_event_op_t;


typedef struct lu_event_config_entry_s {
    TAILQ_ENTRY(lu_event_config_entry_s) next;
    //aviod method
    const char *avoid_method;
} lu_event_config_entry_t;


/** Internal structure: describes the configuration we want for an event_base
 * that we're about to allocate. */
typedef struct lu_event_config_s {
    //定义一个双向链表的头部。它将被用作存储lu_event_config_entry_s类型的结构体的链表
    TAILQ_HEAD(lu_event_config_q, lu_event_config_entry_s) entries;
    int n_cpus_hint; //指定CPU的数量。

    //指定最大分派间隔的时间值。
	struct timeval max_dispatch_interval;
    
    //指定在一次循环中最大的分派回调数量。
    int max_dispatch_callbacks;
    
    //用于限制在特定优先级之后的回调数量。Use to limit the number of callbacks after a specific priority.
	int limit_callbacks_after_priority;

    lu_event_method_feature_t required_features; //指定所需的事件方法特性。

     //指定事件基础配置的标志
    lu_event_base_config_flag_t flags;

}lu_event_config_t;



#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com


 