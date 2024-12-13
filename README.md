# Monday 9 Dec 2024
1.  封装lu_mm-internal.h 头文件
    LU_EVENT__DISABLE_CUSTOM_MM_REPLACEMENT 宏 
     
    
~~~c
//实现了宏函数 
#define mm_malloc(size) 			    lu_event_mm_malloc_(size)
#define mm_calloc(nitems, size) 	    lu_event_mm_calloc_((nitems), (size))
#define mm_strdup(str) 			        lu_event_mm_strdup_(str)
#define mm_realloc(ptr, size) 		    lu_event_mm_realloc_((ptr), (size))
#define mm_free(ptr) 				    lu_event_mm_free_(ptr)
~~~
update lu_utils.h

# Tuesday 10 Dec 2024
sizeof(*ptr) 无法准确反映动态分配的内存大小，可以考虑在内存分配时额外存储内存块大小，或者使用自定义的内存管理来追踪内存块大小。
完成了mm-internal.h头文件的封装，实现了mm_malloc、mm_calloc、mm_strdup、mm_realloc、mm_free等函数。 以及条件日志输出

# Wednesday 11 Dec 2024
完善lu_error.h lu_error.c 提供宏
~~~c
 // 自定义错误码，从0x100开始
#define LU_ERROR_OPERATION_NOT_PERMITTED 0x100
#define LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY 0x101
#define LU_ERROR_NO_SUCH_PROCESS 0x102
#define LU_ERROR_INTERRUPTED_SYSTEM_CALL 0x103
#define LU_ERROR_INPUT_OUTPUT_ERROR 0x104
#define LU_ERROR_NO_SUCH_DEVICE_OR_ADDRESS 0x105
#define LU_ERROR_ARGUMENT_LIST_TOO_LONG 0x106
#define LU_ERROR_EXEC_FORMAT_ERROR 0x107
#define LU_ERROR_BAD_FILE_DESCRIPTOR 0x108
#define LU_ERROR_NO_CHILD_PROCESSES 0x109
#define LU_ERROR_TRY_AGAIN 0x10A
#define LU_ERROR_OUT_OF_MEMORY 0x10B
#define LU_ERROR_PERMISSION_DENIED 0x10C
#define LU_ERROR_BAD_ADDRESS 0x10D
#define LU_ERROR_BLOCK_DEVICE_REQUIRED 0x10E
#define LU_ERROR_DEVICE_OR_RESOURCE_BUSY 0x10F
#define LU_ERROR_FILE_EXISTS 0x110
#define LU_ERROR_CROSS_DEVICE_LINK 0x111
#define LU_ERROR_NO_SUCH_DEVICE 0x112
#define LU_ERROR_NOT_A_DIRECTORY 0x113
#define LU_ERROR_IS_A_DIRECTORY 0x114
#define LU_ERROR_INVALID_ARGUMENT 0x115
#define LU_ERROR_FILE_TABLE_OVERFLOW 0x116
#define LU_ERROR_TOO_MANY_OPEN_FILES 0x117
#define LU_ERROR_NOT_A_TYPEWRITER 0x118
#define LU_ERROR_TEXT_FILE_BUSY 0x119
#define LU_ERROR_FILE_TOO_LARGE 0x11A
#define LU_ERROR_NO_SPACE_LEFT_ON_DEVICE 0x11B
#define LU_ERROR_ILLEGAL_SEEK 0x11C
#define LU_ERROR_READ_ONLY_FILE_SYSTEM 0x11D
#define LU_ERROR_TOO_MANY_LINKS 0x11E
#define LU_ERROR_BROKEN_PIPE 0x11F
#define LU_ERROR_MATH_ARGUMENT_OUT_OF_DOMAIN_OF_FUNCTION 0x120
#define LU_ERROR_RESULT_TOO_REPRESENTABLE 0x121
#define LU_MAX_ERROR_CODE 0x122  // 假设错误码的最大值
~~~
对外提供的接口：
~~~c
const char* lu_get_error_string(int errno);
const char* lu_get_error_string_hash(int errno);
~~~

使用数组实现以及hash表实现的字符串映射，采用惰性加载的方式，减少内存占用。

# Friday 13 Dec 2024

默认启用log
~~~ c
#if !defined(LU_EVENT__DISABLE_DEBUG_MODE) || defined(LU_USE_DEBUG)
#define LU_EVENT_DEBUG_LOGGING_ENABLED
#endif

#ifdef LU_EVENT_DEBUG_LOGGING_ENABLED
LU_EVENT_EXPORT_SYMBOL extern lu_uint32_t lu_event_debug_logging_mask_;
#define lu_event_debug_get_logging_mask_() (lu_event_debug_logging_mask_)
#else
#define lu_event_debug_get_logging_mask_() (0)
#endif
~~~
提供宏LU_EVENT__DISABLE_DEBUG_MODE 用于关闭调试日志输出。 LU_USE_DEBUG  用于控制是否编译调试日志输出代码。

~~~c
#define LU_EVENT_LOG_DEBUG  0
#define LU_EVENT_LOG_MSG    1
#define LU_EVENT_LOG_WARN   2
#define LU_EVENT_LOG_ERROR  3
~~~

~~~c
static lu_event_log_cb 		lu_event_log_global_fn_ = NULL;
static lu_event_fatal_cb 	lu_event_fatal_global_fn_ = NULL;
~~~
提供lu_event_log_global_cb 回调函数，用于自定义输出日志。
提供lu_event_fatal_global_fn_，用于自定义输出致命错误日志。


使用 inline 和符号导出结合可能导致编译器不能正确生成符号。
在lu_util.h 中封装lu_evutil_vsnprintf lu_evutil_snprintf 函数来兼容不同平台。

逐步完善lu_log.c lu_log-internal.h 提供日志输出接口。lu_event_enable_debug_logging(which_mask) 用于打开调试日志输出。

