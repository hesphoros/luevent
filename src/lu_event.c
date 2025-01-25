#include "lu_event-internal.h"
#include "lu_log-internal.h"
#include "lu_memory_manager.h"
#include "lu_min_heap.h"
#include "lu_changelist-internal.h"
#include "lu_event.h"
#include "lu_util.h"



#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <error.h>
#include <stdlib.h>


#define LU_EVENT_BASE_ASSERT_LOCKED(base) \
  LU_EVLOCK_ASSERT_LOCKED((base)->th_base_lock)

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
  LU_EVENT_BASE_ASSERT_LOCKED(base);
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



lu_event_base_t *lu_event_base_new_with_config(lu_event_config_t * ev_cfg_t_) {
//TODO: to be implemented
  int i;
  lu_event_base_t * ev_base_t;
  int should_check_enviroment;

  // 安全分配内存用于存储 event_base 结构体，并初始化为 0
  if(NULL == (ev_base_t = mm_calloc(1, sizeof(lu_event_base_t)))) {
      // 内存分配失败
      //lu_event_warn("%s:%d: calloc failed", __func__,(int) sizeof(lu_event_base_t));
      LU_EVENT_LOG_WARN("%s:%d: calloc failed", __func__,(int) sizeof(lu_event_base_t));
      return (NULL);
  }
  if(ev_cfg_t_)
    ev_base_t->flags = ev_cfg_t_->flags;
  should_check_enviroment =
    !(ev_cfg_t_ && (ev_cfg_t_->flags & LU_EVENT_BASE_FLAG_IGNORE_ENV));

  {
    //检查是否需要精确时间 TODO:
    struct timeval tmp_timeval;
    int precise_time =
      (ev_cfg_t_ && (ev_cfg_t_->flags & LU_EVENT_BASE_FLAG_PRECISE_TIMER));
    int flags;
    if(should_check_enviroment && !precise_time){
      //如果环境变量中设置了精确时间，则启用精确时间
      precise_time = lu_evutil_getenv_("LU_EVENT_PRECISE_TIMER") != NULL;
      if(precise_time)
        ev_base_t->flags |= LU_EVENT_BASE_FLAG_PRECISE_TIMER;

    }
    flags = precise_time ? LU_EVENT_MONOT_PRECISE : 0;
    lu_evutil_configure_monotonic_time_(&ev_base_t->monotonic_timer, flags);
    // 捕捉当前时间
    gettime(ev_base_t,&tmp_timeval);
  }

  


  //TODO: 最小堆
  //TODO: 信号处理
  //TODO: 延迟事件激活队列
  //TODO: 超时事件激活队列
  //TODO: 事件处理器
  //TODO: 事件处理器队列
  return (ev_base_t);
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


