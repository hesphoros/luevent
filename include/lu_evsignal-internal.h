#ifndef _LU_EVENT_SIGNAL_INTERNAL_H_
#define _LU_EVENT_SIGNAL_INTERNAL_H_

#include <signal.h>
#include "lu_util.h"
#include "lu_event_struct.h"
#include <sys/signalfd.h>  // 用于检查是否支持 signalfd

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*ev_sighandler_t)(int);

typedef struct lu_evsig_info_s{
    /* Event watching ev_signal_pair[1] */
    lu_event_t ev_signal;
    /* Socketpair used to send notifications from the signal handler */
    lu_evutil_socket_t ev_signal_pair[2];
    /* 当且仅当我们已经添加了 ev_signal 事件，否则为真。 */
    int ev_signal_added;
    /* 当前正在观察的信号数量。 */
    int ev_n_signals_added;
    struct sigaction **sh_old;
    /* Size of sh_old. */
	int sh_old_max;
    lu_event_t *ev_sigevent;

}lu_evsig_info_t;



int sigfd_init_(lu_event_base_t *base);




#ifdef __cplusplus
}
#endif


#endif /* _LU_EVENT_SIGNAL_INTERNAL_H_ */