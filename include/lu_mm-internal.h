#ifndef LU_MM_INTERNAL_H
#define LU_MM_INTERNAL_H


#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif

// Custom memory management functions for malloc, calloc realloc  free aligned_malloc , Memory Logging etc.

// TODO 使用内存池进行管理
static void*(*lu_mm_malloc_fn_)(size_t size_) = NULL;
static void*(*lu_mm_calloc_fn_)(void* ptr_, size_t size_) = NULL;
static void *(*lu_mm_realloc_fn_)(void *ptr, size_t size) = NULL;
static void*(*lu_mm_free_fn_)(void* ptr_) = NULL;
static void*(*lu_mm_aligned_malloc_fn_)(size_t size_, size_t alignment) = NULL;


//static 日志函数指针
static void (*lu_mm_malloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_calloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_realloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_free_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_aligned_malloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;

 
void default_memory_log(const char* operation, void* ptr, size_t size);
 
 
extern void* lu_log_functions_global_[];


// 内存对齐等等
// void *aligned_malloc(size_t size, size_t alignment) {
//     void *ptr = malloc(size + alignment);
//     uintptr_t addr = (uintptr_t)ptr;
//     uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
//     return (void*)aligned;
// }

/** 
 * @briefMemory management functions 
 * @{
*/
#ifndef LU_EVENT__DISABLE_CUSTOM_MM_REPLACEMENT
#define mm_malloc(size) 			    lu_event_mm_malloc_(size)
#define mm_calloc(nitems, size) 	    lu_event_mm_calloc_((nitems), (size))
#define mm_strdup(str) 			        lu_event_mm_strdup_(str)
#define mm_realloc(ptr, size) 		    lu_event_mm_realloc_((ptr), (size))
#define mm_free(ptr) 				    lu_event_mm_free_(ptr)

#else
// If custom memory management is disabled, use malloc, calloc, etc. from the standard library
#define mm_malloc(sz) malloc(sz)
#define mm_calloc(n, sz) calloc((n), (sz))
#define mm_strdup(s) strdup(s)
#define mm_realloc(p, sz) realloc((p), (sz))
#define mm_free(p) free(p)
#endif

/** @}  */

void lu_enable_default_memory_logging(int);


#ifdef __cplusplus
}
#endif


#endif //LU_MM_INTERNAL_H