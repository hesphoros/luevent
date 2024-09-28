#ifndef LU_EVENT_INTERNAL_H_INCLUDED_
#define LU_EVENT_INTERNAL_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


#include <time.h>
#include <sys/queue.h>

struct lu_event_op{
    int Placeholder; //TODO
    // int (*evsel_init)(struct lu_event_base *base);
    // void (*evsel_cleanup)(struct lu_event_base *base);
    // int (*evsel_add)(struct lu_event_base *base, struct lu_event *ev);
    // int (*evsel_del)(struct lu_event_base *base, struct lu_event *ev);
};


struct lu_event_base{
    const struct lu_event_op * evsel;
};


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*LU_EVENT_INTERNAL_H_INCLUDED_*/