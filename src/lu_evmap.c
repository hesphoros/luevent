#include "lu_evmap.h"

#include "lu_util.h"
#include "lu_event-internal.h"

static int
lu_evmap_signal_delete_all_iter_fn(lu_event_base_t *base, int signum,
    lu_evmap_signal_t *sig_info, void *arg);
static int
lu_evmap_io_delete_all_iter_fn( lu_event_base_t *base, lu_evutil_socket_t fd,
    lu_evmap_io_t *io_info, void *arg);

void lu_evmap_io_initmap(lu_event_io_map_t* ctx){
    ctx->nentries = 0;
    ctx->entries = NULL;
}

void lu_evmap_siganl_initmap(lu_event_signal_map_t* ctx){
    ctx->nentries = 0;
    ctx->entries = NULL;
}

// void lu_event_changelist_init(lu_event_changelist_t* ctx){
//     ctx->changes = NULL;
//     ctx->n_changes = 0;
//     ctx->changes_size = 0;
// }




/*
*   于遍历 event_base 中所有可能的信号事件，执行用户提供的回调函数。
*/
static int lu_evmap_signal_foreach_signal(lu_event_base_t *base,
    lu_evmap_signal_foreach_signal_cb fn,
    void *arg)
{
	lu_event_signal_map_t *sigmap = &base->sigmap;
	int r = 0;
	int signum;

	for (signum = 0; signum < sigmap->nentries; ++signum) {
		lu_evmap_signal_t *ctx = sigmap->entries[signum];
		if (!ctx)
			continue;
		if ((r = fn(base, signum, ctx, arg)))
			break;
	}
	return r;
}

/* Helper for evmap_delete_all_: delete every event pending on a signal. */
static int
lu_evmap_signal_delete_all_iter_fn(lu_event_base_t *base, int signum,
    lu_evmap_signal_t *sig_info, void *arg)
{
	return lu_event_delete_all_in_dlist(&sig_info->events);
}


/* Helper for evmap_delete_all_: delete every event pending on an fd. */
static int
lu_evmap_io_delete_all_iter_fn( lu_event_base_t *base, lu_evutil_socket_t fd,
    lu_evmap_io_t *io_info, void *arg)
{
	return lu_event_delete_all_in_dlist(&io_info->events);
}


void lu_evmap_delete_all_(lu_event_base_t *base){
    lu_evmap_signal_foreach_signal(base,lu_evmap_signal_delete_all_iter_fn,NULL);
    lu_evmap_io_foreach_fd(base, lu_evmap_io_delete_all_iter_fn, NULL);
}