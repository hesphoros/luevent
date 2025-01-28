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
static void*(*lu_mm_malloc_fn_)(size_t size) = NULL;
static void*(*lu_mm_calloc_fn_)(void *ptr, size_t size) = NULL;
static void*(*lu_mm_realloc_fn_)(void *ptr, size_t size) = NULL;
static void*(*lu_mm_free_fn_)(void *ptr) = NULL;

//int posix_memalign(void **memptr, size_t alignment, size_t size);
static int(*lu_mm_aligned_malloc_fn_)(void **ptr,size_t size, size_t alignment) = NULL;
static void* (*lu_mm_memzero_fn_)(void *ptr, size_t size) = NULL;
static void* (*lu_mm_memcpy_fn_)(void *dest, const void *src, size_t n) = NULL;

//使用union进行简化参数传递

//static 日志函数指针
static void (*lu_mm_malloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_calloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_realloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_free_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_aligned_malloc_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_memzero_log_fn_)(const char* operation, void* ptr, size_t size) = NULL;
static void (*lu_mm_memcpy_log_fn_)(const char* operation, void* ptr, size_t n) = NULL;


/**
 * This functions is luevent's offer for custom memory management functions
 * It will create a file with the name "memory_log.txt" in the current directory.
 * log_level : define some macro string in lu_utils.h
 * */
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
#define mm_free(ptr) 				    lu_event_mm_free_((ptr))
#define mm_memalign(size, alignment)    lu_event_mm_aligned_malloc_((size), (alignment))
#define mm_memzero(ptr, size) 		    lu_event_mm_memzero_((ptr), (size))
#define mm_memcpy(dest, src, n) 	    lu_event_mm_memcpy_((dest), (src), (n))
#else
// If custom memory management is disabled, use malloc, calloc, etc. from the standard library
#define mm_malloc(sz) malloc(sz)
#define mm_calloc(n, sz) calloc((n), (sz))
#define mm_strdup(s) strdup(s)
#define mm_realloc(p, sz) realloc((p), (sz))
#define mm_free(p) free(p)
#define mm_memzero(p, sz) memset((p), 0, (sz))
#define mm_memcpy(dest, src, n) memcpy((dest), (src), (n))
#endif

/** @}  */

void lu_enable_default_memory_logging(int);


#ifdef __cplusplus
}
#endif


#endif //LU_MM_INTERNAL_H