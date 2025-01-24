#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "lu_util.h"
#include "lu_memory_manager.h"
#include "lu_log-internal.h"
#include "lu_mm-internal.h"


struct lu_evutil_monotonic_timer_t;



int lu_evutil_gettime_monotonic_(lu_evutil_monotonic_timer_t * base,struct timeval * tp){
    //获取单调时间

    //存储高精度时间戳
    struct timespec ts;

    //判断是否初始化成功
    if(base->monotonic_clock < 0){
        if(lu_evutil_gettimeofday(tp,NULL)< 0){
            return -1;
        }

        lu_evutil_adjust_monotonic_time(base,tp);
        return 0;
    }
    // 使用clock_gettime获取高精度时间，参数base->monotonic_clock指定时钟类型
    if(clock_gettime(base->monotonic_clock,&ts) == -1){
        return -1;
    }
    // 将timespec结构体中的秒和纳秒转换为timeval结构体中的秒和微秒
   	tp->tv_sec = ts.tv_sec;
	tp->tv_usec = ts.tv_nsec / 1000;
    return 0;

}



//用于配置和初始化 evutil_monotonic_timer 结构体中的时间信息,选择合适的单调时钟
/// @brief  Configure the monotonic timer.
/// @param base  The monotonic timer to configure.
/// @param flags Flags to control the configuration.(LU_EVENT_MONOT_PRECISE LU_EVENT_MONOT_FALLBACK  )

int lu_evutil_configure_monotonic_time_(lu_evutil_monotonic_timer_t *base,
    int flags)
{
 const int fallback_clock = flags & LU_EVENT_MONOT_FALLBACK;
//posix 系统提供的更粗略的单调时钟
#ifdef CLOCK_MONOTONIC_COARSE
    const int precise_clock = flags & LU_EVENT_MONOT_PRECISE;

#endif

    struct timespec ts;//存储高精度时间戳
    mm_memzero(base, sizeof(*base));

#ifdef CLOCK_MONOTONIC_COARSE
	if (CLOCK_MONOTONIC_COARSE < 0) {
        LU_EVENT_LOG_ERRORX(1, "I didn't expect CLOCK_MONOTONIC_COARSE to be < 0");

	}
	if (! precise_clock && ! fallback_clock) {
		if (clock_gettime(CLOCK_MONOTONIC_COARSE, &ts) == 0) {
			base->monotonic_clock = CLOCK_MONOTONIC_COARSE;
			return 0;
		}
	}
#endif
	if (!fallback_clock && clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
		base->monotonic_clock = CLOCK_MONOTONIC;
		return 0;
	}

	if (CLOCK_MONOTONIC < 0) {
		LU_EVENT_LOG_ERRORX(1,"I didn't expect CLOCK_MONOTONIC to be < 0");
	}

	base->monotonic_clock = -1;
	return 0;

 return 0;
}

/// @brief  Adjust the monotonic clock based on the current time.
/// @param base  The monotonic timer to adjust.
/// @param tv    The current time.
void lu_evutil_adjust_monotonic_time(lu_evutil_monotonic_timer_t *base,struct timeval *tv)
{
	lu_evutil_timeradd(tv, &base->adjust_monotonic_clock, tv);

	if (lu_evutil_timercmp(tv, &base->last_time, < )) {

		struct timeval adjust;
		lu_evutil_timersub(&base->last_time, tv, &adjust);
		lu_evutil_timeradd(&adjust, &base->adjust_monotonic_clock,
		    &base->adjust_monotonic_clock);
		*tv = base->last_time;
	}
	base->last_time = *tv;
}