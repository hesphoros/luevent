#ifndef LU_INCLUDE_MIN_HEAP_H
#define LU_INCLUDE_MIN_HEAP_H

#include "lu_event-internal.h"
//#include "lu_event_struct.h"
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
static inline int  lu_min_heap_element_is_top_(const lu_event_t * event);
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

#define lu_min_heap_element_greater(a, b) \
  (lu_evutil_timercmp(&(a)->ev_timeout, &(b)->ev_timeout, >))

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

int lu_min_heap_empty_(lu_min_heap_t * heap){
    return 0 == heap->n;
}

size_t lu_min_heaps_size_(lu_min_heap_t * heap){
  return heap->n;
}

lu_event_t * lu_min_heap_top_(lu_min_heap_t * heap){
  //Explanaition: If the heap is empty, return NULL. Otherwise, return the first element of the heap.  
  return heap->n ? *heap->elements : NULL;
}

int lu_min_heap_element_is_top_(const lu_event_t *event){
  return event->ev_timeout_pos.min_heap_idx == 0;
}


int lu_min_heap_reserve_(lu_min_heap_t * heap, size_t n){
  if (heap->capacity < n){
    lu_event_t** new_elements;
    size_t new_capacity = heap->capacity ? heap->capacity*2 : 8;
    if(new_capacity < n){
      new_capacity = n;
    }
    if (!(new_elements = (lu_event_t**)mm_realloc(heap->elements, new_capacity * sizeof *new_elements)))
      return -1;
    
    heap->elements = new_elements;
    heap->capacity = new_capacity;
  }
  
  return 0;
}

int  lu_min_heap_push_(lu_min_heap_t * heap, lu_event_t * event){
  if(lu_min_heap_reserve_(heap, heap->n + 1))
    return -1;
  lu_min_heap_shift_up_(heap,heap->n++, event);
  return 0;

}

lu_event_t * lu_min_heap_pop_(lu_min_heap_t * heap){
  if(heap->n){
    lu_event_t * element = *heap->elements;
    lu_min_heap_shift_down_(heap, 0, heap->elements[--heap->n]);
    element->ev_timeout_pos.min_heap_idx = LU_SIZE_MAX;
    return element;
  }
  return 0;
}


int  lu_min_heap_adjust_(lu_min_heap_t * heap, lu_event_t * event){
  if (LU_SIZE_MAX == event->ev_timeout_pos.min_heap_idx) {
		return lu_min_heap_push_(heap, event);
	} else {
		size_t parent = (event->ev_timeout_pos.min_heap_idx - 1) / 2;
		/* The position of e has changed; we shift it up or down
		 * as needed.  We can't need to do both. */
		if (event->ev_timeout_pos.min_heap_idx > 0 && lu_min_heap_element_greater(heap->elements[parent], event))
			lu_min_heap_shift_up_unconditional_(heap, event->ev_timeout_pos.min_heap_idx, event);
		else
			lu_min_heap_shift_down_(heap, event->ev_timeout_pos.min_heap_idx, event);
		return 0;
	}
}


int  lu_min_heap_erase_(lu_min_heap_t * heap, lu_event_t * event)
{
  if (LU_SIZE_MAX != event->ev_timeout_pos.min_heap_idx) 
  {
	
		lu_event_t *last = heap->elements[--heap->n];
		size_t parent = (event->ev_timeout_pos.min_heap_idx - 1) / 2;
		/* we replace e with the last element in the heap.  We might need to
		   shift it upward if it is less than its parent, or downward if it is
		   greater than one or both its children. Since the children are known
		   to be less than the parent, it can't need to shift both up and
		   down. */
		if (event->ev_timeout_pos.min_heap_idx > 0 && lu_min_heap_element_greater(heap->elements[parent], last))  
			lu_min_heap_shift_up_unconditional_(heap, event->ev_timeout_pos.min_heap_idx, last);
		else
			lu_min_heap_shift_down_(heap, event->ev_timeout_pos.min_heap_idx, last);
		event->ev_timeout_pos.min_heap_idx = LU_SIZE_MAX;
		return 0;
	}
	return -1;

}




void lu_min_heap_shift_up_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event){
  size_t parent = (hole_index - 1) / 2;
    while (hole_index && lu_min_heap_element_greater(heap->elements[parent], event))
    {
      (heap->elements[hole_index] = heap->elements[parent])->ev_timeout_pos.min_heap_idx = hole_index;
      hole_index = parent;
      parent = (hole_index - 1) / 2;
    }
    (heap->elements[hole_index] = event)->ev_timeout_pos.min_heap_idx = hole_index;
}


void lu_min_heap_shift_down_(lu_min_heap_t * heap, size_t hole_index, lu_event_t * event){
  size_t min_child = hole_index * 2 + 1;
  while(min_child < heap->n){
    min_child -= min_child == heap->n || lu_min_heap_element_greater(heap->elements[min_child], heap->elements[min_child - 1]);
    if (!(lu_min_heap_element_greater(event, heap->elements[min_child])))
        break;
    (heap->elements[hole_index] = heap->elements[min_child])->ev_timeout_pos.min_heap_idx = hole_index;
    hole_index = min_child;
    min_child = 2 * (hole_index + 1);
  }
  (heap->elements[hole_index] = event)->ev_timeout_pos.min_heap_idx = hole_index;
}

void lu_min_heap_shift_up_unconditional_(lu_min_heap_t *heap,size_t hole_index, lu_event_t *event)
{
  size_t parent = (hole_index - 1) / 2;
  do{
    (heap->elements[hole_index] = heap->elements[parent])->ev_timeout_pos.min_heap_idx = hole_index;
    hole_index = parent;
    parent = (hole_index - 1) / 2;
  }while(hole_index && lu_min_heap_element_greater(heap->elements[parent], event));
   (heap->elements[hole_index] = event)->ev_timeout_pos.min_heap_idx = hole_index;

}

#endif /* LU_INCLUDE_MIN_HEAP_H */