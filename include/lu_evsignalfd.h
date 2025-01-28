#ifndef LU_EVSIGNALFD_H
#define LU_EVSIGNALFD_H

#include <signal.h>
#include <sys/signalfd.h>
#include "lu_util.h"
#include <unistd.h>
#include <lu_event-internal.h>
#include <lu_event_struct.h>




#ifdef __cplusplus
extern "C" {
#endif

static int sigfd_add(lu_event_base_t *, lu_evutil_socket_t, short, short, void *);
static int sigfd_del(lu_event_base_t *, lu_evutil_socket_t, short, short, void *);




static const  lu_event_op_t sigfdops = {
	"signalfd_signal",
	NULL,
	sigfd_add,
	sigfd_del,
	NULL,
	NULL,
	(int)0, (int)0,(int) 0
};



#ifdef __cplusplus
}
#endif

#endif /* LU_EVSIGNALFD_H */