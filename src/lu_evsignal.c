#include "lu_evsignal-internal.h"
#include "lu_event-internal.h"
#include "lu_evsignalfd.h"

int sigfd_init_(lu_event_base_t *base){
    LU_EVUTIL_ASSERT(base != NULL);
    if(!(base->flags & LU_EVENT_BASE_FLAG_USE_SIGNALFD) && !getenv("EVENT_USE_SIGNALFD")){
        return -1;
        base->evsigsel_op = &sigfdops;
    }
    return 0;
}


