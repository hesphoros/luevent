#ifndef LU_INCLUDE_MIN_HEAP_H
#define LU_INCLUDE_MIN_HEAP_H

#include "lu_event-internal.h"
#include "lu_event_struct.h"
//#include "lu_mm-internal.h"
#include "lu_memory_manager.h"
#include "lu_util.h"

typedef struct lu_min_heap_s {
  lu_event_t ** elements;
  lu_size_t n;
  lu_size_t capacity;
}lu_min_heap_t;



static inline void lu_min_heap_constructor_(lu_min_heap_t * heap);
static inline void lu_min_heap_destructor_(lu_min_heap_t * heap);
static inline void lu_min_heap_element_init_(lu_event_t * event);
static inline int  lu_min_heap_elt_is_top_(const lu_event_t * event);
static inline int  lu_min_heap_empty_(lu_min_heap_t * heap);
static inline size_t lu_min_heaps_size_(lu_min_heap_t * heap);
static inline lu_event_t * lu_min_heap_top_(lu_min_heap_t * heap);
static inline int  lu_min_heap_reserve_(lu_min_heap_t * heap, size_t n);
static inline int  lu_min_heap_push_(lu_min_heap_t * heap, lu_event_t * event);
static inline lu_event_t * lu_min_heap_pop_(lu_min_heap_t * heap);
static inline int  lu_min_heap_adjust_(lu_min_heap_t * heap, lu_event_t * event);
static inline int  lu_min_heap_erase_(lu_min_heap_t * heap, lu_event_t * event);
static inline void lu_min_heap_shift_up_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event);
static inline void lu_min_heap_shift_up_unconditional_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event);
static inline void lu_min_heap_shift_down_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event);

#define lu_min_heap_element_greater_(a, b) \
  (lu_evutil_timercmp(&(a)->ev_timeout, &(b)->ev_timeout, >))//TODO:

void lu_min_heap_constructor_(lu_min_heap_t *heap) {
  heap->elements = NULL;
  heap->n = 0;
  heap->capacity = 0;
}

void lu_min_heap_destructor_(lu_min_heap_t *heap) {
  if (heap->elements) {
    mm_free(heap->elements);
  }
}

void lu_min_heap_element_init_(lu_event_t *event) {
  event->ev_timeout_pos.min_heap_idx = LU_SIZE_MAX;
}

void lu_min_heap_empty_(lu_min_heap_t * heap){
  
}


#endif /* LU_INCLUDE_MIN_HEAP_H */