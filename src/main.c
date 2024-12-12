#include "lu_memory_manager.h"
#include <stdio.h>
#include "lu_erron.h"
#include <memory.h>


void test_error_to_string(){
    printf("%s\n",lu_get_error_string(LU_ERROR_BAD_ADDRESS));
    printf("%s\n",lu_get_error_string(LU_ERROR_OUT_OF_MEMORY));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_BAD_FILE_DESCRIPTOR));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_BROKEN_PIPE));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_PROCESS));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_DEVICE));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_DEVICE_OR_ADDRESS));    
    
}

void test_mm_memory(){
    lu_enable_default_memory_logging(1); 
    void *p = mm_malloc(100);
    printf("%p size: %d\n",p,sizeof(*p));
   
    p = mm_realloc(p,200);
    printf("%p size: %d\n",p,sizeof(p));

    p = mm_calloc(1,200);
    printf("%p %s size: %d\n",p,*(int*)p,sizeof(p));

    p = mm_strdup("hello");
    printf("%p %s\n",p,(char*)p);
    
    mm_free(p);

}

//#define LU_EVENT__ENABLE_DEFAULT_MEMORY_LOGGING
int main(){
    
    test_error_to_string();
    //test_mm_memory();

    return 0;
}