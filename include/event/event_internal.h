#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <time.h>
#include <sys/queue.h>

struct event_op{
    int Placeholder; //TODO
    // int (*evsel_init)(struct lu_event_base *base);
    // void (*evsel_cleanup)(struct lu_event_base *base);
    // int (*evsel_add)(struct lu_event_base *base, struct lu_event *ev);
    // int (*evsel_del)(struct lu_event_base *base, struct lu_event *ev);
};

struct event_changelist{
    int paser;
};

struct evcallback_list{
    int paser;
};

struct event_io_map{ 
    //TODO
    int paser;
};

struct event_signal_map{
    //TODO
    int paser;
};

struct min_heap       {
    //TODO
    int paser;
};

struct evutil_monotonic_timer{
  int paser;  
};

struct event_base{
    const struct lu_event_op*   evsel;                  //指向后端数据的指针，用于描述event_base端的数据
    void*                       ev_base;                //一个指向特定于后端数据的指针，用于指向底层事件驱动后端的实现。
    struct event_changelist     changelist;             //一个结构体，用于描述在下次调度时需要通知后端更改的事件
    const struct lu_event_op*   evsigsel;               //用于处理信号   
    struct       ev_sig_info*   sigev;                  //用于实现信号处理通用代码的数据。

/** Number of virtual events */
    int                         virtual_event_count;    //虚拟事件计数
    int                         virtual_event_max;      //虚拟事件最大值
    int                         event_count;            //事件计数 (已经添加到event_base中的事件)
    int                         event_max;              //事件最大值
	int                         event_count_active;     //用于表示当前活动事件数量。
	int                         event_count_active_max; //用于表示最大活动事件数量。
	int                         event_gotterm;          //用于表示是否应该在处理完事件后终止循环。
	int                         event_break;            //用于表示是否应该立即终止循环。    
	int                         event_continue;         //用于表示是否应该在处理完事件后继续执行循环。   
	int                         event_running_priority; //用于表示当前正在运行的事件优先级。    
	int                         running_loop;           //用于表示是否正在运行event_base_loop函数，以防止重入调用。

    
	/** Set to the number of deferred_cbs we've made 'active' in the
	 * loop.  This is a hack to prevent starvation; it would be smarter
	 * to just use event_config_set_max_dispatch_interval's max_callbacks
	 * feature */
	int                         n_deferreds_queued;     //用于表示已 deferred_cbs 数量
    struct evcallback_list *    activequeues;           //用于存储活动队列    
	int                         nactivequeues;          /** The length of the activequeues array */
    struct evcallback_list      active_later_queue;     //于存储应该在下次处理事件时激活的事件。


/* common timeout logic */    
	struct common_timeout_list** common_timeout_queues;  //用于存储所有已知的时间outs。
    int                          n_common_timeouts_allocated;   /** The total size of common_timeout_queues. */

    /** Mapping from file descriptors to enabled (added) events */
    struct event_io_map         io;

    /** Mapping from signal numbers to enabled (added) events. */
    
	struct event_signal_map     sigmap;

    /** Priority queue of events with timeouts. */
    //1个优先队列，用于存储具有超时的事件。
    struct min_heap             timeheap;

    /** Stored timeval: used to avoid calling gettimeofday/clock_gettime too often. */
    struct timeval              tv_cache;

    struct evutil_monotonic_timer monotonic_timer;//用于实现单调时钟
    /** Difference between internal time (maybe from clock_gettime) and  gettimeofday. */
    struct timeval              tv_clock_diff;
};


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com
