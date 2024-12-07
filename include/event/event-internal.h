#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <time.h>
#include <sys/queue.h>
#include "util.h"

#define PASER int paser



typedef struct lu_event_base_s{
    const struct lu_event_operate_t* evsel_op;
    void*                            evbase;
}lu_event_base_t;


 
typedef struct lu_event_operate_s{
    // name
    const char*     name;

    /**
     * return : 返回一个新创建的结构体
     *          返回的指针将由event_init()储存在event_base_s.evbase字段中;
     *          如果初始化失败则返回NULL
     */
    void* (*init)(struct lu_event_base_s *);

}event_operate_t;




 




#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com


 