#include "include/lu_memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
  
int main(){
    
    int *x = mm_malloc(sizeof(int));
    *x = 1;
    printf("%d\n", *x);
    return 0;
}