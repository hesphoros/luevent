#include "lu_changelist-internal.h"
#include "lu_memory_manager.h"

void lu_event_changelist_init(lu_event_changelist_t* ctx){
    ctx->changes = NULL;
    ctx->n_changes = 0;
    ctx->changes_size = 0;
}

void lu_event_changelist_freemem_(lu_event_changelist_t* changelist){
    if (changelist->changes)
		mm_free(changelist->changes);
	lu_event_changelist_init(changelist); /* zero it all out. */

}