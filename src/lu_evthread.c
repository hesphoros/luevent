#include "lu_evthread-internal.h"
#include "lu_util.h"

/* globals */
static int lu_lu_evthread_lock_debugging_enabled_ = 0;
static unsigned long (*evthread_id_fn_)(void) = NULL;