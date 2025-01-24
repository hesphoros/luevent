#ifndef LU_INCLUDE_MIN_HEAP_H
#define LU_INCLUDE_MIN_HEAP_H


#include "lu_event_struct.h"
#include "lu_memory_manager.h"
#include "lu_util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lu_event_s lu_event_t;

/// @brief 最小时间堆结构/
typedef struct lu_min_heap_s {
  lu_event_t ** elements;
  lu_size_t n;
  lu_size_t capacity;
}lu_min_heap_t;



/// @brief 最小时间堆构造函数
/// @param[in] heap 最小时间堆结构指针
static inline void lu_min_heap_constructor_(lu_min_heap_t * heap);

/// @brief 最小时间堆析构函数
/// @param[in] heap 最小时间堆结构指针
static inline void lu_min_heap_destructor_(lu_min_heap_t * heap);


static inline void lu_min_heap_element_init_(lu_event_t * event);
static inline int  lu_min_heap_element_is_top_(const lu_event_t * event);
static inline int  lu_min_heap_empty_(lu_min_heap_t * heap);
static inline size_t lu_min_heaps_size_(lu_min_heap_t * heap);

/// @brief  获取最小时间堆的堆顶元素
/// @param heap 最小时间堆结构指针
/// @return 最小时间堆的堆顶元素指针
static inline lu_event_t * lu_min_heap_top_(lu_min_heap_t * heap);
static inline int  lu_min_heap_reserve_(lu_min_heap_t * heap, size_t n);

/// @brief 插入一个lu_event_t到最小时间堆中
/// @param heap 最小时间堆结构指针
/// @param event 要插入的lu_event_t指针
/// @return 0表示成功，-1表示失败
static inline int  lu_min_heap_push_(lu_min_heap_t * heap, lu_event_t * event);

static inline lu_event_t * lu_min_heap_pop_(lu_min_heap_t * heap);
static inline int  lu_min_heap_adjust_(lu_min_heap_t * heap, lu_event_t * event);
static inline int  lu_min_heap_erase_(lu_min_heap_t * heap, lu_event_t * event);

/// @brief 向上调整元素位置,一般在堆里面添加(push)了lu_event_t调用
/// @param heap 最小时间堆结构指针
/// @param hole_index 要调整的位置
/// @param event 要调整的元素
static inline void lu_min_heap_shift_up_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event);
static inline void lu_min_heap_shift_up_unconditional_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event);

/// @brief 向下调整元素位置,一般在堆里面删除(pop)了lu_event_t调用
/// @param heap 
/// @param hole_index 
/// @param event 
static inline void lu_min_heap_shift_down_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event);

#define lu_min_heap_element_greater(a, b) \
  (lu_evutil_timercmp(&(a)->ev_timeout, &(b)->ev_timeout, >))



#ifdef __cplusplus
}
#endif

#endif /* LU_INCLUDE_MIN_HEAP_H */