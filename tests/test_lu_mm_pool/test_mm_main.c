#include "lu_mm_alloc.h"
#include "lu_mm_core.h"
#include "lu_mm_pool_palloc.h"
#include <time.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_POOL_SIZE 1024*4
#define TEST_COUNT 1024*500
#define BLOCK_SIZE 16

int main(){
 int i = 0, k = 0;
    struct timeval start_time, end_time;
    char *pool_str = NULL;

    // 获取页面大小
    lu_mm_pagesize = getpagesize();

    // 使用内存池分配（lu_mm_pool）
    gettimeofday(&start_time, NULL);
    for (k = 0; k < 1024; k++) {
        // 创建内存池，每次分配 BLOCK_SIZE 大小的内存块
        lu_mm_pool_t *pool = lu_mm_create_pool(MEM_POOL_SIZE);

        // 内存池分配
        for (i = 0; i < TEST_COUNT; i++) {
            pool_str = (char*)lu_mm_pool_alloc(pool, BLOCK_SIZE);
            if (!pool_str) {
                fprintf(stderr, "Memory pool allocation failed. Reason: %s\n", strerror(errno));
                continue; // 如果分配失败，继续下一个循环
            } else {
                *pool_str = '\0'; // 初始化
                *(pool_str + BLOCK_SIZE - 1) = '\0';
            }
        }

        // 销毁内存池
        lu_mm_destroy_pool(pool);
    }
    gettimeofday(&end_time, NULL);
    double pool_elapsed_time = (end_time.tv_sec - start_time.tv_sec) +(end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Total lu_mm_pool elapsed time: %.4f seconds\n", pool_elapsed_time);
    printf("lu_mm_pool allocates %ld bytes per block.\n", BLOCK_SIZE * lu_mm_pagesize);

    // 使用 malloc/free 分配（确保内存分配与内存池相同）
    gettimeofday(&start_time, NULL);
    char *ptr[TEST_COUNT];
    for (k = 0; k < 1024; k++) {
        // malloc/free 分配，每次分配 BLOCK_SIZE 大小的内存块
        for (i = 0; i < TEST_COUNT; i++) {
            ptr[i] = (char*)malloc(BLOCK_SIZE);
            if (!ptr[i]) {
                fprintf(stderr, "malloc failed. Reason: %s\n", strerror(errno));
            } else {
                *ptr[i] = '\0'; // 初始化
                *(ptr[i] + BLOCK_SIZE - 1) = '\0';
            }
        }

        // 释放内存
        for (i = 0; i < TEST_COUNT; i++) {
            if (ptr[i]) free(ptr[i]);
        }

    }
    gettimeofday(&end_time, NULL);
    double malloc_elapsed_time = (end_time.tv_sec - start_time.tv_sec) +(end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("Total malloc elapsed time: %.4f seconds\n", malloc_elapsed_time);

    // 比较两者的运行时间并输出加速比
    double speedup = (malloc_elapsed_time > pool_elapsed_time) ?
                     malloc_elapsed_time / pool_elapsed_time :
                     pool_elapsed_time / malloc_elapsed_time;

    const char *faster = (malloc_elapsed_time > pool_elapsed_time) ? "malloc/free" : "lu_mm_pool";
    printf("%s is %.2f times faster than %s.\n", faster, speedup,
           (malloc_elapsed_time > pool_elapsed_time) ? "lu_mm_pool" : "malloc/free");

    return 0;
}