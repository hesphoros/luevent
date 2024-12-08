#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <sys/queue.h>
#include "util.h"
#include <sys/time.h>
 



typedef struct lu_event_operate_s lu_event_operate_t;
//typedef struct lu_event_base_s    lu_event_base_t;


 
typedef struct lu_event_base_s {
    // 引用 lu_event_operate_t 类型的指针 
    const struct lu_event_operate_s* evsel_op;  
    // 操作lu_event_base_s
    void* evbase;
} lu_event_base_t;

 
typedef struct lu_event_operate_s {
    const char* name;

    /**
     * return : 返回一个新创建的结构体指针;返回的指针将由 event_init() 储存在 event_base_s.evbase 字段中；
     *          如果初始化失败则返回 NULL。
     */
    void* (*init)(lu_event_base_t*);  // 这里的类型已变为 lu_event_base_t*
} lu_event_operate_t;


typedef struct lu_event_config_entry_s {
    TAILQ_ENTRY(lu_event_config_entry_s) next;
    const char *avoid_method;
} lu_event_config_entry_t;


/** Internal structure: describes the configuration we want for an event_base
 * that we're about to allocate. */
typedef struct lu_event_config_s {
    //定义一个双向链表的头部。它将被用作存储lu_event_config_entry_s类型的结构体的链表
    TAILQ_HEAD(lu_event_config_q, lu_event_config_entry_s) entries;
    int n_cpus_hint;

    //指定最大分派间隔的时间值。
	struct timeval max_dispatch_interval;

}lu_event_config_t;
 




#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com


 