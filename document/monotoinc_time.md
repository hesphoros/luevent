
# Monotonic Time

~~~c

struct evutil_monotonic_timer {

//此定义面向的是对于 macOS
#ifdef HAVE_MACH_MONOTONIC
 struct mach_timebase_info mach_timebase_units;
#endif

// POSIX 系统 (HAVE_POSIX_MONOTONIC) linux 系统
#ifdef HAVE_POSIX_MONOTONIC
 int monotonic_clock;
    //POSIX 系统可以使用 clock_gettime(CLOCK_MONOTONIC, &time) 来获取单调时钟的时间。
#endif


//对于win32
#ifdef HAVE_WIN32_MONOTONIC
 ev_GetTickCount_func GetTickCount64_fn;
 ev_GetTickCount_func GetTickCount_fn;
 ev_uint64_t last_tick_count;
 ev_uint64_t adjust_tick_count;

 ev_uint64_t first_tick;
 ev_uint64_t first_counter;
 double usec_per_count;
 int use_performance_counter;
#endif

    //用于调整单调时钟的时间。timeval 结构体包含秒和微秒，可以用来微调定时器，或者处理时钟漂移问题。
 struct timeval adjust_monotonic_clock;
 //存储上一次获取的时间。这个字段通常用来计算时间间隔，表示上次记录的时间点。
    struct timeval last_time;
};


## CLOCK_MONOTONIC_COARSE
~~~

`CLOCK_MONOTONIC_COARSE` 是由操作系统提供的一个常量，通常用于访问一种较为粗略的单调时钟（monotonic clock）。

### 解释：
- **`CLOCK_MONOTONIC_COARSE`** 是 POSIX 标准中定义的，尤其是在 Linux 和其他一些操作系统上，它表示一种单调时钟，该时钟的精度比 `CLOCK_MONOTONIC` 更低，但可能更加高效。
- 它通常用于那些要求高精度但不要求非常精确时间的应用，特别是当应用需要获取系统的单调时钟，但又不想付出过高的计算开销时。

### 与 `CLOCK_MONOTONIC` 的区别：
- **`CLOCK_MONOTONIC`**：这是一个更精确的单调时钟，通常用于测量时间间隔。它的精度取决于硬件和操作系统的实现，可能会有更小的时间粒度（例如，微秒级别）。
- **`CLOCK_MONOTONIC_COARSE`**：这种时钟的精度较低，可能只有毫秒级别，通常会比 `CLOCK_MONOTONIC` 更高效，因此在需要快速获得时间戳但不关心过高精度时，它是一个理想的选择。

### 支持情况：
- 在 **Linux** 中，`CLOCK_MONOTONIC_COARSE` 是存在的，通常需要 `#define _POSIX_C_SOURCE` 来启用对它的支持。
- 在 **FreeBSD**、**Solaris** 等一些操作系统中，也支持 `CLOCK_MONOTONIC_COARSE`，不过它并不是所有系统的标准部分，具体取决于系统的实现。

### 用法：
- `CLOCK_MONOTONIC_COARSE` 通常通过 `clock_gettime()` 函数来访问。例如：

  ```c
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC_COARSE, &ts) == 0) {
      printf("Time: %ld seconds, %ld nanoseconds\n", ts.tv_sec, ts.tv_nsec);
  }
  ```

- 如果系统不支持 `CLOCK_MONOTONIC_COARSE`，你可以回退到使用更精确但可能更昂贵的 `CLOCK_MONOTONIC`。

### 总结：
`CLOCK_MONOTONIC_COARSE` 是系统定义的常量，表示一种低精度的单调时钟，通常用于那些对时间精度要求不高但对性能要求较高的场景。如果你希望在系统中使用这个常量，确保你的操作系统和开发环境支持它。