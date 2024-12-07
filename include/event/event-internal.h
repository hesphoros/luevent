#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <time.h>
#include <sys/queue.h>
#include "util.h"

#define PASER (int paser)

struct event_op{
    int paser;
};



struct event_base{
    //用于描述这个event_base端
    const struct event_op *  evsel;

    void*                   _evbase; 
};




#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/ //com


 