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
//TODO : FINISH THE IMPLEMENTATION OF THREAD SUPPORT
LU_EVENT_EXPORT_SYMBOL
	extern lu_evthread_lock_callback_t evthread_lock_fns_;
LU_EVENT_EXPORT_SYMBOL
    extern lu_evthread_condition_callbacks_t evthread_condition_fns_;
extern unsigned long (*evthread_id_fn_) (void);
extern int lu_evthread_lock_debugging_enabled_;//是否开启了debug调试

/**Return the ID of the current thread, or 1 if threading is not enabled. */
#define LU_EVTHREAD_GET_ID()\
	(evthread_id_fn_? evthread_id_fn_() : 1)

/** 当且仅当我们正在运行给定 event_base 循环的线程中时，才返回 true。需要锁定。*/
#define LU_EVBASE_IN_THREAD(evbase) \
	(evthread_id_fn_ == NULL|| \
	(evbase)->th_owner_id == evthread_id_fn_())

/** 当且仅当我们需要通知基类的主线程其状态发生变化时才返回 true，因为它当前正在另一个线程中运行主循环。需要锁定。*/
#define LU_EVBASE_NEED_NOTIFY(evbase) \
	(evthread_id_fn_ != NULL && 	\
	(evbase)->running_loop && \
	(evbase)->th_owner_id != evthread_id_fn_())

/** 分配一个新锁，并将其存储在 lockvar（void*）中。如果未启用锁定，则将 lockvar 设置为 NULL。*/
#define LU_EVTHREAD_ALLOC_LOCK(lockvar,locktype) \
	((lockvar) = evthread_lock_fns_.alloc ? \
	evthread_lock_fns_.alloc(locktype) : NULL)


/** 如果存在并且启用了锁定，则释放给定的锁。 */
#define LU_EVTHREAD_FREE_LOCK(lockvar,locktype) \
	do{											\
		void* lock_tmp_ = (lockvar);			\
		if(lock_tmp_ && evthread_lock_fns_.free) {	\
			evthread_lock_fns_.free(lock_tmp_,(locktype));	\
		}											\
	}while(0)


/*Acquire a lock */
#define LU_EVLOCK_LOCK(lockvar,mode)				\
	do {											\
		if(lockvar) {								\
			evthread_lock_fns_.lock(mode,lockvar);	\
		}											\
	} while(0)

/*Release a lock */
#define LU_EVLOCK_UNLOCK(lockvar,mode)				\
	do {											\
		if(lockvar) {								\
			evthread_lock_fns_.unlock(mode,lockvar);	\
		}											\
	} while(0)


/** Helper function: lockvar1 和 lockvar2 按指针升序排列。 */
#define LU_EVLOCK_SORTLOCKS_(lockvar1,lockvar2) \
	do { \
		if(lockvar1 && lockvar2 && lockvar1 > lockvar2){		\
			void* tmp_ = (void*)lockvar1; 						\
			lockvar1 = lockvar2; 		\
			lockvar2 = (void*)tmp_; 	\
		}										\
	}while(0)


/** 如果已设置锁定，则锁定 event_base。获取字段名为“lockvar”的基础结构中的锁。*/
#define LU_EVBASE_ACQUIRE_LOCK(evbase,lockvar)			\
	do {													\
		LU_EVLOCK_LOCK((evbase)->lockvar,0);							\
	} while(0)

/** 如果设置了锁定，则解锁 event_base。 */
#define LU_EVBASE_RELEASE_LOCK(evbase,lockvar)			\
	do {													\
		LU_EVLOCK_UNLOCK((evbase)->lockvar,0);						\
	} while(0)
//TODO:finish the implementation of thread support
///** 如果启用了锁调试，并且锁不为空，则断言“锁”已被 锁定并由我们持有。
#define LU_EVLOCK_ASSERT_LOCKED(lock)					\
	do {												\
		if(lock && lu_evthread_lock_debugging_enabled_){	\
			LU_EVUTIL_ASSERT(lu_evthread_is_debug_lock_held_(lock));					\
	}while(0)

#endif /**!define(LU_EVENT__DISABLE_THREAD_SUPPORT) */




/** If lock debugging is enabled, and lock is non-null, assert that 'lock' is
 * locked and held by us. */
/*
#define LU_EVLOCK_ASSERT_LOCKED(lock)					\
	do {								\
		if ((lock) && evthreadimpl_is_lock_debugging_enabled_()) { \
			LU_EVUTIL_ASSERT(evthread_is_debug_lock_held_(lock)); \
		}							\
	} while (0)
*/


#ifdef __cplusplus
}
#endif


#endif /* _LU_EVTHREAD_INTERNAL_INCLUDED_H_ */
