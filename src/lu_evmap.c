#include "lu_evmap.h"

#include "lu_util.h"
#include "lu_event-internal.h"
#include "lu_event.h"
#include <sys/queue.h>
#include <signal.h>
#include <sys/signal.h> // 某些平台需要额外包含

static int
lu_evmap_signal_delete_all_iter_fn(lu_event_base_t *base, int signum,
    lu_evmap_signal_t *sig_info, void *arg);
static int
lu_evmap_io_delete_all_iter_fn( lu_event_base_t *base, lu_evutil_socket_t fd,
    lu_evmap_io_t *io_info, void *arg);

void lu_evmap_signal_clear_(lu_event_signal_map_t *ctx);

static void lu_evmap_signal_init(lu_evmap_signal_t *entry);

static int
	lu_event_delete_all_in_dlist(struct lu_event_dlist *dlist);
static int
	lu_evmap_make_space(lu_event_signal_map_t *map, int slot, int msize);

typedef int (*lu_evmap_io_foreach_fd_cb)(
	lu_event_base_t *, lu_evutil_socket_t, lu_evmap_io_t *, void *);

void lu_evmap_io_init( lu_evmap_io_t *entry);

// 函数声明：初始化事件I/O映射表
void lu_evmap_io_initmap(lu_event_io_map_t* ctx){
    // 将映射表中的条目数量初始化为0
    ctx->nentries = 0;
    // 将映射表中的条目指针初始化为NULL，表示当前没有条目
    ctx->entries = NULL;
}


void lu_evmap_io_init( lu_evmap_io_t *entry){

	LIST_INIT(&entry->events);
	entry->nread = 0;
	entry->nwrite = 0;
	entry->nclose = 0;
}

// 函数声明：初始化事件信号映射表
void lu_evmap_siganl_initmap(lu_event_signal_map_t* ctx){
    // 设置映射表中的条目数量为0，表示当前没有条目
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


/* Helper for evmap_delete_all_: delete every event in an event_dlist. */
static int
lu_event_delete_all_in_dlist(struct lu_event_dlist *dlist)
{
	lu_event_t *ev;
	while ((ev = LIST_FIRST(dlist)))
		lu_event_del(ev);
	return 0;
}

/* Helper for evmap_delete_all_: delete every event pending on a signal. */
static int
lu_evmap_signal_delete_all_iter_fn(lu_event_base_t *base, int signum,
    lu_evmap_signal_t *sig_info, void *arg)
{
	LU_UNUSED(signum);
	LU_UNUSED(sig_info);
	LU_UNUSED(arg);
	LU_UNUSED(base);
	return lu_event_delete_all_in_dlist(&sig_info->events);
}


/* Helper for evmap_delete_all_: delete every event pending on an fd. */
static int
lu_evmap_io_delete_all_iter_fn( lu_event_base_t *base, lu_evutil_socket_t fd,
    lu_evmap_io_t *io_info, void *arg)
{

	LU_UNUSED(fd);
	LU_UNUSED(arg);
	LU_UNUSED(base);
	return lu_event_delete_all_in_dlist(&io_info->events);
}


static int
lu_evmap_io_foreach_fd(lu_event_base_t *base,
    lu_evmap_io_foreach_fd_cb fn,
    void *arg)
{
	lu_evutil_socket_t fd;
	lu_event_io_map_t *iomap = &base->io;
	int r = 0;
// #ifdef LU_EVMAP_USE_HT
// 	struct event_map_entry **mapent;
// 	HT_FOREACH(mapent, event_io_map, iomap) {
// 		struct evmap_io *ctx = &(*mapent)->ent.evmap_io;
// 		fd = (*mapent)->fd;
// #else
	for (fd = 0; fd < iomap->nentries; ++fd) {
		lu_evmap_io_t *ctx = iomap->entries[fd];
		if (!ctx)
			continue;
// #endif
		if ((r = fn(base, fd, ctx, arg)))
			break;
	}
	return r;
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

	GET_IO_SLOT(ctx, io, fd, lu_evmap_io_s);

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
	struct lu_event_signal_map_s *map = &base->sigmap;
	lu_evmap_signal_t *ctx;

	if (sig < 0 || sig >= map->nentries)
		return (-1);

	GET_SIGNAL_SLOT(ctx, map, sig, lu_evmap_signal_s);

	LIST_REMOVE(ev, ev_signal_next);

	if (LIST_FIRST(&ctx->events) == NULL) {
		if (evsel->del(base, ev->ev_fd, 0, LU_EV_SIGNAL, NULL) == -1)
			return (-1);
	}

	return (1);
}



/* code specific to signals */

static void lu_evmap_signal_init(lu_evmap_signal_t *entry)
{
	LIST_INIT(&entry->events);
}




/** Expand 'map' with new entries of width 'msize' until it is big enough
	to store a value in 'slot'.
 */
static int
	lu_evmap_make_space(lu_event_signal_map_t *map, int slot, int msize)
{
	if (map->nentries <= slot) {
		int nentries = map->nentries ? map->nentries : 32;
		void **tmp;

		if (slot > INT_MAX / 2)
			return (-1);

		while (nentries <= slot)
			nentries <<= 1;

		if (nentries > INT_MAX / msize)
			return (-1);

		tmp = (void **)mm_realloc(map->entries, nentries * msize);
		if (tmp == NULL)
			return (-1);

		mm_memzero(&tmp[map->nentries],
		    (nentries - map->nentries) * msize);

		map->nentries = nentries;
		map->entries = tmp;
	}

	return (0);
}



int lu_evmap_signal_add_(lu_event_base_t *base, int sig, lu_event_t *ev){
	const lu_event_op_t *evsel = base->evsigsel_op;
	lu_event_signal_map_t *map = &base->sigmap;
	lu_evmap_signal_t *ctx = NULL;

	if (sig < 0 || sig >= NSIG)
		return (-1);

	if (sig >= map->nentries) {
		if (lu_evmap_make_space(
			map, sig, sizeof(struct evmap_signal *)) == -1)
			return (-1);
	}
	GET_SIGNAL_SLOT_AND_CTOR(ctx, map, sig, lu_evmap_signal_s, lu_evmap_signal_init,
	    base->evsigsel_op->fdinfo_len);

	if (LIST_EMPTY(&ctx->events)) {
		if (evsel->add(base, ev->ev_fd, 0, LU_EV_SIGNAL, ev)
		    == -1)
			return (-1);
	}

	LIST_INSERT_HEAD(&ctx->events, ev, ev_signal_next);

	return (1);
}



int lu_evmap_io_add_(lu_event_base_t *base, lu_evutil_socket_t fd, lu_event_t *ev){
	const lu_event_op_t *evsel = base->evsel_op;
	lu_event_io_map_t *io = &base->io;
	lu_evmap_io_t *ctx = NULL;
	int nread, nwrite, nclose, retval = 0;
	short res = 0, old = 0;
	lu_event_t *old_ev;

	LU_EVUTIL_ASSERT(fd == ev->ev_fd);

	if (fd < 0)
		return 0;

	if (fd >= io->nentries) {
		if (lu_evmap_make_space(io, fd, sizeof(struct evmap_io *)) == -1)
			return (-1);
	}

	GET_IO_SLOT_AND_CTOR(ctx, io, fd, lu_evmap_io_s, lu_evmap_io_init,
						 evsel->fdinfo_len);

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
		if (++nread == 1)
			res |= LU_EV_READ;
	}
	if (ev->ev_events & LU_EV_WRITE) {
		if (++nwrite == 1)
			res |= LU_EV_WRITE;
	}
	if (ev->ev_events & LU_EV_CLOSED) {
		if (++nclose == 1)
			res |= LU_EV_CLOSED;
	}
	if (LU_EVUTIL_UNLIKELY(nread > 0xffff || nwrite > 0xffff || nclose > 0xffff)) {
		LU_EVENT_LOG_WARNX("Too many events reading or writing on fd %d",(int)fd);
		return -1;
	}
	if (LU_EVENT_DEBUG_MODE_IS_ON() &&
	    (old_ev = LIST_FIRST(&ctx->events)) &&
		(old_ev->ev_events&LU_EV_ET) != (ev->ev_events&LU_EV_ET)) {
		LU_EVENT_LOG_WARNX("Tried to mix edge-triggered and non-edge-triggered"" events on fd %d", (int)fd);
		return -1;
	}

	if (res) {
		void *extra = ((char*)ctx) + sizeof( lu_evmap_io_t);
		/* XXX(niels): we cannot mix edge-triggered and
		 * level-triggered, we should probably assert on
		 * this. */
		if (evsel->add(base, ev->ev_fd,
			old, (ev->ev_events & LU_EV_ET) | res, extra) == -1)
			return (-1);
		retval = 1;
	}

	ctx->nread = (lu_uint16_t) nread;
	ctx->nwrite = (lu_uint16_t) nwrite;
	ctx->nclose = (lu_uint16_t) nclose;
	LIST_INSERT_HEAD(&ctx->events, ev, ev_io_next);

	return (retval);
}


