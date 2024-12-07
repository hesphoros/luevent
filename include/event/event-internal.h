#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <time.h>
#include <sys/queue.h>
#include "util.h"

 



typedef struct lu_event_operate_s lu_event_operate_t;
//typedef struct lu_event_base_s    lu_event_base_t;


// 定义 lu_event_base_s 结构体
typedef struct lu_event_base_s {
    // 引用 lu_event_operate_t 类型的指针
    const lu_event_operate_t* evsel_op;  
    // 操作lu_event_base_s
    void* evbase;
} lu_event_base_t;

// 定义 lu_event_operate_s 结构体
typedef struct lu_event_operate_s {
    const char* name;

    /**
     * 返回值：  返回一个新创建的结构体指针;返回的指针将由 event_init() 储存在 event_base_s.evbase 字段中；
     *          如果初始化失败则返回 NULL。
     */
    void* (*init)(lu_event_base_t*);  // 这里的类型已变为 lu_event_base_t*
} lu_event_operate_t;



/** Internal structure: describes the configuration we want for an event_base
 * that we're about to allocate. */
typedef struct lu_event_config_s {
    int flags;
}lu_event_config_t;
 




#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com


 