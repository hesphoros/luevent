#include "lu_event-internal.h"
#include "lu_event_struct.h"
#include "lu_evthread-internal.h"
#include "lu_log-internal.h"
#include "lu_memory_manager.h"
#include "lu_min_heap.h"
#include "lu_changelist-internal.h"
#include "lu_event.h"

#include "lu_util.h"
#include "lu_evmap.h"
#include "lu_epoll.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <error.h>
#include <stdlib.h>

#define LU_EVENT_BASE_ASSERT_LOCKED(evbase)				\
		LU_EVLOCK_ASSERT_LOCKED((evbase)->th_base_lock)
int lu_event_callback_cancel_nolock_(lu_event_base_t *base,lu_event_callback_t *evcb, int even_if_finalizing);

static int lu_event_config_is_avoided_method(lu_event_config_t * cfg, const char *method_name) ;
static void lu_event_base_free_(lu_event_base_t * base, int run_finalizers);
static int lu_event_del_(lu_event_t  *ev, int blocking);
static int lu_event_is_method_disabled(const char *method_name);

int lu_event_del_nolock_(lu_event_t *ev, int blocking);
static inline
lu_event_t *lu_event_callback_to_event(lu_event_callback_t *evcb);

static void
lu_event_once_cb(lu_evutil_socket_t fd, short events, void *arg);

static void
lu_event_queue_remove_timeout(lu_event_base_t *base, lu_event_t *ev);

static inline lu_common_timeout_list_t *lu_get_common_timeout_list(lu_event_base_t *base, const struct timeval *tv);


static inline lu_event_callback_t *
	lu_event_to_event_callback(lu_event_t *ev);


static inline lu_event_t *
	lu_event_callback_to_event(lu_event_callback_t *evcb);

static void
	lu_event_queue_remove_inserted(lu_event_base_t *, lu_event_t *);


static int
	lu_evthread_notify_base(lu_event_base_t *base);

static int
	lu_event_haveevents(lu_event_base_t *);


static inline int
	lu_is_common_timeout(const struct timeval *tv,const lu_event_base_t *base);

static const lu_event_op_t* eventops[] = {
  &epool_ops,
 // &lu_poll_ops,
  //&lu_kqueue_ops,
  //&lu_select_ops,

};



/* Global state; deprecated */
LU_EVENT_EXPORT_SYMBOL
lu_event_base_t *event_global_current_base_ = NULL;
#define current_base event_global_current_base_
/* Global state */
static void *event_self_cbarg_ptr_ = NULL;

static void lu_event_config_entry_free(lu_event_config_entry_t * entry);

lu_event_config_t * lu_event_config_new(void)
{
   lu_event_config_t *ev_cfg_t = mm_calloc(1, sizeof(*ev_cfg_t));

    if (ev_cfg_t == NULL)
      return (NULL);

    TAILQ_INIT(&ev_cfg_t->entries);
    ev_cfg_t->max_dispatch_interval.tv_sec = -1;
    ev_cfg_t->max_dispatch_callbacks = INT_MAX;
    ev_cfg_t->limit_callbacks_after_priority =   1;
    ev_cfg_t->max_dispatch_interval.tv_usec     = -1;

    return (ev_cfg_t);
}

/// @brief 从事件基础结构 base 中获取当前时间，并将其存储在 timeval 结构体 tp 中
/// @param base
/// @param tp
/// @return
static int
gettime(lu_event_base_t *base, struct timeval *tp)
{
  //如果缓存时间有效，则直接返回缓存时间
  if(base->tv_cache.tv_sec){
    *tp = base->tv_cache;
    return 0;
  }

  if(lu_evutil_gettime_monotonic_(&base->monotonic_timer, tp)== -1){
    return -1;
  }



  return 0;
}



lu_event_base_t *lu_event_base_new_with_config(lu_event_config_t * cfg) {
//TODO: to be implemented
  int i;
  lu_event_base_t * evbase;
  int should_check_environment;

  // 安全分配内存用于存储 event_base 结构体，并初始化为 0
  if(NULL == (evbase = mm_calloc(1, sizeof(lu_event_base_t)))) {
      // 内存分配失败
      LU_EVENT_LOG_WARN("%s:%d: calloc failed", __func__,(int) sizeof(lu_event_base_t));
      return (NULL);
  }
  if(cfg)
    cfg->flags = evbase->flags;
  should_check_environment =
    !(cfg && (cfg->flags & LU_EVENT_BASE_FLAG_IGNORE_ENV));

  {

    struct timeval tmp_timeval;
    int precise_time =
      (cfg && (cfg->flags & LU_EVENT_BASE_FLAG_PRECISE_TIMER));
    int flags;
    if(should_check_environment && !precise_time){
      //如果环境变量中设置了精确时间，则启用精确时间
      precise_time = lu_evutil_getenv_("LU_EVENT_PRECISE_TIMER") != NULL;
      if(precise_time)
        evbase->flags |= LU_EVENT_BASE_FLAG_PRECISE_TIMER;

    }
    flags = precise_time ? LU_EVENT_MONOT_PRECISE : 0;
    lu_evutil_configure_monotonic_time_(&evbase->monotonic_timer, flags);
    // 捕捉当前时间
    gettime(evbase,&tmp_timeval);
  }
  //构造evbase中的时间堆  处理时间堆
  lu_min_heap_constructor_(&evbase->time_heap);

  evbase->sig_info_.ev_signal_pair[0] = -1;
  evbase->sig_info_.ev_signal_pair[1] = -1;

  //通知fd 初始化
  evbase->th_notify_fd[0] = -1;
  evbase->th_notify_fd[1] = -1;

  TAILQ_INIT(&evbase->active_later_queue);
  //初始化事件处理器队列 默认采用链表结构
  lu_evmap_io_initmap(&evbase->io);
  lu_evmap_siganl_initmap(&evbase->sigmap);

  lu_event_changelist_init(&evbase->changelist);
  evbase->evbase = NULL;

  if(cfg){
    mm_memcpy(&evbase->max_dispatch_time, &cfg->max_dispatch_interval, sizeof(struct timeval));
    evbase->limit_callbacks_after_priority = cfg->limit_callbacks_after_priority;
  }else{
    evbase->max_dispatch_time.tv_sec = -1;
    evbase->limit_callbacks_after_priority = 1;//默认限制回调函数数量
  }
  if (cfg && cfg->max_dispatch_callbacks >= 0) {
    evbase->max_dispatch_callbacks = cfg->max_dispatch_callbacks;
  }else{
    evbase->max_dispatch_callbacks = INT_MAX;
  }

  if(evbase->max_dispatch_callbacks == INT_MAX &&
    evbase->max_dispatch_time.tv_sec == -1)
    evbase->limit_callbacks_after_priority = INT_MAX;

  //**选择合适的后端 */
	for (i = 0; eventops[i] && !evbase->evbase; i++) {
		if (cfg != NULL) {
			/* determine if this backend should be avoided */
			if (lu_event_config_is_avoided_method(cfg,
				eventops[i]->name))
				continue;
			if ((eventops[i]->features & cfg->required_features)
			    != cfg->required_features)
				continue;
		}

		/* also obey the environment variables */
		if (should_check_environment &&
		    lu_event_is_method_disabled(eventops[i]->name))
			continue;

		evbase->evsel_op = eventops[i];

		evbase->evbase = evbase->evsel_op->init(evbase);
	}

  	if (evbase->evbase == NULL) {
      LU_EVENT_LOG_WARNX("%s: no event mechanism available",
          __func__);
      evbase->evsel_op = NULL;
      lu_event_base_free(evbase);
      return NULL;
    }

  //TODO: 事件处理器队列
  return (evbase);
}

static void lu_event_config_entry_free(lu_event_config_entry_t * entry) {

  if(entry->avoid_method != NULL)
    mm_free((char*)entry->avoid_method);
  mm_free(entry);
}

void lu_event_config_free(lu_event_config_t * ev_cfg_t_) {
	lu_event_config_entry_t *entry;

  while((entry = TAILQ_FIRST(&ev_cfg_t_->entries))!= NULL){
    TAILQ_REMOVE(&ev_cfg_t_->entries, entry, next);
    lu_event_config_entry_free(entry);
  }
  mm_free(ev_cfg_t_);
}


lu_event_base_t *lu_event_base_new(void) {
  lu_event_base_t *ev_base_t = NULL;
  lu_event_config_t *ev_cfg_t = lu_event_config_new();
  if (ev_cfg_t) {

    ev_base_t = lu_event_base_new_with_config(ev_cfg_t);
    lu_event_config_free(ev_cfg_t);
  }
  return (ev_base_t);
}


// 用于检查给定的方法名是否在事件配置中被避免
static int lu_event_config_is_avoided_method(lu_event_config_t * cfg, const char *method_name)
{
    // 定义一个指向事件配置条目的指针
    lu_event_config_entry_t *entry;
    // 使用TAILQ_FOREACH宏遍历cfg->entries链表
    // entry是指向当前遍历条目的指针
    // &cfg->entries是链表的头指针
    // next是链表中每个条目的下一个条目的指针字段
    TAILQ_FOREACH(entry, &cfg->entries, next) {
        // 检查当前条目的avoid_method字段是否不为NULL
        // 并且避免的方法名与给定的方法名相等
      if(entry->avoid_method != NULL &&
        strcmp(entry->avoid_method, method_name) == 0)
          // 如果条件满足，返回1，表示该方法名被避免
        return (1);
    }
    // 如果遍历完所有条目都没有找到匹配的方法名，返回0
  return 0;
}



void lu_event_base_free(lu_event_base_t *base)
{
	//TODO: to finish this function
	lu_event_base_free_(base, 1);
}



int
/* workaround for -Werror=maybe-uninitialized bug in gcc 11/12 */
#if defined(__GNUC__) && (__GNUC__ == 11 || __GNUC__ == 12)
__attribute__((noinline))
#endif
lu_event_assign(lu_event_t *ev, lu_event_base_t *base, lu_evutil_socket_t fd, short events, lu_event_callback_fn callback, void *callback_arg) {
    if(!base)
      base = current_base;
    if(callback_arg == &event_self_cbarg_ptr_)
      callback_arg  = ev;
   if (!(events & LU_EV_SIGNAL))
      lu_event_debug_assert_socket_nonblocking(fd);
    lu_event_debug_assert_not_added_(ev);
    ev->ev_base = base;
    ev->ev_callback= callback;
    ev->ev_arg = callback_arg;
    ev->ev_events = events;
    ev->ev_res = 0;
    ev->ev_flags = LU_EVLIST_INIT;
    ev->ev_ncalls = 0;
    ev->ev_pncalls = NULL;


    if (events & LU_EV_SIGNAL) {
        if ((events & (LU_EV_READ|LU_EV_WRITE|LU_EV_CLOSED)) != 0) {
          LU_EVENT_LOG_WARNX("%s: EV_SIGNAL is not compatible with "
              "EV_READ, EV_WRITE or EV_CLOSED", __func__);
          return -1;
        }
        ev->ev_closure = LU_EV_CLOSURE_EVENT_SIGNAL;
      } else {
        if (events & LU_EV_PERSIST) {
          lu_evutil_timerclear(&ev->ev_io_timeout);
          ev->ev_closure = LU_EV_CLOSURE_EVENT_PERSIST;
        } else {
          ev->ev_closure = LU_EV_CLOSURE_EVENT;
        }
    }

    lu_min_heap_element_init_(ev);
    if(base != NULL) {
      	/* 默认情况下，我们将新事件置于中等优先级 */
		    ev->ev_pri = base->nactivequeues / 2;
    }
    lu_event_debug_note_setup_(ev);
    return 0;
}

static void lu_event_debug_assert_socket_nonblocking(lu_evutil_socket_t fd) {
  if(!event_debug_mode_on_)
    return;
  if(fd < 0)
    return;
  int flags;
  if((flags = fcntl(fd, F_GETFL, 0)) >=  0)
    LU_EVUTIL_ASSERT(flags & O_NONBLOCK);

}

int lu_event_priority_set(lu_event_t *ev, int pri){
  lu_event_debug_assert_is_setup_(ev);
  if(ev->ev_flags & LU_EVLIST_ACTIVE){
    return (-1);
  }
  //判断pri是否在有效范围内
  if(pri < 0 || pri >= ev->ev_base->nactivequeues)
    return (-1);
  ev->ev_pri = pri;
  return (0);
}

static void lu_event_debug_assert_not_added_(const lu_event_t *ev)
{
  //TODO:
  LU_UNUSED(ev);
}


void lu_event_debug_unassign(lu_event_t *ev)
{
	lu_event_debug_assert_not_added_(ev);
	lu_event_debug_note_teardown_(ev);

	ev->ev_flags &= ~LU_EVLIST_INIT;
}


static void lu_event_base_free_(lu_event_base_t * base, int run_finalizers){
  //TODO: finish this function
  	int i;
	size_t n_deleted=0;
	struct lu_event_s *ev;

	struct lu_evwatch_s *watcher;

	/* XXXX grab the lock? If there is contention when one thread frees
	 * the base, then the contending thread will be very sad soon. */

	/* lu_event_base_free(NULL) is how to free the current_base if we
	 * made it with lu_event_init and forgot to hold a reference to it. */
	if (base == NULL && current_base)
		base = current_base;
	/* Don't actually free NULL. */
	if (base == NULL) {
		LU_EVENT_LOG_WARNX("%s: no base to free", __func__);
		return;
	}


	/* threading fds if we have them */
	if (base->th_notify_fd[0] != -1) {
		lu_event_del(&base->th_notify);
		LU_EVUTIL_CLOSESOCKET(base->th_notify_fd[0]);
		if (base->th_notify_fd[1] != -1)
			LU_EVUTIL_CLOSESOCKET(base->th_notify_fd[1]);
		base->th_notify_fd[0] = -1;
		base->th_notify_fd[1] = -1;
		lu_event_debug_unassign(&base->th_notify);
	}
	/* XXX(niels) - check for internal events first */
  /* Delete all non-internal events. */
	lu_evmap_delete_all_(base);

	while ((ev = lu_min_heap_top_(&base->time_heap)) != NULL) {
		lu_event_del(ev);
		++n_deleted;
	}
	for (i = 0; i < base->n_common_timeouts; ++i) {
		lu_common_timeout_list_t  *ctl =
		    base->common_timeout_queues[i];
		lu_event_del(&ctl->timeout_event); /* Internal; doesn't count */
		lu_event_debug_unassign(&ctl->timeout_event);
		for (ev = TAILQ_FIRST(&ctl->events); ev; ) {
			lu_event_t *next = TAILQ_NEXT(ev,
			    ev_timeout_pos.ev_next_with_common_timeout);
			if (!(ev->ev_flags & LU_EVLIST_INTERNAL)) {
				lu_event_del(ev);
				++n_deleted;
			}
			ev = next;
		}
		mm_free(ctl);
	}
	if (base->common_timeout_queues)
		mm_free(base->common_timeout_queues);

	for (;;) {
		/* For finalizers we can register yet another finalizer out from
		 * finalizer, and iff finalizer will be in active_later_queue we can
		 * add finalizer to activequeues, and we will have events in
		 * activequeues after this function returns, which is not what we want
		 * (we even have an assertion for this).
		 *
		 * A simple case is bufferevent with underlying (i.e. filters).
		 */
    //TODO: finish lu_event_base_free_queues_ function
		int i = lu_event_base_free_queues_(base, run_finalizers);

		LU_EVENT_LOG_DEBUGX("%s: %d events freed", __func__, i);
		if (!i) {
			break;
		}
		n_deleted += i;
	}

	if (n_deleted)
    LU_EVENT_LOG_DEBUGX("%s: "LU_EV_SIZE_FMT" events were still set in base", __func__, n_deleted);
	while (LIST_FIRST(&base->once_events)) {
		struct lu_event_once_s *eonce = LIST_FIRST(&base->once_events);
		LIST_REMOVE(eonce, next_once);
		mm_free(eonce);
	}

	if (base->evsel_op != NULL && base->evsel_op->dealloc != NULL)
		base->evsel_op->dealloc(base);

	for (i = 0; i < base->nactivequeues; ++i)
		LU_EVUTIL_ASSERT(TAILQ_EMPTY(&base->activequeues[i]));

	LU_EVUTIL_ASSERT(lu_min_heap_empty_(&base->time_heap));
	lu_min_heap_destructor_(&base->time_heap);

	mm_free(base->activequeues);

	lu_evmap_io_clear_(&base->io);
	lu_evmap_signal_clear_(&base->sigmap);
	lu_event_changelist_freemem_(&base->changelist);

	LU_EVTHREAD_FREE_LOCK(base->th_base_lock, 0);
	LU_EVTHREAD_FREE_COND(base->current_event_cond);

	/* Free all event watchers */
	for (i = 0; i < EVWATCH_MAX; ++i) {
		while (!TAILQ_EMPTY(&base->watchers[i])) {
			watcher = TAILQ_FIRST(&base->watchers[i]);
			TAILQ_REMOVE(&base->watchers[i], watcher, next);
			mm_free(watcher);
		}
	}

	/* If we're freeing current_base, there won't be a current_base. */
	if (base == current_base)
		current_base = NULL;
	mm_free(base);
}

int lu_event_del(lu_event_t* ev){
  //TODO:
  lu_event_del_(ev, LU_EVENT_DEL_AUTOBLOCK);
}

// 定义一个静态函数，用于删除事件
static int lu_event_del_(lu_event_t  *ev, int blocking){
  //TODO: TO FINISH
  int res; // 定义一个整型变量用于存储函数返回值
  // 获取事件的基础结构指针
	lu_event_base_t *base = ev->ev_base;

  // 检查事件基础结构是否为空
	if (LU_EVUTIL_FAILURE_CHECK(!base)) {
		// 如果基础结构为空，记录警告日志
		LU_EVENT_LOG_WARNX("%s: event has no event_base set.", __func__);
		// 返回-1表示失败
		return -1;
	}

  // 获取基础结构的锁，确保线程安全
	LU_EVBASE_ACQUIRE_LOCK(base, th_base_lock);
	res = lu_event_del_nolock_(ev, blocking);
	LU_EVBASE_RELEASE_LOCK(base, th_base_lock);

	return (res);
}

int lu_event_base_free_queues_(lu_event_base_t *base,int run_finalizers){
  //TODO: finish this function
  int deleted = 0,  i ;
  for (i = 0; i < base->nactivequeues; ++i) {
		struct lu_event_callback_s *evcb, *next;
		for (evcb = TAILQ_FIRST(&base->activequeues[i]); evcb; ) {
			next = TAILQ_NEXT(evcb, evcb_active_next);
			deleted += lu_event_base_cancel_single_callback_(base, evcb, run_finalizers);
			evcb = next;
		}
	}

	{
		lu_event_callback_t *evcb;
		while ((evcb = TAILQ_FIRST(&base->active_later_queue))) {
			deleted += lu_event_base_cancel_single_callback_(base, evcb, run_finalizers);
		}
	}

	return deleted;
}

// static inline lu_event_t *lu_event_callback_to_event(lu_event_callback_t *evcb)
// {
// 	LU_EVUTIL_ASSERT((evcb->evcb_flags & LU_EVLIST_INIT));
// 	return LU_EVUTIL_UPCAST(evcb, lu_event_t, ev_evcallback);
// }

static int lu_event_base_cancel_single_callback_(lu_event_base_t *base,lu_event_callback_t *evcb,int run_finalizers){
  int result = 0;

	if (evcb->evcb_flags & LU_EVLIST_INIT){
		lu_event_t *ev = lu_event_callback_to_event(evcb);
		if (!(ev->ev_flags & LU_EVLIST_INTERNAL)) {
			lu_event_del(ev);
			result = 1;
		}
	} else {
		LU_EVBASE_ACQUIRE_LOCK(base, th_base_lock);
		lu_event_callback_cancel_nolock_(base, evcb, 1);
		LU_EVBASE_RELEASE_LOCK(base, th_base_lock);
		result = 1;
	}

	if (run_finalizers && (evcb->evcb_flags & LU_EVLIST_FINALIZING)) {
		switch (evcb->evcb_closure) {
		case LU_EV_CLOSURE_EVENT_FINALIZE:
		case LU_EV_CLOSURE_EVENT_FINALIZE_FREE: {
			lu_event_t *ev = lu_event_callback_to_event(evcb);
			ev->ev_evcallback.evcb_cb_union.evcb_evfinalize(ev, ev->ev_arg);
			if (evcb->evcb_closure == LU_EV_CLOSURE_EVENT_FINALIZE_FREE)
				mm_free(ev);
			break;
		}
		case LU_EV_CLOSURE_CB_FINALIZE:
			evcb->evcb_cb_union.evcb_cbfinalize(evcb, evcb->evcb_arg);
			break;
		default:
			break;
		}
	}
	return result;
}

int lu_event_del_noblock(lu_event_t  *ev){
  //TODO: finish this function
  return lu_event_del_(ev, LU_EVENT_DEL_NOBLOCK);
}


static int lu_event_is_method_disabled(const char *method_name){

	char environment[64];
	int i;

	lu_evutil_snprintf(environment, sizeof(environment), "EVENT_NO%s", method_name);
	for (i = 8; environment[i] != '\0'; ++i)
		environment[i] = LU_EVUTIL_TOUPPER_(environment[i]);
	/* Note that evutil_getenv_() ignores the environment entirely if
	 * we're setuid */
	return (lu_evutil_getenv_(environment) != NULL);
}

/* One-time callback to implement event_base_once: invokes the user callback,
 * then deletes the allocated storage */
static void
lu_event_once_cb(lu_evutil_socket_t fd, short events, void *arg)
{
	lu_event_once_t *eonce = arg;

	(*eonce->cb)(fd, events, eonce->arg);
	LU_EVBASE_ACQUIRE_LOCK(eonce->ev.ev_base, th_base_lock);
	LIST_REMOVE(eonce, next_once);
	LU_EVBASE_RELEASE_LOCK(eonce->ev.ev_base, th_base_lock);
	lu_event_debug_unassign(&eonce->ev);
	mm_free(eonce);
}

/* Schedules an event once */
int lu_event_base_once(lu_event_base_t *base, lu_evutil_socket_t fd, short events,
    void (*callback)(lu_evutil_socket_t, short, void *),
    void *arg, const struct timeval *tv)
{
	lu_event_once_t *eonce;
	int res = 0;
	int activate = 0;

	if (!base)
		return (-1);

	/* We cannot support signals that just fire once, or persistent
	 * events. */
	if (events & (LU_EV_SIGNAL|LU_EV_PERSIST))
		return (-1);

	if ((eonce = mm_calloc(1, sizeof(lu_event_once_t))) == NULL)
		return (-1);

	eonce->cb = callback;
	eonce->arg = arg;

	if ((events & (LU_EV_TIMEOUT|LU_EV_SIGNAL|LU_EV_READ|LU_EV_WRITE|LU_EV_CLOSED)) == LU_EV_TIMEOUT) {
		lu_evtimer_assign(&eonce->ev, base, lu_event_once_cb, eonce);

		if (tv == NULL || ! lu_evutil_timerisset(tv)) {
			/* If the event is going to become active immediately,
			 * don't put it on the timeout queue.  This is one
			 * idiom for scheduling a callback, so let's make
			 * it fast (and order-preserving). */
			activate = 1;
		}
	} else if (events & (LU_EV_READ|LU_EV_WRITE|LU_EV_CLOSED)) {
		events &= LU_EV_READ|LU_EV_WRITE|LU_EV_CLOSED;

		lu_event_assign(&eonce->ev, base, fd, events, lu_event_once_cb, eonce);
	} else {
		/* Bad event combination */
		mm_free(eonce);
		return (-1);
	}


	LU_EVBASE_ACQUIRE_LOCK(base, th_base_lock);
	if (activate)
		lu_event_active_nolock_(&eonce->ev, LU_EV_TIMEOUT, 1);
	else
		res = lu_event_add_nolock_(&eonce->ev, tv, 0);

	if (res != 0) {
		mm_free(eonce);
		return (res);
	} else {
		LIST_INSERT_HEAD(&base->once_events, eonce, next_once);
	}
	LU_EVBASE_RELEASE_LOCK(base, th_base_lock);

	return (0);
}


/* not threadsafe, event scheduled once. */
int lu_event_once(lu_evutil_socket_t fd, short events,
	void (*callback)(lu_evutil_socket_t, short, void *),
	void *arg, const struct timeval *tv)
{
	//TODO: finish this function
	return lu_event_base_once(current_base, fd, events, callback, arg, tv);
}



static void
lu_event_queue_remove_active(lu_event_base_t *base, lu_event_callback_t *evcb)
{
	LU_EVENT_BASE_ASSERT_LOCKED(base);
	if (LU_EVUTIL_FAILURE_CHECK(!(evcb->evcb_flags & LU_EVLIST_ACTIVE))) {
		LU_EVENT_LOG_ERRORX(1, "%s: %p not on queue %x", __func__,
                          (void *)evcb, LU_EVLIST_ACTIVE);
		return;
	}
	LU_DECR_EVENT_COUNT(base, evcb->evcb_flags);
	evcb->evcb_flags &= ~LU_EVLIST_ACTIVE;
	base->event_count_active--;

	TAILQ_REMOVE(&base->activequeues[evcb->evcb_pri],
	    evcb, evcb_active_next);
}


static void
lu_event_queue_remove_active_later(lu_event_base_t *base, lu_event_callback_t *evcb)
{
	LU_EVENT_BASE_ASSERT_LOCKED(base);
	if (LU_EVUTIL_FAILURE_CHECK(!(evcb->evcb_flags & LU_EVLIST_ACTIVE_LATER))) {
		LU_EVENT_LOG_ERRORX(1, "%s: %p not on queue %x", __func__,
                          (void *)evcb, LU_EVLIST_ACTIVE_LATER);
		return;
	}
	LU_DECR_EVENT_COUNT(base, evcb->evcb_flags);
	evcb->evcb_flags &= ~LU_EVLIST_ACTIVE_LATER;
	base->event_count_active--;

	TAILQ_REMOVE(&base->active_later_queue, evcb, evcb_active_next);
}


static inline lu_event_t *
lu_event_callback_to_event(lu_event_callback_t *evcb)
{
	LU_EVUTIL_ASSERT((evcb->evcb_flags & LU_EVLIST_INIT));
	return LU_EVUTIL_UPCAST(evcb, lu_event_t, ev_evcallback);
}

static inline lu_event_callback_t *
lu_event_to_event_callback(lu_event_t *ev)
{
	return &ev->ev_evcallback;
}


/** Helper for event_del: always called with th_base_lock held.
 *
 * "blocking" must be one of the EVENT_DEL_{BLOCK, NOBLOCK, AUTOBLOCK,
 * EVEN_IF_FINALIZING} values. See those for more information.
 */
int lu_event_del_nolock_(lu_event_t *ev, int blocking){
  //TODO: finish thie func
  lu_event_base_t * base;
  int res = 0,notify = 0;
  LU_EVENT_LOG_DEBUGX("event_del: %p (fd "LU_EV_SOCK_FMT"),callback %p",
			(void*)ev, LU_EV_SOCK_ARG(ev->ev_fd), (void*)(uintptr_t) ev->ev_callback);
  	/* An event without a base has not been added */
	if (ev->ev_base == NULL){
    return (-1);
  }

  LU_EVENT_BASE_ASSERT_LOCKED(ev->ev_base);

	if (blocking != LU_EVENT_DEL_EVEN_IF_FINALIZING) {
		if (ev->ev_flags & LU_EVLIST_FINALIZING){
			//XXX DEBUG
			return 0;
		}
	}
  base = ev->ev_base;
  LU_EVUTIL_ASSERT(!(ev->ev_flags & ~LU_EVLIST_ALL));
  /* See if we are just active executing this event in a loop */
	if (ev->ev_events & LU_EV_SIGNAL) {
		if (ev->ev_ncalls && ev->ev_pncalls) {
			/* Abort loop */
			*ev->ev_pncalls = 0;
		}
	}

	if (ev->ev_flags & LU_EVLIST_TIMEOUT) {
		/* Notify the base if this was the minimal timeout */
		if (lu_min_heap_top_(&base->time_heap) == ev)
			notify = 1;
		lu_event_queue_remove_timeout(base, ev);
	}

	if (ev->ev_flags & LU_EVLIST_ACTIVE)
		lu_event_queue_remove_active(base, lu_event_to_event_callback(ev));
	else if (ev->ev_flags & LU_EVLIST_ACTIVE_LATER)
		lu_event_queue_remove_active_later(base, lu_event_to_event_callback(ev));

	if (ev->ev_flags & LU_EVLIST_INSERTED) {
		lu_event_queue_remove_inserted(base, ev);
		if (ev->ev_events & (LU_EV_READ|LU_EV_WRITE|LU_EV_CLOSED))

			res = lu_evmap_io_del_(base, ev->ev_fd, ev);
		else
			res = lu_evmap_signal_del_(base, (int)ev->ev_fd, ev);
		if (res == 1) {
			/* evmap says we need to notify the main thread. */
			notify = 1;
			res = 0;
		}

		/* If we do not have events, let's notify event base so it can
		 * exit without waiting */
		if (!lu_event_haveevents(base) && !N_ACTIVE_CALLBACKS(base))
			notify = 1;
	}
	//NOW:
	/* if we are not in the right thread, we need to wake up the loop */
	if (res != -1 && notify && LU_EVBASE_NEED_NOTIFY(base) )
		lu_evthread_notify_base(base);

	lu_event_debug_note_del_(ev);

	/* If the main thread is currently executing this event's callback,
	 * and we are not the main thread, then we want to wait until the
	 * callback is done before returning. That way, when this function
	 * returns, it will be safe to free the user-supplied argument.
	 */
#ifndef LU_EVENT__DISABLE_THREAD_SUPPORT
	if (blocking != LU_EVENT_DEL_NOBLOCK &&
	    base->current_event == lu_event_to_event_callback(ev) &&
	    !LU_EVBASE_IN_THREAD(base) &&
	    (blocking == LU_EVENT_DEL_BLOCK || !(ev->ev_events & LU_EV_FINALIZE))) {
		++base->current_event_waiters;
		LU_EVTHREAD_COND_WAIT(base->current_event_cond, base->th_base_lock);
	}
#endif

	return (res);

}




int
lu_event_callback_cancel_nolock_(lu_event_base_t *base,
    lu_event_callback_t *evcb, int even_if_finalizing)

{
	//TODO: FINSIH THIS FUNCTION
	if ((evcb->evcb_flags & LU_EVLIST_FINALIZING) && !even_if_finalizing)
		return 0;

	if (evcb->evcb_flags & LU_EVLIST_INIT)
		return lu_event_del_nolock_(lu_event_callback_to_event(evcb),
		    even_if_finalizing ? LU_EVENT_DEL_EVEN_IF_FINALIZING : LU_EVENT_DEL_AUTOBLOCK);

	switch ((evcb->evcb_flags & (LU_EVLIST_ACTIVE|LU_EVLIST_ACTIVE_LATER))) {
	default:
	case LU_EVLIST_ACTIVE|LU_EVLIST_ACTIVE_LATER:
		LU_EVUTIL_ASSERT(0);
		break;
	case LU_EVLIST_ACTIVE:
		/* We get different kinds of events, add them together */
		lu_event_queue_remove_active(base, evcb);
		return 0;
	case LU_EVLIST_ACTIVE_LATER:
		lu_event_queue_remove_active_later(base, evcb);
		break;
	case 0:
		break;
	}

	return 0;
}


static void
lu_event_queue_remove_timeout(lu_event_base_t *base, lu_event_t *ev){

	LU_EVENT_BASE_ASSERT_LOCKED(base);
	if (LU_EVUTIL_FAILURE_CHECK(!(ev->ev_flags & LU_EVLIST_TIMEOUT))) {
		LU_EVENT_LOG_ERRORX(1, "%s: %p(fd "LU_EV_SOCK_FMT") not on queue %x", __func__,
                   (void *)ev, LU_EV_SOCK_ARG(ev->ev_fd), LU_EVLIST_TIMEOUT);
		return;
	}
	LU_DECR_EVENT_COUNT(base, ev->ev_flags);
	ev->ev_flags &= ~LU_EVLIST_TIMEOUT;

	if (lu_is_common_timeout(&ev->ev_timeout, base)) {
		lu_common_timeout_list_t *ctl =
		    lu_get_common_timeout_list(base, &ev->ev_timeout);
		TAILQ_REMOVE(&ctl->events, ev,
		    ev_timeout_pos.ev_next_with_common_timeout);
	} else {
		lu_min_heap_erase_(&base->time_heap, ev);
	}
}

/** Requires that 'tv' is a common timeout.  Return the corresponding
 * common_timeout_list. */
static inline lu_common_timeout_list_t *
lu_get_common_timeout_list(lu_event_base_t *base, const struct timeval *tv)
{
	return base->common_timeout_queues[LU_COMMON_TIMEOUT_IDX(tv)];
}

static void lu_event_queue_remove_inserted(lu_event_base_t *base, lu_event_t *ev){
	LU_EVENT_BASE_ASSERT_LOCKED(base);
	if (LU_EVUTIL_FAILURE_CHECK(!(ev->ev_flags & LU_EVLIST_INSERTED))) {
		LU_EVENT_LOG_ERRORX(1, "%s: %p(fd "LU_EV_SOCK_FMT") not on queue %x", __func__,
                   (void *)ev, LU_EV_SOCK_ARG(ev->ev_fd), LU_EVLIST_INSERTED);
		return;
	}
	LU_DECR_EVENT_COUNT(base, ev->ev_flags);
	ev->ev_flags &= ~LU_EVLIST_INSERTED;
}




static int lu_event_haveevents(lu_event_base_t *base)
{
	/* Caller must hold th_base_lock */
	return (base->virtual_event_count > 0 || base->event_count > 0);
}


static int lu_evthread_notify_base(lu_event_base_t *base){
	LU_EVENT_BASE_ASSERT_LOCKED(base);
	if (!base->th_notify_fn)
		return -1;
	if (base->is_notify_pending)
		return 0;
	base->is_notify_pending = 1;
	return base->th_notify_fn(base);
}


void lu_event_active_nolock_(lu_event_t *ev, int res, short ncalls){
	lu_event_base_t *base;

	LU_EVENT_LOG_DEBUGX("event_active: %p (fd " LU_EV_SOCK_FMT "), res %d, callback %p",
		(void*)ev, LU_EV_SOCK_ARG(ev->ev_fd), (int)res, (void*)(uintptr_t)(ev->ev_callback));

	base = ev->ev_base;
	LU_EVENT_BASE_ASSERT_LOCKED(base);

	if (ev->ev_flags & LU_EVLIST_FINALIZING) {
		/* XXXX debug */
		return;
	}

	switch ((ev->ev_flags & (LU_EVLIST_ACTIVE|LU_EVLIST_ACTIVE_LATER))) {
	default:
	case LU_EVLIST_ACTIVE|LU_EVLIST_ACTIVE_LATER:
		LU_EVUTIL_ASSERT(0);
		break;
	case LU_EVLIST_ACTIVE:
		/* We get different kinds of events, add them together */
		ev->ev_res |= res;
		return;
	case LU_EVLIST_ACTIVE_LATER:
		ev->ev_res |= res;
		break;
	case 0:
		ev->ev_res = res;
		break;
	}

	if (ev->ev_pri < base->event_running_priority)
		base->event_continue = 1;

	if (ev->ev_events & LU_EV_SIGNAL) {
#ifndef LU_EVENT__DISABLE_THREAD_SUPPORT
		if (base->current_event == lu_event_to_event_callback(ev) &&
		    !LU_EVBASE_IN_THREAD(base)) {
			++base->current_event_waiters;
			LU_EVTHREAD_COND_WAIT(base->current_event_cond, base->th_base_lock);
		}
#endif
		ev->ev_ncalls = ncalls;
		ev->ev_pncalls = NULL;
	}

	lu_event_callback_activate_nolock_(base, lu_event_to_event_callback(ev));
}

int lu_event_add_nolock_(lu_event_t *ev,const struct timeval *tv, int tv_is_absolute){
	lu_event_base_t *base = ev->ev_base;
	int res = 0;
	int notify = 0;

	LU_EVENT_BASE_ASSERT_LOCKED(base);
	lu_event_debug_assert_is_setup_(ev);

	LU_EVENT_LOG_DEBUGX(
		 "event_add: event: %p (fd "LU_EV_SOCK_FMT"), %s%s%s%scall %p",
		 (void*)ev,
		 LU_EV_SOCK_ARG(ev->ev_fd),
		 ev->ev_events & LU_EV_READ ? "LU_EV_READ " : " ",
		 ev->ev_events & LU_EV_WRITE ? "LU_EV_WRITE " : " ",
		 ev->ev_events & LU_EV_CLOSED ? "LU_EV_CLOSED " : " ",
		 tv ? "LU_EV_TIMEOUT " : " ",
		 (void *)(uintptr_t)ev->ev_callback);

	LU_EVUTIL_ASSERT(!(ev->ev_flags & ~LU_EVLIST_ALL));

	if (ev->ev_flags & LU_EVLIST_FINALIZING) {
		/* XXXX debug */
		return (-1);
	}

	/*
	 * prepare for timeout insertion further below, if we get a
	 * failure on any step, we should not change any state.
	 */
	if (tv != NULL && !(ev->ev_flags & LU_EVLIST_TIMEOUT)) {
		if (lu_min_heap_reserve_(&base->time_heap,
			1 + lu_min_heaps_size_(&base->time_heap)) == -1)
			return (-1);  /* ENOMEM == errno */
	}

	/* If the main thread is currently executing a signal event's
	 * callback, and we are not the main thread, then we want to wait
	 * until the callback is done before we mess with the event, or else
	 * we can race on ev_ncalls and ev_pncalls below. */
#ifndef LU_EVENT__DISABLE_THREAD_SUPPORT
	if (base->current_event == lu_event_to_event_callback(ev) &&
	    (ev->ev_events & LU_EV_SIGNAL)
	    && !LU_EVBASE_IN_THREAD(base)) {
		++base->current_event_waiters;
		LU_EVTHREAD_COND_WAIT(base->current_event_cond, base->th_base_lock);
	}
#endif

	if ((ev->ev_events & (LU_EV_READ|LU_EV_WRITE|LU_EV_CLOSED|LU_EV_SIGNAL)) &&
	    !(ev->ev_flags & (LU_EVLIST_INSERTED|LU_EVLIST_ACTIVE|LU_EVLIST_ACTIVE_LATER))) {
		if (ev->ev_events & (LU_EV_READ|LU_EV_WRITE|LU_EV_CLOSED))
			res = lu_evmap_io_add_(base, ev->ev_fd, ev);
		else if (ev->ev_events & LU_EV_SIGNAL)
			res = lu_evmap_signal_add_(base, (int)ev->ev_fd, ev);
		if (res != -1)
			lu_event_queue_insert_inserted(base, ev);
		if (res == 1) {
			/* evmap says we need to notify the main thread. */
			notify = 1;
			res = 0;
		}
	}

	/*
	 * we should change the timeout state only if the previous event
	 * addition succeeded.
	 */
	if (res != -1 && tv != NULL) {
		struct timeval now;
		int common_timeout;
#ifdef USE_REINSERT_TIMEOUT
		int was_common;
		int old_timeout_idx;
#endif

		/*
		 * for persistent timeout events, we remember the
		 * timeout value and re-add the event.
		 *
		 * If tv_is_absolute, this was already set.
		 */
		if (ev->ev_closure == LU_EV_CLOSURE_EVENT_PERSIST && !tv_is_absolute)
			ev->ev_io_timeout = *tv;

#ifndef USE_REINSERT_TIMEOUT
		if (ev->ev_flags & LU_EVLIST_TIMEOUT) {
			lu_event_queue_remove_timeout(base, ev);
		}
#endif

		/* Check if it is active due to a timeout.  Rescheduling
		 * this timeout before the callback can be executed
		 * removes it from the active list. */
		if ((ev->ev_flags & LU_EVLIST_ACTIVE) &&
		    (ev->ev_res & LU_EV_TIMEOUT)) {
			if (ev->ev_events & LU_EV_SIGNAL) {
				/* See if we are just active executing
				 * this event in a loop
				 */
				if (ev->ev_ncalls && ev->ev_pncalls) {
					/* Abort loop */
					*ev->ev_pncalls = 0;
				}
			}

			lu_event_queue_remove_active(base, lu_event_to_event_callback(ev));
		}

		gettime(base, &now);

		common_timeout = lu_is_common_timeout(tv, base);
#ifdef USE_REINSERT_TIMEOUT
		was_common = lu_is_common_timeout(&ev->ev_timeout, base);
		old_timeout_idx = COMMON_TIMEOUT_IDX(&ev->ev_timeout);
#endif

		if (tv_is_absolute) {
			ev->ev_timeout = *tv;
		} else if (common_timeout) {
			struct timeval tmp = *tv;
			tmp.tv_usec &= MICROSECONDS_MASK;
			lu_evutil_timeradd(&now, &tmp, &ev->ev_timeout);
			ev->ev_timeout.tv_usec |=
			    (tv->tv_usec & ~MICROSECONDS_MASK);
		} else {
			lu_evutil_timeradd(&now, tv, &ev->ev_timeout);
		}


		LU_EVENT_LOG_DEBUGX("event_add: event %p, timeout in %d seconds %d useconds, call %p",
                    (void*)ev, (int)tv->tv_sec, (int)tv->tv_usec, (void *)(uintptr_t)ev->ev_callback);

// #ifdef USE_REINSERT_TIMEOUT
// 		event_queue_reinsert_timeout(base, ev, was_common, common_timeout, old_timeout_idx);
// #else
// 		lu_event_queue_insert_timeout(base, ev);
// #endif
// 		lu_event_queue_insert_timeout(base, ev);
		if (common_timeout) {
			lu_common_timeout_list_t *ctl =
			    lu_get_common_timeout_list(base, &ev->ev_timeout);
			if (ev == TAILQ_FIRST(&ctl->events)) {
				lu_common_timeout_schedule(ctl, &now, ev);
			}
		} else {
			lu_event_t* top = NULL;
			/* See if the earliest timeout is now earlier than it
			 * was before: if so, we will need to tell the main
			 * thread to wake up earlier than it would otherwise.
			 * We double check the timeout of the top element to
			 * handle time distortions due to system suspension.
			 */
			if (lu_min_heap_element_is_top_(ev))
				notify = 1;
			else if ((top = lu_min_heap_top_(&base->time_heap)) != NULL &&
					lu_evutil_timercmp(&top->ev_timeout, &now, <))
				notify = 1;
		}
	}

	/* if we are not in the right thread, we need to wake up the loop */
	if (res != -1 && notify && LU_EVBASE_NEED_NOTIFY(base))
		lu_evthread_notify_base(base);

	lu_event_debug_note_add_(ev);

	return (res);

}



/** Return true iff if 'tv' is a common timeout in 'base' */
static inline int
	lu_is_common_timeout(const struct timeval *tv,
    const lu_event_base_t *base)
{
	int idx;
	if ((tv->tv_usec & COMMON_TIMEOUT_MASK) != COMMON_TIMEOUT_MAGIC)
		return 0;
	idx = LU_COMMON_TIMEOUT_IDX(tv);
	return idx < base->n_common_timeouts;
}



int lu_event_callback_activate_nolock_(lu_event_base_t *base, lu_event_callback_t *evcb){
	int r = 1;

	if (evcb->evcb_flags & LU_EVLIST_FINALIZING)
		return 0;

	switch (evcb->evcb_flags & (LU_EVLIST_ACTIVE|LU_EVLIST_ACTIVE_LATER)) {
	default:
		LU_EVUTIL_ASSERT(0);
		LU_EVUTIL_FALLTHROUGH;
	case LU_EVLIST_ACTIVE_LATER:
		lu_event_queue_remove_active_later(base, evcb);
		r = 0;
		break;
	case LU_EVLIST_ACTIVE:
		return 0;
	case 0:
		break;
	}

	lu_event_queue_insert_active(base, evcb);

	if (LU_EVBASE_NEED_NOTIFY(base))
		evthread_notify_base(base);

	return r;
}