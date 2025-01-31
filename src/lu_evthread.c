#include "lu_evthread-internal.h"
#include "lu_util.h"



struct debug_lock {
	unsigned signature;
	unsigned locktype;
	unsigned long held_by;
	/* XXXX if we ever use read-write locks, we will need a separate
	 * lock to protect count. */
	int count;
	void *lock;
};

/* globals */
int lu_lu_evthread_lock_debugging_enabled_ = 0;
unsigned long (*evthread_id_fn_)(void) = NULL;

lu_evthread_lock_callback_t evthread_lock_fns_ = {
    0,0,NULL,NULL,NULL,NULL
};

lu_evthread_condition_callbacks_t evthread_condition_fns_ = {
	0, NULL, NULL, NULL, NULL
};


/* Used for debugging */
 lu_evthread_lock_callback_t original_lock_fns_ = {
	0, 0, NULL, NULL, NULL, NULL
};
lu_evthread_condition_callbacks_t original_cond_fns_ = {
	0, NULL, NULL, NULL, NULL
};


void
lu_evthread_set_id_callback(unsigned long (*id_fn)(void))
{
	evthread_id_fn_ = id_fn;
}


lu_evthread_lock_callback_t*evthread_get_lock_callbacks(void)
{
	return lu_evthread_lock_debugging_enabled_
	    ? &original_lock_fns_ : &evthread_lock_fns_;
}


int lu_evthread_is_debug_lock_held_(void *lock_)
{
	struct debug_lock *lock = lock_;
	if (! lock->count)
		return 0;
	if (evthread_id_fn_) {
		unsigned long me = evthread_id_fn_();
		if (lock->held_by != me)
			return 0;
	}
	return 1;
}