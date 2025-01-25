#ifndef _LU_EVTHREAD_INTERNAL_INCLUDED_H_
#define _LU_EVTHREAD_INTERNAL_INCLUDED_H_

#include "lu_util.h"
#include "lu_visibility.h"
#include "lu_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lu_event_base_s lu_event_base_t;

#if ! defined(LU_EVENT__DISABLE_THREAD_SUPPORT)

LU_EVENT_EXPORT_SYMBOL
	extern lu_evthread_lock_callback_t evthread_lock_fns_;
LU_EVENT_EXPORT_SYMBOL
    extern lu_evthread_condition_callbacks_t evthread_condition_fns_;
extern unsigned long (*evthread_id_fn_) (void);
extern int evthread_lock_debugging_enabled_;

/**Return the ID of the current thread, or 1 if threading is not enabled. */
#define LU_EVTHREAD_GET_ID()\
	(evthread_id_fn_? evthread_id_fn_() : 1)


#endif /**!define(LU_EVENT__DISABLE_THREAD_SUPPORT) */




/** If lock debugging is enabled, and lock is non-null, assert that 'lock' is
 * locked and held by us. */
#define EVLOCK_ASSERT_LOCKED(lock)					\
	do {								\
		if ((lock) && evthreadimpl_is_lock_debugging_enabled_()) { \
			LU_EVUTIL_ASSERT(evthread_is_debug_lock_held_(lock)); \
		}							\
	} while (0)


#ifdef __cplusplus
}
#endif


#endif /* _LU_EVTHREAD_INTERNAL_INCLUDED_H_ */
