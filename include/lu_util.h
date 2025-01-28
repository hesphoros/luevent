#ifndef LU_EVENT_UTIL_H
#define LU_EVENT_UTIL_H


#include "lu_visibility.h"

#include <time.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


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

#define lu_uchar_t      unsigned char
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
#define MM_MEMZERO_STR              "__MM_MEMZERO__"
#define MEMZERO_STR                 "__MEMZERO__"
#define MM_MEMCPY_STR               "__MM_MEMCPY__"
#define MEMCPY_STR                  "__MEMCPY__"

/**@}*/

//debug level
#define LU_EVENT_DBG_NONE 0
#define LU_EVENT_DBG_ALL  0xffffffffu


/** @name Log severities
 */
/**@{*/
#define LU_EVENT_LOG_LEVEL_DEBUG  0
#define LU_EVENT_LOG_LEVEL_MSG    1
#define LU_EVENT_LOG_LEVEL_WARN   2
#define LU_EVENT_LOG_LEVEL_ERROR  3
#define LU_EVENT_LOG_LEVEL_FATAL  4
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


#define LU_UNUSED(x) (void)(x)

// #define LU_MM_POOL_OK    0x01
// #define LU_MM_POOL_ERROR 0x02
// #define LU_MM_POOL_AGAIN 0x03
// #define LU_MM_POOL_BUSY  0x04
// #define LU_MM_POOL_DONE  0x05
// #define LU_MM_POOL_DECLINED 0x06
// #define LU_MM_POOL_ABORT 0x07


typedef struct lu_evutil_monotonic_timer_s{
        //posix 系统直接使用 clock_gettime 获取时间，
        int monotonic_clock;
        //用于调整单调时钟的时间。timeval 结构体包含秒和微秒，可以用来微调定时器，或者处理时钟漂移问题
        struct timeval adjust_monotonic_clock;
        //这个字段通常用来计算时间间隔，表示上次记录的时间点
        struct timeval last_time;
}lu_evutil_monotonic_timer_t;


void lu_evutil_adjust_monotonic_time(lu_evutil_monotonic_timer_t *base,struct timeval *tv);

const char * lu_evutil_getenv_(const char *varname);
LU_EVENT_EXPORT_SYMBOL
    int lu_evutil_configure_monotonic_time_( lu_evutil_monotonic_timer_t *base,int flags);

LU_EVENT_EXPORT_SYMBOL
    int lu_evutil_gettime_monotonic_(lu_evutil_monotonic_timer_t * base,struct timeval * tv);



/**文件操作相关函数 */
int lu_evutil_create_dictionay(const char * path);
int lu_evutil_check_dict_file_exist(const char *path);
int lu_evutil_check_contain_directory(const char *filename);
const char* lu_evutil_get_directory(const char *filename,char * out_buf,size_t out_buffer_size);

//只对posix系统有效，对windows系统无效(不打算做windows兼容)
#define lu_evutil_gettimeofday(tv,tz) gettimeofday((tv),(tz))


//lu_evutil_timercmp
#define lu_evutil_timercmp(tvp,uvp,cmp) \
    (((tvp)->tv_sec == (uvp)->tv_sec) ? \
    ((tvp)->tv_usec cmp (uvp)->tv_usec) :       \
     ((tvp)->tv_sec cmp (uvp)->tv_sec))

//时间加法
#define lu_evutil_timeradd(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)


//时间减法
#define	lu_evutil_timersub(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)



#ifndef LU_EVENT_ERROR_ABORT_
#define LU_EVENT_ERROR_ABORT_ ((int)0xdeaddead)
#endif

#if defined(__GNUC__) && __GNUC__ >= 3
#define LU_EVUTIL_UNLIKELY(x) __builtin_expect((x), 0)
#endif

//断言
#define LU_EVUTIL_ASSERT(cond)                                 \
    do {                                                        \
        if (LU_EVUTIL_UNLIKELY(!(cond))) {                                          \
            LU_EVENT_LOG_ERRORX(LU_EVENT_ERROR_ABORT_,"%s:%d: Assertion %s failed in %s",\
            __FILE__,__LINE__,#cond,__func__);                    \
            /* 如果用户提供的处理程序尝试 */                     \
            /* 将控制权返回给我们，在此记录并中止。 */              \
            (void)fprintf(stderr, "Assertion %s failed in %s:%d\n", #cond, __FILE__, __LINE__); \
            abort();                                              \
        }                                                      \
    } while (0)


int lu_evutil_make_socket_closeonexec(lu_evutil_socket_t fd);

int lu_evutil_make_internal_pipe_(lu_evutil_socket_t fd[2]);

#ifdef __cplusplus
}
#endif

#endif //LU_EVENT_UTIL_H