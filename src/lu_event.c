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

#define LU_EVENT_BASE_ASSERT_LOCKED(evbase)                    \
  LU_EVLOCK_ASSERT_LOCKED((evbase)->th_base_lock)

static int lu_event_config_is_avoided_method(lu_event_config_t * cfg, const char *method_name) ;
static void lu_event_base_free_(lu_event_base_t * base, int run_finalizers);
static int lu_event_del_(lu_event_t  *ev, int blocking);

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
          evutil_timerclear(&ev->ev_io_timeout);
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
	lu_event_t *ev;
	lu_evwatch_t *watcher;

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

	while ((ev = min_heap_top_(&base->time_heap)) != NULL) {
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
		event_debug(("%s: %d events freed", __func__, i));
		if (!i) {
			break;
		}
		n_deleted += i;
	}

	if (n_deleted)
    LU_EVENT_LOG_DEBUGX("%s: "LU_EV_SIZE_FMT" events were still set in base", __func__, n_deleted);
	while (LIST_FIRST(&base->once_events)) {
		 lu_event_once_t *eonce = LIST_FIRST(&base->once_events);
		LIST_REMOVE(eonce, next_once);
		mm_free(eonce);
	}

	if (base->evsel_op != NULL && base->evsel_op->dealloc != NULL)
		base->evsel_op->dealloc(base);

	for (i = 0; i < base->nactivequeues; ++i)
		EVUTIL_ASSERT(TAILQ_EMPTY(&base->activequeues[i]));

	EVUTIL_ASSERT(min_heap_empty_(&base->time_heap));
	min_heap_dtor_(&base->time_heap);

	mm_free(base->activequeues);

	evmap_io_clear_(&base->io);
	evmap_signal_clear_(&base->sigmap);
	event_changelist_freemem_(&base->changelist);

	EVTHREAD_FREE_LOCK(base->th_base_lock, 0);
	EVTHREAD_FREE_COND(base->current_event_cond);

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
		lu_event_callback_t *evcb, *next;
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

static inline struct event *lu_event_callback_to_event(lu_event_callback_t *evcb)
{
	LU_EVUTIL_ASSERT((evcb->evcb_flags & LU_EVLIST_INIT));
	return LU_EVUTIL_UPCAST(evcb, lu_event_t, ev_evcallback);
}

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
			ev->ev_evcallback.evcb_cb_union.evcb_cbfinalize(ev, ev->ev_arg);
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