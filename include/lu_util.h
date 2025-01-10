#ifndef LU_EVENT_UTIL_H
#define LU_EVENT_UTIL_H


#include "lu_visibility.h"
//#include <time.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


// socket type
#define lu_evutil_socket_t  int
#define lu_evutil_socklen_t socklen_t
 
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
#define lu_size_t       size_t
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


/**
   @name Memory allocation macros
   These macros are used to track memory allocation and deallocation.
   @{
*/
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
/**@}*/

//debug level
#define LU_EVENT_DBG_NONE 0
#define LU_EVENT_DBG_ALL  0xffffffffu
 

/** @name Log severities
 */
/**@{*/
#define LU_EVENT_LOG_DEBUG  0
#define LU_EVENT_LOG_MSG    1
#define LU_EVENT_LOG_WARN   2
#define LU_EVENT_LOG_ERROR  3
#define LU_EVENT_LOG_FATAL  4
/**@}*/

/*
*   Replacement for snprintf to get consistent behavior on platforms for
    which the return value of snprintf does not conform to C99.
*/
LU_EVENT_EXPORT_SYMBOL int lu_evutil_snprintf(char *str, size_t size, const char *format,...)
#ifdef __GNUC__
	__attribute__((format(printf, 3, 4)))
#endif
;


/** Replacement for vsnprintf to get consistent behavior on platforms for
    which the return value of snprintf does not conform to C99.
 */
LU_EVENT_EXPORT_SYMBOL int lu_evutil_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap)
#ifdef __GNUC__
	__attribute__((format(printf, 3, 0)))
#endif
;

 
/**
   @name Socket error functions
   @{
*/
/** Return the most recent socket error to occur on sock. */
LU_EVENT_EXPORT_SYMBOL int lu_evutil_socket_geterror(lu_evutil_socket_t sock_t);
/** Convert a socket error to a string. */
LU_EVENT_EXPORT_SYMBOL const char *lu_evutil_socket_error_to_string(int errcode);


#define LU_EVUTIL_INVALID_SOCKET (-1)

/**@}*/



#define LU_EVENT_HASH_TABLE_SIZE 32  // 哈希表大小
#define LU_EVENT_MONOT_PRECISE  1 // 高精度
#define LU_EVENT_MONOT_FALLBACK 2 // 低精度


typedef struct lu_evutil_monotonic_timer_s{
    //TODO: to be implemented
    int dummy;
}lu_evutil_monotonic_timer_t;


const char * lu_evutil_getenv_(const char *varname);
int lu_evutil_configure_monotonic_time_( lu_evutil_monotonic_timer_t *base,int flags);

int lu_evutil_create_dictionay(const char * path);


//lu_evutil_timercmp
#define lu_evutil_timercmp(tvp,uvp,cmp) \
    (((tvp)->tv_sec == (uvp)->tv_sec) ? \
    ((tvp)->tv_usec cmp (uvp)->tv_usec) :       \
     ((tvp)->tv_sec cmp (uvp)->tv_sec))


#ifdef __cplusplus  
}
#endif

#endif //LU_EVENT_UTIL_H