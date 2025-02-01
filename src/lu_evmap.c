#include "lu_evmap.h"

#include "lu_util.h"
#include "lu_event-internal.h"
#include "lu_event.h"

static int
lu_evmap_signal_delete_all_iter_fn(lu_event_base_t *base, int signum,
    lu_evmap_signal_t *sig_info, void *arg);
static int
lu_evmap_io_delete_all_iter_fn( lu_event_base_t *base, lu_evutil_socket_t fd,
    lu_evmap_io_t *io_info, void *arg);

void lu_evmap_signal_clear_(lu_event_signal_map_t *ctx);


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



void
lu_evmap_signal_clear_(lu_event_signal_map_t *ctx)
{
	if (ctx->entries != NULL) {
		int i;
		for (i = 0; i < ctx->nentries; ++i) {
			if (ctx->entries[i] != NULL)
				mm_free(ctx->entries[i]);
		}
		mm_free(ctx->entries);
		ctx->entries = NULL;
	}
	ctx->nentries = 0;
}

void lu_evmap_io_clear_(lu_event_io_map_t* ctx)
{
	lu_evmap_signal_clear_(ctx);
}



/* return -1 on error, 0 on success if nothing changed in the event backend,
 * and 1 on success if something did. */
int
lu_evmap_io_del_(lu_event_base_t *base, lu_evutil_socket_t fd, lu_event_t *ev)
{
	const lu_event_op_t *evsel = base->evsel_op;
	lu_event_io_map_t *io = &base->io;
	lu_evmap_io_t *ctx;
	int nread, nwrite, nclose, retval = 0;
	short res = 0, old = 0;

	if (fd < 0)
		return 0;

	LU_EVUTIL_ASSERT(fd == ev->ev_fd);

#ifndef LU_EVMAP_USE_HT
	if (fd >= io->nentries)
		return (-1);
#endif

	GET_IO_SLOT(ctx, io, fd, evmap_io);

	nread = ctx->nread;
	nwrite = ctx->nwrite;
	nclose = ctx->nclose;

	if (nread)
		old |= LU_EV_READ;
	if (nwrite)
		old |= LU_EV_WRITE;
	if (nclose)
		old |= LU_EV_CLOSED;

	if (ev->ev_events & LU_EV_READ) {
		if (--nread == 0)
			res |= LU_EV_READ;
		LU_EVUTIL_ASSERT(nread >= 0);
	}
	if (ev->ev_events & LU_EV_WRITE) {
		if (--nwrite == 0)
			res |= LU_EV_WRITE;
		LU_EVUTIL_ASSERT(nwrite >= 0);
	}
	if (ev->ev_events & LU_EV_CLOSED) {
		if (--nclose == 0)
			res |= LU_EV_CLOSED;
		LU_EVUTIL_ASSERT(nclose >= 0);
	}

	if (res) {
		void *extra = ((char*)ctx) + sizeof(lu_evmap_io_t);
		if (evsel->del(base, ev->ev_fd,
			old, (ev->ev_events & LU_EV_ET) | res, extra) == -1) {
			retval = -1;
		} else {
			retval = 1;
		}
	}

	ctx->nread = nread;
	ctx->nwrite = nwrite;
	ctx->nclose = nclose;
	LIST_REMOVE(ev, ev_io_next);

	return (retval);
}


int lu_evmap_signal_del_(lu_event_base_t *base, int sig, lu_event_t *ev){
	const lu_event_op_t *evsel = base->evsigsel_op;
	lu_event_signal_map_t *map = &base->sigmap;
	lu_evmap_signal_t *ctx;

	if (sig < 0 || sig >= map->nentries)
		return (-1);

	GET_SIGNAL_SLOT(ctx, map, sig, evmap_signal);

	LIST_REMOVE(ev, ev_signal_next);

	if (LIST_FIRST(&ctx->events) == NULL) {
		if (evsel->del(base, ev->ev_fd, 0, LU_EV_SIGNAL, NULL) == -1)
			return (-1);
	}

	return (1);
}