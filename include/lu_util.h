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
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

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



#define LU_EVUTIL_FALLTHROUGH  __attribute__((fallthrough))


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

void lu_evutil_assert_impl(const char *file, int line, const char *cond, const char *func) ;

/**文件操作相关函数 */
int lu_evutil_create_dictionay(const char * path);
int lu_evutil_check_dict_file_exist(const char *path);
int lu_evutil_check_contain_directory(const char *filename);
const char* lu_evutil_get_directory(const char *filename,char * out_buf,size_t out_buffer_size);

//只对posix系统有效，对windows系统无效(不打算做windows兼容)
#define lu_evutil_gettimeofday(tv,tz) gettimeofday((tv),(tz))


// #define lu_evutil_timerclear(tvp) (tvp)->tv_sec = (tvp)->tv_usec = 0
#define lu_evutil_timerclear(tvp) timerclear(tvp)
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




#define LU_EVUTIL_ASSERT(cond) do {                              \
    if (LU_EVUTIL_UNLIKELY(!(cond))) {                           \
        lu_evutil_assert_impl(__FILE__, __LINE__, #cond, __func__); \
    }                                                            \
} while(0)

#define LU_EVUTIL_FAILURE_CHECK(cond) LU_EVUTIL_UNLIKELY(cond)


#define LU_EVUTIL_UPCAST(ptr, type, field)				\
	((type *)(((char*)(ptr)) - offsetof(type, field)))


int lu_evutil_make_socket_closeonexec(lu_evutil_socket_t fd);

int lu_evutil_make_internal_pipe_(lu_evutil_socket_t fd[2]);

int lu_evutil_fast_socket_nonblocking(lu_evutil_socket_t fd);
int lu_evutil_fast_socket_closeonexec(lu_evutil_socket_t fd);

int lu_evutil_closesocket(lu_evutil_socket_t s){ return close(s); }

#define LU_EVUTIL_CLOSESOCKET(s) lu_evutil_closesocket(s)
#define lu_evutil_timerisset(tvp) timerisset(tvp)

#define lu_evtimer_assign(ev,b,cb,arg)  \
    lu_event_assign((ev),(b),-1,0,(cb),(arg))

#define LU_EVUTIL_ERR_IS_EAGAIN(e) \
	((e) == EAGAIN)

#define LU_EVUTIL_ERR_RW_RETRIABLE(e)				\
	((e) == EINTR || LU_EVUTIL_ERR_IS_EAGAIN(e))


#define LU_EV_SIZE_FMT "%lu"
#define LU_EV_SSIZE_FMT "%zd"
#define LU_EV_SIZE_ARG(x) (unsigned long)(x)
#define LU_EV_SSIZE_ARG(x) (long)(x)


#define LU_EV_SOCK_FMT "%d"
#define LU_EV_SOCK_ARG(x) (x)




/** Mask used to get the real tv_usec value from a common timeout. */
#define COMMON_TIMEOUT_MICROSECONDS_MASK       0x000fffff
#define MICROSECONDS_MASK       COMMON_TIMEOUT_MICROSECONDS_MASK
#define COMMON_TIMEOUT_IDX_MASK 0x0ff00000
#define COMMON_TIMEOUT_IDX_SHIFT 20
#define COMMON_TIMEOUT_MASK     0xf0000000
#define COMMON_TIMEOUT_MAGIC    0x50000000

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define MAX_EVENT_COUNT(var, v) var = MAX(var, v)

#define LU_DECR_EVENT_COUNT(base,flags) \
	((base)->event_count -= !((flags) & LU_EVLIST_INTERNAL))

#define LU_COMMON_TIMEOUT_IDX(tv) \
	(((tv)->tv_usec & COMMON_TIMEOUT_IDX_MASK)>>COMMON_TIMEOUT_IDX_SHIFT)
#define LU_INCR_EVENT_COUNT(base,flags) do {					\
	((base)->event_count += !((flags) & LU_EVLIST_INTERNAL));			\
	MAX_EVENT_COUNT((base)->event_count_max, (base)->event_count);		\
} while (0)

char LU_EVUTIL_TOUPPER_(char c);
char LU_EVUTIL_TOLOWER_(char c);



/* Set the variable 'x' to the field in event_map 'map' with fields of type
   'struct type *' corresponding to the fd or signal 'slot'.  Set 'x' to NULL
   if there are no entries for 'slot'.  Does no bounds-checking. */
#define GET_SIGNAL_SLOT(x, map, slot, type)			\
	(x) = (struct type *)((map)->entries[slot])
/* As GET_SLOT, but construct the entry for 'slot' if it is not present,
   by allocating enough memory for a 'struct type', and initializing the new
   value by calling the function 'ctor' on it.  Makes the function
   return -1 on allocation failure.
 */
#define GET_SIGNAL_SLOT_AND_CTOR(x, map, slot, type, ctor, fdinfo_len)	\
	do {								\
		if ((map)->entries[slot] == NULL) {			\
			(map)->entries[slot] =				\
			    mm_calloc(1,sizeof(struct type)+fdinfo_len); \
			if (LU_EVUTIL_UNLIKELY((map)->entries[slot] == NULL)) \
				return (-1);				\
			(ctor)((struct type *)(map)->entries[slot]);	\
		}							\
		(x) = (struct type *)((map)->entries[slot]);		\
	} while (0)

/* If we aren't using hashtables, then define the IO_SLOT macros and functions
   as thin aliases over the SIGNAL_SLOT versions. */

#define GET_IO_SLOT(x,map,slot,type) GET_SIGNAL_SLOT(x,map,slot,type)
#define GET_IO_SLOT_AND_CTOR(x,map,slot,type,ctor,fdinfo_len)	\
	GET_SIGNAL_SLOT_AND_CTOR(x,map,slot,type,ctor,fdinfo_len)
#define FDINFO_OFFSET sizeof(struct lu_evmap_io_s)




#define N_ACTIVE_CALLBACKS(base)					\
	((base)->event_count_active)

#define LU_EVENT_DEBUG_MODE_IS_ON() event_debug_mode_on_


#ifdef __cplusplus
}
#endif

#endif //LU_EVENT_UTIL_H