#ifndef LU_EVENT_UTIL_H
#define LU_EVENT_UTIL_H

#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


// socket type
#define lu_evutil_socket_t int


/**
 * @name Standard integer types.
 * @{
*/  

#define lu_uint64_t     uint64_t
#define lu_int64_t      int64_t
#define lu_uint32_t     uint32_t
#define lu_int32_t      int32_t
#define lu_uint16_t     uint16_t
#define lu_int16_t      int16_t
#define lu_uint8_t      uint8_t
#define lu_int8_t       int8_t

//UINT_PTR_T
#define lu_uintptr_t    uintptr_t
#define lu_intptr_t     intptr_t

//ssize_t 
#define lu_ssize_t      ssize_t
//off_t
#define lu_off_t        lu_int64_t
/**
 * @}
 * */


/**
   @name Limits for integer types

   These macros hold the largest or smallest values possible for the
   ev_[u]int*_t types.

   @{
*/

#ifdef __has_include
    #if __has_include(<limits.h>)
        #include <limits.h>
    #endif
#endif

#ifndef INT32_MAX
    #define LU_UINT64_MAX   ((((lu_uint64_t)0xffffffffUL) << 32) | 0xffffffffUL)
    #define LU_INT64_MAX    ((((lu_int64_t) 0x7fffffffL) << 32) | 0xffffffffL)
    #define LU_INT32_MAX    ((lu_int32_t) 0x7fffffffL)
    #define LU_INT32_MIN    ((-LU_INT32_MAX) - 1)
    #define LU_UINT16_MAX   ((lu_uint16_t)0xffffUL)
    #define LU_INT16_MAX    ((lu_int16_t) 0x7fffL)
    #define LU_INT16_MIN    ((-LU_INT16_MAX) - 1)
    #define LU_UINT8_MAX    255
    #define LU_INT8_MAX     127
    #define LU_INT8_MIN     ((-LU_INT8_MAX) - 1)
#else
    #define LU_UINT64_MAX   UINT64_MAX
    #define LU_INT64_MAX    INT64_MAX
    #define LU_INT32_MAX    INT32_MAX
    #define LU_INT32_MIN    INT32_MIN
    #define LU_UINT16_MAX   UINT16_MAX
    #define LU_INT16_MAX    INT16_MAX
    #define LU_INT16_MIN    INT16_MIN
    #define LU_UINT8_MAX    UINT8_MAX
    #define LU_INT8_MAX     INT8_MAX
    #define LU_INT8_MIN     INT8_MIN
#endif
/** @} */


/**
   @name Limits for SIZE_T and SSIZE_T

   @{
*/
#define LU_SIZE_MAX         LU_UINT64_MAX
#define LU_SSIZE_MAX        LU_INT64_MAX

/**@}*/

// #define MM_MALLOC_STR      "__mm_malloc__"
// #define MM_CALLOC_STR      "__mm_calloc__"
// #define MM_FREE_STR        "__mm_free__"
// #define MM_REALLOC_STR     "__mm_realloc__"
// #define MM_STRDUP_STR      "__mm_strdup__"
// #define MM_ALIGEND_MALLOC_STR  "__mm_ali_malloc__"

// #define ALIGEND_MALLOC_STR  "__ali_malloc__"
// #define MALLOC_STR          "__malloc__"
// #define CALLOC_STR          "__calloc__"
// #define FREE_STR            "__free__"
// #define REALLOC_STR         "__realloc__"
// #define STRDUP_STR          "__strdup__"


#define MM_MALLOC_STR               "__MM_MALLOC__"
#define MALLOC_STR                  "__MALLOC__"
#define MM_CALLOC_STR               "__MM_CALLOC__"
#define CALLOC_STR                  "__CALLOC__"
#define MM_FREE_STR                 "__MM_FREE__"
#define FREE_STR                    "__FREE__"
#define MM_REALLOC_STR              "__MM_REALLOC__"
#define REALLOC_STR                 "__REALLOC__"
#define MM_STRDUP_STR               "__MM_STRDUP__"
#define STRDUP_STR                  "__STRDUP__"
#define MM_ALIGEND_MALLOC_STR       "__MM_ALIG_M__"
#define ALIGEND_MALLOC_STR          "__ALIGNED_M__"


#ifdef __cplusplus  
}
#endif

#endif //LU_EVENT_UTIL_H