#include "include/lu_mm-internal.h"
#include <errno.h>  
#include "include/lu_erron.h"
#include <string.h>



//#define LU_ERROR_NO_MEMORY 12
void* lu_event_mm_malloc_(size_t size){
    if(size == 0){
        return NULL;
    }
    
    if(lu_mm_malloc_fn_)
        return lu_mm_malloc_fn_(size);
    else
        return malloc(size);
}

void* lu_event_mm_calloc_(size_t nitems,size_t size){
    if(nitems == 0 || size == 0)
        return NULL;
    if(lu_mm_calloc_fn_){
        size_t sz = nitems * size;
        void *p = NULL;
        if(nitems > LU_SIZE_MAX / size )
            goto error;
        p = lu_event_mm_malloc_(sz);
        if(p)
            return memset(p,0,sz);
        
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

    if(lu_mm_malloc_fn_){
        size_t len = strlen(str);
        void * p   = NULL;
        if(len == LU_SIZE_MAX)
            goto error;
        p = lu_mm_malloc_fn_(len+1);
        if(p)
            return memcpy(p,str,len+1);
    }
    else
        return strdup(str);

error:
    errno = ENOMEM;
    return  NULL;

}

void* lu_event_mm_realloc_(void* ptr,size_t size){
    if (lu_mm_realloc_fn_)  
        return lu_mm_realloc_fn_(ptr, size);  
    else  
        return realloc(ptr, size); 
}

void lu_event_mm_free_(void* ptr){
    if(lu_mm_free_fn_)
        lu_mm_free_fn_(ptr);
    else
        free(ptr);
}