#include "include/lu_mm-internal.h"
#include <errno.h>  
#include "include/lu_erron.h"
#include <string.h>
#include <stdio.h>


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
            lu_mm_malloc_log_fn_("__mm_malloc__", ptr, size);  // 记录日志
        }
    } else {
        ptr = malloc(size);  
    }
   

    return ptr;
}

void* lu_event_mm_calloc_(size_t nitems,size_t size){
    if(nitems == 0 || size == 0)
        return NULL;
    size_t sz = nitems * size;
    if(lu_mm_calloc_fn_){
        
        void *p = NULL;
        if(nitems > LU_SIZE_MAX / size )
            goto error;
        p = lu_event_mm_malloc_(sz);
        if(lu_mm_calloc_log_fn_){
            lu_mm_calloc_log_fn_("__mm_calloc__", p,  sz);
        }
        if(p)
            return memset(p,0,sz);
        
    }else{
        void *ptr = calloc(nitems, size); 
        if (ptr == NULL)
			goto error;
        
        
        return ptr;
    }
   
error:
	errno = ENOMEM; 
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
    else
        return strdup(str);

error:
    errno = ENOMEM;
    return  NULL;

}

void* lu_event_mm_realloc_(void* ptr,size_t size){
    void *p = NULL;
    if (lu_mm_realloc_fn_)  {
        p = lu_mm_realloc_fn_(ptr, size);
        if (p && lu_mm_realloc_log_fn_)
            lu_mm_realloc_log_fn_("__mm_realloc__", p, size);
       
        return p;
        
    }
    else{
        return realloc(ptr, size);
    }  
        
}

void lu_event_mm_free_(void* ptr){
    if(ptr == NULL)
        return;

    if(lu_mm_free_fn_){
        if (lu_mm_free_log_fn_)
            lu_mm_free_log_fn_("__mm_free__", ptr,ptr?sizeof(*ptr):0);
        
        lu_mm_free_fn_(ptr);
    }
    else
        free(ptr);
}


static void default_memory_log(const char* operation, void* ptr, size_t size) {
    if (ptr == NULL) {
        printf("[%s] Failed to allocate memory (size: %zu bytes)\n", operation, size);
    } else {
        printf("[%s] %p allocated/freed (size: %zu bytes)\n", operation, ptr, size);
    }
}
