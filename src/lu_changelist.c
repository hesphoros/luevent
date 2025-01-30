#include "lu_changelist-internal.h"

void lu_event_changelist_init(lu_event_changelist_t* ctx){
    ctx->changes = NULL;
    ctx->n_changes = 0;
    ctx->changes_size = 0;
}

