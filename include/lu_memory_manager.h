#ifndef LU_MEMORY_MANAGER_H
#define LU_MEMORY_MANAGER_H

#include <stddef.h>

// 只在这里定义内存分配宏（无需直接包含 mm-internal.h）
#include "lu_mm-internal.h"

// 只提供外部使用的内存管理宏
void* lu_event_mm_malloc_(size_t size);
void* lu_event_mm_calloc_(size_t nitems,size_t size);
char* lu_event_mm_strdup_(const char *str);
void* lu_event_mm_realloc_(void* ptr,size_t size);
void lu_event_mm_free_(void* ptr);

#endif // LU_MEMORY_MANAGER_H
