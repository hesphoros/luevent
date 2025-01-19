#include "lu_mm-internal.h"
#include "lu_erron.h"
#include <errno.h>  
#include "lu_util.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>



void* lu_log_functions_global_[] = {
    &lu_mm_malloc_log_fn_,
    &lu_mm_calloc_log_fn_,
    &lu_mm_realloc_log_fn_,
    &lu_mm_free_log_fn_,
    &lu_mm_aligned_malloc_log_fn_
};

void lu_enable_default_memory_logging(int enable) {
    //void (*log_fn)(const char*, void*, size_t) = enable ? default_memory_log : NULL;
    uintptr_t log_fn_ptr = (uintptr_t)(enable ? default_memory_log : NULL);
    for (size_t i = 0; i < sizeof(lu_log_functions_global_) / sizeof(lu_log_functions_global_[0]); i++) {
      // *(void**)lu_log_functions_global_[i] = (void*)log_fn;
        *(uintptr_t*)lu_log_functions_global_[i] = log_fn_ptr;

    }
}


//#define LU_ERROR_NO_MEMORY 12
void* lu_event_mm_malloc_(size_t size){
    if(size == 0){
        return NULL;
    }
    void* ptr = NULL;
    
    if (lu_mm_malloc_fn_) {
        ptr = lu_mm_malloc_fn_(size);  // 调用自定义的内存分配函数
         // 如果启用了日志记录函数，则记录日志
        if ( lu_mm_malloc_log_fn_) {
            lu_mm_malloc_log_fn_(MM_MALLOC_STR, ptr, size);  // 记录日志
        }
    } else {
        ptr = malloc(size);  
        
        //if (ptr == NULL && lu_mm_malloc_log_fn_) {
        if (lu_mm_malloc_log_fn_) {
            lu_mm_malloc_log_fn_(MALLOC_STR, ptr, size);  // 记录内存分配的日志           
        }
        
    }
   

    return ptr;
}

void* lu_event_mm_calloc_(size_t nitems, size_t size) {
    if (nitems == 0 || size == 0)
        return NULL;

    size_t sz = nitems * size;
    if (nitems > LU_SIZE_MAX / size) 
    {
        goto error;
    }
       

    void *p = NULL;
    if (lu_mm_calloc_fn_) {
        p = lu_event_mm_malloc_(sz);
        if (p == NULL) {
            return NULL;  
        }
        if (lu_mm_calloc_log_fn_) {
            lu_mm_calloc_log_fn_(MM_CALLOC_STR, p, sz);
        }
        if (p){
            if (memset(p, 0, sz) == NULL)
            {
                free(p);  
                return NULL;
            }
        }
           
    } else {
        p = calloc(nitems, size);
        if (p == NULL) 
            goto error;
        if(lu_mm_calloc_log_fn_) {
            lu_mm_calloc_log_fn_(CALLOC_STR, p, sz);
        }
           
    }
    return p;
error:
    errno = LU_ERROR_OUT_OF_MEMORY;
    return NULL;
}

char* lu_event_mm_strdup_(const char *str){
    if(!str){
        errno = EINVAL;
        return NULL;
    }
    void * ptr   = NULL;
    if(lu_mm_malloc_fn_){
        size_t len = strlen(str);
       
        if(len == LU_SIZE_MAX)
            goto error;
        ptr = lu_mm_malloc_fn_(len+1);
         
        if(ptr)
            return memcpy(ptr,str,len+1);
    }
    else{
      
        return strdup(str);
    }
        

error:
    errno = LU_ERROR_OUT_OF_MEMORY;
    return  NULL;

}

void* lu_event_mm_realloc_(void* ptr,size_t size){
    void *p = NULL;
    if (lu_mm_realloc_fn_)  {
        p = lu_mm_realloc_fn_(ptr, size);
        if (p && lu_mm_realloc_log_fn_)
            lu_mm_realloc_log_fn_(MM_REALLOC_STR, p, size);
        return p;
        
    }
    else{
        p = realloc(ptr, size);
        if (p && lu_mm_realloc_log_fn_)
            lu_mm_realloc_log_fn_(REALLOC_STR, p, size);
                     
    }  
        return p;
}

void lu_event_mm_free_(void* ptr){
    if(ptr == NULL)
        return;

    //sizeof(*ptr) 无法准确反映动态分配的内存大小，可以考虑在内存分配时额外存储内存块大小，或者使用自定义的内存管理来追踪内存块大小。
    if(lu_mm_free_fn_){
        lu_mm_free_fn_(ptr);
        // if (lu_mm_free_log_fn_){
        //     lu_mm_free_log_fn_(MM_FREE_STR, ptr,ptr?sizeof(*ptr):0);        
        // }
    }
    else
    {
        //FIXME: 
        free(ptr);
    //     if (lu_mm_free_log_fn_){
    //         lu_mm_free_log_fn_(FREE_STR, ptr,ptr?sizeof(*ptr):0);
    //     }
     }
         
}

 

void* lu_event_mm_aligned_malloc_(size_t size, size_t alignment) {
    void* ptr = NULL;
    // 检查对齐值是否符合要求（必须是 2 的幂并且是 sizeof(void*) 的倍数）
    if (alignment % 2 != 0 || alignment % sizeof(void*) != 0) {
        // 如果不符合要求，返回 NULL 或者报错
        return NULL;
    }
    if(lu_mm_aligned_malloc_fn_){
        int ret = lu_mm_aligned_malloc_fn_(&ptr, size, alignment);
        if (ret == 0 && lu_mm_aligned_malloc_log_fn_) {
            lu_mm_aligned_malloc_log_fn_(MM_ALIGEND_MALLOC_STR, ptr, size);
        }
        return ptr;
    }else{

        int ret = posix_memalign(&ptr, alignment, size);
        //if (ret == 0 && lu_mm_aligned_malloc_log_fn_) {
        if(lu_mm_aligned_malloc_log_fn_){
            lu_mm_aligned_malloc_log_fn_(ALIGEND_MALLOC_STR, ptr, size);
        }
        return ptr;
    }
}

 




void default_memory_log(const char* operation, void* ptr, size_t size) {
    // 获取当前时间
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];  

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // 打开日志文件，O_APPEND 标志表示追加写入，O_CREAT 表示文件不存在时创建
    int log_file = open("memory_log.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    
    if (log_file == -1) {
        // 如果打开文件失败，输出错误信息
        perror("Error opening log file");
        return;
    }

    char log_message[256];
    ssize_t message_len;

    if (ptr == NULL && strcmp(operation, MM_MALLOC_STR) != 0 && strcmp(operation, MALLOC_STR) != 0 && strcmp(operation,MM_ALIGEND_MALLOC_STR) != 0 && strcmp(operation, ALIGEND_MALLOC_STR) != 0) {
        // 内存分配失败的格式化日志信息
        message_len = snprintf(log_message, sizeof(log_message),
            "[%-19s] %10s Failed to allocate memory (size: %zu bytes), errno: %d, error: %s\n",
            time_str, operation, size, errno, strerror(errno));
    } else {
        // 根据不同的操作类型来调整日志内容
        if (strcmp(operation, MM_MALLOC_STR) == 0 || strcmp(operation, MALLOC_STR) == 0) {
            // malloc 或 mm_malloc 操作
            message_len = snprintf(log_message, sizeof(log_message),
                "[%-19s] %-14s %-14p allocated           (size: %zu bytes)\n", time_str, operation, ptr, size);
        } else if (strcmp(operation, MM_CALLOC_STR) == 0 || strcmp(operation, CALLOC_STR) == 0) {
            // calloc 或 mm_calloc 操作
            message_len = snprintf(log_message, sizeof(log_message),
                "[%-19s] %-14s %-14p calloc allocated    (size: %zu bytes)\n", time_str, operation, ptr, size);
        } else if (strcmp(operation, MM_FREE_STR) == 0 || strcmp(operation, FREE_STR) == 0) {
            // free 或 mm_free 操作
            message_len = snprintf(log_message, sizeof(log_message),
                "[%-19s] %-14s %-14p freed\n", time_str, operation, ptr);
        } else if (strcmp(operation, MM_REALLOC_STR) == 0 || strcmp(operation, REALLOC_STR) == 0) {
            // realloc 或 mm_realloc 操作
            message_len = snprintf(log_message, sizeof(log_message),
                "[%-19s] %-14s %-14p realloc allocated   (new size: %zu bytes)\n", time_str, operation, ptr, size);
        } else if (strcmp(operation, MM_STRDUP_STR) == 0 || strcmp(operation, STRDUP_STR) == 0) {
            // strdup 或 mm_strdup 操作
            message_len = snprintf(log_message, sizeof(log_message),
                "[%-19s] %-14s %-14p strdup allocated    (size: %zu bytes)\n", time_str, operation, ptr, size);
        } else if(strcmp(operation, MM_ALIGEND_MALLOC_STR) == 0 || strcmp(operation, ALIGEND_MALLOC_STR) == 0) {
            // aligned_malloc 或 mm_aligned_malloc 操作
            message_len = snprintf(log_message, sizeof(log_message),
                "[%-19s] %-14s %-14p aligned_malloc allocated (size: %zu bytes)\n", time_str, operation, ptr, size);
        }else {
            // 其他操作
            message_len = snprintf(log_message, sizeof(log_message),
                "[%-19s] %-14s %-14p allocated/freed (size: %zu bytes)\n", time_str, operation, ptr, size);
        }
    }

    // 使用 write 系统调用写入日志信息到文件
    if (message_len > 0) {
        ssize_t written = write(log_file, log_message, message_len);
        if (written == -1) {
            // 如果写入失败，输出错误信息
            perror("Error writing to log file");
        }
    }

    // 关闭文件
    close(log_file);
}


/*

void default_memory_log(const char* operation, void* ptr, size_t size) {
    // 打开日志文件，O_APPEND 标志表示追加写入，O_CREAT 表示文件不存在时创建
    int log_file = open("memory_log.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (log_file == -1) {
        // 如果打开文件失败，输出错误信息
        perror("Error opening log file");
        return;
    }

    char log_message[256];
    ssize_t message_len;

    if (ptr == NULL && strcmp(operation, MM_MALLOC_STR) != 0 && strcmp(operation, MALLOC_STR) != 0)  {
        // 格式化日志信息：内存分配失败
        message_len = snprintf(log_message, sizeof(log_message),
            "[%s] Failed to allocate memory (size: %zu bytes), errno: %d, error: %s\n",
            operation, size, errno, strerror(errno));
    } else {
        // 格式化日志信息：内存分配或释放成功
        message_len = snprintf(log_message, sizeof(log_message),
            "[%s] %p allocated/freed (size: %zu bytes)\n", operation, ptr, size);
            
    }

    // 使用 write 系统调用写入日志信息到文件
    if (message_len > 0) {
        ssize_t written = write(log_file, log_message, message_len);
        if (written == -1) {
            // 如果写入失败，输出错误信息
            perror("Error writing to log file");
        }
    }

    // 关闭文件
    close(log_file);
}
*/