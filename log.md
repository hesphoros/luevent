# Monday 9 Dec 2024
1.  封装lu_mm-internal.h 头文件
    LU_EVENT__DISABLE_CUSTOM_MM_REPLACEMENT 宏 
    LU_EVENT__DISABLE_DEFAULT_MEMORY_LOGGING 
    
实现了宏函数
#define mm_malloc(size) 			    lu_event_mm_malloc_(size)
#define mm_calloc(nitems, size) 	    lu_event_mm_calloc_((nitems), (size))
#define mm_strdup(str) 			        lu_event_mm_strdup_(str)
#define mm_realloc(ptr, size) 		    lu_event_mm_realloc_((ptr), (size))
#define mm_free(ptr) 				    lu_event_mm_free_(ptr)