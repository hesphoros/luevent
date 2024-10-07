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

struct event_base{
    const struct lu_event_op*   evsel;
    void*                       ev_base;
    struct event_changelist     changelist;
    const struct lu_event_op*   evsigsel;               //用于处理信号   
    struct ev_sig_info*         sigev;                  //用于实现信号处理通用代码的数据。

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
};


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com
