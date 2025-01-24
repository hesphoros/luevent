#ifndef _LU_EVTHREAD_INTERNAL_INCLUDED_H_
#define _LU_EVTHREAD_INTERNAL_INCLUDED_H_

#include "lu_util.h"
struct lu_event_base_t;

//TODO: 实现多线程下的断言锁机制

/** If lock debugging is enabled, and lock is non-null, assert that 'lock' is
 * locked and held by us. */
#define EVLOCK_ASSERT_LOCKED(lock)					\
	do {								\
		if ((lock) && evthreadimpl_is_lock_debugging_enabled_()) { \
			LU_EVUTIL_ASSERT(evthread_is_debug_lock_held_(lock)); \
		}							\
	} while (0)

#endif /* _LU_EVTHREAD_INTERNAL_INCLUDED_H_ */