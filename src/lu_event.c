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
  //TODO: signal info 初始化
  evbase->sig_info_.ev_signal_pair[0] = -1;
  evbase->sig_info_.ev_signal_pair[1] = -1;

  //通知fd 初始化
  evbase->th_notify_fd[0] = -1;
  evbase->th_notify_fd[1] = -1;

  TAILQ_INIT(&evbase->active_later_queue);
  //初始化事件处理器队列 默认采用链表结构
  lu_evmap_io_initmap(&evbase->io);
  lu_evmap_siganl_initmap(&evbase->signal);

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
		    event_is_method_disabled(eventops[i]->name))
			continue;

		evbase->evsel_op = eventops[i];

		evbase->evbase = evbase->evsel_op->init(evbase);
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



#define ev_io_timeout	ev_.ev_io.ev_timeout
#define ev_callback ev_evcallback.evcb_cb_union.evcb_callback
#define ev_arg ev_evcallback.evcb_arg
#define ev_flags ev_evcallback.evcb_flags

#define ev_ncalls	ev_.ev_signal.ev_ncalls
#define ev_pncalls	ev_.ev_signal.ev_pncalls
#define ev_closure ev_evcallback.evcb_closure

//priority
#define ev_pri ev_evcallback.evcb_pri


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
    if(!events & (LU_EV_SIGNAL))
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

static void lu_event_debug_assert_not_added_(const lu_event_t *ev)
{
  //TODO:
  LU_UNUSED(ev);
}


