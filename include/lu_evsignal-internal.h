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

    // 这是一个指向 sigaction 结构体指针的指针，用于保存信号处理程序的旧设置。
    // sigaction 结构体用于描述信号处理的行为。例如，指定信号发生时应该执行的处理函数。
    // 该字段用于保存信号处理程序被安装之前的原始信号处理设置，便于之后恢复
    struct sigaction **sh_old;
    /* Size of sh_old. */
	int sh_old_max;
    lu_event_t *ev_sigevent;

}lu_evsig_info_t;





int sigfd_init_(lu_event_base_t *base);

int lu_evsig_init_(lu_event_base_t *base);


#ifdef __cplusplus
}
#endif


#endif /* _LU_EVENT_SIGNAL_INTERNAL_H_ */