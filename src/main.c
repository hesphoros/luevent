#include "lu_memory_manager.h"
#include <stdio.h>
#include "lu_erron.h"

//#define LU_EVENT__ENABLE_DEFAULT_MEMORY_LOGGING
int main(){
    
    lu_enable_default_memory_logging(1);    
    int *x = (int*)mm_malloc(sizeof(int));
    *x = 123;
    printf("%d\n", *x);
    *x = 2;
    printf("%s\n",lu_get_error_string(LU_ERROR_BAD_ADDRESS));
    printf("%s\n",lu_get_error_string(LU_ERROR_OUT_OF_MEMORY));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_BAD_FILE_DESCRIPTOR));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_BAD_FILE_DESCRIPTOR));
    printf("end\n");
    mm_free(x);
    return 0;
}