#include "lu_evmap.h"

#include "lu_util.h"


void lu_evmap_io_initmap(lu_event_io_map_t* ctx){
    ctx->nentries = 0;
    ctx->entries = NULL;
}

void lu_evmap_siganl_initmap(lu_event_signal_map_t* ctx){
    ctx->nentries = 0;
    ctx->entries = NULL;
}

void lu_event_changelist_init(lu_event_changelist_t* ctx){
    ctx->changes = NULL;
    ctx->n_changes = 0;
    ctx->changes_size = 0;
}