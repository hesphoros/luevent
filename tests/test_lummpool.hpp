#ifndef TEST_LUMMPOOL_HPP
#define TEST_LUMMPOOL_HPP
#include <iostream>
#include <string>
#include "test_common.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
//lu_mm_pool
#include "lu_mm_core.h"
#include "lu_mm_alloc.h"
#include "lu_mm_palloc.h"

#define BLOCK_SIZE 1024 //每次分配内存卡大小 1kb
#define TEST_COUNT 1024*500
#define MEM_POOL_SIZE (1024*4) //内存池每块大小 4kb

TEST(Testlummpool, test_lummpool) {
    int i = 0, k = 0;
    int use_free = 0;

    // 内存池部分
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    char *ptr = NULL;
    // 使用 lu_mm_pool
    lu_mm_pagesize = getpagesize();
    //printf("pagesize: %d\n", lu_mm_pagesize);

    for (k = 0; k < TEST_COUNT ; k++) {
        lu_mm_pool_t *pool = lu_mm_create_pool(102 * 4);

        for (i = 0; i < TEST_COUNT; i++) {
            ptr = (char*)lu_mm_pool_alloc(pool, BLOCK_SIZE);
            if (!ptr) {
                fprintf(stderr, "palloc failed. reason:%s\n", strerror(errno));
            } else {
                *ptr = '\0';
                *(ptr + BLOCK_SIZE - 1) = '\0';
            }
        }

        lu_mm_destroy_pool(pool);
    }

    // 记录程序结束时间
    gettimeofday(&end_time, NULL);
    // 计算并输出运行时间（单位：秒）
    double pool_elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                               (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("Total lu_mm_pool elapsed time: %.4f seconds\n", pool_elapsed_time);
    LU_EVENT_LOG_MSGX( "Total lu_mm_pool elapsed time: %.4f seconds", pool_elapsed_time);
    // malloc/free 部分
    gettimeofday(&start_time, NULL);

    char *malloc_ptr[TEST_COUNT];
    for (k = 0; k < TEST_COUNT ; k++) {
        for (i = 0; i < TEST_COUNT; i++) {
            malloc_ptr[i] = (char*)malloc(BLOCK_SIZE);
            if (!malloc_ptr[i]) {
                fprintf(stderr, "malloc failed. reason:%s\n", strerror(errno));
            } else {
                *malloc_ptr[i] = '\0';
                *(malloc_ptr[i] + BLOCK_SIZE - 1) = '\0';
            }
        }

        for (i = 0; i < TEST_COUNT; i++) {
            if (malloc_ptr[i]) {
                free(malloc_ptr[i]);
            }
        }
    }

    gettimeofday(&end_time, NULL);
    double malloc_elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                                 (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("Total malloc elapsed time: %.4f seconds\n", malloc_elapsed_time);
    LU_EVENT_LOG_MSGX( "Total malloc elapsed time: %.4f seconds", malloc_elapsed_time);
    // 比较两者的运行时间并输出加速比
    if (malloc_elapsed_time > pool_elapsed_time) {
        double speedup = malloc_elapsed_time / pool_elapsed_time;
        printf("lu_mm_pool is %.2f times faster than malloc/free.\n", speedup);
        LU_EVENT_LOG_MSGX( "lu_mm_pool is %.2f times faster than malloc/free.", speedup);
    } else {
        double speedup = pool_elapsed_time / malloc_elapsed_time;
        printf("malloc/free is %.2f times faster than lu_mm_pool.\n", speedup);
        LU_EVENT_LOG_MSGX( "malloc/free is %.2f times faster than lu_mm_pool.", speedup);
    }
}


#endif // TEST_LUMMPOOL_HPP