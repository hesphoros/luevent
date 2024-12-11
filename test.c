#include "include/lu_memory_manager.h"
#include <stdio.h>


//#define LU_EVENT__ENABLE_DEFAULT_MEMORY_LOGGING
int main(){

    lu_enable_default_memory_logging(1);    
    int *x = mm_malloc(sizeof(int));
    *x = 1;
    printf("%d\n", *x);

    mm_free(x);
    return 0;
}