#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <time.h>
#include <sys/queue.h>

#include "util.h"

enum event_method_feature{}; //事件方法特性


struct event_op{
    const char* name;//后端名称

    /** 初始化函数，用于设置事件基础结构以使用该后端。它应该创建一个新的结构体，
     * 保存运行该后端所需的任何信息，并将其返回。返回的指针将由event_init存储在
     * event_base.evbase字段中。如果初始化失败，该函数应返回NULL。 */
    void *(*init)(struct event_base*);


    /** 启用给定文件描述符或信号的读写事件。'events'参数表示我们要启用的事件类型，
     * 可能是EV_READ、EV_WRITE、EV_SIGNAL和EV_ET的组合。'old'参数表示之前在该
     * 文件描述符上启用的事件。'fdinfo'参数是与文件描述符相关联的结构体，在evmap
     * 中管理；其大小由下面的fdinfo_len字段定义。第一次添加文件描述符时，
     * 它将被设置为0。该函数应在成功时返回0，在错误时返回-1。 */
    int (*add)(struct event_base *, evutil_socket_t fd, short old, short events, void *fd_info);
   
    /** 类似于'add'函数，但'events'参数表示我们要禁用的事件类型。 */
    int (*del)(struct event_base *, evutil_socket_t fd, short old, short events, void *fdinfo);


    /** 实现事件循环的核心功能。它需要检查哪些已添加的事件已准备就绪，并为每个活动事件
     * 调用event_active函数（通常通过event_io_active等方式）。该函数应在成功时返回0，
     * 在错误时返回-1。 */
    int (*dispatch)(struct event_base *, struct timeval *);

    /** 用于清理和释放事件基础结构中的数据的函数。 */
    void (*dealloc)(struct event_base *);

    /** 标志：如果我们在fork之后需要重新初始化事件基础结构，则设置此标志。 */
    int need_reinit;

    /** 支持的事件方法特性的位数组。 */
    enum event_method_feature features;

    /** 每个具有一个或多个活动事件的文件描述符应记录的额外信息的长度。
     * 此信息作为每个文件描述符的evmap条目的一部分记录，并作为参数传递给上述的
     * 'add'和'del'函数。 */
    size_t fdinfo_len;


};

struct event{
    int paser;
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

enum  event_base_config_flag{

};

struct evutil_weakrand_state{
    int paser;      

};



struct event_config{
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

    /** Second in which we last updated tv_clock_diff, in monotonic time. */
	time_t last_updated_clock_diff;             //用于存储上次更新tv_clock_diff时的单调时间。

#ifndef EVENT__DISABLE_THREAD_SUPPORT    
    /* threading support */
    /** The thread currently running the event_loop for this base */
	unsigned long th_owner_id;
	/** A lock to prevent conflicting accesses to this event_base */
	void *th_base_lock;
	/** A condition that gets signalled when we're done processing an
	 * event with waiters on it. */
	void *current_event_cond;   
	/** Number of threads blocking on current_event_cond. */
	int current_event_waiters;
#endif
    /** The event whose callback is executing right now */
	struct event_callback *current_event;
#ifdef _WIN32
	/** IOCP support structure, if IOCP is enabled. */
    /**：一个指向`event_iocp_port`结构体的指针，如果在Windows平台上使用IOCP（I/O Completion Port）作为事件驱动后端，则使用此字段。
*/
	struct event_iocp_port *iocp;
#endif    

	/** Flags that this base was configured with */
    //`flags`：一个枚举类型，用于表示`event_base`的配置标志。
    
	enum event_base_config_flag flags;
	
    //用于表示最大调度时间。
	struct timeval max_dispatch_time;

    int max_dispatch_callbacks;//用于表示最大调度回调数量。
	int limit_callbacks_after_prio;//用于表示在达到特定优先级后限制回调数量。

	/* Notify main thread to wake up break, etc. */
	/** True if the base already has a pending notify, and we don't need
	 * to add any more. */
	int is_notify_pending;//用于表示`event_base`是否已经有一个待处理的通知。
	/** A socketpair used by some th_notify functions to wake up the main
	 * thread. */
    
    //用于free base
	evutil_socket_t th_notify_fd[2];//一个套接字对，用于在另一个线程中唤醒主线程。
	/** An event used by some th_notify functions to wake up the main
	 * thread. */
    
	struct event th_notify;//用于在主线程中唤醒其他线程。

	/** A function used to wake up the main thread from another thread. */
    /**`th_notify`：一个`event`结构体，用于在主线程中唤醒其他线程。*/
	int (*th_notify_fn)(struct event_base *base);

	/** Saved seed for weak random number generator. Some backends use
	 * this to produce fairness among sockets. Protected by th_base_lock. */
    //一个`evutil_weakrand_state`结构体，用于存储弱随机数生成器的种子。
	struct evutil_weakrand_state weakrand_seed;

	/** List of event_onces that have not yet fired. */
    /**`once_events`：一个`LIST_HEAD`结构体，用于存储尚未触发的事件。*/
	LIST_HEAD(once_event_list, event_once) once_events;
    
};


//some changes make by lu
#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com


 